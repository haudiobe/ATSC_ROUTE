<?php
$micro_date = microtime();
$date_array = explode(" ",$micro_date);
$date = date("Y-m-d H:i:s",$date_array[1]);

ini_set('memory_limit','-1');//remove memory limit

/* 
Main script for starting flure reception and MPD re-writing
 */

$buffer = $_REQUEST['buffer'];

file_put_contents ( "buffer.mp4" , $buffer , FILE_APPEND );

?>
