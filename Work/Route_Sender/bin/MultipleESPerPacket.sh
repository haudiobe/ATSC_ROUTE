#!/bin/bash

#This script:
#	1- Converts MPD to live and sets Availability Start Time to NOW + Delay
#	2- After (Delay - x) seconds has passed, FLUTE sender is triggered.

#Define Directories
DASHContent=BBB
DASHContent2=ED
FLUTESender=.

#Variables
Delay=8					#AST will be set to NOW + Delay seconds	
Delay2=8.5
x=4					#FLUTE receiver will be started after Delay - x seconds
bitRate=10000			#Bitrate in kb/s to be used in FLUTE Sender

fdtVid=fdt_Video_OneSBN_repeat_init_MultiESPerPacket.xml
fdtAud=fdt_Audio_NoFEC_repeat_init_MultiESPerPacket.xml

fdtVid2=fdt_Video2_OneSBN_repeat_init_MultiESPerPacket.xml
fdtAud2=fdt_Audio2_NoFEC_repeat_init_MultiESPerPacket.xml

sdp=SDP.sdp
sdp2=SDP2.sdp					#SDP to be used by sender

sdp3=SDP3.sdp
sdp4=SDP4.sdp					#SDP to be used by sender

videoSegDur=1000000		
audioSegDur=980104

video2SegDur=999063		
audio2SegDur=980292	

FLUTEVideoInput="FluteInput_Video.txt"
FLUTEAudioInput="FluteInput_Audio.txt"

FLUTEVideo2Input="FluteInput_Video2.txt"
FLUTEAudio2Input="FluteInput_Audio2.txt"

encodingSymbolsPerPacket=0

Log=Send_Log_Video.txt			#Log containing delays corresponding to FLUTE server
Log2=Send_Log_Audio.txt			#Log containing delays corresponding to FLUTE server

Log3=Send_Log_Video2.txt			#Log containing delays corresponding to FLUTE server
Log4=Send_Log_Audio2.txt			#Log containing delays corresponding to FLUTE server

[ $encodingSymbolsPerPacket -lt 0 ] && echo "The number of encoding symbols should be zero or greater" && exit 

#Convert MPD from static to dyanamic
echo "Converting MPD"

#Brackets are used to temporarily change working directory
(cd $DASHContent && ./ConvertMPD_Malek_CONVERTONLY.sh MultiRate.mpd 1 2 $Delay $encodingSymbolsPerPacket $videoSegDur $audioSegDur $FLUTEVideoInput $FLUTEAudioInput)
(cd $DASHContent2 && ./ConvertMPD_Malek_CONVERTONLY.sh MultiRate.mpd 1 2 $Delay2 $encodingSymbolsPerPacket $video2SegDur $audio2SegDur $FLUTEVideo2Input $FLUTEAudio2Input)
#(cd $DASHContent && ./ConvertMPD_Malek_VideoONLY.sh MultiRate.mpd 1 $Delay)

chmod 777 BBB/*
chmod 777 ED/*

mv $DASHContent/$FLUTEVideoInput $FLUTESender
mv $DASHContent/$FLUTEAudioInput $FLUTESender
mv $DASHContent2/$FLUTEVideo2Input $FLUTESender
mv $DASHContent2/$FLUTEAudio2Input $FLUTESender
echo "Done"

#Starting the FLUTE Sender After Sleep
echo "Starting FLUTE Sender in $(($Delay - $x)) seconds"
sleep $(($Delay - $x))

#(cd $FLUTESender && ./flute -S -r:$bitRate -B:$DASHContent -Q -f:$fdtVid -m:224.1.1.1 -p:4000 -t:1 -v:4 -y:$videoSegDur -Y:$videoSegDur -J:$Log)
(cd $FLUTESender && ./flute -S -r:$bitRate -B:$DASHContent -f:$fdtVid -d:$sdp  -y:$FLUTEVideoInput -Y:$encodingSymbolsPerPacket -J:$Log&)
(cd $FLUTESender && ./flute -S -r:$bitRate -B:$DASHContent -f:$fdtAud -d:$sdp2 -y:$FLUTEAudioInput -Y:$encodingSymbolsPerPacket -J:$Log2&)


#Sending of second video
(cd $FLUTESender && ./flute -S -r:$bitRate -B:$DASHContent2 -f:$fdtVid2 -d:$sdp3  -y:$FLUTEVideo2Input -Y:$encodingSymbolsPerPacket -J:$Log3&)
(cd $FLUTESender && ./flute -S -r:$bitRate -B:$DASHContent2 -f:$fdtAud2 -d:$sdp4 -y:$FLUTEAudio2Input -Y:$encodingSymbolsPerPacket -J:$Log4 && fg)
