#!/bin/bash

jobpath=${SIMDATADIR}/CrystalsOnly/Ana/jobs

joblogpath=${SIMDATADIR}/CrystalsOnly/Ana/joblogs

analogpath=${SIMDATADIR}/CrystalsOnly/Ana/analogs
echo $analogpath
cat > ${jobpath}/jobCO.sh <<EOF
	#!/bin/bash
	#
	#PBS -N jobCO
	#PBS -j oe
	#PBS -o ${joblogpath}/jobCO.log
	#PBS -V
	#PBS -l nodes=1:ppn=1,walltime=10:00:00

	export PATH=\$PBS_O_PATH
	cd \$PBS_O_WORKDIR

	echo "Start Analysis of File ."
	root -l -q -b ../AnalyseThetaRadiusCorrelation.C &> ${analogpath}/AnalyseThetaRadiusCorrelation.log
EOF

qsub ${jobpath}/jobCO.sh


