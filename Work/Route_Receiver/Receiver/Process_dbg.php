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
chdir('../bin/');
$currDir=dirname(__FILE__);

$channel = $_REQUEST['channel'];
echo "Started channel ". $channel;

#Define Paths

$DASHContentBase="DASH_Content";
$DASHContentDir=$DASHContentBase . (string)$channel;
$DASHContent=$currDir . "/" . $DASHContentDir;
$OriginalMPD= "MultiRate_Dynamic.mpd";
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

$initAudio = "audio_64k_init.mp4";
$initVideo = "video_8M_init.mp4";
$segTemplateAudio = "audio_64k_*.mp4";
$segTemplateVideo = "video_8M_*.mp4";

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

$index=($channel - 1)*2 + 1;
$sdp="SDP" . (string)$index . ".sdp";			#SDP to be used by sender
$index=$index + 1;
$sdp2="SDP" . (string)$index . ".sdp";			#SDP to be used by sender

$Log="Rcv_Log_Video" . (string)$channel . ".txt";			#Log containing delays corresponding to FLUTE receiver
$Log2="Rcv_Log_Audio" . (string)$channel . ".txt";
$encodingSymbolsPerPacket=1;	#For Receiver, Only a value of zero makes a difference. Otherwise, it is ignored 
							#This means that more than one encoding symbol is included packet. This could be varying

#Clear HTML5 Local Storage
#if [ -e "$HTMLLocalStorage"*${Client:7}*localstorage-journal -o -e "$HTMLLocalStorage"*${Client:7}*localstorage ]; then
#  echo "Delete Old HTML Local Storage"
#  rm "$HTMLLocalStorage"*${Client:7}*
#fi

#Initialize DASHContent Folder
exec("mkdir -p $DASHContent");
#array_map('unlink', glob("$DASHContent/*"));

#In case previous instances are running due to may be a crash
exec("sudo killall flute");
$micro_date = microtime();
$date_array = explode(" ",$micro_date);
$date = date("Y-m-d H:i:s",$date_array[1]);
file_put_contents ( "timelog.txt" , "Launching FLUTE:" . $date . $date_array[0] . " \r\n" , FILE_APPEND );
# Start first flute sender
$cmd=  "sudo nice --20 ./flute -A -B:". $DASHContent ." -d:" . $sdp . " -Q -Y:" . $encodingSymbolsPerPacket . " -J:" . $Log . " > logout1.txt &";
#exec($cmd);

# Start second flute sender
$cmd=  "sudo nice --20 ./flute -A -B:". $DASHContent ." -d:" . $sdp2 . " -Q -Y:" . $encodingSymbolsPerPacket . " -J:" . $Log . " > logout2.txt &";
#exec($cmd);

$micro_date = microtime();
$date_array = explode(" ",$micro_date);
$date = date("Y-m-d H:i:s",$date_array[1]);
file_put_contents ( "timelog.txt" , "Started Globbing:" . $date . $date_array[0] . " \r\n" , FILE_APPEND );
while (!glob($DASHContent."/$OriginalMPD")) usleep(5000);

#For using with the canned trace file, re-write the AST to current system time when MPD is received
#$cmd=  "./UpdateMPD_AST_ToSystemTime.sh ". $DASHContent . "/" . $OriginalMPD . " " . $Delay . " " . $DASHContent . "/" . $PatchedMPD;
#exec("cp ". $DASHContent . "/" . $OriginalMPD  . " " . $DASHContent . "/" . $PatchedMPD);
#exec($cmd);
 

while (!glob($DASHContent.'/'.$initAudio)) usleep(5000);
while (!glob($DASHContent.'/'.$initVideo)) usleep(5000);
while (!array_diff(glob($DASHContent.'/'.$segTemplateAudio), array($DASHContent.'/'.$initAudio))) usleep(5000);
while (!array_diff(glob($DASHContent.'/'.$segTemplateVideo), array($DASHContent.'/'.$initVideo))) usleep(5000);

$micro_date = microtime();
$date_array = explode(" ",$micro_date);
$date = date("Y-m-d H:i:s",$date_array[1]);
file_put_contents ( "timelog.txt" , "Tuned in:" . $date . $date_array[0] . " \r\n" , FILE_APPEND );

$AST_SEC = new DateTime( 'now',  new DateTimeZone( 'UTC' ) );	/* initializer for availability start time */
$AST_SEC->add(new DateInterval('PT1S'));
$AST_SEC_W3C = $AST_SEC->format(DATE_W3C);

$microsec = explode(".",$date_array[0]);
$extension_pos = strrpos($AST_SEC_W3C, '+'); // find position of the last + in W3C date to slip frac seconds
$AST_W3C = substr($AST_SEC_W3C, 0, $extension_pos) . "Z" ;//"." .$microsec[1] . substr($AST_SEC_W3C, $extension_pos);
file_put_contents ( "timelog.txt" , "Setting AST: " . $AST_W3C . " \r\n" , FILE_APPEND );

#exec("date >> timelog.txt");
#exec("date +%s%N | cut -b11-13 >> timelog.txt");
#$videoSegs=glob($DASHContent.'/'.$segTemplateVideo);

$files = glob($DASHContent.'/'.$segTemplateAudio);
$files = array_diff($files, array($DASHContent.'/'.$initAudio));
$files = array_combine($files, array_map("filemtime", $files));
arsort($files);
$latest_audio = key($files);

$latest_audio_num = 29;//explode('.',array_pop(explode('_', $latest_audio)))[0];

