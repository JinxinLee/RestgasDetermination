#!/bin/bash

set +xv 

COUNTER=0

cd $PANDAPATH;
. $PANDABUILD/config.sh

for i in /nfs/hicran/data/tpc/fopi/2010/decoded/runC*repaired*root
do
    REST=1
    REST=`expr $COUNTER % 3`
    if [ $REST -eq $1 ]
    then root -l -b -q macro/tpc/TestBench/runRecoFOPI_batch.C\(\"$i\",\"/nfs/hicran/data/tpc/fopi/2010\"\) 
    fi
    COUNTER=`expr $COUNTER + 1`
done