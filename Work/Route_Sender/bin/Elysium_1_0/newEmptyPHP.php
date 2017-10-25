<?php
error_reporting(null);
$currDir=dirname(__FILE__);
header('Content-Type: text/html; charset=ISO-8859-1');
$DASHContent=$currDir . "/";
$string = "<element><child>Hello World</child></element>";


$rret1 = file_exists ( $DASHContent . "MultiRate_Dynamic.mpd");
$rret = file_get_contents($DASHContent . "MultiRate_Dynamic.mpd");
file_put_contents ($DASHContent . "test.mpd" ,$rret);
$rret3 = file_get_contents($DASHContent . "test.mpd");
echo $rret;
exit;
?>
