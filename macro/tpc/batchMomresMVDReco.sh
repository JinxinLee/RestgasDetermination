#!/bin/bash
#$ -l h_vmem=2000M,medium=TRUE
# Divert output to directory
#$ -o /nfs/hicran/scratch/user/fboehmer/sgeout
# Write error in same file
#$ -j y

ID=$SGE_TASK_ID
#ID=145
FIRST=$SGE_TASK_FIRST
LAST=$SGE_TASK_LAST

source ~/.bashrc
cd $PANDAPATH
source build/config.sh

#MOM=5.0
PDG=211

DATAPATH=/nfs/nas/data/panda/tpc/SIM/momres/mult1

for file in $DATAPATH/*PDG$PDG*mom$MOM*deg$ID*raw.root 
do
    if [ -e $file ] 
    then
	root -l -b -q macro/tpc/runMomresRecoMVD_batch.C\(\"$file\"\)
    fi
done



exit 0