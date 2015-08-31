<?php

/* 
Clean up processes
 */

exec("sudo killall flute");
unlink('../bin/socketServerReady.trig');

?>
