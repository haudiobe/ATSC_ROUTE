#!/bin/bash

sqlExportFile="ExportSql.txt";
sqlExportFileModify="ExportSqlModified.txt"

if [ -e $sqlExportFileModify ]; then
	rm $sqlExportFileModify
fi

b=$(awk -v file=$sqlExportFileModify '{ if (index($1,"DecodedFrames") > 0) {print $0} else if (index($1,"http://") > 0) {printf ("%s %0.f %0.f %0.f %0.f %0.f %s\n",$1,$2+$3,$2+$4,$2+$5,$2+$6,$2+$7,$8) > file} else {print $0 > file}}' $sqlExportFile)
#b=$(awk -v file=$sqlExportFileModify '{ if (index($1,"DecodedFrames") > 0) {print $0} else {print $0 > file}}' $sqlExportFile)

decodedFrames=($b);
index=2

while [ $index -lt ${#decodedFrames[@]} ]
do
	Time=$(printf "%0.f" "${decodedFrames[$index]}")
	decodedFrames[$index]=$((${decodedFrames[1]} + Time))
	index=$(($index + 3))
done

index=2
decFram="DecodedFrames";

while [ $index -lt ${#decodedFrames[@]} ]
do
	decFram=$decFram" "${decodedFrames[$index]}
	
	index=$(($index + 1))
done

echo $decFram >> $sqlExportFileModify
