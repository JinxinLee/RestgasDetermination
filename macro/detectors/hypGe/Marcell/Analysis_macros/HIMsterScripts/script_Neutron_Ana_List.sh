#!/bin/bash

path=${SIMDATADIR}/Neutron/Ana/joblogs
if [ ! -d $path ]; then 
  mkdir -p $path
fi

AnaLogpath=${SIMDATADIR}/Neutron/Ana/analogs
if [ ! -d $AnaLogpath ]; then 
  mkdir -p $AnaLogpath
fi

jobpath=${SIMDATADIR}/Neutron/Ana/jobs
if [ ! -d $jobpath ]; then 
  mkdir -p $jobpath
fi
while read line; do
    #echo $line
    
	cat >$jobpath/job_$line.sh <<EOF
	#!/bin/bash
	#
	#PBS -N ana_$line
	#PBS -j oe
	#PBS -o ${path}/0-job_$line.log
	#PBS -V
	#PBS -l nodes=1:ppn=1,walltime=02:00:00

	export PATH=\$PBS_O_PATH
	cd \$PBS_O_WORKDIR

	echo "Start Analysis of $line."
	root -l -q -b ../AllNeutronAnalysis_job.C\(\"$line\"\) &> $AnaLogpath/ana_$line.log


EOF
	echo "Analysis of $line"
### submit job to batch system
	qsub $jobpath/job_$line.sh
	#root -l -q -b AllNeutronAnalysis_job.C\(\"$line\"\)
done < /data/work/kpha1/steinen/Neutron/NeutronSimulationsToAnaList.txt
