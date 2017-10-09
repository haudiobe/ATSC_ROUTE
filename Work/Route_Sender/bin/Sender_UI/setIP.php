<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

chdir("../");
//$session_id = json_decode($_POST['sessionid']);
exec("sudo pkill -f sender.py");
$ip=json_decode($_POST['ip']);;
echo $ip;
exec("sudo python SLT_signalling/genSLT.py " . $ip);
exec("sudo python SLT_signalling/sender.py > /dev/null 2>&1 &");
// pkill is an utility. We kill the previous SLT signalling sender.
// The dev null part is to make the python script execute in the background.
// http://stackoverflow.com/questions/25619229/how-to-make-python-script-executed-in-background

// Earlier this command was there to generated sdp, now we remove this. 
//$command= "sudo ../../Config/gensdp.sh ".$ip;
//$output=array();
///exec($command,$output);
//var_dump($output);

echo "done";
?>
