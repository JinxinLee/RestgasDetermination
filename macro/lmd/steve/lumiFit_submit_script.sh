#!/bin/bash

. bashFunctions.sh

while getopts "a:" OPTION
do
    case $OPTION in
    a)
        acceptance_directory="$OPTARG"
    ;;
    [?])
        echo "Usage: $0 [-a acceptance_path] paths_to_elastic_data" >&2
        exit 1
    ;;
    esac
done
shift $(($OPTIND-1))

num_jobs_per_array=100

for dir in $@; do
  #we have two kinds of modes
  #user specified a bunch top folder containing subfolders of bunch_num-num folders
  num_samples_in_this_dir=$(ls $dir | grep bunch_ | wc -l)
  if [[ ${num_samples_in_this_dir} -gt "0" ]]; then
    echo ${num_samples_in_this_dir}
    for i in `seq 0 $((${num_samples_in_this_dir}/${num_jobs_per_array}))`; do
      minval=$(min ${num_samples_in_this_dir} $(($(($i+1))*$num_jobs_per_array)))
      qsub -t $((1+$(($i*$num_jobs_per_array))))-$minval -N lumiFit -l nodes=1:ppn=1,walltime=00:20:00,mem=100mb,vmem=700mb -j oe -o $dir/fit_pbs.log -v var1=$dir,var2=${acceptance_directory},var3=${num_samples_in_this_dir} -V runLumiFit.sh
    done
  else
    #if its just some folder that doesnt have bunch_* subfolders in it then just treat it as folder with data
    qsub -N lumiFit -l nodes=1:ppn=1,walltime=00:20:00,mem=100mb,vmem=700mb -j oe -o $dir/fit_pbs.log -v var1=${dir},var2=${acceptance_directory},var3=${num_samples_in_this_dir} -V runLumiFit.sh
  fi
done

exit 0;
