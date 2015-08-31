#!/bin/bash

#This script:
#	1- Fetches User Service Description Files which are the Media Presentation Description (MPD), initialization audio and video segments, and Service Description Protocol (SDP) file
#	2- The FLUTE receiver is activated and put in automatic mode using received SDP
#	3- Reference Client is launched with google chrome and received MPD

#Define Paths
Client=http://10.4.192.60
DASHContent=/home/nomor/Documents/Moraes/Video_test
FLUTEReceiver=/home/nomor/Documents/Moraes/FLUTE/Software/mad_fcl_v1.7_linux_bin_Malek/bin
FLUTESender=root@10.4.127.99:/home/elkhatib/Malek/FLUTE/mad_fcl_v1.7_linux_bin_Malek/bin
USDFiles=$FLUTESender/1b_1M_test/\{MultiRate_Dynamic.mpd,BBB_720_1M_video_init.mp4,BBB_32k_init.mp4,../SDP.sdp,../SDP2.sdp\}
RefClient=$Client/dash.js-development_Original/UptoDate_WithMetrics_WithSourceBuffer/dash.js/?mpd=$Client/Video_test/MultiRate_Dynamic.mpd
HTMLLocalStorage="/home/nomor/.config/google-chrome-unstable/Default/Local Storage/"
LogsFolder=$FLUTEReceiver/Logs

#Variables
sdp=SDP.sdp			#SDP to be used by sender
sdp2=SDP2.sdp
Log=Rcv_Log_Video.txt			#Log containing delays corresponding to FLUTE receiver
Log2=Rcv_Log_Audio.txt
SenderLog=Send_Log_Video.txt		#Log containing delays and stored at server side
videoData=video_Data.txt	#Final video log file containing all recorded time instants
encodingSymbolsPerPacket=0	#For Receiver, Only a value of zero makes a difference. Otherwise, it is ignored 
							#This means that more than one encoding symbol is included packet. This could be varying

#Clear HTML5 Local Storage
if [ -e "$HTMLLocalStorage"*${Client:7}*localstorage-journal -o -e "$HTMLLocalStorage"*${Client:7}*localstorage ]; then
  echo "Delete Old HTML Local Storage"
  rm "$HTMLLocalStorage"*${Client:7}*
fi

echo "Entering Ctrl+C after playback has been initiated would stop it and process any available logs"

#Initialize DASHContent Folder
if [ "$(ls -A $DASHContent)" ]; then
  rm $DASHContent/*
fi

#Fetching USD FILES
echo "Fetching USD files"

#rsync or scp could be used
rsync $USDFiles $DASHContent
chmod 777 $DASHContent/*

#Move sdp to flute receiver directory
mv $DASHContent/$sdp $FLUTEReceiver
mv $DASHContent/$sdp2 $FLUTEReceiver

#Brackets are used to temporarily change working directory
echo "Starting FLUTE Receiver"
(cd $FLUTEReceiver && ./flute -A -B:$DASHContent -Q -v:0 -d:$sdp -Y:$encodingSymbolsPerPacket -J:$Log&)
(cd $FLUTEReceiver && ./flute -A -B:$DASHContent -Q -v:0 -d:$sdp2 -Y:$encodingSymbolsPerPacket -J:$Log2&)
#Sleep is used to make sure that segments are received. Should be removed after optimzation of sending processes and setting of Availability Start Time (AST)
sleep 6
#Launching DASH reference client using google chrome
google-chrome-unstable $RefClient 2> /dev/null

#When the user presses Ctrl+C to exit, the processing of log files is initiated. Note that it might be necessary
#to enter Ctrl+C multiple times to exit previously triggered processes.

#Processing Log Files from FLUTE server, FLUTE receiver and DASH client
echo "Processing Logs"

echo "Fetching Sender Logs"
rsync $FLUTESender/$SenderLog $LogsFolder

echo "Fetching Reference Client Logs"
cp "$HTMLLocalStorage"*${Client:7}*localstorage $LogsFolder

echo "Moving Receiver Logs"
cp $Log $LogsFolder


(cd $LogsFolder && ./OrganizeData.sh $SenderLog $Log *localstorage $videoData)

echo "Moving Final Video Log File to 10.4.127.99 for Matlab plotting"

scp $LogsFolder"/"$videoData $FLUTESender"/Logs"

cat

