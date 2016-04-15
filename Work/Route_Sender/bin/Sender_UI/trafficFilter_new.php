<?php

$output=shell_exec("sudo ifconfig");
$findme="eth";
$pos = strpos($output, $findme);
if ($pos === false) {
    echo "The string '$findme' was not found in the string '$output'";
}else {
    $startpos=$pos;
    $endpos=strpos($output," ",$startpos);
    //echo $endpos;
    $netname = substr($output,$startpos,$endpos-$startpos);
    echo $netname;
} 

ini_set('memory_limit','-1');//remove memory limit
exec("sudo tc qdisc delete dev $netname root");
exec("sudo tc qdisc add dev $netname root handle 1: cbq avpkt 1000 bandwidth 5000mbit");
//exec("sudo tc class add dev $netname parent 1: classid 1:1 cbq rate 35000kbit allot 1500 prio 5 bounded isolated");
//exec("sudo tc filter add dev $netname parent 1: protocol ip prio 16 u32 match ip dst 224.1.1.1 flowid 1:1");

exec("sudo tc class add dev $netname parent 1: classid 1:1 cbq rate 16000kbit allot 1500 prio 5 bounded");
exec("sudo tc filter add dev $netname parent 1: protocol ip prio 16 u32 match ip dst 224.1.1.1 match ip dport 4001 0xffff flowid 1:1");

exec("sudo tc class add dev $netname parent 1: classid 1:2 cbq rate 1200kbit allot 1500 prio 5 bounded");
exec("sudo tc filter add dev $netname parent 1: protocol ip prio 16 u32 match ip dst 224.1.1.1 match ip dport 4002 0xffff flowid 1:2");

exec("sudo tc class add dev $netname parent 1: classid 1:3 cbq rate 16000kbit allot 1500 prio 5 bounded");
exec("sudo tc filter add dev $netname parent 1: protocol ip prio 16 u32 match ip dst 224.1.1.1 match ip dport 4003 0xffff flowid 1:3");

exec("sudo tc class add dev $netname parent 1: classid 1:4 cbq rate 12000kbit allot 1500 prio 5 bounded");
exec("sudo tc filter add dev $netname parent 1: protocol ip prio 16 u32 match ip dst 224.1.1.1 match ip dport 4004 0xffff flowid 1:4");

echo "Done setting traffic pattern!";

?>