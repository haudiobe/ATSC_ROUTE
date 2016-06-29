#!/bin/bash

for i in 1 2 3 4 5 6
do

tsi=`expr 0 + $i`
port=`expr 4000 + $i`
#port=4000

echo "v=0
o=root 2890844526 2890842807 IN IP4 $1
s=DASH
t=0 32511974400
a=source-filter: incl IN IP4 * $1
a=flute-tsi:$tsi
a=flute-ch:1
m=application $port FLUTE/UDP 0
c=IN IP4 224.1.1.1/$port" > SDP$i.sdp

done