#!/bin/bash

### script to find jobs that didn't produce any output file, new jobs are sent automatically

numberOfFiles=250
TargetNumber=12
GermaniumGeo=43

#DataDir=/data/work/kpha1/rittgen/analysis/COSY
SubDir=Gamma
AnaSubDir=Ana
DataDir=${SIMDATADIR}/${SubDir}/$AnaSubDir
DataSubDir=${DataDir}/CombinedData
MissingFilesFile=MissingFilesAna.txt
AnaFolderListFile=AnaFolderList.txt
TxtDir=${SIMDATADIR}/${SubDir}/txtfiles
rm -f ${TxtDir}/WrongFilesInFolder.txt														# removes log file for erroneous runs , uncomment to automate
mkdir -p ${DataDir}/CombinedData
ls -d ${DataDir}/Ana_Geo${GermaniumGeo}_E**ST${TargetNumber}*_OQP_Psf* -d | wc -w				# write all folders with runs (parameter configs) to file
ls -d ${DataDir}/Ana_Geo${GermaniumGeo}_E**ST${TargetNumber}*_OQP_Psf*  &> ${TxtDir}/${AnaFolderListFile}				# write all folders with runs (parameter configs) to file
errors=0
errorcounter=0

rm ${TxtDir}/${MissingFilesFile}
while read line
do
	line=${line%/}																									### remove '/' from end of line  (shortest pattern matching, %% for longest)
	line=${line##*/}																								### remove longest pattern matching "*/" from the front of line		(# = shortest, ## = longest)
	#echo $line	
	#echo ls ${DataDir}/${line} | wc -w														
	x=$(ls ${DataDir}/${line}/${line}*.root | grep -v Simpara | wc -w )														# get number of files in run, paramater files are excluded

	if [ $x -ne $numberOfFiles ]																								# check if right number of files, otherwise run name to file 
	then
		errors=1
		echo -e "\t\tMissing files in $line   $x"
		for ((j = 1; j<=$numberOfFiles; j++))
		do
			if [ ! -f ${DataDir}/${line}/${line}*__$j.root ]
			#if [ $? -eq 2 ]							### check exit status of ls in the line above, 2 = file not found				, if (file not found) -> write jobname into file
			then
			#echo  ${DataDir}/${line}/${line}__$j.root
				errorcounter=$((errorcounter+1))
				jobline=job_${line}__${j}.sh
				echo -e "$jobline is missing"
				numbers="$numbers $j"
				#echo -e "\t\tjob_${line}_${j}.sh is missing"
				echo $jobline >> ${TxtDir}/${MissingFilesFile}
			fi 
		done
		echo $numbers
		echo $line >> ${TxtDir}/WrongFilesInFolderAna.txt								# file must be empty before!!!!  			 ### this shows the runs with errors
	else
		echo "$line    $x"
	fi
done < ${TxtDir}/${AnaFolderListFile}

if [ $errors -ne 0 ]
then
	echo "there were ${errorcounter} errors, check \"WrongFilesInFolderAna.txt\" to see in which run"
else
	echo "No errors found :)"
fi

### start jobs without output file

#./StartMissingJobsAna.sh
