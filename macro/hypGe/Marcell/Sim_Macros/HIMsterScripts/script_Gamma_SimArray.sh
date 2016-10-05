#!/bin/bash

# This script runs hypGe macros 

# reads number of Events, name of run and verbose Level
nEvts=10000000
nJobs=200
Geometry=36
Energy=1
GenType=1
addSecTar=22
lowerLimit=1
evtpJobExt=50000

while getopts ":E:e:n:g:G:s:i:p" minvalue ; do
case $minvalue in
	E) Energy=$OPTARG;;
	e) nEvts=$OPTARG;;
	n) nJobs=$OPTARG;;
	g) Geometry=$OPTARG;;
	G) GenType=$OPTARG;;
	s) addSecTar=$OPTARG;;
	i) lowerLimit=$OPTARG ;;
	p) evtpJobExt=$OPTARG ;;
esac
done	
echo "E $Energy, e $nEvts, n $nJobs, g $Geometry , G $GenType , s $addSecTar, i $lowerLimit"

subdir=Gamma

JobMax=$(( 1000 - 1))
#evtpJob=$(($nEvts / $nJobs))
if [ $nJobs -eq 1 ]
then
	evtpJob=$(($nEvts / 250))
else
	evtpJob=$(($nEvts / 250))
fi


# make data directory, if not there. all data file go there, makes them easier to move around


path=${SIMDATADIR}/$subdir

if [ ! -d $path ]; then 
  mkdir -p $path
fi

joblogpath=$path/joblogs

if [ ! -d $joblogpath ]; then 
  mkdir -p $joblogpath
fi

SimLogpath=$path/simlogs
if [ ! -d $SimLogpath ]; then 
  mkdir -p $SimLogpath
fi

TempSimLogpath=~/work/Tempsimlogs
if [ ! -d $TempSimLogpath ]; then 
  mkdir -p $TempSimLogpath
fi
jobpath=$path/jobs
if [ ! -d $jobpath ]; then 
  mkdir -p $jobpath
fi

# let's go

fileadd=Sim_Geo${Geometry}_E${Energy}MeV_Evts${nEvts}_FileEvts${evtpJob}_Gen${GenType}_ST${addSecTar}

		

		
if [ ! -f job_GammaSim.sh ]
then
				cat >job_GammaSim.sh <<EOF
#!/bin/bash
#PBS -j oe
#PBS -o $SimLogpath/Sim_Geo\${qGeometry}_E\${qEnergy}MeV_Evts\${qnEvts}_FileEvts\${qevtpJob}_Gen\${qGenType}_ST\${qaddSecTar}__\${PBS_ARRAYID}.log
#PBS -V
### #PBS -l nodes=1:ppn=1,walltime=02:00:00,mem=4000mb
### #PBS -l nodes=1:ppn=1,walltime=02:00:00

export PATH=\${PBS_O_PATH}
cd \${PBS_O_WORKDIR}

SUB_ID=\${PBS_ARRAYID}

echo root -l -q -b '../sim_Gamma.C('\${qGeometry}', '\${qEnergy}', '\${qnEvts}', '\${qevtpJob}' , ' \${qGenType} ',  '\${qaddSecTar} ', '\${SUB_ID}')'

root -l -q -b '../sim_Gamma.C('\${qGeometry}','\${qEnergy}','\${qnEvts}','\${qevtpJob}','\${qGenType}','\${qaddSecTar}','\${SUB_ID}')'
echo blablabla
EOF
fi

fileadd=Sim_Geo${Geometry}_E${Energy}MeV_Evts${nEvts}_FileEvts${evtpJob}_Gen${GenType}_ST${addSecTar}
counter=0

echo -e "for ((i=$lowerLimit; i < $((nJobs+lowerLimit-100)); i=$((i+100)) ))"
for ((i=$lowerLimit; i < $((nJobs+lowerLimit-100)); i=$((i+100)) ))
do
	counter=$((counter+1))
	upperLimit=$((counter * 100+lowerLimit-1))	 
	echo qsub -V -t $i-${upperLimit} -N ${fileadd} -v qGeometry=\"${Geometry}\",qEnergy=\"${Energy}\",qnEvts=\"${nEvts}\",qevtpJob=\"$evtpJob\",qGenType=\"$GenType\",qaddSecTar=\"$addSecTar\" job_GammaSim.sh
	qsub -V -t $i-${upperLimit} -N ${fileadd} -v qGeometry=\"${Geometry}\",qEnergy=\"${Energy}\",qnEvts=\"${nEvts}\",qevtpJob=\"$evtpJob\",qGenType=\"$GenType\",qaddSecTar=\"$addSecTar\" job_GammaSim.sh
done	
echo "remaining jobs"
	lowerRemaining=$((counter * 100 +lowerLimit))
	echo qsub -V -t $lowerRemaining-$((lowerLimit+nJobs-1)) -N ${fileadd} -v qGeometry=\"${Geometry}\",qEnergy=\"${Energy}\",qnEvts=\"${nEvts}\",qevtpJob=\"$evtpJob\",qGenType=\"$GenType\",qaddSecTar=\"$addSecTar\" job_GammaSim.sh
	qsub -V -t $lowerRemaining-$((lowerLimit+nJobs-1)) -N ${fileadd} -v qGeometry=\"${Geometry}\",qEnergy=\"${Energy}\",qnEvts=\"${nEvts}\",qevtpJob=\"$evtpJob\",qGenType=\"$GenType\",qaddSecTar=\"$addSecTar\" job_GammaSim.sh
