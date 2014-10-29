#!/bin/bash

NoOfEvents=200000
NoOfJobs=2
EventsPerJob=$(($NoOfEvents/$NoOfJobs))
while read line; do

	fNameChopped=${line%.root}
	echo $fNameChopped
    #echo $line
    
  subdir=COSY/Ana/ToCombine/${fNameChopped}
  path=${SIMDATADIR}/${subdir}
	if [ ! -d $path ]; then 
		mkdir -p $path
	fi
	
	JobLogPath=${SIMDATADIR}/${subdir}/joblogs
	if [ ! -d $JobLogPath ]; then 
		mkdir -p $JobLogPath
	fi

	AnaLogpath=${SIMDATADIR}/${subdir}/analogs
	if [ ! -d $AnaLogpath ]; then 
		mkdir -p $AnaLogpath
	fi

	jobpath=${SIMDATADIR}/${subdir}/jobs
	if [ ! -d $jobpath ]; then 
		mkdir -p $jobpath
	fi

	JobNo=0
	if [ line ]
	then
		for ((StartEvent=0; StartEvent<$NoOfEvents; StartEvent=$(($StartEvent+$EventsPerJob))))
		do
			JobNo=$((JobNo+1))
			fileAddition=${line}_StartEvent_${StartEvent}_NoOfEvents_${EventsPerJob}
			echo "StartEvent $StartEvent"
			cat >$jobpath/job_$fileAddition.sh <<EOF
			#!/bin/bash
			#
			#PBS -N ana_${JobNo}_$line
			#PBS -j oe
			#PBS -o ${JobLogPath}/0-job_$fileAddition.log
			#PBS -V
      #PBS -l nodes=1:ppn=1,walltime=24:00:00

			export PATH=\$PBS_O_PATH
			cd \$PBS_O_WORKDIR

			echo "Start Analysis of $line."
			root -l -q -b ../COSYBackgroundAnalysis.C\(\"$line\"\,$StartEvent\,$EventsPerJob\) &> $AnaLogpath/ana_$fileAddition.log


EOF
			echo "Analysis of $line"
### submit job to batch system
			qsub $jobpath/job_$fileAddition.sh
			echo "root -l -q -b ../COSYBackgroundAnalysis.C\(\"$line\"\,$StartEvent\,$EventsPerJob\) &> $AnaLogpath/ana_$fileAddition.log"
		done
	fi

done < ../../Lists/COSYSimulationsToAnaList.txt
