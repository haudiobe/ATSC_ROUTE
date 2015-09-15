<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
header('Content-Type: text/event-stream');
// recommended to prevent caching of event data.
header('Cache-Control: no-cache'); 

chdir("/var/www/html/ATSC_ROUTE/Work/Route_Sender/bin/");
$command="sudo nice --20 ./Start2.sh 1000 > ServiceLog.txt &";
echo $command;
$output=array();
exec($command,$output);
//var_dump($output);


echo "Done";

?>