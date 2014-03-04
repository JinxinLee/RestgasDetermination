#! /bin/bash

if [ $1 ] && [ $2 ] && [ $3 ] && [ $4 ] && [ $5 ]; then
  export var1=$1
  export var2=$2
  export var3=$3
  export var5=$4

  export PBS_ARRAYID=$5

  ./runLumiFullSimPixel.sh >> $4/sim.log
else
  echo  "run like: ./... numevents mom inputfiletemplate pathname index"
fi
