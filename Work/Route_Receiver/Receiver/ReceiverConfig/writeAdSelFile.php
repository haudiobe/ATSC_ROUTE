<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
chdir("/var/www/html/ATSC_ROUTE/Work/Route_Receiver/bin/");
$myfile=fopen("AdSelection", "w");
$num=$_POST["num"];
fwrite($myfile, $num);
fclose($myfile);
?>
