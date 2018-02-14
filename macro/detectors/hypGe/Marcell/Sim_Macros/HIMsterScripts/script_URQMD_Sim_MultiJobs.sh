# This script runs hypGe urqmd neutron macros 

# reads number of Events, name of run and verbose Level
DefnEvts=50000
DefGeometry=36
DefiUrqmd=1
DefnJobs=10
DefaddSecTar=1



echo "Enter number of Events (Default $DefnEvts)":
read nEvts
if [ -z "$nEvts" ]; then 
	nEvts=$DefnEvts
fi
echo "Number of Events: $nEvts!"
echo "Enter number of jobs (Default $DefnJobs)" 
read nJobs
if [ -z "$nJobs" ]; then 
	nJobs=$DefnJobs
fi
echo "Number of jobs: $nJobs!"

echo "Which geometry (enter number, Default $DefGeometry)":
echo "3 hypGe_GeoMarcell.root"
echo 31 hypGeGeoTripleCluster_V3.root
echo 32 hypGeGeoTripleCluster_Straight.root
echo 33 hypGeGeoTripleCluster_Ball40_Offset10.root
echo 34 hypGeGeoTripleCluster_Ball40_Offset20.root
echo 35 hypGeGeoTripleCluster_Ball40_Offset10_STTFitting.root
echo 36 hypGeGeoTripleCluster_Ball40_Offset20_STTFitting.root
echo 2 hypGe_GeoMarcell_2er.root
echo 21 hypGeGeoDoubleCluster_V3.root
read Geometry
if [ -z "$Geometry" ]; then 
	Geometry=$DefGeometry
fi
echo "Geometry: $Geometry!"

echo "Add Secondary Target for gamma absorption (1 or 0; Default: $DefaddSecTar)":
read addSecTar
if [ -z "$addSecTar" ]; then 
	addSecTar=$DefaddSecTar
fi
echo "Add Secondary Target: $addSecTar!"

echo "Which urqmd File? (1 - 10)"
read iUrqmd
if [ -z "$iUrqmd" ]; then 
	iUrqmd=$DefiUrqmd
fi
echo "urqmdFile: $iUrqmd!" 
#step=`expr $nEvts / $nJobs`



# make data directory, if not there. all data file go there, makes them easier to move around

path=${SIMDATADIR}/Neutron

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

jobpath=$path/jobs
if [ ! -d $jobpath ]; then 
  mkdir -p $jobpath
fi

#if [ ! -d jobs/${folder}/ ]; then 
#  mkdir -p jobs/${folder}/
#fi

# let's go

#last=`expr $nEvts - 1`
#for start in `seq 0 $step $last` ; do
#name="run"$start
#seed=$(expr 1 \+ $seed)
EvtsPerJob=$(($nEvts/$nJobs))
iJob=0
for Geometry in 36 41 42
do
for iUrqmd in {1..10}
do
for (( i=0; i<${nEvts};i=i+$(($nEvts/$nJobs)) ))
do
	iJob=$(($iJob+1))
	jobname=job_${Geometry}_${EvtsPerJob}Evts_urqmd${iUrqmd}_StartEvent_${i}_SekTar${addSecTar}
	cat >${jobpath}/${jobname}.sh <<EOF
	#!/bin/bash
	#
	#PBS -N $jobname
	#PBS -j oe
	#PBS -o ${joblogpath}/$jobname.log
	#PBS -V
	#PBS -l nodes=1:ppn=1,walltime=20:00:00

	export PATH=\$PBS_O_PATH
	cd \$PBS_O_WORKDIR

	echo "Start PandaRoot Simulation $jobname."
	root -l -q -b ../sim_hypGe_extparameters_urqmd.C\($EvtsPerJob\,$Geometry\,$iUrqmd\,$i\,$addSecTar\) &> ${SimLogpath}/sim_${jobname}.log


EOF
	
	echo "root -l -q -b ../sim_hypGe_extparameters_urqmd.C\($EvtsPerJob\,$Geometry\,$iUrqmd\,$i\,$addSecTar\)"

	### submit job to batch system
	qsub ${jobpath}/${jobname}.sh
	#root -l -q -b sim_hypGe_extparameters_urqmd.C\($EvtsPerJob\,$Geometry\,$iUrqmd\,$i\,$addSecTar\)
done
done
done
echo $iJob
