#/bin/bash

nfiles=`ls $1*ana.log|wc -l`
nsum=0
for f in $(find . -name "DPM_2254*ana.log")
do 
	sum=`tail -n20 $f| grep PndSimpleCombiner | tail -1|cut -c 28-`
	nsum=$((nsum+sum))
done

echo "Found "$nfiles" files with pattern "$1
echo "Sum of events="$nsum
