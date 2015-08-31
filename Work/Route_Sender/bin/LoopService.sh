#!/bin/bash

#cd /var/www/html/Work/Route_Sender/bin/
echo "" > ServiceLog.txt
count=1;

segmentDur=500;

if [ "$#" -gt 0 ] && [ "$1" -eq 1000 ]
then
   segmentDur=1000;
fi

while true
do

    sudo nice --20 ./Start2.sh $segmentDur &>> ServiceLog.txt &
    echo -n "Looping, iteration: " $count ", started at: "
    date
    count=`expr $count + 1`
    wait
done
