<?php

$currDir=dirname(__FILE__);
corsHeader();
header("Content-Type: application/xml");
$DASHContent=$currDir . "/";
$string = "<element><child>Hello World</child></element>";


$rret1 = file_exists ( $DASHContent . "MultiRate_Dynamic.mpd");
$rret = file_get_contents($DASHContent . "MultiRate_Dynamic.mpd");
file_put_contents ($DASHContent . "test.mpd" ,$rret);
$rret3 = file_get_contents($DASHContent . "test.mpd");
echo $rret;


/*Send out CORS header*/
function corsHeader()
{
	header("Access-Control-Allow-Headers: origin,range");
	header("Access-Control-Expose-Headers: Server,range");
	header("Access-Control-Allow-Methods: GET, HEAD, OPTIONS");
	header("Access-Control-Allow-Origin: *");
}
?>
