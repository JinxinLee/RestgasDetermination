#!/bin/bash

prefix=9999
nEvts=1000
dec="pp_DpDm_Dp_Kpipi_incl.dec"
mom=6.56903
res="pbarpSystem0"

sig=1

if test "$1" != ""; then
  prefix=$1
fi

if test "$2" != ""; then
  nEvts=$2
fi

if test "$3" != ""; then
  dec=$3
fi

if test "$4" != ""; then
  mom=$4
fi

if test "$5" != ""; then
  res=$5
fi

if test "$dec" == "DPM"; then
  sig=0
fi

outprefix="data/M"$prefix

root -l -q -b -w prod_sim.C\(\"$outprefix\",$nEvts,\"$dec\",$mom,\"$res\"\) &> $outprefix"_sim.log"
root -l -b -q -w prod_dig.C\(\"$outprefix\"\) &> $outprefix"_dig.log"
root -l -b -q -w prod_rec.C\(\"$outprefix\"\) &> $outprefix"_rec.log"
root -l -b -q -w prod_pid.C\(\"$outprefix\"\) &> $outprefix"_pid.log"
