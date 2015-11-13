<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
 

header('Content-Type: text/event-stream');
// recommended to prevent caching of event data.
header('Cache-Control: no-cache');

exec("sudo killall flute_sender");
exec("sudo killall Start2.sh");
exec("sudo killall LoopService.sh");

 $output2=shell_exec("sudo ifconfig");
 
 //echo $output2;
 //echo PHP_EOL;
 $index = 0;
 $ip = array();
 $pos = 0;
 
 while(true)
 {
	 $findme="inet addr";
	 $pos = strpos($output2, $findme, $pos + strlen($findme));
	 if($pos === FALSE)
		 break;
	 //echo $pos, PHP_EOL;
	 //echo PHP_EOL;
	 $startpos=$pos+10;
	 $endpos=strpos($output2," ",$startpos);
	 //echo $endpos;
	 $thisIP = substr($output2,$startpos,$endpos-$startpos);
	 if($thisIP === "127.0.0.1")
	 {
		$ip[$index]=$thisIP;
		$index = $index + 1;
	 }
 }
 echo json_encode($ip);
// echo "done";
//var_dump($output);
//echo exec('whoami');
//echo $command;
?>