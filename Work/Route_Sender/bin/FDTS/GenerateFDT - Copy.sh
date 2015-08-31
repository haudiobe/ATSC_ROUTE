#!/bin/bash

#This script is used to generate the fdt.xml (without MD5)to be used by the server:
#	1- At first we list all the files with corresponding sizes in bytes
#	2- we need to know the order in which the segments are to be sent.
#	   Some calculation is needed since audio and video segment durations might be different

#Declare Variables
numOfAudSeg=303					#Total number of audio segments
audioDelay=1921979				#This is audio segment duration in microseconds
#audioDelay=1960000				#For Testing
numOfVidSeg=297					#Total number of video segments
videoDelay=1960000				#This is video segment duration in microseconds
#videoDelay=1921979				#For Testing
firstToRead=0					#This determines where to start reading (i.e. type with lowest duration)
secondToRead=0
difference=0
firstWait=0
secondWait=0
loopCount=0
toggle=0			#Used when we reach a point where two segments of same type needs to be sent before continuing with other type
					#This occurs at some point in time for type with shorter segment duration
  

#Generating list containing files to be sent with respective sizes in bytes
#It is assumed that files.txt would audio segments then video segments in ascending order.
#Moreover, number template is used to name video and audio segments
#files_ordered is populated with MPD and initialization segmetns. It will be updated later
#to contain audio and video segments in the order they are supposed to be sent
echo -e "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<FDT-Instance Expires=\"32511974400\"\n\tFEC-OTI-FEC-Encoding-ID=\"129\"\n\tFEC-OTI-FEC-Instance-ID=\"0\"\n\tFEC-OTI-Maximum-Source-Block-Length=\"64\"\n\tFEC-OTI-Encoding-Symbol-Length=\"1428\"\n\tFEC-OTI-Max-Number-of-Encoding-Symbols=\"96\">" > fdt_Complete.xml
TOI=1
ls -l -r *Dynamic.mpd *init* | awk -v id=$TOI '{if ($5 !="" && $9 != "") {print "<File TOI=\""id"\"\n\tContent-Location=\"file:///"$9"\"\n\tContent-Length=\""$5"\"/>";id++}}' >> fdt_Complete.xml
ls -l --ignore=*mpd --ignore=*init* --ignore=*sh --ignore=*txt| awk '{if ($5 != "" && $9 != "") {print $9" "$5}}' | sort -V > files.txt

#Sort the list in files.txt using the available segment durations
if [ $audioDelay -lt $videoDelay ]
then
	#These values are used to determine sleep duration before every send. Note, 1 video (audio) segment is sent per videoDelay (audioDelay)
	difference=$(($videoDelay - $audioDelay))
	firstWait=$audioDelay
	firstToRead=1
	secondToRead=$(($numOfAudSeg + 1))
else
	difference=$(($audioDelay - videoDelay))
	firstWait=$videoDelay;
	firstToRead=$(($numOfAudSeg + 1))
	secondToRead=1
fi
secondWait=$difference;

TOI=4										#We are starting from three since first 3 TOI are for MPD and init files
max=$(($numOfAudSeg + $numOfVidSeg + 4))
while [ $TOI -lt $max ]
do
	#Below algorith is the same as that used in the FLUTE code (sender.c) to determine the duration of the sleep before sending next file
	if [ $loopCount -eq 0 ]
	then
	  #printf("FirstWait: %lu \n",firstWait);
	  #usleep(firstWait);
	  #awk -v curr=$firstToRead 'NR==curr {print $1, $2}' files.txt >> files_ordered.txt
	  awk -v curr=$firstToRead -v id=$TOI 'NR==curr {print "<File TOI=\""id"\"\n\tContent-Location=\"file:///"$1"\"\n\tContent-Length=\""$2"\"/>"}' files.txt >> fdt_Complete.xml
	  firstToRead=$(($firstToRead + 1))

		if [ $firstWait -ge $difference ] && [ $toggle -eq 0 ]
		then
			firstWait=$(($firstWait - $difference))
			#printf("Normal %lu \n",firstWait);
		elif [ $toggle -eq 1 ]
		then
			firstWait=$(($firstWait - $secondWait))
			toggle=0;
			#printf("Toggle %lu \n",firstWait);
		else
			firstWait=$audioDelay;
			secondWait=$(($secondWait - $firstWait))
			toggle=1;
			#printf("DoubleSend %lu %lu \n",firstWait,secondWait);
			#The below is needed so that the format with lower segment duration is sent again at this stage before sending the other format
			loopCount=-1;
		fi
	elif [ $loopCount -ge 1 ]
	then
	 #printf("SecondWait: %lu \n",secondWait);
	 loopCount=-1;
	 #usleep(secondWait);
	 #awk -v curr=$secondToRead 'NR==curr {print $1, $2}' files.txt >> files_ordered.txt
	 awk -v curr=$secondToRead -v id=$TOI 'NR==curr {print "<File TOI=\""id"\"\n\tContent-Location=\"file:///"$1"\"\n\tContent-Length=\""$2"\"/>"}' files.txt >> fdt_Complete.xml
	 secondToRead=$(($secondToRead + 1))
	 secondWait=$(($secondWait + $difference));
	fi
	loopCount=$(($loopCount + 1))
	TOI=$(($TOI + 1))
done
echo "</FDT-Instance>" >> fdt_Complete.xml