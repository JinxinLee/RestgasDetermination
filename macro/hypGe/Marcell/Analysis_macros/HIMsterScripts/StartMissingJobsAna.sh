#!/bin/bash

#this script starts missing jobs, use another script (FindMissingFiles.sh) before to Create the input file
subdir=Gamma
Anadir=Ana

DataDir=${SIMDATADIR}/$subdir

TxtDir=${SIMDATADIR}/$subdir/txtfiles
MissingFilesFile=MissingFilesAna.txt
InputFile=${TxtDir}/${MissingFilesFile}
while read line
do
	qsub ${DataDir}/${Anadir}/jobs/$line
done < $InputFile
