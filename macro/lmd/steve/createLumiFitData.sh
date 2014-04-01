#!/bin/bash 
###############################################################################
## script for creating LMD data used for the fitting procedure etc.
## for batch use run submit script version of create LumiFit data instead
###############################################################################

batchjob=0
if [ $PBS_O_WORKDIR ]; then
  batchjob=1
fi

# lab momentum of the beam antiprotons
pbeam=$var1
#number of events used for the fitting procedure
numEv=$var2
echo "data path: ${data_path}!"
# directory of the data generated with dpm
if [[ ${PBS_ARRAYID} ]]; then
  data_path=$var3/bunch_${PBS_ARRAYID}-$var6
else
  data_path=$var3
fi
type=$var4
echo "data type: ${type}"
elastic_cross_section=$var5

if [ ! $numEv ]; then
  numEv=0
fi

if [ "$type" == "a" ]; then
  if [ -z "${elastic_cross_section}" ]; then
    dpm_logfile=${GEN_DATA}/`echo ${data_path} | sed -rn 's/^.*\/(.*)_pixel.*$/\1/p'`/*_1.log
    elastic_cross_section=$(cat ${dpm_logfile} | sed -rn 's/elastic cross section[ ]*([0-9]*.[0-9]*).*/\1/p')
    echo cross section is ${elastic_cross_section}
  fi
  echo "using elastic cross section of ${elastic_cross_section}"
  echo $VMCWORKDIR/macro/lmd/LMD_fit/createLumiFitData -m $pbeam -t $type -p ${data_path} -n ${numEv} -c ${elastic_cross_section}
  if [ $batchjob -eq "0" ]; then
    $VMCWORKDIR/macro/lmd/LMD_fit/createLumiFitData -m $pbeam -t $type -p ${data_path} -n ${numEv} -c ${elastic_cross_section} 2>&1 >> ${data_path}/createLumiFitData.log
  else
    $VMCWORKDIR/macro/lmd/LMD_fit/createLumiFitData -m $pbeam -t $type -p ${data_path} -n ${numEv} -c ${elastic_cross_section}
  fi
else
  if [ $batchjob -eq "0" ]; then
    $VMCWORKDIR/macro/lmd/LMD_fit/createLumiFitData -m $pbeam -t $type -p ${data_path} -n ${numEv} 2>&1 >> ${data_path}/createLumiFitData.log
  else
    $VMCWORKDIR/macro/lmd/LMD_fit/createLumiFitData -m $pbeam -t $type -p ${data_path} -n ${numEv}
  fi
fi

sleep 10;
exit 0;
