# This script runs hypGe macros 

# reads number of Events, name of run and verbose Level
DefnEvts=10000000
DefGeometry=36
DefEnergy=0.00001
DefaddSecTar=0


echo "Enter number of Events (Default 10000000)":
read nEvts
if [ -z "$nEvts" ]; then 
	nEvts=$DefnEvts
fi
echo "Number of Events: $nEvts!"

echo "Which geometry (enter number, Default ${DefGeometry}) NOT ALL IMPLEMENTED YET!!!(only default!!!)":
echo "3 hypGe_GeoMarcell.root"
echo 31 hypGeGeoTripleCluster_V3CrystalsOnly.root
echo 32 hypGeGeoTripleCluster_StraightCrystalsOnly.root
echo 33 hypGeGeoTripleCluster_Ball40_Offset10CrystalsOnly.root
echo 34 hypGeGeoTripleCluster_Ball40_Offset20CrystalsOnly.root
echo 35 hypGeGeoTripleCluster_Ball40_Offset10_STTFitting.root
echo 36 hypGeGeoTripleCluster_Ball40_Offset20_STTFitting.root
echo 2 hypGe_GeoMarcell_2erCrystalsOnly.root
echo 21 hypGeGeoDoubleCluster_V3CrystalsOnly.root
read Geometry
if [ -z "$Geometry" ]; then 
	Geometry=$DefGeometry
fi
echo "Geometry: $Geometry!"

#step=`expr $nEvts / $nJobs`

echo "Enter gamma energy in GeV(Default 0.00001 GeV)":
read Energy
if [ -z "$Energy" ]; then 
	Energy=$DefEnergy
fi
echo "Energy: $Energy!"

echo "Add Secondary Target for gamma absorption (1 or 0; Default: 0)":
read addSecTar
if [ -z "$addSecTar" ]; then 
	addSecTar=$DefaddSecTar
fi
echo "Add Secondary Target: $addSecTar!"
# make data directory, if not there. all data file go there, makes them easier to move around

path=${SIMDATADIR}/CrystalsOnly/joblogs

if [ ! -d $path ]; then 
  mkdir -p $path
fi

SimLogpath=${SIMDATADIR}/CrystalsOnly/simlogs
if [ ! -d $SimLogpath ]; then 
  mkdir -p $SimLogpath
fi

jobpath=${SIMDATADIR}/CrystalsOnly/jobs
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

if [ $addSecTar -eq 0 ]; then
	cat >$jobpath/job_${Geometry}_${nEvts}Evts_${Energy}GeV.sh <<EOF
	#!/bin/bash
	#
	#PBS -N dpm.${Geometry}_${nEvts}Evts_${Energy}GeV
	#PBS -j oe
	#PBS -o ${path}/job_${Geometry}_${nEvts}Evts_${Energy}GeV.log
	#PBS -V
	#PBS -l nodes=1:ppn=1,walltime=10:00:00

	export PATH=\$PBS_O_PATH
	cd \$PBS_O_WORKDIR

	echo "Start PandaRoot HypGe Simulation $i with $nEvts events, seed  and name ${Geometry}_${nEvts}Evts_${Energy}GeV."
	root -l -q -b sim_hypGe_CrystalsOnly.C\($nEvts\,$Geometry\,$Energy\,$addSecTar\) &> $SimLogpath/sim_${Geometry}_${nEvts}Evts_${Energy}GeV.log


EOF
	echo "Just HypGe"
### submit job to batch system
	qsub $jobpath/job_${Geometry}_${nEvts}Evts_${Energy}GeV.sh
	
else

	cat >$jobpath/job_${Geometry}_${nEvts}Evts_${Energy}GeV_SecTar.sh <<EOF
	#!/bin/bash
	#
	#PBS -N dpm.${Geometry}_${nEvts}Evts_${Energy}GeV_SecTar
	#PBS -j oe
	#PBS -o ${path}/job_${Geometry}_${nEvts}Evts_${Energy}GeV_SecTar.log
	#PBS -V
	#PBS -l nodes=1:x86_64,walltime=10:00:00

	export PATH=\$PBS_O_PATH
	cd \$PBS_O_WORKDIR

	echo "Start PandaRoot HypGe Simulation $i with $nEvts events, seed  and name ${Geometry}_${nEvts}Evts_${Energy}GeV. Secondary Target added for absorption of gamma"
	root -l -q -b sim_hypGe_extparameters.C\($nEvts\,$Geometry\,$Energy\,$addSecTar\) &> $SimLogpath/sim_${Geometry}_${nEvts}Evts_${Energy}GeV_SecTar.log


EOF

		echo "HypGe + passive SecTar"
### submit job to batch system
	qsub $jobpath/job_${Geometry}_${nEvts}Evts_${Energy}GeV_SecTar.sh

fi

#done
