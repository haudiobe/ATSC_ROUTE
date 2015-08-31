#!/bin/bash

#This script takes original FDT as input and generates two FDT files having the following properties:
#	1- One FDT lists all video objects and the other lists all audio objects
#	2- For each object, maximum source block length and encoding symbol length are calculated so that we would have N blocks per segment.
#	The blocks are used to simulate partial sending of segments. For example if we have two second segments and 10 blocks per segment then
#	we simulate sending 1 block every 200 msec (i.e. 200 msec of content) 

#Declare Variables
numOfBlocks = 10				#This determines size of segment fractions to send at a time
encodingSym = 1428				#Choose the maximum length of encoding symbol 
numOfAudSeg=303					#Total number of audio segments
numOfVidSeg=297					#Total number of video segments
loopCount=0
toggle=0			#Used when we reach a point where two segments of same type needs to be sent before continuing with other type
					#This occurs at some point in time for type with shorter segment duration
  

echo -e "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<FDT-Instance Expires=\"32511974400\"\n\tFEC-OTI-FEC-Encoding-ID=\"129\"\n\tFEC-OTI-FEC-Instance-ID=\"0\"\n\tFEC-OTI-Maximum-Source-Block-Length=\"64\"\n\tFEC-OTI-Encoding-Symbol-Length=\"1428\"\n\tFEC-OTI-Max-Number-of-Encoding-Symbols=\"96\">" > fdt_Video.xml
echo -e "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<FDT-Instance Expires=\"32511974400\"\n\tFEC-OTI-FEC-Encoding-ID=\"129\"\n\tFEC-OTI-FEC-Instance-ID=\"0\"\n\tFEC-OTI-Maximum-Source-Block-Length=\"64\"\n\tFEC-OTI-Encoding-Symbol-Length=\"1428\"\n\tFEC-OTI-Max-Number-of-Encoding-Symbols=\"96\">" > fdt_Audio.xml
TOI_Video=1
TOI_Audio=1

ls -l -r *Dynamic.mpd | awk -v id=$TOI_Video '{if ($5 !="" && $9 != "") {print "<File TOI=\""id"\"\n\tContent-Location=\"file:///"$9"\"\n\tContent-Length=\""$5"\"/>";id++}}' >> fdt_Video.xml
ls -l *init* | awk '{if ($5 != "" && $9 != "") {print $9" "$5}}' | sort -V > files.txt
ls -l --ignore=*mpd --ignore=*init* --ignore=*sh --ignore=*txt| awk -v N=$numOfBlocks -v maxE=$encodingSym '{if ($5 != "" && $9 != "") {EB=int($5/N+0.5); if (EB < maxE) {E=EB;B=1};print $9" "$5 }}' | sort -V >> files.txt

awk -v vidFile='fdt_Video.xml' -v audFile='fdt_Audio.xml' -v id=$(($TOI_Video+1)) -v idAud=$TOI_Audio '{if (index($0,"BBB_720_1M_video")) {print "<File TOI=\""id"\"\n\tContent-Location=\"file:///"$1"\"\n\tContent-Length=\""$2"\"/>" >> vidFile; id++} else if (index($0,"BBB_32k")){print "<File TOI=\""idAud"\"\n\tContent-Location=\"file:///"$1"\"\n\tContent-Length=\""$2"\"/>" >> audFile;idAud++}}' files.txt

echo "</FDT-Instance>" >> fdt_Video.xml
echo "</FDT-Instance>" >> fdt_Audio.xml