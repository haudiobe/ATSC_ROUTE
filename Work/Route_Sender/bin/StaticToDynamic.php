<?php


if (isset($argv[0])) {
    echo $argv[0]. PHP_EOL;
    parse_str(implode('&', array_slice($argv, 1)), $_GET);
}
$adInsertion = TRUE;
$adInsertionTimeRequest = intval(file_get_contents("../UIConfig.txt"));  //In seconds from start
$OriginalMPD=$_GET['MPD'];
$PatchedMPD=$_GET['uMPD'];
$AdMPDName="Ad1_MultiRate.mpd";//$_GET['AdMPD'];

$videoFDTFile = "fdt_Video.xml";
$audioFDTFile = "fdt_Audio.xml";
$videoTimingFile = "FluteInput_Video.txt";
$audioTimingFile = "FluteInput_Audio.txt";

if(isset($_GET['AST']))
{
    $AST_W3C=$_GET['AST'];
}
else
{
    $AST_SEC = new DateTime( 'now',  new DateTimeZone( 'UTC' ) );	/* initializer for availability start time */
    $AST_SEC->setTimestamp($date_array[1]);    //Better use a single time than now above
    //$AST_SEC->add(new DateInterval('PT1S'));
    $AST_SEC_W3C = $AST_SEC->format(DATE_W3C);

    preg_match('/\.\d*/',$date_array[0],$dateFracPart);
    $extension_pos = strrpos($AST_SEC_W3C, '+'); // find position of the last + in W3C date to slip frac seconds
    $AST_W3C = substr($AST_SEC_W3C, 0, $extension_pos) . $dateFracPart[0] . "Z" ;//substr($AST_SEC_W3C, $extension_pos);
}

$ASTUNIX=$_GET['ASTUNIX'];

$AST_SEC = new DateTime( 'now',  new DateTimeZone( 'UTC' ) );	/* initializer for availability start time */
$AST_SEC->add(new DateInterval('PT1S'));   //????????????????? some rounding issue??
$AST_SEC_W3C = $AST_SEC->format(DATE_W3C);

$MPD = simplexml_load_file($OriginalMPD);
if (!$MPD)
        die("Failed loading XML file");

$dom_sxe = dom_import_simplexml($MPD);
if (!$dom_sxe) 
{
    echo 'Error while converting XML';
    exit;
}

$dom = new DOMDocument('1.0');
$dom_sxe = $dom->importNode($dom_sxe, true);
$dom_sxe = $dom->appendChild($dom_sxe);

if($adInsertion)
{
	$AdMPD = simplexml_load_file($AdMPDName);
	if (!$AdMPD)
		die("Failed loading Ad MPD");

	$dom_sxe_ad = dom_import_simplexml($AdMPD);
	if (!$dom_sxe_ad) 
	{
		echo 'Error while converting XML';
		exit;
	}

	$domAd = new DOMDocument('1.0');
	$dom_sxe_ad = $domAd->importNode($dom_sxe_ad, true);
	$dom_sxe_ad = $domAd->appendChild($dom_sxe_ad);
}

$periods = parseMPD($dom, !$adInsertion ? NULL : $domAd);

$cumulativeOriginalDuration = 0;    //Cumulation of period duration on source MPD
$cumulativeUpdatedDuration = 0;    //Cumulation of period duration on updated MPD

$originalAST = new DateTime($periods[0]['node']->parentNode->getAttribute("availabilityStartTime"));   

$periods[0]['node']->parentNode->setAttribute("type","dynamic");

$mediaPresentationDuration=$periods[0]['node']->parentNode->getAttribute("mediaPresentationDuration");
$periods[0]['node']->parentNode->removeAttribute("mediaPresentationDuration");

$profiles=$periods[0]['node']->parentNode->getAttribute("profiles");
$periods[0]['node']->parentNode->removeAttribute("profiles");

$periods[0]['node']->parentNode->setAttribute("availabilityStartTime",$AST_W3C);    //Set AST to tune-in time
$periods[0]['node']->parentNode->setAttribute("timeShiftBufferDepth","PT60S");
$periods[0]['node']->parentNode->setAttribute("mediaPresentationDuration",$mediaPresentationDuration);
$periods[0]['node']->parentNode->setAttribute("profiles",$profiles);

$savedTotalDuration = 0;
$restPeriodDuration = 0;
$numVideoSegments = 0;
$numAudioSegments = 0;
$deltaAudio = 0;
$deltaVideo = 0;

