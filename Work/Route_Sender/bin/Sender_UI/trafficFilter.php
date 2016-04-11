<?php

ini_set('memory_limit','-1');//remove memory limit
exec("sudo tc qdisc delete dev eth0 root");
exec("sudo tc qdisc add dev eth0 root handle 1: cbq avpkt 1000 bandwidth 5000mbit");
exec("sudo tc class add dev eth0 parent 1: classid 1:1 cbq rate 35000kbit allot 1500 prio 5 bounded isolated");
exec("sudo tc filter add dev eth0 parent 1: protocol ip prio 16 u32 match ip dst 224.1.1.1 flowid 1:1");

echo "Done setting traffic pattern!";

?>