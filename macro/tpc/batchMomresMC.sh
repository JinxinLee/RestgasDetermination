#!/bin/bash
#$ -l h_vmem=800M,special=TRUE
# Divert output to directory
#$ -o /nfs/hicran/scratch/user/fboehmer/sgeout
# Write error in same file
#$ -j y

ID=$SGE_TASK_ID
#ID=45
FIRST=$SGE_TASK_FIRST
LAST=$SGE_TASK_LAST

source ~/.bashrc
cd $PANDAPATH
source build/config.sh

#MOM=1.0
#INT=2
PDG=13
mult=1

root -l -b -q macro/tpc/runMomresMC_batch.C\($MOM,$ID,$INT,$PDG,$mult,\"/nfs/nas/data/panda/tpc/SIM/momres/mult1\"\)

exit 0