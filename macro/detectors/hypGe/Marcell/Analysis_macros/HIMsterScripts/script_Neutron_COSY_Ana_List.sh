#!/bin/bash

NoOfEvents=1000000
#NoOfEvents=100000
#NoOfJobs=10
NoOfJobs=1
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
      #PBS -l nodes=1:ppn=1,walltime=02:00:00

			export PATH=\$PBS_O_PATH
			cd \$PBS_O_WORKDIR

			echo "Start Analysis of $line."
			root -l -q -b ../NeutronAnalysis_COSY_edit.C\(\"$line\"\,$StartEvent\,$EventsPerJob\) &> $AnaLogpath/ana_$fileAddition.log


EOF
			echo "Analysis of $line $StartEvent $EventsPerJob"
### submit job to batch system
			qsub $jobpath/job_$fileAddition.sh
			sleep 1s
		done
		x=$(qstat | grep -c ${USER})		
			while [ $x -gt 900 ]
			do
				echo "Still some jobs running: waiting to start new jobs"
				date
				sleep 2m #30s
				#x=$(ls | grep -c ext)
				x=$(qstat | grep -c ${USER})
			done			### end of while "double queue" loop
	fi

done < ../../Lists/COSYSimulationsToAnaList.txt
