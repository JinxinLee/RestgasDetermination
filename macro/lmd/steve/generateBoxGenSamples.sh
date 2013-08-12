#! /bin/bash

#include some helper functions
. bashFunctions.sh

#check that user has $GEN_DATA set
check_output_dir

#ok we have the path. get all options the user specified!
while getopts "m:n:s:i:f:" OPTION
do
    case $OPTION in
    m)
        lab_momentum="$OPTARG"
    ;;
    n)
        evts_per_sample="$OPTARG"
    ;;
    s)
        start_sample_index="$OPTARG"
    ;;
    e)
        end_sample_index="$OPTARG"
    ;;
    l)
        lower_theta_bound="$OPTARG"
    ;;
    u)
        upper_theta_bound="$OPTARG"
    ;;
    f)
        filename_prefix="$OPTARG"
    ;;
    [?])
        echo "Usage: $0 [-m lab_momentum] [-n events_per_sample] [-s number_of_samples] [-i start_sample_index] [-e end_sample_index] [-l lower theta bound] [-u upper theta bound] [-f filename_prefix]" >&2
        exit 1
    ;;
    esac
done
#shift $(($OPTIND-1))

check_or_get_float_option "lab momentum" ${lab_momentum}
lab_momentum=$value

check_or_get_float_option "lower theta bound" ${lower_theta_bound}
lower_theta_bound=$value

check_or_get_float_option "upper theta bound" ${upper_theta_bound}
upper_theta_bound=$value

check_or_get_int_option "events per sample" ${evts_per_sample}
evts_per_sample=$value

check_or_get_int_option "start sample index" ${start_sample_index}
start_sample_index=$value

check_or_get_int_option "end sample index" ${end_sample_index}
end_sample_index=$value

if [ ! ${filename_prefix} ]; then
  #construct default filename prefix
  filename_prefix=${evts_per_sample}_box_plab_${lab_momentum}GeV_th_${lower_theta_bound}-${upper_theta_bound}deg
fi

check_or_get_dir_option "filename prefix" ${filename_prefix}
filename_prefix=$value

echo "using following options:"
printf "lab momentum: \t\t %8s \n" ${lab_momentum}
printf "lower theta bound: \t\t %8s \n" ${lower_theta_bound}
printf "upper theta bound: \t\t %8s \n" ${upper_theta_bound}
printf "events per sample: \t %8s \n" ${evts_per_sample}
printf "start sample index: \t\t %8s \n" ${start_sample_index}
printf "end sample index: \t %8s \n" ${end_sample_index}
printf "filename prefix: \t %8s \n" ${filename_prefix}

user_agree

if [ "$?" -eq 0 ]
then
  exit 1;
fi

# max array submit list size is 100 (this is current setting at himster...)
maxjobs=100

parallel -j8 'starti={}; qsub -t $starti-$(echo $(($(($starti+'$(($maxjobs-1))'))<'${end_sample_index}'?$(($starti+'$(($maxjobs-1))')):'${end_sample_index}'))) -N runBoxGen_'${filename_prefix}' -l nodes=1:ppn=1,walltime=00:30:00 -j oe -o /home/pflueger/himster_output_logs/runBoxGen_'${filename_prefix}' -v var1="'${lab_momentum}'",var2="'${evts_per_sample}'",var3="'${lower_theta_bound}'",var4="'${upper_theta_bound}'",var5="'${filename_prefix}'" -V ./runBoxGen.sh' ::: `seq ${start_sample_index} $maxjobs ${end_sample_index}`
