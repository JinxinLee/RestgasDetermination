#!/bin/bash


# Task name
#SBATCH -J RadmapTool
# Run time limit
#SBATCH --time=1:30:00
# Working directory on shared storage
#SBATCH -D /lustre/nyx/panda/carsten/fairsoft_nov15
# Standard and error output in different files
#SBATCH -o log/%j_%N.out.log
#SBATCH -e log/%j_%N.err.log
# Execute application code


source $VMCWORKDIR/buildPanda/config.sh


path=
base=

dir0=$(echo "($SLURM_ARRAY_TASK_ID)/100" | bc);
dir1=$(echo "(($SLURM_ARRAY_TASK_ID)-$dir0*100)/10" | bc)

cd $base/$dir0/$dir1
hadd -f RadMap_Out2.root `find . -name RadMap_Out.root`
