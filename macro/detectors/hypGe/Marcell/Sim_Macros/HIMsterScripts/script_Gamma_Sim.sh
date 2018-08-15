#!/bin/bash

# This script runs hypGe macros 

# reads number of Events, name of run and verbose Level
nEvts=10000000
nJobs=200
Geometry=36
Energy=1
GenType=1
addSecTar=22

while getopts ":E:e:n:g:G:s:" minvalue ; do
case $minvalue in
	E) Energy=$OPTARG;;
	e) nEvts=$OPTARG;;
	n) nJobs=$OPTARG;;
	g) Geometry=$OPTARG;;
	G) GenType=$OPTARG;;
	s) addSecTar=$OPTARG;;
esac
done	
echo "E $Energy, e $nEvts, n $nJobs, g $Geometry , G $GenType , s $addSecTar"

subdir=Gamma

JobMax=$(( 1000 - 1))
evtpJob=$(($nEvts / $nJobs))

#echo "Enter number of Events (Default $DefnEvts)":
#read nEvts
#if [ -z "$nEvts" ]; then 
	#nEvts=$DefnEvts
#fi
#echo "Number of Events: $nEvts!"

#echo "Enter number of Jobs (Default $DefnJobs)":
#read nJobs
#if [ -z "$nJobs" ]; then 
	#nJobs=$DefnJobs
#fi
#echo "Number of Jobs: $nJobs!"
#JobMax=$(( 1000 - 1))
#echo $JobMax
#evtpJob=$(($nEvts / $nJobs))
#echo "Events per Job: $evtpJob"

#echo "Which geometry (enter number, Default $DefGeometry):"
#echo "3 hypGe_GeoMarcell.root"
#echo "31 hypGeGeoTripleCluster_V3.root"
#echo "32 hypGeGeoTripleCluster_Straight.root"
#echo "33 hypGeGeoTripleCluster_Ball40_Offset10.root"
#echo "34 hypGeGeoTripleCluster_Ball40_Offset20.root"
#echo "35 hypGeGeoTripleCluster_Ball40_Offset10_STTFitting.root"
#echo "36 hypGeGeoTripleCluster_Ball40_Offset20_STTFitting.root"
#echo "2 hypGe_GeoMarcell_2er.root"
#echo "21 hypGeGeoDoubleCluster_V3.root"
#read Geometry
#if [ -z "$Geometry" ]; then 
	#Geometry=$DefGeometry
#fi
#echo "Geometry: $Geometry!"


##echo "Enter gamma energy in GeV(Default 1 MeV):"
##read Energy
##if [ -z "$Energy" ]; then 
##	Energy=$DefEnergy
##fi
##echo "Energy: $Energy!"
#echo "Simulation runs with multiple gamma energies, see script file"

#echo "Which generator (enter number, Default $DefGenType):"
#echo "0 boxgen"
#echo "1 hypgen"
#read GenType
#if [ -z "$GenType" ]; then 
	#GenType=$DefGenType
#fi
#echo "Generator: $GenType!"

#echo "Add Secondary Target for gamma absorption ( Default: $DefaddSecTar):"
#echo "0 no ST"
#echo "1 TargetSystem_WindowTi_filledCorners_TcT1500um_longBP.root"
#echo "2 TargetSystem_AbsWindow_filledCorners_addSens.root"
#echo "3 TargetSystem_WindowAbsB_insideAbsB_primTarget.root"
#echo "4 hypTargetSystem_XiAtoms_v2_iron.root"
#echo "5 hypTargetSystem_XiAtoms_v1_tantalum.root"
#echo "6 hypTargetSystem_XiAtoms_v2_tantalum.root"
#echo "7 hypTargetSystem_XiAtoms_v2_iron_ST.root"
#echo "8 hypTargetSystem_XiAtoms_v2_tantalum_ST.root"
#read addSecTar
#if [ -z "$addSecTar" ]; then 
	#addSecTar=$DefaddSecTar
#fi
#echo "Add Secondary Target: $addSecTar!"

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


#for Energy in 0.500 1.000 1.332 2.000 4.000 6.000 8.000 10.000 1.684 2.429 2.78 3.049 4.704 5.59 6.38 6.76 7.94
#for Energy in 1.684 2.429 2.78 3.049 4.704 5.59 6.38 6.76 7.94
#for Energy in 0.200 0.300 0.400
#for Energy in 0.050 0.100 0.150 0.200 0.250 0.284 0.300 0.350 0.400 0.450 0.500 0.550 0.600 0.650 0.700 0.750 0.800 0.850 0.900 0.950 1.000
#for Energy in 0.050 .150 0.250 0.350 0.450 0.550 0.650 0.750 0.850 0.950
#for Energy in 0.150

#do
	for ((JobNr=0; JobNr<$nJobs; JobNr++ ))
	do
		##Sim_Geo36_E1.332MeV_Evts1000_FileEvts200_Gen1_ST1__1

		fileadd=Sim_Geo${Geometry}_E${Energy}MeV_Evts${nEvts}_FileEvts${evtpJob}_Gen${GenType}_ST${addSecTar}__${JobNr}
		echostring="Start PandaRoot HypGe Simulation $JobNr of ${nJobs} with $evtpJob events and name ${fileadd}."
		rootstring="root -l -q -b ../sim_Gamma.C\($Geometry\,$Energy\,$nEvts\,$evtpJob\,$GenType\,$addSecTar\,$JobNr\)"
		#echo echostring="Start PandaRoot HypGe Simulation $JobNr of ${nJobs} with $evtpJob events and name ${fileadd}."
		echo rootstring="root -l -q -b ../sim_Gamma.C\($Geometry\,$Energy\,$nEvts\,$evtpJob\,$GenType\,$addSecTar\,$JobNr\)"
		if [ $IsHimster -eq 1 ] ; 
		then
		

		
				cat >$jobpath/job_${fileadd}.sh <<EOF
#!/bin/bash

#PBS -N Sim_$subdir.${fileadd}
#PBS -j oe
#PBS -o ${joblogpath}/job_${fileadd}.log
#PBS -V
#PBS -l nodes=1:ppn=1,walltime=02:00:00

export PATH=\$PBS_O_PATH
cd \$PBS_O_WORKDIR

echo $echostring

$rootstring &> $TempSimLogpath/sim_${fileadd}.log ##/dev/null   ##
mv $TempSimLogpath/sim_${fileadd}.log $SimLogpath/sim_${fileadd}.log
cat ${joblogpath}/job_${fileadd}.log >> $SimLogpath/sim_${fileadd}.log
EOF
### submit job to batch system
				qsub $jobpath/job_${fileadd}.sh
			else
				$rootstring 2>&1 | tee $SimLogpath/sim_${fileadd}.log
			fi
		x=$(qstat | grep -c ${USER})		
		while [ $x -gt $JobMax ]
		do
			echo "Still some jobs running: waiting to start new jobs"
			date
			sleep 2m #30s
			#x=$(ls | grep -c ext)
			x=$(qstat | grep -c ${USER})
		done			### end of while "double queue" loop
	done
#done
#done
