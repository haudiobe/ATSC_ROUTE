#!/bin/bash

#while [ ! -f $1 ]
#do
#  sleep 0.01
#done

#Get CurrentTime
currTime=$(date --date="$2 seconds")

#Get date in UTC (This is the time reference used by the DASH reference client
AST=$(date -u +"%Y-%m-%dT%T" -d "$currTime")
#echo $AST

sudo sed "s/availabilityStartTime=\".*\" timeShiftBufferDepth/availabilityStartTime=\"${AST}Z\" timeShiftBufferDepth/g" $1 > $3

