#!/bin/bash

prefix=9999
nEvts=1000
dec="pp_DpDm_Dp_Kpipi_incl.dec"
mom=6.56903
res="pbarpSystem0"

sig=1

if [ $# -lt 4 ]; then
  echo -e "\nPerforms simulation of EvtGen or DPM events.\n"
  echo -e "USAGE: ./runall_prod.sh <prefix> <nevts> <dec-file> <pbar-mom> <init. resonance>\n"
  echo -e " <prefix>    : Prefix of output files"
  echo -e " <nevts>     : Number of events to be simulated"
  echo -e " <dec-file>  : Name of EvtGen decay file. Keyword 'DPM' instead runs DPM events"
  echo -e " <pbar-mom>  : Momentum of pbar-beam"
  echo -e " <resonance> : Initial resonance in decay-file. Skipped for DPM simulation\n"
  echo -e "Creates output files: <prefix>_sim.root, <prefix>_par.root, <prefix>_dig.root, <prefix>_rec.root, <prefix>_pid.root"
  echo -e "and corresponding log files.\n"
  exit 1
fi

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

outprefix="data/"$prefix

root -l -q -b -w prod_sim.C\(\"$outprefix\",$nEvts,\"$dec\",$mom,\"$res\"\) &> $outprefix"_sim.log" 
NUMEV=`grep 'Generated Events' $outprefix"_sim.log"`
root -l -b -q -w prod_dig.C\(\"$outprefix\"\) &> $outprefix"_dig.log"
root -l -b -q -w prod_rec.C\(\"$outprefix\"\) &> $outprefix"_rec.log"
root -l -b -q -w prod_pid.C\(\"$outprefix\"\) &> $outprefix"_pid.log"
echo $NUMEV >> $outprefix"_pid.log"
