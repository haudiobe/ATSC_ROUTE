#!/bin/bash

#This script is intended to correct set TOIs to increasing order in a manually compiled fdt.xml

if [ $# -ne 1 ]
then
	echo "Usage: ./FixTOINumbers.sh fdt.xml "
	exit
fi 

TOI=1;	#This is used to incrementally set periods in MPD

awk -v period=$TOI -v fdt=$1 'BEGIN {gsub(/\./,"_Corrected.",fdt)} {if ($2 ~ /TOI/) {$2 = "TOI=\""period"\"";period++};print $0 > fdt}' $1
