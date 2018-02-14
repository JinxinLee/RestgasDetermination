#/bin/bash

nfilespat=`ls $1*fsim.log 2> /dev/null|wc -l`
nfiles=`tail -n 50 $1*fsim.log 2> /dev/null| grep "Generated Events = "|wc -l`
nsum=`tail -n 50 $1*fsim.log 2> /dev/null| grep "Generated Events = "|cut -c 20- | awk '{ sum+=$1} END {print sum}'`

#echo "Found "$nfiles" files with pattern "$1" containing event info"
#echo "Sum of events ("$nfiles"/"$nfilespat" files '"$1"') = "$nsum
printf "%-25s (%4d/%4d files) = %12d\n" $1 $nfiles $nfilespat $nsum
