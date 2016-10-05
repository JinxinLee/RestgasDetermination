#!/bin/bash

#this script starts missing jobs, use another script (FindMissingFiles.sh) before to Create the input file
subdir=Gamma
DataDir=${SIMDATADIR}/$subdir

TxtDir=${SIMDATADIR}/$subdir/txtfiles
MissingFilesFile=SimMissingFiles.txt
InputFile=${TxtDir}/${MissingFilesFile}
while read line
do
	qsub ${DataDir}/jobs/$line
done < $InputFile
