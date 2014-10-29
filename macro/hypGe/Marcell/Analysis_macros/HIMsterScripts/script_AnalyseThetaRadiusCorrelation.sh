#!/bin/bash


    
	cat >job_AnalyseThetaRadiusCorrelation.sh <<EOF
	#!/bin/bash
	#
	#PBS -N dpm.AnalyseThetaRadiusCorrelation
	#PBS -j oe
	#PBS -o 0-job_AnalyseThetaRadiusCorrelation.log
	#PBS -V
	#PBS -l nodes=1:x86_64,walltime=10:00:00

	export PATH=\$PBS_O_PATH
	cd \$PBS_O_WORKDIR

	echo "Start Analysis of File $line."
	root -l -q -b ../AnalyseThetaRadiusCorrelation.C\(\) &> AnalyseThetaRadiusCorrelation.log


EOF
	echo "Analysis of $line"
### submit job to batch system
	qsub job_AnalyseThetaRadiusCorrelation.sh
	#root -l -q -b GammaSpectraAnalysis_extparameter_NoH.C\(\"$line\"\)
