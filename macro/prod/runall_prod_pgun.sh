#!/bin/bash

prefix=13
nEvts=1000
pdg=13
mom=10.
mult=1

if [ $# -lt 3 ]; then
  echo -e "\nPerforms simulation of Box Generator events for a specific particle type.\n"
  echo -e "USAGE: ./runall_prod_pgun.sh <prefix> <nevts> <pdgcode> <max. mom> <mult>\n"
  echo -e " <prefix>  : Prefix of output files"
  echo -e " <nevts>   : Number of events to be simulated"
  echo -e " <pdgcode> : Pdg code of particles to be simulated"
  echo -e " <max-mom> : Maximum momentum of particles. Simulation of 0.01 < p < max.mom, 0 < theta < 180, 0 < phi < 360."
  echo -e " <mult>    : Number of particles per event\n"
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
  pdg=$3
fi

if test "$4" != ""; then
  mom=$4
fi

if test "$5" != ""; then
  mult=$5
fi

outprefix="data/"$prefix
pidfile=$outprefix"_pid.root"

root -l -q -b prod_sim_pgun.C\(\"$outprefix\",$nEvts,$pdg,$mom,$mult\) &> $outprefix"_sim.log"
root -l -b -q prod_dig.C\(\"$outprefix\"\) &> $outprefix"_dig.log"
root -l -b -q prod_rec.C\(\"$outprefix\"\) &> $outprefix"_rec.log"
root -l -b -q prod_pid.C\(\"$outprefix\"\) &> $outprefix"_pid.log"
root -l -b -q quickana.C\(\"$pidfile\",1,\"\",0,\"qapart\",0,0,0,$pdg\)   &> $outprefix"_ana.log"
