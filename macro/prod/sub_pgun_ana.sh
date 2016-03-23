#!/bin/bash
#SBATCH --time=8:00:00
#SBATCH -o /lustre/nyx/panda/kgoetzen/pandaroot/trunk01/macro/prod/data/%j_%N.out.log
#SBATCH -e /lustre/nyx/panda/kgoetzen/pandaroot/trunk01/macro/prod/data/%j_%N.out.log

. $FAIRLIBDIR"/../config.sh"

nyx=$VMCWORKDIR"/macro/prod"
_target=$nyx"/data/"

prefix=13
nEvts=1000
pdg=13
mom=10.
mult=1
save=""
run=$SLURM_ARRAY_TASK_ID

tmpdir=$TMPDIR"/"$USER"_"$SLURM_JOB_ID"/"
#"/tmp/"$USER"_"$SLURM_JOB_ID"/"
mkdir $tmpdir

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

if test "$6" != ""; then
  save=$6
fi

outprefix=$tmpdir$prefix"_"$run
pidfile=$outprefix"_pid.root"

root -l -q -b $nyx"/"prod_sim_pgun.C\(\"$outprefix\",$nEvts,$pdg,$mom,$mult\) &> $outprefix"_sim.log"
root -l -b -q $nyx"/"prod_dig.C\(\"$outprefix\"\) &> $outprefix"_dig.log"
root -l -b -q $nyx"/"prod_rec.C\(\"$outprefix\"\) &> $outprefix"_rec.log"
root -l -b -q $nyx"/"prod_pid.C\(\"$outprefix\"\) &> $outprefix"_pid.log"
root -l -b -q $nyx"/"quickana.C\(\"$pidfile\",1,\"\",0,\"qapart:!mc:!neut\",0,0,$run,$pdg\)   &> $outprefix"_ana.log"

ls $tmpdir

#cp  $tmpdir"_"$prefix$run"_ana.log" $_target
#cp  $outprefix"_"$run"_sim.root" $_target
cp  $outprefix"_ana.log" $_target
cp  $outprefix"_pid_ana.root" $_target
cp  $outprefix"*" $_target

rm  $outprefix"_sim.log"
rm  $outprefix"_dig.log"
rm  $outprefix"_rec.log"
rm  $outprefix"_pid.log"
rm  $outprefix"_ana.log"
rm  $outprefix"_sim.root"
rm  $outprefix"_dig.root"
rm  $outprefix"_rec.root"
rm  $outprefix"_pid.root"
rm  $outprefix"_pid_ana.root"


#rm $tmpdir"*"
#rmdir $tmpdir
