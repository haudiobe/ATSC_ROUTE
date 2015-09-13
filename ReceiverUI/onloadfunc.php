<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

header('Content-Type: text/event-stream');
// recommended to prevent caching of event data.
header('Cache-Control: no-cache'); 
//$myfile=fopen("/var/www/html/ATSC_ROUTE/Work/Route_Receiver/bin/SDP1.sdp", "r");
$contents = file_get_contents("/var/www/html/ATSC_ROUTE/Work/Route_Receiver/bin/SDP1.sdp") ;

$findme="IP4";
$pos = strpos($contents, $findme);
$start=$pos+4;
 $end=strpos($contents,"\n",$start);
 $ip=substr($contents,$start,$end-$start);
 //echo $ip;
 echo json_encode($ip);
 ?>