for ($periodIndex = 0; $periodIndex < count($periods); $periodIndex++)  //Loop on all periods in the (semi-processed in case of Ad-insertion) MPD
{
    $durationInMPD =$periods[$periodIndex]['node']->getAttribute("duration");
    $duration = somehowPleaseGetDurationInFractionalSecondsBecuasePHPHasABug($durationInMPD);
    
    if($adInsertion)
    {
        $videoSegmentTemplate = &$periods[$periodIndex]['adaptationSet'][0]['representation'][0]['segmentTemplate']['node'];
        $audioSegmentTemplate = &$periods[$periodIndex]['adaptationSet'][1]['representation'][0]['segmentTemplate']['node'];
        
        $videoTimescale = $videoSegmentTemplate->getAttribute("timescale");
        $videoSegmentDuration = $videoSegmentTemplate->getAttribute("duration");
        $videoStartNum = $videoSegmentTemplate->getAttribute("startNumber");
        $videoPTO = $videoSegmentTemplate->getAttribute("presentationTimeOffset");
        
        $audioTimescale = $audioSegmentTemplate->getAttribute("timescale");
        $audioSegmentDuration = $audioSegmentTemplate->getAttribute("duration");
        $audioStartNum = $audioSegmentTemplate->getAttribute("startNumber");
        $audioPTO = $audioSegmentTemplate->getAttribute("presentationTimeOffset");
        
        if($periodIndex == 0)
        {
			$adInsertionTime = getadInsertionTime($adInsertionTimeRequest,$videoSegmentDuration/$videoTimescale,0,$audioSegmentDuration/$audioTimescale,0);
            $numVideoSegments = round($adInsertionTime*$videoTimescale/$videoSegmentDuration);
            $numAudioSegments = round($adInsertionTime*$audioTimescale/$audioSegmentDuration);
            
            $savedTotalDuration = $duration;            
            
            if($numVideoSegments*$videoSegmentDuration/$videoTimescale < $numAudioSegments*$audioSegmentDuration/$audioTimescale)
            {
                $duration = $numVideoSegments*$videoSegmentDuration/$videoTimescale;
            }
            else
            {
                $duration = $numAudioSegments*$audioSegmentDuration/$audioTimescale;
            }
            
            $restPeriodDuration = $savedTotalDuration - $duration;
        }
		else if($periodIndex == 1)
		{
			//Ad period
		}
        else if($periodIndex == 2)
        {
            $duration = $restPeriodDuration;
            
            $newVideoStartNumber = $numVideoSegments + $videoStartNum;
            $newVideoPTO = ($newVideoStartNumber - $videoStartNum) * $videoSegmentDuration + $videoPTO;

            $newAudioStartNumber = $numAudioSegments + $audioStartNum;
            $newAudioPTO = ($newAudioStartNumber - $audioStartNum) * $audioSegmentDuration + $audioPTO; 

            // Find the smaller PTO of audio and video, set the other to the smaller
            if($newVideoPTO/$videoTimescale < $newAudioPTO/$audioTimescale)
            {
				$deltaVideo = 0;
				$deltaAudio = $newAudioPTO/$audioTimescale - $newVideoPTO/$videoTimescale;
              $newAudioPTO = round($newVideoPTO/$videoTimescale*$audioTimescale);
            }
            else
            {
				$deltaAudio = 0;
				$deltaVideo = $newVideoPTO/$videoTimescale - $newAudioPTO/$audioTimescale;				
              $newVideoPTO = round($newAudioPTO/$audioTimescale*$videoTimescale);
            }        

            $videoSegmentTemplate->setAttribute("presentationTimeOffset",$newVideoPTO);
            $videoSegmentTemplate->setAttribute("startNumber",$newVideoStartNumber);

            $audioSegmentTemplate->setAttribute("presentationTimeOffset",$newAudioPTO);
            $audioSegmentTemplate->setAttribute("startNumber",$newAudioStartNumber);
        }
		
		generateFDTAndTiming($cumulativeOriginalDuration,$videoSegmentTemplate,$audioSegmentTemplate,$duration,$deltaVideo,$deltaAudio);
    }

    $cumulativeDurationPreceedingPeriods = $cumulativeOriginalDuration; //Save it for later use
    $cumulativeOriginalDuration += $duration; 

    $periods[$periodIndex]['node']->removeAttribute("id");
    $periods[$periodIndex]['node']->removeAttribute("duration");
    
    $periods[$periodIndex]['node']->setAttribute("start","PT".round($cumulativeDurationPreceedingPeriods,2) ."S");
    $periods[$periodIndex]['node']->setAttribute("id",$periodIndex+1);
    $periods[$periodIndex]['node']->setAttribute("duration", "PT". round($duration , 2) . "S");
}

$dom->save($PatchedMPD);

//read the entire string
$fileContents=file_get_contents($audioFDTFile);

//replace something in the file string - this is a VERY simple example
$fileContents=str_replace("MPDNamePlaceholder", $PatchedMPD,$fileContents);
$fileContents=str_replace("MPDSizePlaceholder", filesize($PatchedMPD),$fileContents);

