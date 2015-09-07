<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
header('Content-Type: text/event-stream');
// recommended to prevent caching of event data.
header('Cache-Control: no-cache'); 

function send_message( $progress) {
    $d = array('progress' => $progress);
      

    echo "data: " . json_encode($d) . PHP_EOL;
    echo PHP_EOL;
      
    ob_flush();
    flush();
}
  
  

for($i = 1; $i <= 730; $i++) {
    send_message($i); 
 
    sleep(1);
}

?>