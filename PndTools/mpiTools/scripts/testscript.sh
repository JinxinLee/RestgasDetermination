#!/bin/bash
#
# Simple test script which can be used to check the MPI code
#
# USAGE: in your job description file, include the line
#
# JOB <splitlevel> <path_to>testscript.sh <input> <output> <nr_of_pars> <par 1> <par 2> ...
#
#
# argument 1: run identification (=unique number)
# argument 2: par 1
# argument 3: par 2
# ...
#

args=("$@")
echo "***************************************" > environment
echo "<I> Script is called with" $# "arguments" >> environment
for (( i=0; i<$#; i++ )); do
  echo "<I> Argument" $i "is" ${args[$i]} >> environment
done
echo "***************************************" >> environment

source $HOME/scripts/pandaroot.sh > /dev/null 2>&1

printenv >> environment

exit 0
