#!/bin/bash
#PBS -j oe
#PBS -o /data/work/kpha1/steinen/Gamma/simlogs/Sim_Geo${qGeometry}_E${qEnergy}MeV_Evts${qnEvts}_FileEvts${qevtpJob}_Gen${qGenType}_ST${qaddSecTar}__${PBS_ARRAYID}.log
#PBS -V
### #PBS -l nodes=1:ppn=1,walltime=02:00:00,mem=4000mb
### #PBS -l nodes=1:ppn=1,walltime=02:00:00

export PATH=${PBS_O_PATH}
cd ${PBS_O_WORKDIR}

SUB_ID=${PBS_ARRAYID}

echo root -l -q -b '../sim_Gamma.C('${qGeometry}','${qEnergy}', '${qnEvts}', '${qevtpJob}' , ' ${qGenType} ',  '${qaddSecTar} ', '${SUB_ID}')'

root -l -q -b '../sim_Gamma.C('${qGeometry}','${qEnergy}','${qnEvts}','${qevtpJob}','${qGenType}','${qaddSecTar}','${SUB_ID}')'
echo blablabla
