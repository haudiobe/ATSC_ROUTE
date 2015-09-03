<?php
	chdir('/var/www/Work/Route_Sender/bin/');
	exec("sudo nice --20 ./Start2.sh 1000 > ServiceLog.txt &");
	exit;
?>