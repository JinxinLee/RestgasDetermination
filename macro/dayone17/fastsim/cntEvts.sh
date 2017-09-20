#/bin/bash

nfiles=`tail -n 50 $1*fsim.log| grep "Generated Events = "|wc -l`
nsum=`tail -n 50 $1*fsim.log| grep "Generated Events = "|cut -c 20- | awk '{ sum+=$1} END {print sum}'`

echo "Found "$nfiles" files with pattern "$1" containing event info"
echo "Sum of events = "$nsum
