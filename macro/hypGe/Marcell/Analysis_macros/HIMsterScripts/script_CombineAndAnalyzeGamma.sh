#!/bin/bash


find $SIMDATADIR/Gamma/Ana/ -mindepth 1 -maxdepth 1 -type d  \( ! -iname ".*" \)  \( ! -iname "ana*" \) \( ! -iname "job*" \) |sed 's|^\./||g' &> ../Lists/GammaAnaFolderList.txt
echo $PWD
 thispath=$PWD
echo $thispath
mkdir -p ../Lists/GammaFolder
gammapath=$thispath/../Lists/GammaFolder
i=1
while read line; 
do
	cd $line
	
	hadd -f1 Combined.root Ana*.root
	root -l -b -q $thispath/../AnalyseCombinedGammaFiles.C\(\"$line\"\,\"$line/Combined.root\"\)
	
done < ../../Lists/GammaAnaFolderList.txt

cd $thispath
