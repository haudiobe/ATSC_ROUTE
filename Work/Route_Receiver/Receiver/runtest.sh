#!/bin/bash
cd /var/www/html/Work/Route_Sender/bin/
echo "" > ServiceLog.txt
sudo nice --20 ./Start2.sh 1000 &>> ServiceLog.txt &

sleep 5

xdg-open "http://localhost/Work/Route_Receiver/Receiver/"

wait