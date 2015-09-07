<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

chdir("/var/www/html/ATSC_ROUTE/Work/Route_Sender/bin/");
//$session_id = json_decode($_POST['sessionid']);

$ip=json_decode($_POST['ip']);;
echo $ip;
$command= "sudo ../../Config/gensdp.sh ".$ip;
$output=array();
exec($command,$output);
//var_dump($output);

echo "done";
?>
