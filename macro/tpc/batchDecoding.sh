#!/bin/bash
#$ -l h_vmem=1000M,medium=TRUE
# Divert output to directory
#$ -o /nfs/hicran/home/fboehmer/private/sgeout/decoding/
# Write error in same file
#$ -j y

ID=$1
TOT=$2

source ~/.bashrc
cd $PANDAPATH
cd ../monitoring/clean
echo $PWD

counter=0
for i in {2454,2455,2456,2457}
do
    mod=`expr $counter % $TOT`  
    if [ $mod -eq $ID ] 
    then
	./tpcMonitor -c config/fopi2011_munich.config /nfs/hicran/data/tpc/fopi/2011/raw/*$i*.lmd -batch	
    fi
    counter=`expr $counter + 1`
done

#root -l -b -q macro/tpc/runMomresMC_batch.C\($MOM,$ID,$PDG,\"/nfs/nas/data/panda/tpc/SIM/momres\"\)

exit 0