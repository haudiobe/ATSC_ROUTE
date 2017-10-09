<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
 
$OriginalMPD=$_GET['MPD'];
$PatchedMPD=$_GET['uMPD']; 
 
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

$doc->save("test.xml");

?>
