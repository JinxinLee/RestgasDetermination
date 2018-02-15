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


echo $LD_LIBRARY_PATH
echo $PATH

base=/lustre/nyx/panda/carsten/fairsoft_nov15/Data/Geant4.15GeV
path=/lustre/nyx/panda/carsten/fairsoft_nov15/pandaroot/tools/RadMapTool

cd $base

hadd -f RadMap_Out_Final.root `find $base -name RadMap_Out3.root`
