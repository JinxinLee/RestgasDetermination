#!/bin/bash
#
# USAGE:
# sbatch -a<min>-<max> jobsim_kronos.sh <pref> <nevt> <dec> <mom> <saveall>
#
#SBATCH -J pndsim
#SBATCH --time=8:00:00
#SBATCH --get-user-env
#SBATCH -e data/slurmlog/slurm_%j_errout.log
#SBATCH -o data/slurmlog/slurm_%j_errout.log


if [ $# -lt 1 ]; then
  echo -e "\nJob script for submission of PandaRoot simulation jobs on KRONOS.\n"
  echo -e "USAGE: sbatch -a<min>-<max> jobsim_kronos.sh <prefix> <nevts> <dec-file> <pbar-mom>\n"
  echo -e " <min>     : Minimum job number"
  echo -e " <max>     : Maximum job number"
  echo -e " <prefix>  : Prefix of output files"
  echo -e " <nevts>   : Number of events to be simulated"
  echo -e " <gen>     : Name of EvtGen decay file 'xxx.dec:iniRes'. Keyword 'DPM/FTF/BOX' instead runs other generator"
  echo -e " <pbeam>   : Momentum of pbar-beam.\n"
  echo -e "Example 1 : sbatch -a1-20 jobsim_kronos.sh d0sim 1000 D0toKpi.dec 12."
  echo -e "Example 2 : sbatch -a1-20 jobsim_kronos.sh dpmbkg 1000 dpm 12."
  echo -e "Example 3 : sbatch -a1-20 jobsim_kronos.sh singleK 1000 \"box:type(321,1):p(0.05,8):tht(0,180):phi(0,360)\" 12.\n"
  
  exit 1
fi


nyx=$VMCWORKDIR"/macro/prod"
_target=$nyx"/data/"

prefix=mysim
nevt=20
dec="D0toKpi.dec"
mom=12.0
save=""
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
  save=$5
fi

outprefix=$tmpdir$prefix"_"$run
pidfile=$outprefix"_pid.root"

root -l -q -b $nyx"/"prod_sim.C\(\"$outprefix\",$nevt,\"$dec\",$mom\) &> $outprefix"_sim.log"
root -l -b -q $nyx"/"prod_aod.C\(\"$outprefix\"\) &> $outprefix"_pid.log"
#root -l -b -q $nyx"/"quickana.C\(\"$pidfile\",1,\"\",0,\"qapart:!mc:!neut\",0,0,$run,$pdg\)   &> $outprefix"_ana.log"

# copy number of generated events from FairFilteredPrimaryGenerator in ...sim.log to ...pid.log
NUMEV=`grep 'Generated Events' $outprefix"_sim.log"`
echo $NUMEV >> $outprefix"_pid.log"

# copy output to storage element
if test "$save" == "saveall"; then
    cp  $outprefix"_sim.log" $_target
    cp  $outprefix"_sim.root" $_target
fi
   
cp  $outprefix"_par.root" $_target
cp  $outprefix"_pid.log" $_target
cp  $outprefix"_pid.root" $_target

# tidy up
rm  $outprefix"_par.root"
rm  $outprefix"_sim.log"
rm  $outprefix"_pid.log"
rm  $outprefix"_sim.root"
rm  $outprefix"_pid.root"


#rm $tmpdir"*"
#rmdir $tmpdir
