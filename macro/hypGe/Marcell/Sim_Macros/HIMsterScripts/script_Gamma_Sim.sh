# This script runs hypGe macros 

# reads number of Events, name of run and verbose Level
DefnEvts=10000000
DefnJobs=1
DefGeometry=36
#DefEnergy=0.001
DefGenType=0
DefaddSecTar=0



subdir=Gamma



echo "Enter number of Events (Default $DefnEvts)":
read nEvts
if [ -z "$nEvts" ]; then 
	nEvts=$DefnEvts
fi
echo "Number of Events: $nEvts!"

echo "Enter number of Jobs (Default $DefnJobs)":
read nJobs
if [ -z "$nJobs" ]; then 
	nJobs=$DefnJobs
fi
echo "Number of Jobs: $nJobs!"

evtpJob=$(($nEvts / $nJobs))
echo "Events per Job: $evtpJob"

echo "Which geometry (enter number, Default $DefGeometry):"
echo "3 hypGe_GeoMarcell.root"
echo "31 hypGeGeoTripleCluster_V3.root"
echo "32 hypGeGeoTripleCluster_Straight.root"
echo "33 hypGeGeoTripleCluster_Ball40_Offset10.root"
echo "34 hypGeGeoTripleCluster_Ball40_Offset20.root"
echo "35 hypGeGeoTripleCluster_Ball40_Offset10_STTFitting.root"
echo "36 hypGeGeoTripleCluster_Ball40_Offset20_STTFitting.root"
echo "2 hypGe_GeoMarcell_2er.root"
echo "21 hypGeGeoDoubleCluster_V3.root"
read Geometry
if [ -z "$Geometry" ]; then 
	Geometry=$DefGeometry
fi
echo "Geometry: $Geometry!"


#echo "Enter gamma energy in GeV(Default 0.001 GeV):"
#read Energy
#if [ -z "$Energy" ]; then 
#	Energy=$DefEnergy
#fi
#echo "Energy: $Energy!"
echo "Simulation runs with 0.5,1,1.332,2,4,8 MeV"

echo "Which generator (enter number, Default $DefGenType):"
echo "0 boxgen"
echo "1 hypgen"
read GenType
if [ -z "$GenType" ]; then 
	GenType=$DefGenType
fi
echo "Generator: $GenType!"

echo "Add Secondary Target for gamma absorption ( Default: $DefaddSecTar):"
echo "0 no ST"
echo "1 TargetSystem_Ti_TcT150um_filledCorners_addSens.root"
echo "2 TargetSystem_AbsWindow_filledCorners_addSens.root"
read addSecTar
if [ -z "$addSecTar" ]; then 
	addSecTar=$DefaddSecTar
fi
echo "Add Secondary Target: $addSecTar!"

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

jobpath=$path/jobs
if [ ! -d $jobpath ]; then 
  mkdir -p $jobpath
fi
#if [ ! -d jobs/${folder}/ ]; then 
#  mkdir -p jobs/${folder}/
#fi

# let's go

if uname -a | grep himster ; 
then
	IsHimster=1
else
	IsHimster=0
fi


#for Energy in 0.0005 0.001 0.001332 0.002 0.004 0.008
for Energy in 0.001
do
	for ((JobNr=0; JobNr<$nJobs; JobNr++ ))
	do

		fileadd=${Geometry}_${evtpJob}Evts_${Energy}GeV_ST${addSecTar}_Gen${GenType}_${JobNr}
		echostring="Start PandaRoot HypGe Simulation $JobNr of ${nJobs} with $evtpJob events and name ${fileadd}."
		rootstring="root -l -q -b ../sim_Gamma.C($evtpJob,$Geometry,$Energy,$GenType,$addSecTar,$nEvts,$JobNr)"

		if [ $IsHimster -eq 1 ] ; 
		then
				cat >$jobpath/job_${fileadd}.sh <<EOF
#!/bin/bash
#
#PBS -N Sim_$subdir.${fileadd}
#PBS -j oe
#PBS -o ${joblogpath}/job_${fileadd}.log
#PBS -V
#PBS -l nodes=1:ppn=1,walltime=20:00:00

export PATH=\$PBS_O_PATH
cd \$PBS_O_WORKDIR

echo $echostring
$rootstring &> $SimLogpath/sim_${fileadd}.log
EOF
### submit job to batch system
				qsub $jobpath/job_${fileadd}.sh
			else
				$rootstring 2>&1 | tee $SimLogpath/sim_${fileadd}.log
			fi

	done
done
#done
