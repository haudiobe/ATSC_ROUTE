<?php
$micro_date = microtime();
$date_array = explode(" ",$micro_date);


ini_set('memory_limit','-1');//remove memory limit
$channel=1;
/* 
Main script for starting flure reception and MPD re-writing
 */
#chdir('../bin/');
$currDir=dirname(__FILE__);

#Define Paths

$DASHContentBase="DASH_Content";
$DASHContentDir=$DASHContentBase . (string)$channel;
$DASHContent=$currDir . "/" . $DASHContentDir;
$OriginalMPD= "MultiRate_Dynamic.mpd";


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

    foreach ($dom->documentElement->childNodes as $node)
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
    $periods[0]['adaptationSet'][0]['representation'][0]['segmentTemplate']['node'];
    $periodNode[0]->setAttribute("start","PT111S");
    $periodNode[0]->setAttribute("id","1");
    $periodNode[1]->setAttribute("start","PT333S");
    #if($baseURLNode)
    #    $periodNode[0]->parentNode->insertBefore($baseURLNode,$periodNode[0]);
    
    #if($locationNode)
    #    $periodNode[0]->parentNode->insertBefore($locationNode,$periodNode[0]);
    
    $dom->save("somethingnew.mpd");

?>