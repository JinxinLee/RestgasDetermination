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
  echo -e " <logf>    : logfile name. If not given, a default is name is created and stored in subdir log/..\n"
  echo -e "Example 1: sbatch job_rootmacro.sh 'mymacro.C+(1,2,3)' mylog.log\n"
  echo -e "Example 2: sbatch job_rootmacro.sh 'mymacro.C+(1,2,3)' # --> creates logfile log_mymacro__1_2_3.log\n\n"
  
  exit 1
fi

nyx=$PWD

# macro call is parameter 1
macro=$1

# prepare default logfile name from macro call
logname=$(echo $1 | tr -d '["()+]'| sed 's/.C/__/'| tr '[," ()]' '_')
logname="log__"$logname".log"

# or take the given one
if test "$2" != ""; then
  logname=$2
fi

# some log output
echo "Macro    : "$macro
echo "Log file : "$logname
echo "ROOTSYS  : "$ROOTSYS


# prepend absolute nyx path
logname=$nyx"/"$logname
echo "Command  : root -l -b -q $nyx/$macro &> $logname"

## the actual command
root -l -b -q "$nyx/$macro" &> $logname

