#/bin/bash

nfiles=`ls $1*ana.log|wc -l`
nsum=`tail -n 1 $1*ana.log| grep "PndSimpleCombinerTask"|cut -c 28- | awk '{ sum+=$1} END {print sum}'`

echo "Found "$nfiles" files with pattern "$1
echo "Sum of events="$nsum
