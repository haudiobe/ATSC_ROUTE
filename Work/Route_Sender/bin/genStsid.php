<?php
if (isset($argv[0])) {
    echo $argv[0]. PHP_EOL;
    parse_str(implode('&', array_slice($argv, 1)), $_GET);
}
$ip=$_GET['IP'];
$DASHContent=$_GET['DASHContent'];

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
$LS1->setAttribute("id","1");
$LS1->setAttribute("tsi","1");

$LS2 = $doc->createElement('LS');
$LS2 = $RS->appendChild($LS2);
$LS2->setAttribute("id","2");
$LS2->setAttribute("tsi","2");

$doc->save($DASHContent . "/S-TSID.xml");

?>
