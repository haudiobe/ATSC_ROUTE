#!/bin/bash

#This script:
#	1- Converts MPD to live and sets Availability Start Time to NOW + Delay
#	2- After (Delay - x) seconds has passed, FLUTE sender is triggered.

#Define Directories
#DASHContent=ToS720p_0_5
#DASHContent2=Elysium720p_0_5
#DASHContent=ToSLC_0_5
#DASHContent2=ElysiumLC_0_5

if [ -f "Hotel/MultiRate.mpd" ]
then
	DASHContent=Hotel
else
	DASHContent=ToS
fi

if [ -f "Wave/MultiRate.mpd" ]
then
	DASHContent2=Wave
else
	DASHContent2=Sintel
fi

FLUTESender=.

#Variables
Delay=1.5					#AST will be set to NOW + Delay seconds	
Delay2=1.75
#x=10					#FLUTE receiver will be started after Delay - x seconds
bitRate=50000			#Bitrate in kb/s to be used in FLUTE Sender

fdtVid=fdt_Video.xml
fdtAud=fdt_Audio.xml

FLUTEVideoInput="FluteInput_Video.txt"
FLUTEAudioInput="FluteInput_Audio.txt"

sdp=SDP1.sdp
sdp2=SDP2.sdp					#SDP to be used by sender

sdp3=SDP3.sdp
sdp4=SDP4.sdp					#SDP to be used by sender	

encodingSymbolsPerPacket=1		#A value of zero indicates that different chunks of segment have different delay
								#and maximum transmission unit size is used (e.g. 1500 bytes per packet)

Log=Send_Log_Video.txt			#Log containing delays corresponding to FLUTE server
Log2=Send_Log_Audio.txt			#Log containing delays corresponding to FLUTE server

Log3=Send_Log_Video2.txt			#Log containing delays corresponding to FLUTE server
Log4=Send_Log_Audio2.txt			#Log containing delays corresponding to FLUTE server

[ $encodingSymbolsPerPacket -lt 0 ] && echo "The number of encoding symbols should be zero or greater" && exit 

#Convert MPD from static to dyanamic
echo "Converting MPD"

#Brackets are used to temporarilSimAdminy change working directory
./ConvertMPD.sh $DASHContent MultiRate.mpd $Delay
./ConvertMPD.sh $DASHContent2 MultiRate.mpd $Delay2

chmod 777 $DASHContent/*
chmod 777 $DASHContent2/*

echo "Done"

#Starting the FLUTE Sender After Sleep
#echo "Starting FLUTE Sender in $(($Delay - $x)) seconds"
#sleep $(($Delay - $x))

#Make sure that sudo route -n add -net  224.0.0.0 netmask 240.0.0.0 dev eth0 is run...Check results with route -n

#Kill any previous leftovers
killall flute_sender


#(cd $FLUTESender && ./flute -S -r:$bitRate -B:$DASHContent -Q -f:$fdtVid -m:224.1.1.1 -p:4000 -t:1 -v:4 -y:$videoSegDur -Y:$videoSegDur -J:$Log)
(cd $FLUTESender && ./flute_sender -S -r:$bitRate -B:$DASHContent -f:$DASHContent/$fdtVid -d:$sdp  -y:$DASHContent/$FLUTEVideoInput -Y:$encodingSymbolsPerPacket -J:$Log&)
(cd $FLUTESender && ./flute_sender -S -r:$bitRate -B:$DASHContent -f:$DASHContent/$fdtAud -d:$sdp2 -y:$DASHContent/$FLUTEAudioInput -Y:$encodingSymbolsPerPacket -J:$Log2&)

#Sending of second video
(cd $FLUTESender && ./flute_sender -S -r:$bitRate -B:$DASHContent2 -f:$DASHContent2/$fdtVid -d:$sdp3 -y:$DASHContent2/$FLUTEVideoInput -Y:$encodingSymbolsPerPacket -J:$Log3&)
(cd $FLUTESender && ./flute_sender -S -r:$bitRate -B:$DASHContent2 -f:$DASHContent2/$fdtAud -d:$sdp4 -y:$DASHContent2/$FLUTEAudioInput -Y:$encodingSymbolsPerPacket -J:$Log4 && fg)