//write the entire string
file_put_contents($audioFDTFile, $fileContents);
    
function &parseMPD($dom,$domAd)
{
    global $adInsertion;
    $finalPeriodInserted = FALSE;
	
	$docElement = $dom->documentElement;
	$adDocElement = NULL;
	
	if($adInsertion)
	{
		$adDocElement = $domAd->documentElement;
		foreach ($adDocElement->childNodes as $node)
		{  
			if($node->nodeName === 'Period')
				$adPeriod = $node;
		}
	}
    
    foreach ($docElement->childNodes as $node)
    {
        //echo $node->nodeName; // body
        if($node->nodeName === 'Location')
            $locationNode = $node;
        if($node->nodeName === 'BaseURL')
            $baseURLNode = $node;    
        if($node->nodeName === 'Period')
        {
            if($adInsertion && !$finalPeriodInserted)
            {
				$adNode = $dom->importNode($adPeriod, true);
				$node->parentNode->appendChild($adNode);	//Ad
				
                $finalPeriod = $node->cloneNode (true);
                $node->parentNode->appendChild($finalPeriod);
                $finalPeriodInserted = TRUE;
            }
            
            $periods[]['node'] = $node;

            $currentPeriod = &$periods[count($periods) - 1];
            foreach ($currentPeriod['node']->childNodes as $node)
            {
                if($node->nodeName === 'AdaptationSet')
                {
                    $currentPeriod['adaptationSet'][]['node'] = $node;
                    
                    $currentAdaptationSet = &$currentPeriod['adaptationSet'][count($currentPeriod['adaptationSet']) - 1];                    
                    foreach ($currentAdaptationSet['node']->childNodes as $node)
                    {
                        if($node->nodeName === 'Representation')
                        {
                            $currentAdaptationSet['representation'][]['node'] = $node;
                            
                            $currentRepresentation = &$currentAdaptationSet['representation'][count($currentAdaptationSet['representation']) - 1];

                            foreach ($currentRepresentation['node']->childNodes as $node)
                            {
                                if($node->nodeName === 'SegmentTemplate')
                                    $currentRepresentation['segmentTemplate']['node'] = $node;
                            }
                        }
                    }
                }
            }            
        }
    }
    
    return $periods;
}

function getadInsertionTime($adInsertionTimeRequest,$videoSegmentDurationInSec,$initialVideoOffset,$audioSegmentDurationInSec,$initialAudioOffset)
{
	$nearestVideoSegmentEndingTime = round(($adInsertionTimeRequest - $initialVideoOffset)/$videoSegmentDurationInSec)*$videoSegmentDurationInSec + $initialVideoOffset;
	$nearestAudioSegmentEndingTime = round(($nearestVideoSegmentEndingTime - $initialAudioOffset)/$audioSegmentDurationInSec)*$audioSegmentDurationInSec + $initialAudioOffset;
	return min($nearestVideoSegmentEndingTime,$nearestAudioSegmentEndingTime);
}

