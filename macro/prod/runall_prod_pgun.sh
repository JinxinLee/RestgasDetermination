#!/bin/bash

prefix=13
nEvts=1000
pdg=13
mom=10.
mult=1


if test "$1" != ""; then
  prefix=$1
fi

if test "$2" != ""; then
  nEvts=$2
fi

if test "$3" != ""; then
  pdg=$3
fi

if test "$4" != ""; then
  mom=$4
fi

if test "$5" != ""; then
  mult=$5
fi

outprefix="data/M"$prefix

root -l -q -b -w prod_sim_pgun.C\(\"$outprefix\",$nEvts,$pdg,$mom,$mult\) &> $outprefix"_sim.log"
root -l -b -q -w prod_dig.C\(\"$outprefix\"\) &> $outprefix"_dig.log"
root -l -b -q -w prod_rec.C\(\"$outprefix\"\) &> $outprefix"_rec.log"
root -l -b -q -w prod_pid.C\(\"$outprefix\"\) &> $outprefix"_pid.log"
