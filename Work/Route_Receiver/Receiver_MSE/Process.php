<?php
$micro_date = microtime();
$date_array = explode(" ",$micro_date);
$date = date("Y-m-d H:i:s",$date_array[1]);
unlink ('../bin/timelog.txt');
file_put_contents ( "timelog.txt" , "Start:" . $date . $date_array[0] . " \r\n" , FILE_APPEND );

ini_set('memory_limit','-1');//remove memory limit

/* 
Main script for starting flute reception and MPD re-writing
 */
chdir('../bin/');
$currDir=dirname(__FILE__);

$channel = $_REQUEST['channel'];
echo "Started channel ". $channel;

#Define Paths

$DASHContentBase="DASH_Content";
$DASHContentDir=$DASHContentBase . (string)$channel;
$DASHContent=$currDir . "/" . $DASHContentDir;

$FLUTEReceiver="./";

unlink('../bin/socketServerReady.trig');

$index=($channel - 1)*2 + 1;
$sdp="SDP" . (string)$index . ".sdp";			#SDP to be used by sender
$index=$index + 1;
$sdp2="SDP" . (string)$index . ".sdp";			#SDP to be used by sender

$Log="Rcv_Log_Video" . (string)$channel . ".txt";			#Log containing delays corresponding to FLUTE receiver
$Log2="Rcv_Log_Audio" . (string)$channel . ".txt";
$encodingSymbolsPerPacket=1;	#For Receiver, Only a value of zero makes a difference. Otherwise, it is ignored 
							#This means that more than one encoding symbol is included packet. This could be varying

# ----------------------------------------------------------------------
#Initialize DASHContent Folder
exec("mkdir -p $DASHContent");
array_map('unlink', glob("$DASHContent/*"));

#In case previous instances are running due to may be a crash
exec("sudo killall flute");
$micro_date = microtime();
$date_array = explode(" ",$micro_date);
$date = date("Y-m-d H:i:s",$date_array[1]);
file_put_contents ( "timelog.txt" , "Launching FLUTE:" . $date . $date_array[0] . " \r\n" , FILE_APPEND );

# ----------------------------------------------------------------------
# Start first flute sender
# Start MPD receiving session followed by Audio and Video sessions.
chdir('../Receiver/SLT_signalling');
$result = json_decode(exec('sudo python readFromSLT.py ' . $channel), true);
chdir('../../bin');
#$cmd=  "sudo nice --20 ./flute -A -B:". $DASHContent ." -d:" . $sdp . " -Q -Y:" . $encodingSymbolsPerPacket . " -J:" . $Log . " > /dev/null &"; // > logout2.txt &";
$cmd=  "sudo nice --20 ./flute -A -B:". $DASHContent ." -m:". $result[0] ." -s:". $result[1] ." -p:". $result[2] ." -t:". $result[3] ." -Q -Y:". $encodingSymbolsPerPacket ." -J:". $Log ." > log_sls.txt &"; // > logout2.txt &";
#$cmd=  "sudo nice --20 ./flute -A -B:". $DASHContent ." -m:224.1.1.1 -s:10.4.247.130 -p:4005 -t:5 -Q -Y:1 > /dev/null &"; // > logout2.txt &";
exec($cmd);

# ----------------------------------------------------------------------
# This part is related to USBD signalling. 
# We first read the contents of the USBD file and from that file 
# we extract the information (names) of MPD and S-TSID.
# Then this is parsed and the contents are fetched. 
while (!glob($DASHContent."/usbd.xml")) usleep(5000);
#Adi start - Read the content form the usbd.xml file
$bundleDescription = simplexml_load_file($DASHContent."/usbd.xml");
$fullMPDUri = $bundleDescription->userServiceDescription[0]['fullMPDUri'];
$sTSIDUri = $bundleDescription->userServiceDescription[0]['sTSIDUri'];
$OriginalMPD= $fullMPDUri;

while (!glob($DASHContent."/".$sTSIDUri)) usleep(5000);
$s_tsid= simplexml_load_file($DASHContent."/".$sTSIDUri);
$sourceIP=$s_tsid->RS[0]['sIpAddr'];
$tsi_v=$s_tsid->RS[0]->LS[0]['tsi'];
$tsi_a=$s_tsid->RS[0]->LS[1]['tsi'];
$port=$s_tsid->RS[0]['dport'];
$port_v=$port+$tsi_v;
$port_a=$port+$tsi_a;

# ----------------------------------------------------------------------

unlink('../bin/socketServerReady.trig');

# After everytime a flute receiver process in invoked, this socketServerReady.trig is generated.
# It indicates that the flute receiver process is started and working correct.
# After, starting the flute session for the SLS, we have to delete it once.

error_log(print_r($port_a, TRUE)); 

# Start first flute sender
#$cmd=  "sudo nice --20 ./flute -A -B:". $DASHContent ." -m:224.1.1.1 -s:"  .$sourceIP . " -p:".$port_v." -t:".$tsi_v. " -Q -Y:" . $encodingSymbolsPerPacket . " -J:" . $Log2 . " > /dev/null &";
#$cmd=  "sudo nice --20 ./flute -A -B:". $DASHContent ." -m:224.1.1.1 -s:"  .$sourceIP . " -p:".$port_v." -t:".$tsi_v. " -Q -Y:" . $encodingSymbolsPerPacket . " -J:" . $Log2 . " > /dev/null &";
$cmd=  "sudo nice --20 ./flute -A -B:". $DASHContent ." -m:224.1.1.1 -s:"  .$sourceIP . " -p:".$port_v." -t:".$tsi_v. " -Q -Y:" . $encodingSymbolsPerPacket . " -J:" . $Log2 . " > log_video.txt &";
exec($cmd);

# Start second flute sender
$cmd=  "sudo nice --20 ./flute -A -B:". $DASHContent ." -m:224.1.1.1 -s:"  .$sourceIP . " -p:".$port_a." -t:".$tsi_a. " -Q -Y:" . $encodingSymbolsPerPacket . " -J:" . $Log2 . " > log_audio.txt &";
exec($cmd);

$micro_date = microtime();
$date_array = explode(" ",$micro_date);
$date = date("Y-m-d H:i:s",$date_array[1]);
file_put_contents ( "timelog.txt" , "Started Globbing:" . $date . $date_array[0] . " \r\n" , FILE_APPEND );

while (!glob("../bin/socketServerReady.trig")) usleep(1000);

?>
