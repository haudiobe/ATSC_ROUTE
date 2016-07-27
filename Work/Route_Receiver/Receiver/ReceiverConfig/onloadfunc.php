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
exec("sudo python ../SLT_signalling/receiver.py");

// Have to modify the file to get the IP from the SLT.xml file itself.

chdir('../Receiver/SLT_signalling');
$result = json_decode(exec('sudo python readFromSLT.py 1'), true);

// Adithyan
// Earlier was read from SDP, not read from the SLT.xml file itself.
// Here, the assumption is that the both the channels are being sent by the same sender.
// If they are send by different sender, then we would have to change the "readFromSLT.py 1", according to the channel number.
// Channel 1 : sudo python readFromSLT.py 1
// Channel 2 : sudo python readFromSLT.py 2

/*$contents = file_get_contents("../../bin/SDP1.sdp") ;

$findme="IP4";
$pos = strpos($contents, $findme);
$start=$pos+4;
 $end=strpos($contents,"\n",$start);
 $ip=substr($contents,$start,$end-$start);
 //echo $ip;
*/
 echo json_encode($result[1]);
 ?>
