<?php
$micro_date = microtime();
$date_array = explode(" ",$micro_date);
$date = date("Y-m-d H:i:s",$date_array[1]);
//file_put_contents ( "timelog.txt" , "Start:" . $date . $date_array[0] . " \r\n" , FILE_APPEND );

ini_set('memory_limit','-1');//remove memory limit


/* 
Main script for starting flure reception and MPD re-writing
 */
$currDir=dirname(__FILE__);
$channel=1;
#Define Paths

$DASHContentBase="DASH_Content";
$DASHContentDir=$DASHContentBase . (string)$channel;
$DASHContent=$currDir . "/";
$OriginalMPD= "MultiRate_Dynamic.mpd";

$PatchedMPD="MultiRate_Dynamic_Emulated.mpd";

#HTMLLocalStorage="/home/nomor/.config/google-chrome-unstable/Default/Local Storage/"

if (isset($_GET['astoffset'])) {
$offsetToNow=$_GET['astoffset'];
}
else
$offsetToNow = 0;

$micro_date = microtime();
$date_array = explode(" ",$micro_date);
$date_array[0] = round($date_array[0],4);
$date = date("Y-m-d H:i:s",$date_array[1]);
//file_put_contents ( "timelog.txt" , "Tuned in:" . $date . $date_array[0] . " \r\n" , FILE_APPEND );

$AST_SEC = new DateTime( 'now',  new DateTimeZone( 'UTC' ) );	/* initializer for availability start time */
$AST_SEC->setTimestamp($date_array[1]);    //Better use a single time than now above
//$AST_SEC->add(new DateInterval('PT1S'));
$AST_SEC_W3C = $AST_SEC->format(DATE_W3C);

preg_match('/\.\d*/',$date_array[0],$dateFracPart);
$extension_pos = strrpos($AST_SEC_W3C, '+'); // find position of the last + in W3C date to slip frac seconds
$AST_W3C = substr($AST_SEC_W3C, 0, $extension_pos) . $dateFracPart[0] . "Z" ;//substr($AST_SEC_W3C, $extension_pos);
//file_put_contents ( "timelog.txt" , "Setting AST: " . $AST_W3C . " \r\n" , FILE_APPEND );

$ASTO_SEC = new DateTime( 'now',  new DateTimeZone( 'UTC' ) );	/* initializer for availability start time */
$ASTO_SEC->setTimestamp($date_array[1] - $offsetToNow);    //Better use a single time than now above
$ASTO_SEC_W3C = $ASTO_SEC->format(DATE_W3C);

