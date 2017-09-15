#!/bin/bash
#SBATCH -J pndana
#SBATCH --time=8:00:00
#SBATCH --get-user-env
#SBATCH -e data/slurmlog/slurm_%j_errout.log
#SBATCH -o data/slurmlog/slurm_%j_errout.log

if [ $# -lt 1 ]; then
  echo -e "\nJob script for submission of PandaRoot analysis jobs based on macro 'prod_ana.C' on KRONOS. *The macro needs to configured beforehand!*\n"
  echo -e "   ********************************************************"  
  echo -e "   *** RECOMMENDED: Use anasub.pl for easier submission ***"  
  echo -e "   *********************************************************\n"  
  echo -e "USAGE: sbatch jobana_kronos.sh <prefix> <min> <max> <mode>\n"
  echo -e " <prefix>  : Prefix of input files"
  echo -e " <min>     : Minimum job number of files data/<prefix>_<min>_pid.root"
  echo -e " <max>     : Maximum job number of files data/<prefix>_<max>_pid.root"
  echo -e " <mode>    : Arbitrary mode number to be stored in n-tuples.\n"
  echo -e "Example : sbatch jobana_kronos.sh d0sim 1 10 42\n"
  
  exit 1
fi

nyx=$VMCWORKDIR"/macro/prod"
_target=$nyx"/data/"

prefix="mysim"
from=1
to=20
mode=0

if test "$1" != ""; then
  prefix=$1
fi

if test "$2" != ""; then
  from=$2
fi

if test "$3" != ""; then
  to=$3
fi

if test "$4" != ""; then
  mode=$4
fi

root -l -b -q $nyx"/"prod_ana.C\(\"$_target$prefix\",$from,$to,$mode\)  &> $_target$prefix"_"$from"_"$to"_ana.log"

