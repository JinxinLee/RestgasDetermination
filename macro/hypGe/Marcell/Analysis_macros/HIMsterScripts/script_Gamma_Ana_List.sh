#!/bin/bash

subdir=Gamma
Anasubdir=Ana

Datapath=${SIMDATADIR}/$subdir
if [ ! -d $Datapath ]; then 
  mkdir -p $Datapath
fi

path=${SIMDATADIR}/$subdir/$Anasubdir
if [ ! -d $path ]; then 
  mkdir -p $path
fi
JobLogpath=$path/joblogs
if [ ! -d $JobLogpath ]; then 
  mkdir -p $JobLogpath
fi
AnaLogpath=$path/analogs
if [ ! -d $AnaLogpath ]; then 
  mkdir -p $AnaLogpath
fi

jobpath=$path/jobs
if [ ! -d $jobpath ]; then 
  mkdir -p $jobpath
fi

while read line; do
    #echo $line
    
	cat >$jobpath/job_$line.sh <<EOF
	#!/bin/bash
	#
	#PBS -N Ana_$subdir_$line
	#PBS -j oe
	#PBS -o ${JobLogpath}/0-job_$line.log
	#PBS -V
	#PBS -l nodes=1:ppn=1,walltime=01:00:00

	export PATH=\$PBS_O_PATH
	cd \$PBS_O_WORKDIR

	echo "Start Analysis of File $line."
	root -l -q -b ../GammaSpectraAnalysis_NoH.C\(\"$line\"\) &> $AnaLogpath/ana_$line.log


EOF
	echo "Analysis of $line"
### submit job to batch system
	qsub $jobpath/job_$line.sh
	#root -l -q -b GammaSpectraAnalysis_NoH.C\(\"$line\"\)
done < ../../Lists/GammaSimulationsToAnaList.txt