function generateFDTAndTiming($start,$videoSegmentTemplate,$audioSegmentTemplate,$duration,$deltaVideo,$deltaAudio)
{
	global $videoFDTFile, $audioFDTFile, $videoTimingFile, $audioTimingFile, $ASTUNIX;
	static $firstcall = true;
	static $videoDoc = NULL;
	static $audioDoc = NULL;
	static $videoInstance = NULL;
	static $audioInstance = NULL;
	static $videoTOI = 1;
	static $audioTOI = 1;
	
	$videoTimescale = $videoSegmentTemplate->getAttribute("timescale");
	$videoSegmentDuration = $videoSegmentTemplate->getAttribute("duration");
	$videoStartNum = $videoSegmentTemplate->getAttribute("startNumber");
	$videoPTO = $videoSegmentTemplate->getAttribute("presentationTimeOffset");
	$videoMedia = $videoSegmentTemplate->getAttribute("media");
	$videoInit = $videoSegmentTemplate->getAttribute("initialization");
	
	$audioTimescale = $audioSegmentTemplate->getAttribute("timescale");
	$audioSegmentDuration = $audioSegmentTemplate->getAttribute("duration");
	$audioStartNum = $audioSegmentTemplate->getAttribute("startNumber");
	$audioPTO = $audioSegmentTemplate->getAttribute("presentationTimeOffset");
	$audioMedia = $audioSegmentTemplate->getAttribute("media");
	$audioInit = $audioSegmentTemplate->getAttribute("initialization");
	
	if($firstcall)
	{
		$videoDoc = new DOMDocument('1.0');
		$videoDoc->formatOutput = true;

		$videoInstance = $videoDoc->createElement('FDT-Instance');
		$videoInstance->setAttribute("Expires","32511974400");
		$videoInstance->setAttribute("FEC-OTI-FEC-Encoding-ID","0");
		$videoInstance->setAttribute("FEC-OTI-Maximum-Source-Block-Length","5000");
		$videoInstance->setAttribute("FEC-OTI-Encoding-Symbol-Length","1428");
		$videoDoc->appendChild($videoInstance);

		$audioDoc = new DOMDocument('1.0');
		$audioDoc->formatOutput = true;

		$audioInstance = $audioDoc->createElement('FDT-Instance');
		$audioInstance->setAttribute("Expires","32511974400");
		$audioInstance->setAttribute("FEC-OTI-FEC-Encoding-ID","0");
		$audioInstance->setAttribute("FEC-OTI-Maximum-Source-Block-Length","5000");
		$audioInstance->setAttribute("FEC-OTI-Encoding-Symbol-Length","1428");
		$audioDoc->appendChild($audioInstance);
		unlink($videoTimingFile);
		unlink($audioTimingFile);		
	}
	
	echo "deltaVideo: " . $deltaVideo . ", Duration: " . $duration . PHP_EOL;

	for($videoIndex = $videoStartNum ; ; $videoIndex++)
	{
		if((($videoIndex - $videoStartNum)*$videoSegmentDuration/$videoTimescale + $deltaVideo) >= $duration)
			break;
		
		file_put_contents($videoTimingFile,$videoTOI . " " . intval($ASTUNIX + ($start + ($videoIndex - $videoStartNum - 1)*$videoSegmentDuration/$videoTimescale + $deltaVideo)*1000000) . "\n",FILE_APPEND);	
		
		$file = $videoDoc->createElement('File');
		$file->setAttribute("TOI",$videoTOI);$videoTOI++;
		$file->setAttribute("Content-Location",'file:///' . $videoInit);
		$file->setAttribute("Content-Length",filesize($videoInit));
		$videoInstance->appendChild($file);
		
		$file = $videoDoc->createElement('File');
		$file->setAttribute("TOI",$videoTOI);$videoTOI++;
		$filename = preg_replace('/\$Number\$/', $videoIndex, $videoMedia);
		$file->setAttribute("Content-Location",'file:///' . $filename);
		$file->setAttribute("Content-Length",filesize($filename));
		$videoInstance->appendChild($file);		
	}

	$videoDoc->save($videoFDTFile);

	for($audioIndex = $audioStartNum ; ; $audioIndex++)
	{
		if((($audioIndex - $audioStartNum)*$audioSegmentDuration/$audioTimescale + $deltaAudio) >= $duration)
			break;
		
		file_put_contents($audioTimingFile,$audioTOI . " " . intval($ASTUNIX + ($start + ($audioIndex - $audioStartNum - 1)*$audioSegmentDuration/$audioTimescale + $deltaAudio)*1000000) . "\n",FILE_APPEND);			
		
		$file = $audioDoc->createElement('File');
		$file->setAttribute("TOI",$audioTOI);$audioTOI++;
		$file->setAttribute("Content-Location",'file:///' . "MPDNamePlaceholder");
		$file->setAttribute("Content-Length","MPDSizePlaceholder");
		$audioInstance->appendChild($file);

		$file = $audioDoc->createElement('File');
		$file->setAttribute("TOI",$audioTOI);$audioTOI++;
		$file->setAttribute("Content-Location",'file:///' . $audioInit);
		$file->setAttribute("Content-Length",filesize($audioInit));
		$audioInstance->appendChild($file);
		
		$file = $audioDoc->createElement('File');
		$file->setAttribute("TOI",$audioTOI);$audioTOI++;
		$filename = preg_replace('/\$Number\$/', $audioIndex, $audioMedia);
		$file->setAttribute("Content-Location",'file:///' . $filename);
		$file->setAttribute("Content-Length",filesize($filename));
		$audioInstance->appendChild($file);		
	}

	$audioDoc->save($audioFDTFile);
	
	$firstcall = false;
}

function somehowPleaseGetDurationInFractionalSecondsBecuasePHPHasABug($durstr)
{
	    if(strpos($durstr,'.') !== FALSE)	//If indeed there is float values
		{
                        $temp = explode('.', $durstr);
			$durstrint = $temp[0] . 'S';
                        $temp1 = explode('.', $durstr);
                        $temp2 = explode('S',$temp1[1]);
			$fracSec = '0.' . $temp2[0];
		}
		else
		{
			$durstrint = $durstr;
			$fracSec = 0;
		}
		
        $di = new DateInterval($durstrint);

        $durationDT = new DateTime();
        $reft = clone $durationDT;
        $durationDT->add($di);
        $duration = $durationDT->getTimestamp() - $reft->getTimestamp() + $fracSec;
        
        return $duration;
}
?>