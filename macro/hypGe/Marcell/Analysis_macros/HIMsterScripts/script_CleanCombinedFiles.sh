#!/bin/bash

path=${SIMDATADIR}/Gamma/Ana/CombinedData/


txtfile=FilesToClean.txt
txtpath=${SIMDATADIR}/Gamma/txtfiles/${txtfile}
rm ${txtpath}
ls  ${path}/*Psf* > ${txtpath}
#ls  ${path}/*Psf* 
ls  ${path}/*Psf* | wc -w
#root -l ../FileCleaner.C
while read line; 
do
	root -l -q '../FileCleaner.C ("'${line}'")'
	#root -l ../FileCleaner.C
done < ${txtpath}
