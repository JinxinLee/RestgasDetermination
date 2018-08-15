#!/bin/bash

### script to find jobs that didn't produce any output file, new jobs are not sent automatically

#numberofEvents=10000000	
numberOfFiles=250
TargetFileNumber=12
#evtsPerFile=$(( $numberofEvents / $numberOfFiles ))

FolderList=SimFolderList.txt
MissingFilesFile=SimMissingFiles.txt
#DataDir=/data/work/kpha1/rittgen/analysis/COSY
SubDir=Gamma
DataDir=${SIMDATADIR}/${SubDir}
DataSubDir=${DataDir}/CombinedData


TxtDir=${SIMDATADIR}/${SubDir}/txtfiles
rm -f ${TxtDir}/WrongFilesInFolder.txt														# removes log file for erroneous runs , uncomment to automate
mkdir -p ${DataDir}/CombinedData
ls -d ${DataDir}/Sim*Geo*${numberofEvents}*${evtsPerFile}*ST${TargetFileNumber}*,*/ &> ${TxtDir}/${FolderList}				# write all folders with runs (parameter configs) to file
ls -d ${DataDir}/Sim*Geo*${numberofEvents}*${evtsPerFile}*ST${TargetFileNumber}*,* | wc -w
errors=0
rm ${TxtDir}/${MissingFilesFile}
errorcounter=0
while read line
do
	line=${line%/}																									### remove '/' from end of line  (shortest pattern matching, %% for longest)
	line=${line##*/}																								### remove longest pattern matching "*/" from the front of line		(# = shortest, ## = longest)
	echo $line	
	#echo ls ${DataDir}/${line} | wc -w														
	x=$(ls ${DataDir}/${line}/${line}*.root | grep -v Simpara | wc -w )														# get number of files in run, paramater files are excluded
	if [ $x -ne $numberOfFiles ]																								# check if right number of files, otherwise run name to file 
	then
		errors=1
		echo "Missing files in $line :  only $x"
		for ((j = 0; j<$numberOfFiles; j++))
		do
			if [ ! -f ${DataDir}/${line}/${line}__$j.root ]
			
			#ls ${DataDir}/${line}/${line}__${j}.root &> /dev/null
			#if [ $? -eq 2 ]							### check exit status of ls in the line above, 2 = file not found				, if (file not found) -> write jobname into file
			then			
				### transform filename to jobname
				#echo ${DataDir}/${line}/${line}__$j.root
				jobline=job_${line}__${j}.sh
				echo -e "$jobline is missing"
				errorcounter=$((errorcounter+1))
				echo $jobline >> ${TxtDir}/${MissingFilesFile}
				#extract info from filename
				geo=$(echo $line | cut -d '_' -f 2)
				geo=$(echo $geo | cut -d 'o' -f 2	)
				Energy=$(echo $line | cut -d '_' -f 3)
				Energy=$(echo $Energy | cut -d 'E' -f 2	)
				Energy=$(echo $Energy | cut -d 'M' -f 1	)
				allevents=$(echo $line | cut -d '_' -f 4)
				allevents=$(echo $allevents | cut -d 's' -f 2	)
				events=$(echo $line | cut -d '_' -f 5)
				events=$(echo $events | cut -d 's' -f 2	)
				gen=$(echo $line | cut -d '_' -f 6)
				gen=$(echo $gen | cut -d 'n' -f 2	)
				SekTar=$(echo $line | cut -d '_' -f 7)
				SekTar=$(echo $SekTar | cut -d 'T' -f 2	)
				if [[ $line == *"mu"* ]]																								# check if right number of files, otherwise run name to file 
				then
					mu=$(echo $line | cut -d "mu" -f 2)
					mu=$(echo $mu | cut -d ',' -f 1	)
					Q=$(echo $line | cut -d 'Q' -f 2)
					Q=$(echo $Q | cut -d '_' -f 1	)
					echo mu$mu    Q$Q
					echo ./script_Gamma_SimArray.sh -g $geo -e $allevents -G $gen -E $Energy -s $SekTar -n 1 -i $j 
				else
					echo nope
					#./script_Gamma_SimArray.sh -g $geo -e $allevents -G $gen -E $Energy -s $SekTar -n 1 -i $j 
				fi
					
				#./script_Gamma_SimArray.sh -g $geo -e $allevents -G $gen -E $Energy -s $SekTar -n 1 -i $j 
			fi 
		done
		echo $line >> ${TxtDir}/WrongFilesInFolder.txt								# file must be empty before!!!!  			 ### this shows the runs with errors
	else
		echo "$line    $x"
		#ls ${DataDir}/${line}/Sim*.root | grep -v Simpara | xargs hadd -f1 -T ${DataSubDir}/${line}.root				# lists all root files in the run folder and combine them into one file, xargs appends the output of the ls to the command that follows
		echo "File ${DataSubDir}/${line}.root created"
	fi
done < ${TxtDir}/${FolderList}

if [ $errors -ne 0 ]
then
	echo "there were ${errorcounter} error(s), check \"WrongFilesInFolder.txt\" to see in which run"
else
	echo "No errors found :)"
fi

### start jobs without output file

#./StartMissingjobs.sh
