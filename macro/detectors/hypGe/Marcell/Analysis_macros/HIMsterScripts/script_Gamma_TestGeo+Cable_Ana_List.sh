#!/bin/bash

SubPath=GeantTest

JobLogPath=${SIMDATADIR}/${SubPath}/Ana/Cable/joblogs
if [ ! -d $JobLogPath ]; then 
  mkdir -p $JobLogPath
fi

AnaLogPath=${SIMDATADIR}/${SubPath}/Ana/Cable/analogs
if [ ! -d $AnaLogPath ]; then 
  mkdir -p $AnaLogPath
fi

JobPath=${SIMDATADIR}/${SubPath}/Ana/Cable/jobs
if [ ! -d $JobPath ]; then 
  mkdir -p $JobPath
fi

while read line; do
    #echo $line
    
	cat >$JobPath/job_$line.sh <<EOF
	#!/bin/bash
	#
	#PBS -N ana_$line
	#PBS -j oe
	#PBS -o ${JobLogPath}/0-job_$line.log
	#PBS -V
	#PBS -l nodes=1:x86_64,walltime=01:00:00

	export PATH=\$PBS_O_PATH
	cd \$PBS_O_WORKDIR

	echo "Start Analysis of File $line."
	root -l -q -b ../GammaSpectraAnalysis_CableTest.C\(\"$line\"\) &> $AnaLogPath/ana_$line.log


EOF
	echo "Analysis of $line"
### submit job to batch system
	qsub $JobPath/job_$line.sh
	#root -l -q -b GammaSpectraAnalysis_extparameter_NoH.C\(\"$line\"\)
done < $SIMDATADIR/${SubPath}/CableList.txt
