#!/bin/bash

#This script:
#	1- Converts MPD to live and sets Availability Start Time to NOW + Delay
#	2- After (Delay - x) seconds has passed, FLUTE sender is triggered.

#Define Directories
DASHContent=/home/elkhatib/Malek/FLUTE/mad_fcl_v1.7_linux_bin_Malek/bin/1b_1M_test
FLUTESender=/home/elkhatib/Malek/FLUTE//mad_fcl_v1.7_linux_bin_Malek/bin

#Variables
Delay=30					#AST will be set to NOW + Delay seconds	
x=6					#FLUTE receiver will be started after Delay - x seconds
bitRate=100000				#Bitrate in kb/s to be used in FLUTE Sender

fdtVid=fdt_Video_OneSBN.xml
fdtAud=fdt_Audio_NoFEC.xml

sdp=SDP.sdp
sdp2=SDP2.sdp					#SDP to be used by sender

videoSegDur=2000000		
audioSegDur=1961979		

encodingSymbolsPerPacket=1		#A value of zero indicates that different chunks of segment have different delay
								#and maximum transmission unit size is used (e.g. 1500 bytes per packet)

FLUTEVideoInput="FluteInput_Video.txt"
FLUTEAudioInput="FluteInput_Audio.txt"

Log=Send_Log_Video.txt			#Log containing delays corresponding to FLUTE server
Log2=Send_Log_Audio.txt			#Log containing delays corresponding to FLUTE server

[ $encodingSymbolsPerPacket -lt 0 ] && echo "The number of encoding symbols should be zero or greater" && exit 

#Convert MPD from static to dyanamic
echo "Converting MPD"

#Brackets are used to temporarily change working directory
(cd $DASHContent && ./ConvertMPD_Malek_CONVERTONLY.sh MultiRate.mpd 1 2 $Delay $encodingSymbolsPerPacket $videoSegDur $audioSegDur $FLUTEVideoInput $FLUTEAudioInput)
#
chmod 777 /home/elkhatib/Malek/FLUTE/mad_fcl_v1.7_linux_bin_Malek/bin/1b_1M_test/*
#(cd $DASHContent && ./ConvertMPD_Malek_VideoONLY.sh MultiRate.mpd 1 $Delay)
mv $DASHContent/$FLUTEVideoInput $FLUTESender
mv $DASHContent/$FLUTEAudioInput $FLUTESender
echo "Done"

#Starting the FLUTE Sender After Sleep
echo "Starting FLUTE Sender in $(($Delay - $x)) seconds"
sleep $(($Delay - $x))

#(cd $FLUTESender && ./flute -S -r:$bitRate -B:$DASHContent -Q -f:$fdtVid -m:224.1.1.1 -p:4000 -t:1 -v:4 -y:$videoSegDur -Y:$videoSegDur -J:$Log)
(cd $FLUTESender && ./flute -S -r:$bitRate -B:$DASHContent -v:4 -f:$fdtVid -d:$sdp  -y:$FLUTEVideoInput -Y:$encodingSymbolsPerPacket -J:$Log&)
(cd $FLUTESender && ./flute -S -r:$bitRate -B:$DASHContent -v:4 -f:$fdtAud -d:$sdp2 -y:$FLUTEAudioInput -Y:$encodingSymbolsPerPacket -J:$Log2)
