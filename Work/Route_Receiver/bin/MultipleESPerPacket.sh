#!/bin/bash

#This script:
#	1- Converts MPD to live and sets Availability Start Time to NOW + Delay
#	2- After (Delay - x) seconds has passed, FLUTE sender is triggered.

#Define Directories
DASHContent=/home/elkhatib/Malek/FLUTE/mad_fcl_v1.7_src_ModifySending/mad_fcl_v1.7_linux_bin_Malek/bin/1b_1M_test
FLUTESender=/home/elkhatib/Malek/FLUTE/mad_fcl_v1.7_src_ModifySending/mad_fcl_v1.7_linux_bin_Malek/bin

#Variables
Delay=30					#AST will be set to NOW + Delay seconds	
x=28						#FLUTE receiver will be started after Delay - x seconds
bitRate=10000				#Bitrate in kb/s to be used in FLUTE Sender
#fdt=fdt_Complete.xml		#FDT to be used by sender
#fdtVid=fdt_Video_backup.xml
fdtVid=fdt_Video_OneSBN_MultipleESPerPacket.xml
#fdtVid=fdt_Video_OneSBN_Test.xml
#fdtVid=fdt_Video.xml
#fdtAud=fdt_Audio_backup.xml
fdtAud=fdt_Audio_NoFEC_MultipleESPerPacket.xml
sdp=SDP.sdp
sdp2=SDP2.sdp					#SDP to be used by sender
#videoSegDur=6000000			#Video Segment Duration
videoSegDur=2000000		#For testing purposes
audioSegDur=4000000			#Audio Segment Duration
#audioSegDur=1961979		#for testing purposes
encodingSymbolsPerPacket=1428
Log=Send_Log_Video.txt			#Log containing delays corresponding to FLUTE server
Log2=Send_Log_Audio.txt			#Log containing delays corresponding to FLUTE server

#Convert MPD from static to dyanamic
echo "Converting MPD"

#Brackets are used to temporarily change working directory
#(cd $DASHContent && ./ConvertMPD_Malek_CONVERTONLY.sh MultiRate.mpd 1 2 $Delay)
(cd $DASHContent && ./ConvertMPD_Malek_VideoONLY.sh MultiRate.mpd 1 $Delay)
echo "Done"

#Starting the FLUTE Sender After Sleep
echo "Starting FLUTE Sender in $(($Delay - $x)) seconds"
sleep $(($Delay - $x))

#(cd $FLUTESender && ./flute -S -r:$bitRate -B:$DASHContent -Q -f:$fdtVid -m:224.1.1.1 -p:4000 -t:1 -v:4 -y:$videoSegDur -Y:$videoSegDur -J:$Log)
(cd $FLUTESender && ./flute -S -r:$bitRate -B:$DASHContent -Q -f:$fdtVid -d:$sdp -v:4 -y:$videoSegDur -Y:$encodingSymbolsPerPacket -J:$Log)
#(cd $FLUTESender && ./flute -S -r:$bitRate -B:$DASHContent -f:$fdtAud -Q -d:$sdp2 -v:4 -y:$audioSegDur -Y:$encodingSymbolsPerPacket -J:$Log2)
