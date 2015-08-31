#!/bin/bash

#Seperate Audio and Video into two fdt files

#Declare Variables
numOfAudSeg=303					#Total number of audio segments
numOfVidSeg=297					#Total number of video segments
loopCount=0
toggle=0			#Used when we reach a point where two segments of same type needs to be sent before continuing with other type
					#This occurs at some point in time for type with shorter segment duration
  

#Generating list containing files to be sent with respective sizes in bytes
#It is assumed that files.txt would audio segments then video segments in ascending order.
#Moreover, number template is used to name video and audio segments
#files_ordered is populated with MPD and initialization segmetns. It will be updated later
#to contain audio and video segments in the order they are supposed to be sent
echo -e "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<FDT-Instance Expires=\"32511974400\"\n\tFEC-OTI-FEC-Encoding-ID=\"129\"\n\tFEC-OTI-FEC-Instance-ID=\"0\"\n\tFEC-OTI-Maximum-Source-Block-Length=\"64\"\n\tFEC-OTI-Encoding-Symbol-Length=\"1428\"\n\tFEC-OTI-Max-Number-of-Encoding-Symbols=\"96\">" > fdt_Video.xml
echo -e "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<FDT-Instance Expires=\"32511974400\"\n\tFEC-OTI-FEC-Encoding-ID=\"129\"\n\tFEC-OTI-FEC-Instance-ID=\"0\"\n\tFEC-OTI-Maximum-Source-Block-Length=\"64\"\n\tFEC-OTI-Encoding-Symbol-Length=\"1428\"\n\tFEC-OTI-Max-Number-of-Encoding-Symbols=\"96\">" > fdt_Audio.xml
TOI_Video=1
TOI_Audio=1

#ls -l -r *Dynamic.mpd *init* | awk -v id=$TOI_Video '{if ($5 !="" && $9 != "") {print "<File TOI=\""id"\"\n\tContent-Location=\"file:///"$9"\"\n\tContent-Length=\""$5"\"/>";id++}}' >> fdt_Video.xml
#ls -l --ignore=*mpd --ignore=*init* --ignore=*sh --ignore=*txt| awk '{if ($5 != "" && $9 != "") {print $9" "$5}}' | sort -V > files.txt

#awk -v vidFile='fdt_Video.xml' -v audFile='fdt_Audio.xml' -v id=$TOI -v idAud=$TOI_Audio '{if (index($0,"video")) {print "<File TOI=\""id"\"\n\tContent-Location=\"file:///"$1"\"\n\tContent-Length=\""$2"\"/>" >> vidFile; id++} else {print "<File TOI=\""idAud"\"\n\tContent-Location=\"file:///"$1"\"\n\tContent-Length=\""$2"\"/>" >> audFile;idAud++}}' files.txt

echo "</FDT-Instance>" >> fdt_Video.xml
echo "</FDT-Instance>" >> fdt_Audio.xml