#!/bin/bash

#Sepeate Audio and Video into two fdt files

#Declare Variables
numOfAudSeg=1581					#Total number of audio segments
numOfVidSeg=1469					#Total number of video segments
loopCount=0
toggle=0			#Used when we reach a point where two segments of same type needs to be sent before continuing with other type
					#This occurs at some point in time for type with shorter segment duration
  

#Generating list containing files to be sent with respective sizes in bytes
#It is assumed that files.txt would audio segments then video segments in ascending order.
#Moreover, number template is used to name video and audio segments
#files_ordered is populated with MPD and initialization segmetns. It will be updated later
#to contain audio and video segments in the order they are supposed to be sent
echo -e "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<EFDT-Instance TSI=\"1\" IDRef=\"file:///efdt_Video.xml\">" > efdt_Video.xml
echo -e "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<EFDT-Instance TSI=\"2\" IDRef=\"file:///efdt_Audio.xml\">" > efdt_Audio.xml
echo -e "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<EFDT-Instance TSI=\"2\" IDRef=\"file:///efdt_Audio.xml\">" > efdt_MPD.xml

echo -e "<FDT-Parameters Expires=\"32511974400\" FEC-OTI-FEC-Encoding-ID=\"0\" FEC-OTI-Maximum-Source-Block-Length=\"5000\" FEC-OTI-Encoding-Symbol-Length=\"1428\"/>" >> efdt_Video.xml
echo -e "<FDT-Parameters Expires=\"32511974400\" FEC-OTI-FEC-Encoding-ID=\"0\" FEC-OTI-Maximum-Source-Block-Length=\"5000\" FEC-OTI-Encoding-Symbol-Length=\"1428\"/>" >> efdt_Audio.xml
echo -e "<FDT-Parameters Expires=\"32511974400\" FEC-OTI-FEC-Encoding-ID=\"0\" FEC-OTI-Maximum-Source-Block-Length=\"5000\" FEC-OTI-Encoding-Symbol-Length=\"1428\"/>" >> efdt_MPD.xml
TOI_Video=1
TOI_Audio=1
TOI_MPD=1

#####USE THIS BLOCK IF INIT AND MPD ARE NOT REPEATED IN THE FDT
#ls -l -r *Dynamic.mpd | awk -v id=$TOI_Video '{if ($5 !="" && $9 != "") {print "<File TOI=\""id"\"\n\tContent-Location=\"file:///"$9"\"\n\tContent-Length=\""$5"\"/>";id++}}' >> fdt_Audio.xml
#ls -l *init* | awk '{if ($5 != "" && $9 != "") {print $9" "$5}}' | sort -V > files.txt
#ls -l --ignore=*mpd --ignore=*init* --ignore=*sh --ignore=*txt| awk '{if ($5 != "" && $9 != "") {print $9" "$5}}' | sort -V >> files.txt
#awk -v vidFile='fdt_Video.xml' -v audFile='fdt_Audio.xml' -v id=$(($TOI_Video+1)) -v idAud=$TOI_Audio '{if (index($0,"ED_720_1M_MPEG2_video")) {print "<File TOI=\""id"\"\n\tContent-Location=\"file:///"$1"\"\n\tContent-Length=\""$2"\"/>" >> vidFile; id++} else if (index($0,"ED_MPEG2_32k")){print "<File TOI=\""idAud"\"\n\tContent-Location=\"file:///"$1"\"\n\tContent-Length=\""$2"\"/>" >> audFile;idAud++}}' files.txt

#####USE THIS BLOCK IF INIT AND MPD ARE REPEATED IN THE FDT
#ls -l -r *Dynamic.mpd | awk -v id=$TOI_Video '{if ($5 !="" && $9 != "") {print "<File TOI=\""id"\"\n\tContent-Location=\"file:///"$9"\"\n\tContent-Length=\""$5"\"/>";id++}}' >> fdt_Audio.xml
#ls -l *init* | awk '{if ($5 != "" && $9 != "") {print $9" "$5}}' | sort -V > files.txt
ls -l --ignore=*mpd --ignore=*init* --ignore=*sh --ignore=*txt --ignore=*~| awk '{if ($5 != "" && $9 != "") {print $9" "$5}}' | sort -V > files.txt
#awk -v vidFile='fdt_Video.xml' -v audFile='fdt_Audio.xml' -v id=$(($TOI_Video)) -v idAud=$(($TOI_Audio + 1)) '{if (index($0,"ED_720_1M_MPEG2_video")) {print "<File TOI=\""id"\"\n\tContent-Location=\"file:///"$1"\"\n\tContent-Length=\""$2"\"/>" >> vidFile; id++} else if (index($0,"ED_MPEG2_32k")){print "<File TOI=\""idAud"\"\n\tContent-Location=\"file:///"$1"\"\n\tContent-Length=\""$2"\"/>" >> audFile;idAud++}}' files.txt

a=$(ls -l -r *Dynamic.mpd | awk '{print $5" "$9}')
mpd=($a)

a=$(ls -l -r video_8M_init* | awk '{print $5" "$9}')
vidInit=($a)

a=$(ls -l -r audio_64k_init* | awk '{print $5" "$9}')
audInit=($a)

a=$(ls -l -r *S-TSID.xml | awk '{print $5" "$9}')
stsid=($a)

awk -v mpdLen=${mpd[0]} -v mpdNam=${mpd[1]} -v vidLen=${vidInit[0]} -v vidNam=${vidInit[1]} -v audLen=${audInit[0]} -v audNam=${audInit[1]} -v vidFile='efdt_Video.xml' -v audFile='efdt_Audio.xml' -v mpdFile='efdt_MPD.xml' -v stsidLen=${stsid[0]} -v stsidNam=${stsid[1]} -v id=$(($TOI_Video)) -v idAud=$(($TOI_Audio)) -v idMPD=$(($TOI_MPD)) '{if (index($0,"video_8M_")) {print "<File TOI=\""id"\" Content-Location=\"file:///"vidNam"\" Content-Length=\""vidLen"\"/>" >> vidFile; id++;print "<File TOI=\""id"\" Content-Location=\"file:///"$1"\" Content-Length=\""$2"\"/>" >> vidFile; id++} else if (index($0,"audio_64k")){print "<File TOI=\""idMPD"\" Content-Location=\"file:///"mpdNam"\" Content-Length=\""mpdLen"\"/>" >> mpdFile;idMPD++; print "<File TOI=\""idMPD"\" Content-Location=\"file:///"stsidNam"\" Content-Length=\""stsidLen"\"/>" >> mpdFile;idMPD++; print "<File TOI=\""idAud"\" Content-Location=\"file:///"audNam"\" Content-Length=\""audLen"\"/>" >> audFile;idAud++;print "<File TOI=\""idAud"\" Content-Location=\"file:///"$1"\" Content-Length=\""$2"\"/>" >> audFile;idAud++}}' files.txt


echo "</EFDT-Instance>" >> efdt_Video.xml
echo "</EFDT-Instance>" >> efdt_Audio.xml
echo "</EFDT-Instance>" >> efdt_MPD.xml
