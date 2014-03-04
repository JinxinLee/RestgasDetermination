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
generated_luminosity=$var5

if [ ! $numEv ]; then
  numEv=0
fi

if [ "$type" -eq "0" ]; then
  if [ -z "${generated_luminosity}" ]; then
    dpm_logfile=${GEN_DATA}/`echo ${data_path} | sed -rn 's/^.*\/(.*)_pixel.*$/\1/p'`/*_1.log
    cs=$(cat ${dpm_logfile} | sed -rn 's/elastic cross section[ ]*([0-9]*.[0-9]*).*/\1/p')
    echo cross section is $cs
    generated_luminosity=$(awk "BEGIN{print 1.0/$cs}")
  fi
  echo "using generated luminosity per event of " ${generated_luminosity}
  echo $VMCWORKDIR/macro/lmd/LMD_fit/createLumiFitData -m $pbeam -t $type -p ${data_path} -n ${numEv} -g ${generated_luminosity}
  if [ $batchjob -eq "0" ]; then
    $VMCWORKDIR/macro/lmd/LMD_fit/createLumiFitData -m $pbeam -t $type -p ${data_path} -n ${numEv} -g ${generated_luminosity} 2>&1 >> ${data_path}/createLumiFitData.log
  else
    $VMCWORKDIR/macro/lmd/LMD_fit/createLumiFitData -m $pbeam -t $type -p ${data_path} -n ${numEv} -g ${generated_luminosity}
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
