#/bin/bash

nfiles=`ls $1*pid.log|wc -l`
nsum=`tail -n 1 $1*pid.log| grep "Generated Event"|cut -c 20- | awk '{ sum+=$1} END {print sum}'`

echo "Found "$nfiles" files with pattern "$1
echo "Sum of events="$nsum
