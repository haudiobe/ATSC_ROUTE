<?php
$micro_date = microtime();
$date_array = explode(" ",$micro_date);
$date = date("Y-m-d H:i:s",$date_array[1]);
unlink ('../bin/timelog.txt');
file_put_contents ( "timelog.txt" , "Start:" . $date . $date_array[0] . " \r\n" , FILE_APPEND );

ini_set('memory_limit','-1');//remove memory limit

/* 
Main script for starting flure reception and MPD re-writing
 */
$AdSource = intval(file_get_contents("RcvConfig.txt"));
chdir('../bin/');
$currDir=dirname(__FILE__);

$channel = $_REQUEST['channel'];
$responseToSend = array();
$responseToSend[0] = $channel;
//echo "Started channel ". $channel;

#Define Paths

$DASHContentBase="DASH_Content";
$DASHContentDir=$DASHContentBase . (string)$channel;
$DASHContent=$currDir . "/" . $DASHContentDir;
#$OriginalMPD= "MultiRate_Dynamic.mpd";
$AdMPDName="Ad2/Ad2_MultiRate.mpd";



#if($channel == 1)
#{   
#    $initAudio = "BBB_64k_init.mp4";
#    $initVideo = "BBB_720_4M_video_init.mp4";
#    $segTemplateAudio = "BBB_64k_*.mp4";
#    $segTemplateVideo = "BBB_720_4M_video_*.mp4"; 
#}
#else
#{
#    $initAudio = "ToS_64k_init.mp4";
#    $initVideo = "ToS_720_4M_video_init.mp4";
#    $segTemplateAudio = "ToS_64k_*.mp4";
#    $segTemplateVideo = "ToS_720_4M_video_*.mp4";    
#}

$initAudio = "*audio_64k_init.mp4";
$initVideo = "*video_8M_init.mp4";
$segTemplateAudio = "*audio_64k_*.mp4";
$segTemplateVideo = "*video_8M_*.mp4";

#if($channel == 1)
#{
#    $initAudio = "BBB_64k_init.mp4";
#    $initVideo = "BBB_720_4M_video_init.mp4";
#    $segTemplateAudio = "BBB_64k_*.mp4";
#    $segTemplateVideo = "BBB_720_4M_video_*.mp4";    
#}
#else
#{
#    $initAudio = "euro_beauty_t2_2k_6m_dash_track2_init.mp4";
#    $initVideo = "euro_beauty_t2_2k_6m_dash_track1_init.mp4";
#    $segTemplateAudio = "euro_beauty_t2_2k_6m_dash_track2_*.m4s";
#    $segTemplateVideo = "euro_beauty_t2_2k_6m_dash_track2_*.m4s";  
#}
#else
#{
#    $initAudio = "feel_the_beauty_t2_2k_6m_dash_track2_init.mp4";
#    $initVideo = "feel_the_beauty_t2_2k_6m_dash_track1_init.mp4";
#    $segTemplateAudio = "feel_the_beauty_t2_2k_6m_dash_track2_*.m4s";
#    $segTemplateVideo = "feel_the_beauty_t2_2k_6m_dash_track1_*.m4s";  
#}
#else
#{
#    $initAudio = "ED_MPEG2_64k_init.mp4";
#    $initVideo = "ED_720_4M_video_init.mp4";
#    $segTemplateAudio = "ED_MPEG2_64k_*.mp4";
#    $segTemplateVideo = "ED_720_4M_video_*.mp4";  
#}

$Delay=0;	#How much would the AST of the patched MPD be lagging the current system time, i.e. how far in future is the AST (in seconds)?
$PatchedMPD="MultiRate_Dynamic_Patched.mpd";
$FLUTEReceiver="./";
#HTMLLocalStorage="/home/nomor/.config/google-chrome-unstable/Default/Local Storage/"

$index=$channel + 4;
$sdp="SDP" . (string)$index . ".sdp";			#SDP to be used by sender

file_put_contents("SDPcheck.txt", $sdp, FILE_APPEND);

