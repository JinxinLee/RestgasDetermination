#!/bin/bash

TargetNumber=12
GermaniumGeo=43

subdir=Gamma
Anasubdir=Ana
Combodir=CombinedData
path=${SIMDATADIR}/$subdir/$Anasubdir
if [ ! -d $path ]; then 
  mkdir -p $path
fi
Combopath=${SIMDATADIR}/$subdir/$Anasubdir/${Combodir}
if [ ! -d $path ]; then 
  mkdir -p $path
fi
ls -d ${path}/Ana_Geo${GermaniumGeo}_E**ST${TargetNumber}*_OQP_Psf* -d | wc -w
ls -d ${path}/Ana_Geo${GermaniumGeo}_E**ST${TargetNumber}*_OQP_Psf* -d > ${SIMDATADIR}/$subdir/txtfiles/AnaFolderstoCombine.txt

joblogpath=$path/joblogs
if [ ! -d $joblogpath ]; then 
  mkdir -p $joblogpath
fi


jobpath=$path/jobs
if [ ! -d $jobpath ]; then 
  mkdir -p $jobpath
fi

i=1
while read line; 
do
	
	echo $line
	line=${line%/}																									### remove '/' from end of line  (shortest pattern matching, %% for longest)
	folder=${line##*/}																								### remove longest pattern matching "*/" from the front of line		(# = shortest, ## = longest)
	cat >$jobpath/combojob_${folder}.sh <<EOF
#!/bin/bash

#PBS -N combojob_${folder}.
#PBS -j oe
#PBS -o ${joblogpath}/combojob_${folder}.log
#PBS -V
#PBS -l nodes=1:ppn=1,walltime=02:00:00

export PATH=\$PBS_O_PATH
cd \$PBS_O_WORKDIR
	hadd -f1 ${Combopath}/Combined_${folder}.root ${path}/${folder}/Ana*.root
EOF
	qsub $jobpath/combojob_${folder}.sh
done < ${SIMDATADIR}/$subdir/txtfiles/AnaFolderstoCombine.txt

