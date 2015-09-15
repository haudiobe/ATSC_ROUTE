<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

chdir("/var/www/html/ATSC_ROUTE/Work/Route_Sender/bin/");
$txt=  json_decode($_POST["time"]);
//$txt=$_POST("time");
echo $txt;
$myfile = fopen("AdTime.txt", "w");
fwrite($myfile, $txt);
fclose($myfile);