# This script runs hypGe macros 

# reads number of Events, name of run and verbose Level
DefnEvts=10000
DefGeometry=3
DefEnergy=0.001
DefaddSecTar=0


echo "Enter number of Events (Default 10000)":
read nEvts
if [ -z "$nEvts" ]; then 
	nEvts=$DefnEvts
fi
echo "Number of Events: $nEvts!"

#echo "Which geometry (enter number, Default 3)":
#echo "3 hypGe_GeoMarcell.root"
#echo 31 hypGeGeoTripleCluster_V3.root
#echo 32 hypGeGeoTripleCluster_Straight.root
#echo 33 hypGeGeoTripleCluster_Ball40_Offset10.root
#echo 34 hypGeGeoTripleCluster_Ball40_Offset20.root
#echo 2 hypGe_GeoMarcell_2er.root
#echo 21 hypGeGeoDoubleCluster_V3.root
#read Geometry
#if [ -z "$Geometry" ]; then 
	Geometry=$DefGeometry
#fi
#echo "Geometry: $Geometry!"

#step=`expr $nEvts / $nJobs`

echo "Enter gamma energy in GeV(Default 0.001 GeV)":
read Energy
if [ -z "$Energy" ]; then 
	Energy=$DefEnergy
fi
echo "Energy: $Energy!"

#echo "Add Secondary Target for gamma absorption (1 or 0; Default: 0)":
#read addSecTar
#if [ -z "$addSecTar" ]; then 
	addSecTar=$DefaddSecTar
#fi
#echo "Add Secondary Target: $addSecTar!"
# make data directory, if not there. all data file go there, makes them easier to move around

path=${SIMDATADIR}/GeantTest/joblogs

if [ ! -d $path ]; then 
  mkdir -p $path
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
	cat >jobs/job_${Geometry}_${nEvts}Evts_${Energy}GeV.sh <<EOF
	#!/bin/bash
	#
	#PBS -N dpm.${Geometry}_${nEvts}Evts_${Energy}GeV
	#PBS -j oe
	#PBS -o ${path}/0-job_TestGeo_${nEvts}Evts_${Energy}GeV.log
	#PBS -V
	#PBS -l nodes=1:x86_64,walltime=10:00:00

	export PATH=\$PBS_O_PATH
	cd \$PBS_O_WORKDIR

	echo "Start PandaRoot HypGe Simulation $i with $nEvts events, seed  and name ${Geometry}_${nEvts}Evts_${Energy}GeV."
	root -l -q -b ../sim_hypGe_TestGeo.C\($nEvts\,$Geometry\,$Energy\,$addSecTar\) &> ${SIMDATADIR}/GeantTest/0-sim_TestGeo_${nEvts}Evts_${Energy}GeV.log


EOF
	echo "Just HypGe"
### submit job to batch system
	#qsub jobs/job_${Geometry}_${nEvts}Evts_${Energy}GeV.sh
	root -l -q -b ../sim_hypGe_TestGeo.C\($nEvts\,$Geometry\,$Energy\,$addSecTar\)
else

	cat >jobs/job_${Geometry}_${nEvts}Evts_${Energy}GeV_SecTar.sh <<EOF
	#!/bin/bash
	#
	#PBS -N dpm.${Geometry}_${nEvts}Evts_${Energy}GeV_SecTar
	#PBS -j oe
	#PBS -o ${path}/0-job_${Geometry}_${nEvts}Evts_${Energy}GeV_SecTar.log
	#PBS -V
	#PBS -l nodes=1:x86_64,walltime=10:00:00

	export PATH=\$PBS_O_PATH
	cd \$PBS_O_WORKDIR

	echo "Start PandaRoot HypGe Simulation $i with $nEvts events, seed  and name ${Geometry}_${nEvts}Evts_${Energy}GeV. Secondary Target added for absorption of gamma"
	root -l -q -b ../sim_hypGe_extparameters.C\($nEvts\,$Geometry\,$Energy\,$addSecTar\) &> ${SIMDATADIR}/0-sim_${Geometry}_${nEvts}Evts_${Energy}GeV_SecTar.log


EOF

		echo "HypGe + passive SecTar"
### submit job to batch system
	#qsub jobs/job_${Geometry}_${nEvts}Evts_${Energy}GeV_SecTar.sh
	root -l -q -b ../sim_hypGe_TestGeo.C\($nEvts\,$Geometry\,$Energy\,$addSecTar\)
fi

#done
