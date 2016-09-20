<?php
if (isset($argv[0])) {
    echo $argv[0]. PHP_EOL;
    parse_str(implode('&', array_slice($argv, 1)), $_GET);
}
$ip=$_GET['IP'];
$DASHContent=$_GET['DASHContent'];
$Channel=$_GET['Channel'];

// Start create S-TSID.xml file
$doc = new DOMDocument('1.0');
// we want a nice output
$doc->formatOutput = true;
$root = $doc->createElement('S-TSID');
$root = $doc->appendChild($root);

$RS = $doc->createElement('RS');
$RS = $root->appendChild($RS);
$RS->setAttribute("sIpAddr",$ip);
$RS->setAttribute("dIpAddr",$ip);
$RS->setAttribute("dport","4000");
//Useless for now, just setting a dummy port number.

$LS1 = $doc->createElement('LS');
$LS1 = $RS->appendChild($LS1);
$LS1->setAttribute("id",1);
$LS1->setAttribute("tsi",(string)($Channel-1)*2+1);

$LS2 = $doc->createElement('LS');
$LS2 = $RS->appendChild($LS2);
$LS2->setAttribute("id","2");
$LS2->setAttribute("tsi",(string)($Channel-1)*2+2);

$doc->save($DASHContent . "/S-TSID.xml");
// End create S-TSID.xml file


// Start create usbd.xml file
$doc1 = new DOMDocument('1.0');
// we want a nice output
$doc1->formatOutput = true;
$root = $doc1->createElement('bundleDescription');
$root = $doc1->appendChild($root);

$userServiceDescription = $doc1->createElement('userServiceDescription');
$userServiceDescription = $root->appendChild($userServiceDescription);
$userServiceDescription->setAttribute("fullMPDUri","MultiRate_Dynamic.mpd");
$userServiceDescription->setAttribute("sTSIDUri","S-TSID.xml");

$name = $doc1->createElement('name');
$name = $userServiceDescription->appendChild($name);
$name->setAttribute("lang","eng");

$doc1->save($DASHContent . "/usbd.xml");
// End create USBD.xml file

?>