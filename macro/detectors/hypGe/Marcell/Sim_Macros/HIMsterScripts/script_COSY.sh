#!/bin/bash
# This script runs COSY beam time simulation macros 

#some default values
DefnEvts=10000000
DefPDG=2212
DefProtonMom=2.78
DefGaus=1
DefBrems=1
DefDetectorRadius=30
DefTargetThickness=5
DefTargetAngle=0
DefWithDump=0
DefDetectorRadius=30
DefDumpDist=200
# #reads number of Events and beam particle
echo "Enter number of Events (Default ${DefnEvts})":
read nEvts
if [ -z "$nEvts" ]; then 
	nEvts=$DefnEvts
fi
echo "Number of Events: $nEvts!"

echo "Which beam particle? Enter PDG code:"
echo -e "\t proton:\t\t 2212 (default)"
echo -e "\t antiproton:\t -2212":
#read PDG
if [ -z "$PDG" ]; then 
	PDG=$DefPDG
fi
echo "PDG: $PDG!"

echo "Enter primary particle momentum  in GeV (Default ${DefProtonMom})":
read ProtonMom
if [ -z "$ProtonMom" ]; then 
	ProtonMom=$DefProtonMom
fi
echo "primary particle Momentum: $ProtonMom GeV!"

echo "Should the beam position (x,y) be smeared by a gausian shape?":
echo -e "\t no:\t\t 0 (default)"
echo -e "\t yes\t\t 1"
#read useGaus
if [ -z "$useGaus" ]; then 
	useGaus=$DefGaus
fi
echo "Gausian smearing of beam: $useGaus!"


echo "With bremstrahlung? (check $VMCWORKDIR/gconfig/SetCuts.C)"
echo -e "\t no:\t\t 0 "
echo -e "\t yes\t\t 1 (default)" 
#read Brems
if [ -z "$Brems" ]; then 
	Brems=$DefBrems
fi
echo "Which detector radius? (Default: 30 cm)"
#read DetectorRadius
if [ -z "$DetectorRadius" ]; then 
	DetectorRadius=$DefDetectorRadius
fi

echo "Target Thickness: $TargetThickness cm!"
echo "Which Target Thickness? (Default: 5 cm)"
#read TargetThickness
if [ -z "$TargetThickness" ]; then 
	TargetThickness=$DefTargetThickness
fi
echo "Target Thickness: $TargetThickness cm!"

echo "Which Target Angle? (Default: 0°)"
read TargetAngle
if [ -z "$TargetAngle" ]; then 
	TargetAngle=$DefTargetAngle
fi
echo "Target Angle: $TargetAngle °"

echo "With Beam Dump? "
echo -e "\t no:\t\t 0 "
echo -e "\t yes\t\t 1 (default)" 
#read WithDump
if [ -z "$WithDump" ]; then 
	WithDump=$DefWithDump
fi
echo "Use Dump: $WithDump"

if [ $WithDump -eq 1 ];
then
	echo "Distance of beam Dump? (Default: $DefDumpDist cm) "
	#read DumpDist
	
	echo "Dump distance: $DumpDist"
	if [ ! -f ${VMCWORKDIR}/geometry/hypGeGeoCOSYBeamDumpTOF_dist${DumpDist}cm.root ]; then
		echo " BeamDump geometry does not yet exist! Creating"
	root -l -q -b ../Geometry/hypGeGeoCOSYBeamDumpTOF.C\($DumpDist\)
	fi
fi 
if [ -z "$DumpDist" ];
then 
	DumpDist=$DefDumpDist
fi

SubPath=COSY

JobLogPath=${SIMDATADIR}/${SubPath}/joblogs
if [ ! -d $JobLogPath ]; then 
  mkdir -p $JobLogPath
fi

SimLogPath=${SIMDATADIR}/${SubPath}/simlogs
if [ ! -d $SimLogPath ]; then 
  mkdir -p $SimLogPath
fi

JobPath=${SIMDATADIR}/${SubPath}/jobs
if [ ! -d $JobPath ]; then 
  mkdir -p $JobPath
fi
echo

if [ ! -f ${VMCWORKDIR}/geometry/hypGeGeoSingle_radius${DetectorRadius}cm.root ]; then
	echo " Detector geometry does not yet exist! Creating"
	root -l -q -b ../Geometry/hypGeGeoBuilderSingle.C\($DetectorRadius\)
fi

if [ ! -f ${VMCWORKDIR}/geometry/hypGeGeoCOSYTarget_${TargetThickness}cm_${TargetAngle}_Degree.root ]; then
	echo "Target geometry does not yet exist! Creating"
	root -l -q -b ../Geometry/hypGeGeoCOSYTarget.C\($TargetThickness\,$TargetAngle\)
fi
for (( RunNo=0; RunNo<10; RunNo++ ))
do
	#for DetectorRadius in 15 20 25
	for DetectorRadius in 15
	do
		fileAddition=COSYReal_${nEvts}Evts_particle${PDG}_Mom${ProtonMom}_Smearing${useGaus}_Target${TargetThickness}cm${TargetAngle}Deg_Dump${WithDump}_DetDist${DetectorRadius}_DumpDist${DumpDist}__${RunNo}
		Command="root -l -q -b sim_hypGe_COSY.C\($ProtonMom\,$nEvts\,$PDG\,$TargetThickness\,$TargetAngle\,$useGaus\,$Brems\,$WithDump\,$DetectorRadius\,$DumpDist\,$RunNo\) &> ${SimLogPath}/sim_$fileAddition.log"
		cat >$JobPath/job_sim_$fileAddition.sh <<EOF
		#!/bin/bash
		#
		#PBS -N $fileAddition
		#PBS -j oe
		#PBS -o ${JobLogPath}/job_sim_$fileAddition.log
		#PBS -V
		#PBS -l nodes=1:ppn=1,walltime=24:00:00

		export PATH=\$PBS_O_PATH
		cd \$PBS_O_WORKDIR

		echo "Start Simulation of COSY beamtime with $nEvts events and particle $PDG."
		${Command}

EOF
		echo "Just HypGe"
	### submit job to batch system
		qsub $JobPath/job_sim_$fileAddition.sh
		echo "${Command}"	
	done
done
