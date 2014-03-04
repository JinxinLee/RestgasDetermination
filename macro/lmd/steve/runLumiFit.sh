#!/bin/bash 
###############################################################################
## script for running luminosity fitting/extraction procedure for LMD
###############################################################################

batchjob=0

if [[ $PBS_O_WORKDIR ]]; then
  batchjob=1
  cd $PBS_O_WORKDIR
fi

# directory of the data generated with dpm
if [[ ${PBS_ARRAYID} ]]; then
  elastic_data_path=$var1/bunch_${PBS_ARRAYID}-$var3
else
  elastic_data_path=$var1
fi

# directory of data generated with box generator (for acceptance calculation)
acceptance_data_path=$var2
echo "Acceptance Data Path: ${acceptance_data_path}!"

if [ $batchjob -eq "0" ]; then
  ../LMD_fit/runLumiFit -d ${elastic_data_path} -a ${acceptance_data_path} 2>&1 >> ${elastic_data_path}/fit.log
else
  ../LMD_fit/runLumiFit -d ${elastic_data_path} -a ${acceptance_data_path}
fi

sleep 10;
exit 0;
