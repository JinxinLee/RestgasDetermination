#!/bin/bash
#SBATCH -J rootmacro
#SBATCH --time=8:00:00
#SBATCH --get-user-env
#SBATCH -e slurmlog/slurm_%j_errout.log
#SBATCH -o slurmlog/slurm_%j_errout.log

if [ $# -lt 1 ]; then
  echo -e "\nJob script for submission of arbitrary ROOT macro (with options -l -b -q) on KRONOS. *The macro needs to configured beforehand!*\n"
  echo -e "USAGE: sbatch job_rootmacro.sh '<macro>'\n"
  echo -e " <macro>   : Complete call of a ROOT macro."
  echo -e " <logf>    : logfile name.\n"
  echo -e "Example : sbatch job_rootmacro.sh 'mymacro.C+(1,2,3)' mylog.log\n\n"
  
  exit 1
fi

nyx=$PWD

#run=$SLURM_ARRAY_TASK_ID

## create tmp directory to write stuff in
#tmpdir="/tmp/"$USER"_"$SLURM_JOB_ID"_"$run"/"
#mkdir $tmpdir
#cd $tmpdir

## check input parameters
#if test "$1" != ""; then
  #prefix=$1
#fi
logname=$nyx"/log_"$USER"_"$SLURM_JOB_ID"_"$run".log"
macro=$1

if test "$2" != ""; then
  logname=$2
fi

logname=$nyx"/"$logname
# variable to hold prefix with run number
#prefrun=$prefix"_"$run

## replace place-holders PREFIX and RUN with actual values, in particular add absolute paths
#macro=${macro/PREFIX/$prefix}
#macro=${macro/RUN/$run}

## if there is a dec file given with relative path, add the absolute path
#decname=`expr "$macro" : '.*\"\(.*.dec\)\".*'`

#if test "$decname" != ""; then
  #if [[ $decname != \/* ]] ; then
    #macro=${macro/$decname/$nyx"/"$decname}
	#decname=$nyx"/"$decname
  #fi
#fi

#echo "decay file : \"$decname\""

# this is the command to be run
echo "root -l -b -q $nyx/$macro &> $logname"
echo $ROOTSYS

## output and log files
#outname=$prefrun"_ana.root"
#logname=$prefrun"_quickfa.log"

## the actual command
root -l -b -q "$nyx/$macro" &> $logname

## check whether everything is there (in slurmlog visible)
#ls -lh $tmpdir

## move output and remove tmp dir
#rm gphysi.dat
#mv $tmpdir/* $_target
#rm -rf $tmpdir


