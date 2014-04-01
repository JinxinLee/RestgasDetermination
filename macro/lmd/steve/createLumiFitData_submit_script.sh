#!/bin/bash

. bashFunctions.sh

while getopts "m:n:p:t:c:" OPTION
do
    case $OPTION in
    m)
        lab_momentum="$OPTARG"
    ;;
    n)
        num_events="$OPTARG"
    ;;
    t)
        type="$OPTARG"
    ;;
    c)
        elastic_cross_section="$OPTARG"
    ;;
    [?])
        echo "Usage: $0 [-m lab_momentum] [-n number_of_events] [-t data_type] [-c total_elastic_cross_section] paths to data" >&2
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
    jobnum=$((${num_samples_in_this_dir}/${num_jobs_per_array}))
    if [ $((${num_samples_in_this_dir} % ${num_jobs_per_array})) -eq "0" ]; then
      jobnum=$(($jobnum-1))
    fi
    for i in `seq 0 $jobnum`; do
      minval=$(min ${num_samples_in_this_dir} $(($(($i+1))*$num_jobs_per_array)))
      low=$((1+$(($i*$num_jobs_per_array))))
      high=$minval
      echo submitting job array $low - $high
      qsub -t $((1+$(($i*$num_jobs_per_array))))-$minval -N createLumiFitData -l nodes=1:ppn=1,walltime=02:00:00,mem=100mb,vmem=700mb -j oe -o $dir/createLumiFitData_pbs.log -v var1=${lab_momentum},var2=${num_events},var3=$dir,var4=$type,var5=${elastic_cross_section},var6=${num_samples_in_this_dir} -V createLumiFitData.sh
    done
  else
    #if its just some folder that doesnt have bunch_* subfolders in it then just treat it as folder with data
    qsub -N createLumiFitData -l nodes=1:ppn=1,walltime=02:00:00,mem=100mb,vmem=700mb -j oe -o $dir/createLumiFitData_pbs.log -v var1=${lab_momentum},var2=${num_events},var3=$dir,var4=$type,var5=${elastic_cross_section} -V createLumiFitData.sh
  fi
done

exit 0;