//$index=$index + 1;
//$sdp2="SDP" . (string)$index . ".sdp";			#SDP to be used by sender
//$index=$index + 1;
//$sdp3="SDP" . (string)$index . ".sdp";

$Log="Rcv_Log_MPD" . (string)$channel . ".txt";			#Log containing delays corresponding to FLUTE receiver
$Log2="Rcv_Log_Video" . (string)$channel . ".txt";
$Log3="Rcv_Log_Audio" . (string)$channel . ".txt";
$encodingSymbolsPerPacket=1;	#For Receiver, Only a value of zero makes a difference. Otherwise, it is ignored 
							#This means that more than one encoding symbol is included packet. This could be varying

#Clear HTML5 Local Storage
#if [ -e "$HTMLLocalStorage"*${Client:7}*localstorage-journal -o -e "$HTMLLocalStorage"*${Client:7}*localstorage ]; then
#  echo "Delete Old HTML Local Storage"
#  rm "$HTMLLocalStorage"*${Client:7}*
#fi

#Initialize DASHContent Folder
exec("mkdir -p $DASHContent");
array_map('unlink', glob("$DASHContent/*"));

#In case previous instances are running due to may be a crash
exec("sudo killall flute");
$micro_date = microtime();
$date_array = explode(" ",$micro_date);
$date = date("Y-m-d H:i:s",$date_array[1]);
file_put_contents ( "timelog.txt" , "Launching FLUTE:" . $date . $date_array[0] . " \r\n" , FILE_APPEND );
# Start first flute sender
#Start MPD receiving session followed by Audio and Video sessions.
chdir('../Receiver/SLT_signalling');
$result = json_decode(exec('sudo python readFromSLT.py ' . $channel), true);
chdir('../../bin');
#$cmd=  "sudo nice --20 ./flute -A -B:". $DASHContent ." -d:" . $sdp . " -Q -Y:" . $encodingSymbolsPerPacket . " -J:" . $Log . " > /dev/null &"; // > logout2.txt &";
$cmd=  "sudo nice --20 ./flute -A -B:". $DASHContent ." -m:". $result[0] ." -s:". $result[1] ." -p:". $result[2] ." -t:". $result[3] ." -Q -Y:". $encodingSymbolsPerPacket ." -J:". $Log ." > /dev/null &"; // > logout2.txt &";
exec($cmd);

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

// After receive SLS fragments ,i.e S-TSID and MPD. Extract info from S-TSID and start Video and Audio LCT sessions.
while (!glob($DASHContent."/".$sTSIDUri)) usleep(5000);
$s_tsid= simplexml_load_file($DASHContent."/".$sTSIDUri);
$sourceIP=$s_tsid->RS[0]['sIpAddr'];
$tsi_v=$s_tsid->RS[0]->LS[0]['tsi'];
$tsi_a=$s_tsid->RS[0]->LS[1]['tsi'];
$port=$s_tsid->RS[0]['dport'];
$port_v=$port+$tsi_v;
$port_a=$port+$tsi_a;
$cmd=  "sudo nice --20 ./flute -A -B:". $DASHContent ." -m:224.1.1.1 -s:"  .$sourceIP . " -p:".$port_v." -t:".$tsi_v. " -Q -Y:" . $encodingSymbolsPerPacket . " -J:" . $Log2 . " > /dev/null &"; //> logout1.txt &";
exec($cmd);

# Start second flute sender
$cmd=  "sudo nice --20 ./flute -A -B:". $DASHContent ." -m:224.1.1.1 -s:"  .$sourceIP . " -p:".$port_a." -t:".$tsi_a. " -Q -Y:" . $encodingSymbolsPerPacket . " -J:" . $Log3 . " > /dev/null &"; // > logout2.txt &";
exec($cmd);

$micro_date = microtime();
$date_array = explode(" ",$micro_date);
$date = date("Y-m-d H:i:s",$date_array[1]);
file_put_contents ( "timelog.txt" , "Started Globbing:" . $date . $date_array[0] . " \r\n" , FILE_APPEND );

while (!glob("../bin/socketServerReady.trig")) usleep(1000);

?>
