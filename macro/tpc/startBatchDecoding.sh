#!/bin/bash

set +xv

#starting script for running the FOPI reconstruction

CORES=$1  #number of cores to use
COUNTER=0

while [ $COUNTER -lt $CORES ]
do
    screen -dms /bin/bash -S bashDecode$COUNTER
    screen -p 0 -S "bashDecode$COUNTER" -X exec ./batchDecoding.sh $COUNTER $CORES
    COUNTER=`expr $COUNTER + 1`
done