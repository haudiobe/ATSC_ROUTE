#!/bin/bash

#This script:
#	1- Converts MPD to live and sets Availability Start Time to NOW + Delay
#	2- After (Delay - x) seconds has passed, FLUTE sender is triggered.

#Define Directories
DASHContent=/home/nomor/Documents/Moraes/FLUTE/Software/sender_mad_fcl_v1.7_linux_bin_Malek/bin/1b_1M_test
FLUTESender=/home/nomor/Documents/Moraes/FLUTE/Software/sender_mad_fcl_v1.7_linux_bin_Malek/bin/

#Variables
Delay=30					#AST will be set to NOW + Delay seconds	
x=28							#FLUTE receiver will be started after Delay - x seconds
bitRate=10000				#Bitrate in kb/s to be used in FLUTE Sender
#fdt=fdt_One.xml		#FDT to be used by sender
fdtVid=fdt_Video.xml
fdtAud=fdt_Audio.xml
#fdtAud=fdt_Audio2.xml
#fdtVid=fdt_Video_backup.xml
#fdtAud=fdt_Audio_backup.xml
sdp=SDP.sdp					#SDP to be used by sender
sdp2=SDP2.sdp					#SDP to be used by sender
#videoSegDur=1960000			#Video Segment Duration
videoSegDur=2000000		#For testing purposes
#audioSegDur=1921979			#Audio Segment Duration
audioSegDur=1961979		#for testing purposes
Log=Send_Log_Vid.txt			#Log containing delays corresponding to FLUTE server
Log2=Send_Log_Aud.txt
#Convert MPD from static to dyanamic
echo "Converting MPD"

#Brackets are used to temporarily change working directory
(cd $DASHContent && ./ConvertMPD_Malek_CONVERTONLY.sh MultiRate.mpd 1 2 $Delay)
echo "Done"

#Starting the FLUTE Sender After Sleep
echo "Starting FLUTE Sender in $(($Delay - $x)) seconds"
sleep $(($Delay - $x))
#(cd $FLUTESender && ./flute -S -r:$bitRate -B:$DASHContent -f:$fdtVid -Q -d:$sdp -v:4 -y:$videoSegDur -Y:$videoSegDur -J:$Log )
(cd $FLUTESender && ./flute -S -r:$bitRate -B:$DASHContent -f:$fdtAud -d:$sdp2 -v:4 -y:$audioSegDur -Y:$audioSegDur -J:$Log2 )
