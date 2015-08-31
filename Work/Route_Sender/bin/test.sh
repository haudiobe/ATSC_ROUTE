#!/bin/bash

#Get CurrentTime
currTime=$(date --date="$1 seconds")
timenow=$(($(date +'%s * 1000000 + %-N / 1000')))
echo $timenow
ast=`awk -v timenow=$timenow -v delta=$1 'BEGIN { OFMT = "%.0f"; print timenow + delta*1000000 }'`
#echo $ast
astsec=`awk -v ast=$ast 'BEGIN { print int(ast/1000000) }'`
echo $astsec
astmillisec=`awk -v ast=$ast 'BEGIN { print int(ast/1000) - int(ast/1000000)*1000 }'`
echo "AST msec " $astmillisec
astfracsec=`awk -v ast=$ast 'BEGIN { printf "%.4d" , int(ast/100) - int(ast/1000000)*10000 }'`
echo "AST fracsec " $astfracsec
AST_NEW=$(date -u +"%Y-%m-%dT%T" -d @"$astsec")"."$astfracsec

echo "AST New: " $AST_NEW


#echo $currTime

#Get date in UTC (This is the time reference used by the DASH reference client
AST=$(date -u +"%Y-%m-%dT%T" -d "$currTime")
echo "AST:" $AST

#Convert CurrentTime to unix time. This is to be used later to determine when to send data chunks in FLUTE receiver
AST_UnixTime=$(($(date +%s%6N -d "$currTime")))
echo $AST_UnixTime

exit

MPD="MultiRate.mpd"
filename=$(basename $MPD)
extension="${filename##*.}"
filename="${filename%.*}"

dynamicMPDName=$filename"_Dynamic."$extension

#Get CurrentTime
currTime=$(date --date="$4 seconds")

#Get date in UTC (This is the time reference used by the DASH reference client
AST=$(date -u +"%Y-%m-%dT%T" -d "$currTime")

php ../StaticToDynamic.php MPD=$MPD uMPD=$dynamicMPDName AST=$AST"Z"
