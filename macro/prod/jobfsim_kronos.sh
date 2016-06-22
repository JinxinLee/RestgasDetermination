#!/bin/bash
#
# USAGE:
# sbatch -a<min>-<max> jobfsim_kronos.sh <pref> <nevt> <dec> <mom> <saveall>
#
#SBATCH -J pndfsim
#SBATCH --time=8:00:00
#SBATCH --get-user-env
#SBATCH -e data/slurmlog/slurm_%j_errout.log
#SBATCH -o data/slurmlog/slurm_%j_errout.log


if [ $# -lt 1 ]; then
  echo -e "\nJob script for submission of PandaRoot fast simulation (optional combined with analsis) jobs on KRONOS.\n"
  echo -e "USAGE: sbatch -a<min>-<max> jobfsim_kronos.sh <prefix> <nevts> <gen> <pbeam> [opt] [mode]\n"
  echo -e " <min>     : Minimum job number"
  echo -e " <max>     : Maximum job number"
  echo -e " <prefix>  : Prefix of output files"
  echo -e " <nevts>   : Number of events to be simulated"
  echo -e " <gen>     : Name of EvtGen decay file 'xxx.dec:iniRes'. Keyword 'DPM/FTF/BOX' instead runs other generator"
  echo -e " <pbeam>   : Momentum of pbar-beam."
  echo -e " [opt]     : Optional options: if contains 'ana', runs prod_ana.C in addition.";
  echo -e " [mode]    : Optional mode number for analysis\n";
  echo -e "Example 1 : sbatch -a1-20 jobfsim_kronos.sh d0sim 1000 D0toKpi.dec 12. ana 10"
  echo -e "Example 2 : sbatch -a1-20 jobfsim_kronos.sh dpmbkg 1000 dpm 12."
  echo -e "Example 3 : sbatch -a1-20 jobfsim_kronos.sh singleK 1000 \"box:type[321,1]:p[0.05,8]:tht[0,180]:phi[0,360]\" 12.\n"
  
  exit 1
fi


nyx=$VMCWORKDIR"/macro/prod"
_target=$nyx"/data/"

prefix=mysim
nevt=20
dec="D0toKpi.dec"
mom=15.0
ana=""
mode=0
run=$SLURM_ARRAY_TASK_ID

tmpdir="/tmp/"$USER"_"$SLURM_JOB_ID"/"
mkdir $tmpdir

if test "$1" != ""; then
  prefix=$1
fi

if test "$2" != ""; then
  nevt=$2
fi

if test "$3" != ""; then
  dec=$3
fi

if test "$4" != ""; then
  mom=$4
fi

if test "$5" != ""; then
  ana=$5
fi

if test "$6" != ""; then
  mode=$6
fi

outprefix=$tmpdir$prefix"_"$run
pidfile=$outprefix"_fsim.root"

root -l -q -b $nyx"/"prod_fsim.C\(\"$outprefix\",$nevt,\"$dec\",$mom\) &> $outprefix"_fsim.log"

# run analysis stage in addition
if [[ $ana == *"ana"* ]]; then
    root -l -q -b $nyx"/"prod_ana.C\(\"$pidfile\",0,0,$mode,0\) &> $outprefix"_fana.log"

    cp  $outprefix"_fana.log" $_target
    cp  $outprefix"_fsim_ana.root" $_target
    
    # tidy up
    rm  $outprefix"_fana.log"
    rm  $outprefix"_fsim_ana.root"
fi
   
cp  $outprefix"_fsim.log" $_target
cp  $outprefix"_fsim.root" $_target

# tidy up
rm  $outprefix"_fsim.log"
rm  $outprefix"_fsim.root"


