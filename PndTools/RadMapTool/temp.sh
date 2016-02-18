#!/bin/bash

#$ -N job_submit
#$ -j y
#$ -cwd
#$ -l mem=3G,ct=00:45:00
# qsub -t 1-1 job_submit.sh command

#LOG=/hera/panda/makonyi/Work/codes/log/
source /hera/panda/makonyi/PandaRoot/100514/built/config.sh
path=/hera/panda/makonyi/Work/FieldSimu/SimulationMacros/2Store
base=Geant4.15GeV

hadd -f RadMap_Out_Final_$base.root `find $base -name RadMap_Out3.root`