$files = glob($DASHContent.'/'.$segTemplateVideo);
$files = array_diff($files, array($DASHContent.'/'.$initVideo));
$files = array_combine($files, array_map("filemtime", $files));
arsort($files);

$latest_video = key($files);

$latest_video_num = 29;//explode('.',array_pop(explode('_', $latest_video)))[0];

$latestFiles = "Latest files:" . $latest_video . ", Audio: " . $latest_audio;

$largeSegments = false;

if( strpos(file_get_contents($DASHContent."/$OriginalMPD"),'duration="12288"') !== false or strpos(file_get_contents($DASHContent."/$OriginalMPD"),'duration="12274"') !== false)
{
     $largeSegments = true;
}
# In respective timescales
if($largeSegments)
{
    if($channel == 1)
    {
        $videoSegmentDuration = 12288;
        $videoTimeScale = 12288;
        $audioFirstSegmentDuration = 41984;
        $audioSegmentDuration = 44032;
        $audioTimeScale = 44100;
    }
    else
    {
        $videoSegmentDuration = 12288;
        $videoTimeScale = 12288;
        $audioFirstSegmentDuration = 45056;
        $audioSegmentDuration = 47104;
        $audioTimeScale = 48000;
    }
}
else
{
if($channel == 1)
{
  $videoSegmentDuration = 6144;
  $videoTimeScale = 12288;
      #$audioFirstSegmentDuration = 14431;
      #$audioSegmentDuration = 20480;
      $audioFirstSegmentDuration = 19456;
      $audioSegmentDuration = 21504;
  $audioTimeScale = 44100;
}
else
{
  $videoSegmentDuration = 6144;
  $videoTimeScale = 12288;
      #$audioFirstSegmentDuration = 16479;
      #$audioSegmentDuration = 22528;
      $audioFirstSegmentDuration = 21504;
      $audioSegmentDuration = 23552;
  $audioTimeScale = 48000;
}
}
#if($channel == 1)
#{
#  $videoSegmentDuration = 12288;
#  $videoTimeScale = 12288;
#  $audioFirstSegmentDuration = 41984;
#  $audioSegmentDuration = 44032;
#  $audioTimeScale = 44100;
#}
#else
#{
#  $videoSegmentDuration = 12288;
#  $videoTimeScale = 12288;
#  $audioFirstSegmentDuration = 45056;
#  $audioSegmentDuration = 47104;
#  $audioTimeScale = 48000;
#}
#EU{
#  $videoSegmentDuration = 6144;
#  $videoTimeScale = 12800;
#  $audioFirstSegmentDuration = 16479;
#  $audioSegmentDuration = 22528;
#  $audioTimeScale = 48000;
#}

file_put_contents ( "timelog.txt" , "File: " . $latestFiles . ", latest video num: " . $latest_video_num . ", Audio num: " . $latest_audio_num . " \r\n", FILE_APPEND );

$videoPTO = ($latest_video_num - 1) * $videoSegmentDuration;
if($latest_audio_num >= 2)
  $audioPTO = ($latest_audio_num - 2) * $audioSegmentDuration + $audioFirstSegmentDuration;
else
  $audioPTO = 0;
  
if($videoPTO/$videoTimeScale < $audioPTO/$audioTimeScale)
{
  $timeOffset = $videoPTO/$videoTimeScale;
  $audioPTO = $timeOffset*$audioTimeScale;
}
else
{
  $timeOffset = $audioPTO/$audioTimeScale;
  $videoPTO = $timeOffset*$videoTimeScale;
}


#	$MPD = simplexml_load_file($DASHContent . "/" . $OriginalMPD);
#	if (!$MPD)
#		die("Failed loading XML file");

#    $dom_sxe = dom_import_simplexml($MPD);
#    if (!$dom_sxe) 
#    {
#        echo 'Error while converting XML';
#        exit;
#    }
    
#    $dom = new DOMDocument('1.0');
#    $dom_sxe = $dom->importNode($dom_sxe, true);
#    $dom_sxe = $dom->appendChild($dom_sxe);#

#    foreach ($dom->documentElement->childNodes as $node)
#    {
#        //echo $node->nodeName; // body
#        if($node->nodeName === 'Location')
#            $locationNode = $node;
#        if($node->nodeName === 'BaseURL')
#            $baseURLNode = $node;    
#        if($node->nodeName === 'Period')
#            $periodNode = $node;    
#    }
    #$periodNode->setAttribute("start","PT0S");
    #$periodNode->setAttribute("id","1");
    #if($baseURLNode)
    #    $periodNode->parentNode->insertBefore($baseURLNode,$periodNode);
    
    #if($locationNode)
    #    $periodNode->parentNode->insertBefore($locationNode,$periodNode);
    
#    $dom->save($DASHContent . "/" . $PatchedMPD);
if($timeOffset == 0 || $latest_video_num == 0)
    $timeOffset = $timeOffset;
$cmd=  "./UpdateMPD_ToSystemTime.sh ". $DASHContent . "/" . $OriginalMPD . " " . $AST_W3C . " " . $timeOffset . " " . $latest_video_num . " " . $latest_audio_num . " " . $videoPTO . " " . $audioPTO . " " . $DASHContent . "/" . $PatchedMPD;
exec($cmd);

#file_put_contents ( "timelog.txt" , $latestFiles , FILE_APPEND );
$micro_date = microtime();
$date_array = explode(" ",$micro_date);
$date = date("Y-m-d H:i:s",$date_array[1]);
file_put_contents ( "timelog.txt" , "Done:" . $date . $date_array[0] . " \r\n" , FILE_APPEND );
?>