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

MOM=5.0
PDG=211

root -l -b -q macro/tpc/runMomresMC_batch.C\($MOM,$ID,$PDG,\"/nfs/nas/data/panda/tpc/SIM/momres\"\)

exit 0