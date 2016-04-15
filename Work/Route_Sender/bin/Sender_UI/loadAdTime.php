<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

chdir("../");
$filename = "UIConfig.txt";
$myfile = fopen($filename, "r");
$content = fread($myfile, filesize($filename));
echo $content;
fclose($myfile);