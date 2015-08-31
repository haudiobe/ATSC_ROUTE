#!/bin/bash

#while [ ! -f $1 ]
#do
#  sleep 0.01
#done

#Get CurrentTime
#currTime=$(date --date="$2 seconds")

#Get date in UTC (This is the time reference used by the DASH reference client
#AST=$(date -u +"%Y-%m-%dT%T" -d "$currTime")
#echo $AST

sudo sed "s/availabilityStartTime=\".*\" timeShiftBufferDepth/availabilityStartTime=\"$2\" timeShiftBufferDepth/g" $1 > $8
#sudo sed -i "s/Period start=\"PT0S/Period start=\"PT${3}S/g" $8
sudo sed -i "s/video_8M_\$Number\$.mp4\" startNumber=\".*\" duration=/video_8M_\$Number\$.mp4\" startNumber=\"${4}\" duration=/g" $8
sudo sed -i "s/presentationTimeOffset=\"0\" media=\"video_8M_/presentationTimeOffset=\"${6}\" media=\"video_8M_/g" $8
sudo sed -i "s/audio_64k_\$Number\$.mp4\" startNumber=\".*\" duration=/audio_64k_\$Number\$.mp4\" startNumber=\"${5}\" duration=/g" $8
sudo sed -i "s/presentationTimeOffset=\"0\" media=\"audio_/presentationTimeOffset=\"${7}\" media=\"audio_/g" $8

#sudo sed "s/availabilityStartTime=\".*\" timeShiftBufferDepth/availabilityStartTime=\"$2\" timeShiftBufferDepth/g" $1 > $8
#sudo sed -i "s/BBB_720_4M_video_\$Number\$.mp4\" startNumber=\".*\" duration=/BBB_720_4M_video_\$Number\$.mp4\" startNumber=\"${4}\" duration=/g" $8
#sudo sed -i "s/presentationTimeOffset=\"0\" media=\"BBB_720_/presentationTimeOffset=\"${6}\" media=\"BBB_720_/g" $8
#sudo sed -i "s/BBB_64k_\$Number\$.mp4\" startNumber=\".*\" duration=/BBB_64k_\$Number\$.mp4\" startNumber=\"${5}\" duration=/g" $8
#sudo sed -i "s/presentationTimeOffset=\"0\" media=\"BBB_64k_/presentationTimeOffset=\"${7}\" media=\"BBB_64k_/g" $8