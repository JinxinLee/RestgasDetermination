#!/bin/bash
#rm ${SIMDATADIR}/Gamma/Ana/CombinedData/Fitted/*.txt
while read line; 
do
	
	echo $line
	root -l -b -q ../FitGammaSpectra.C\(\"${line}\"\)

#done < ${SIMDATADIR}/Gamma/txtfiles/CombinedFiles.txt
#done < ${SIMDATADIR}/Gamma/txtfiles/CombinedLE.txt
done < ${SIMDATADIR}/Gamma/txtfiles/CombinedDEGASComp.txt
