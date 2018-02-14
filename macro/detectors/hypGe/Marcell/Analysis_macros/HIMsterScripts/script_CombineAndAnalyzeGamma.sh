#!/bin/bash

TargetNumber=12

subdir=Gamma
Anasubdir=Ana
Combodir=CombinedData
path=${SIMDATADIR}/$subdir/$Anasubdir
if [ ! -d $path ]; then 
  mkdir -p $path
fi
Combopath=${SIMDATADIR}/$subdir/$Anasubdir/${Combodir}
if [ ! -d $path ]; then 
  mkdir -p $path
fi
ls ${path}/Ana_Geo36_E*ST${TargetNumber} -d > ${SIMDATADIR}/$subdir/txtfiles/AnaFolderstoCombine.txt

i=1
while read line; 
do
	
	echo $line
	line=${line%/}																									### remove '/' from end of line  (shortest pattern matching, %% for longest)
	folder=${line##*/}																								### remove longest pattern matching "*/" from the front of line		(# = shortest, ## = longest)

	hadd -f1 ${Combopath}/Combined_${folder}.root ${path}/${folder}/Ana*.root

	
done < ${SIMDATADIR}/$subdir/txtfiles/AnaFolderstoCombine.txt

