#!/bin/bash
#
#
# USAGE: run [arg1] [arg2] [arg3] [arg4] [arg5]
#
# with
#
# [arg1] ($1): program
# [arg2] ($2): job description file
# [arg3] ($3): machinelist
# [arg4] ($4): number of workers
# [arg5] ($5): logfile
#

if [ ! "$#" = "5" ]; then
   echo
   echo "<E> Not enough or too many arguments supplied."
   echo 
   echo "    Usage: run [mpi program] [job description file] [machinelist] [number of workers] [logfile]"
   echo
   exit 1
fi
PROG=$1
JDL=$2
MACHINES=$3
NWORKERS=$4
LOGFILE=$5

NPROCS=`expr $NWORKERS + 1`

mpirun -np $NPROCS -machinefile $MACHINES $PROG -j $JDL -v > $LOGFILE 2>&1 
