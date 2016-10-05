#!/bin/bash

#### not implemented yet!!!!


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
  file=`echo  $line | awk -v FS="/" '{print $2}'`
	folder=`echo  $line | awk -v FS="/" '{print $1}'`
	jobadd=`echo  $file | awk -v FS="Sim_" '{print $2}'`
	jobadd=`echo  $jobadd | awk -v FS=".root" '{print $1}'`
	jobadd=Ana_${jobadd}
	#echo $jobadd
	
	cat >$jobpath/job_${jobadd}.sh <<EOF
	#!/bin/bash
	#
	#PBS -N Ana_$subdir_$file
	#PBS -j oe
	#PBS -o ${JobLogpath}/0-job_$file.log
	#PBS -V
	#PBS -l nodes=1:ppn=1,walltime=01:00:00

	export PATH=\$PBS_O_PATH
	cd \$PBS_O_WORKDIR

	echo "Start Analysis of File $line."
	root -l -q -b ../GammaSpectraAnalysis_NoH_Task.C\(\"$file\"\,\"$folder\"\) &> $AnaLogpath/ana_$file.log


EOF



	#echo "Analysis of $folder $file"
### submit job to batch system
	qsub $jobpath/job_${jobadd}.sh
	x=$(qstat | grep -c ${USER})		
		while [ $x -gt 990 ]
		do
			echo "Still some jobs running: waiting to start new jobs"
			date
			sleep 1m #30s
			#x=$(ls | grep -c ext)
			x=$(qstat | grep -c ${USER})
		done			### end of while "double queue" loop
	echo "root -l -q -b ../GammaSpectraAnalysis_NoH_Task.C\(\"$file\"\,\"$folder\"\)"
done < ${SIMDATADIR}/$subdir/txtfiles/GammaFilesToAnaList.txt
 
