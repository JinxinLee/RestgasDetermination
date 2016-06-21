#!/bin/bash

prefix=9999
nEvts=1000
dec="D0toKpi.dec:pbarpSystem0"
mom=15

sig=1

if [ $# -lt 4 ]; then
  echo -e "\nPerforms simulation of EvtGen/DPM/FTF/BOX events.\n"
  echo -e "USAGE: ./runall_prod.sh <prefix> <nevts> <gen> <pbeam>\n"
  echo -e " <prefix>  : Prefix of output files"
  echo -e " <nevts>   : Number of events to be simulated"
  echo -e " <gen>     : Name of EvtGen decay file 'xxx.dec:iniRes'. Keyword 'DPM/FTF/BOX' instead runs other generator"
  echo -e " <pbeam>   : Momentum of pbar-beam.\n"
  echo -e "Creates output files: <prefix>_sim.root, <prefix>_par.root, <prefix>_pid.root"
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

outprefix="data/"$prefix

root -l -q -b prod_sim.C\(\"$outprefix\",$nEvts,\"$dec\",$mom\) &> $outprefix"_sim.log" 
NUMEV=`grep 'Generated Events' $outprefix"_sim.log"`
root -l -b -q prod_aod.C\(\"$outprefix\"\) &> $outprefix"_pid.log"
echo $NUMEV >> $outprefix"_pid.log"
