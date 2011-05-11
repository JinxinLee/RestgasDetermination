#!/bin/bash
#$ -l h_vmem=800M,short=TRUE
# Divert output to directory
#$ -o /nfs/hicran/home/fboehmer/private/sgeout/
# Write error in same file
#$ -j y

ID=$SGE_TASK_ID
FIRST=$SGE_TASK_FIRST
LAST=$SGE_TASK_LAST

source ~/.bashrc
cd $PANDAPATH
source build/config.sh

root -l -b -q macro/tpc/runGeotestMC.C\(1.0,$ID,\"/nfs/nas/data/panda/tpc/SIM/geotest\"\)

exit 0