<?php
$currDir=dirname(__FILE__);
$DASHContentBase="DASH_Content";
$DASHContentDir=$DASHContentBase . (string)1;
$DASHContent=$currDir . "/" . $DASHContentDir;

$initAudio = "*audio_64k_init.mp4";
$initVideo = "*video_8M_init.mp4";
$segTemplateAudio = "*audio_64k_*.mp4";
$segTemplateVideo = "*video_8M_*.mp4";

echo implode(", ",(glob($DASHContent.'/'.$initAudio))). "<br />";
echo implode(", ",(glob($DASHContent.'/'.$initVideo))). "<br />";
echo implode(", ",(array_diff(glob($DASHContent.'/'.$segTemplateAudio), array($DASHContent.'/'.$initAudio)))). "<br />";
echo implode(", ",(array_diff(glob($DASHContent.'/'.$segTemplateVideo), array($DASHContent.'/'.$initVideo)))). "<br />";

$res = "xx";
if(!array_diff(glob($DASHContent.'/'.$segTemplateVideo), array($DASHContent.'/'.$initVideo)))
	$res = "true";
else
	$res = "false";
echo "idiot: ". $res . "; video count: ". count(glob($DASHContent.'/'.$segTemplateVideo)). "; Audio Count: ". count(glob($DASHContent.'/'.$segTemplateAudio))."<br />";
exit;

?>