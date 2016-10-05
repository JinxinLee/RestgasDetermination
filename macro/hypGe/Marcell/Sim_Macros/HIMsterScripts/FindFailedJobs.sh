#!/bin/bash

### script to find failed jobs by checking its error code in log file

### uncomment last line to directly start the erroneous jobs


user=$USER				#### this is taken from system variable and used for job sending via the "double queue" (automatic queue if the internal himster queue is to full)
SubDir=Gamma
JobLogDir=${SIMDATADIR}/${SubDir}/joblogs
JobDir=${SIMDATADIR}/${SubDir}/jobs
TxtDir=${SIMDATADIR}/${SubDir}/txtfiles
if [ ! -d $TxtDir ]; then 
  mkdir -p $TxtDir
fi

FailedJobsFile=FailedJobsSim.txt
rm  ${TxtDir}/${FailedJobsFile}
#touch ${JobLogDir}/${FailedJobsFile}
ls ${JobLogDir} &> ${TxtDir}/AllJobs.txt					# Datei mit allen JobLog Namen erstellen
i=0
while read line 																		# Datei mit Jobs zeilenweise einlesen
do
	x=$(tail -n2 ${JobLogDir}/$line | grep -c 'Job Exit Code: 0')	# Check ob der Job Exit Code stimmt (0 = keine Probleme), x wird dann 1
	i=$(($i+1))
	#echo $i
	if [ $x -ne 1 ]
	then
		line=${line%.log}.sh															#ersetzt .log mit .sh
		echo $line																				#für Testzwecke
		echo $line >> ${TxtDir}/${FailedJobsFile}					#wenn es Probleme gab, Job Namen wegschreiben
	fi
done < ${TxtDir}/AllJobs.txt
echo "Step 1 finished"

while read line 																			# Datei mit fehlgeschlagenen Jobs zeilenweise einlesen
do
	echo ${JobDir}/$line
	#qsub ${JobDir}/$line																#zum testen auskommentiert # Jobs abschicken
	x=$(qstat | grep -c ${user})
	while [ $x -gt 999 ]																#warten, wenn zuviele Jobs schon laufen
	do
		echo "Still some jobs running: waiting to start new jobs"
		date
		sleep 2m
		x=$(qstat | grep -c ${}user})
	done
done < ${TxtDir}/${FailedJobsFile}