$extension_pos = strrpos($ASTO_SEC_W3C, '+'); // find position of the last + in W3C date to slip frac seconds
$ASTO_W3C = substr($ASTO_SEC_W3C, 0, $extension_pos) . $dateFracPart[0] . "Z" ;//substr($AST_SEC_W3C, $extension_pos);

    $MPD = simplexml_load_file($DASHContent . "/" . $OriginalMPD);
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
    
    $periods = parseMPD($dom->documentElement);
    
    $cumulativeOriginalDuration = 0;    //Cumulation of period duration on source MPD
    $cumulativeUpdatedDuration = 0;    //Cumulation of period duration on updated MPD
    $tuneInPeriodStartOffset = 0;
    
    $MPDNode = &$periods[0]['node']->parentNode;
    
    $MPD_AST = $ASTO_W3C;//$MPDNode->getAttribute("availabilityStartTime");
    preg_match('/\.\d*/',$MPD_AST,$matches);
    $fracAST = "0" . $matches[0];
    $originalAST = new DateTime($MPD_AST);   
    $timeOffset = $AST_SEC->getTimestamp() + round($date_array[0],4) - ($originalAST->getTimestamp() + $fracAST);  //Offset relative to start
    
    //file_put_contents ( "timelog.txt" , "TimeOffset: " . $timeOffset . ", Original ts:" . ($originalAST->getTimestamp() + $fracAST) . "Tune-in TS: " . ($AST_SEC->getTimestamp() + round($date_array[0],4)) . "\r\n" , FILE_APPEND );
    
    $MPDNode->setAttribute("availabilityStartTime",$AST_W3C);    //Set AST to tune-in time
    
    for ($periodIndex = 0; $periodIndex < count($periods); $periodIndex++)  //Loop on all periods in orginal MPD
    {
        $duration = somehowPleaseGetDurationInFractionalSecondsBecuasePHPHasABug($periods[$periodIndex]['node']->getAttribute("duration"));
        
        $periods[$periodIndex]['node']->setAttribute("start","PT". round($cumulativeUpdatedDuration + $tuneInPeriodStartOffset,4)."S"); 
        
        if($timeOffset < $cumulativeOriginalDuration)   //Tune-in is before this period, it stays intact
        {
            $cumulativeOriginalDuration += $duration;
            $cumulativeUpdatedDuration += $duration; // The eventual duration of updated MPD
            //$periods[$periodIndex]['node']->removeChild ($periods[$periodIndex]['adaptationSet'][1]['node']);
            continue;
        }
        
        $cumulativeDurationPreceedingPeriods = $cumulativeOriginalDuration; //Save it for later use
        $cumulativeOriginalDuration += $duration;        
        
        if($timeOffset > $cumulativeOriginalDuration)   //This period is no more relavant and is not received, hence remove this
        {
            $dom->documentElement->removeChild ($periods[$periodIndex]['node']);
            continue;
        }
        
        //The only case here is the period in which we tune in
        
        $videoSegmentTemplate = &$periods[$periodIndex]['adaptationSet'][0]['representation'][0]['segmentTemplate']['node'];
        $audioSegmentTemplate = &$periods[$periodIndex]['adaptationSet'][1]['representation'][0]['segmentTemplate']['node'];
        
        $videoTimescale = $videoSegmentTemplate->getAttribute("timescale");
        $videoSegmentDuration = $videoSegmentTemplate->getAttribute("duration");
        $videoStartNum = $videoSegmentTemplate->getAttribute("startNumber");
        $videoPTO = $videoSegmentTemplate->getAttribute("presentationTimeOffset");
        
        $newVideoStartNumber = ceil(($timeOffset - $cumulativeDurationPreceedingPeriods)*$videoTimescale/$videoSegmentDuration) + $videoStartNum;
        //file_put_contents ( "timelog.txt" , "new video offset: " . ($timeOffset - $cumulativeDurationPreceedingPeriods)*$videoTimescale/$videoSegmentDuration . "\r\n" , FILE_APPEND );
        $videoOffsetUpdate = ($newVideoStartNumber - $videoStartNum) * $videoSegmentDuration/$videoTimescale;
        
        $audioTimescale = $audioSegmentTemplate->getAttribute("timescale");
        $audioSegmentDuration = $audioSegmentTemplate->getAttribute("duration");
        $audioStartNum = $audioSegmentTemplate->getAttribute("startNumber");
        $audioPTO = $audioSegmentTemplate->getAttribute("presentationTimeOffset");
        
        $newAudioStartNumber = ceil(($timeOffset - $cumulativeDurationPreceedingPeriods)*$audioTimescale/$audioSegmentDuration) + $audioStartNum;
        //file_put_contents ( "timelog.txt" , "new audio offset: " . ($timeOffset - $cumulativeDurationPreceedingPeriods)*$audioTimescale/$audioSegmentDuration . "\r\n" , FILE_APPEND );
        $audioOffsetUpdate = ($newAudioStartNumber - $audioStartNum) * $audioSegmentDuration/$audioTimescale;
        
        // Find the smaller update offset of audio and video, set the other to the smaller
        $offsetUpdate = min($videoOffsetUpdate , $audioOffsetUpdate);
        
        $newAudioPTO = round($offsetUpdate*$audioTimescale + $audioPTO);
        $newVideoPTO = round($offsetUpdate*$videoTimescale + $videoPTO);
        
        $tuneInPeriodStartOffset = $offsetUpdate - ($timeOffset - $cumulativeDurationPreceedingPeriods);
        $periods[$periodIndex]['node']->setAttribute("start","PT". round($cumulativeUpdatedDuration + $tuneInPeriodStartOffset ,4)."S");         
        
        $remainingPeriodDuration = $duration - $offsetUpdate;
        $cumulativeUpdatedDuration += $remainingPeriodDuration; // The eventual duration of updated MPD
        
        $periods[$periodIndex]['node']->setAttribute("duration", "PT". round($remainingPeriodDuration,4) . "S");
        
        $videoSegmentTemplate->setAttribute("presentationTimeOffset",$newVideoPTO);
        $videoSegmentTemplate->setAttribute("startNumber",$newVideoStartNumber);

        $audioSegmentTemplate->setAttribute("presentationTimeOffset",$newAudioPTO);
        $audioSegmentTemplate->setAttribute("startNumber",$newAudioStartNumber);
        
        //$periods[$periodIndex]['node']->removeChild ($periods[$periodIndex]['adaptationSet'][1]['node']);
    }
    
    //Set the updated MPD duration
    $MPDNode->setAttribute("mediaPresentationDuration","PT". round($cumulativeUpdatedDuration + $tuneInPeriodStartOffset ,4) . "S");

    $dom->save($DASHContent . "/" . $PatchedMPD);
    corsHeader();
    header("Content-Type: application/xml");
    $toecho = $dom->saveXML();

    echo $toecho;
    

#file_put_contents ( "timelog.txt" , $latestFiles , FILE_APPEND );
$micro_date = microtime();
$date_array = explode(" ",$micro_date);
$date = date("Y-m-d H:i:s",$date_array[1]);
//file_put_contents ( "timelog.txt" , "Done:" . $date . $date_array[0] . " \r\n" , FILE_APPEND );

exit;
/*Send out CORS header*/
function corsHeader()
{
	header("Access-Control-Allow-Headers: origin,range");
	header("Access-Control-Expose-Headers: Server,range");
	header("Access-Control-Allow-Methods: GET, HEAD, OPTIONS");
	header("Access-Control-Allow-Origin: *");
}

function &parseMPD($docElement)
{
    foreach ($docElement->childNodes as $node)
    {
        //echo $node->nodeName; // body
        if($node->nodeName === 'Location')
            $locationNode = $node;
        if($node->nodeName === 'BaseURL')
            $baseURLNode = $node;    
        if($node->nodeName === 'Period')
        {
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

function somehowPleaseGetDurationInFractionalSecondsBecuasePHPHasABug($durstr)
{
        $durstrint = explode('.', $durstr)[0] . 'S';
        $fracSec = '0.' . explode('S',explode('.', $durstr)[1])[0];
        $di = new DateInterval($durstrint);

        $durationDT = new DateTime();
        $reft = new DateTime();
        $durationDT->add($di);
        $duration = $durationDT->getTimestamp() - $reft->getTimestamp() + $fracSec;
        
        return $duration;
}
?>