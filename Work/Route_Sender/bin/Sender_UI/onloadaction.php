<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
header('Content-Type: text/event-stream');
// recommended to prevent caching of event data.
header('Cache-Control: no-cache'); 

$command="sudo killall flute_sender";
$output=array();
exec($command,$output);
 //$ip=$_SERVER['SERVER_ADDR'];
 $output2=shell_exec(ifconfig);
 //echo $output2;
 //echo PHP_EOL;
 $findme="inet addr";
 $pos = strpos($output2, $findme);
 //echo $pos, PHP_EOL;
 //echo PHP_EOL;
 $startpos=$pos+10;
 $endpos=strpos($output2," ",$startpos);
 //echo $endpos;
 $ip=substr($output2,$startpos,$endpos-$startpos);
 //echo $ip;
 $temp="data: ". json_encode($ip);
 // $d = array('ip' => $ip);
 echo json_encode($ip);
// echo "done";
//var_dump($output);
//echo exec('whoami');
//echo $command;
?>