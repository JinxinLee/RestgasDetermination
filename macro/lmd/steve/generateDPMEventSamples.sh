#! /bin/bash

#include some helper functions
. bashFunctions.sh

#check that user has $GEN_DATA set
check_output_dir

#ok we have the path. get all options the user specified!
while getopts "m:e:t:n:s:i:f:" OPTION
do
    case $OPTION in
    m)
        lab_momentum="$OPTARG"
    ;;
    i)
        interactions="$OPTARG"
    ;;
    t)
        theta_min="$OPTARG"
    ;;
    n)
        evts_per_sample="$OPTARG"
    ;;
    e)
        end_sample_index="$OPTARG"
    ;;
    s)
        start_sample_index="$OPTARG"
    ;;
    f)
        filename_prefix="$OPTARG"
    ;;
    [?])
        echo "Usage: $0 [-m lab_momentum] [-e interactions] [-t theta_min] [-n events_per_sample] [-i start_sample_index] [-e end_sample_index] [-f filename_prefix]" >&2
        exit 1
    ;;
    esac
done
#shift $(($OPTIND-1))

# array of generator mode names
interactions_names=('inelastic' 'elastic_inelastic' 'elastic')

check_or_get_float_option "lab momentum" ${lab_momentum}
lab_momentum=$value

namesarray=("${interactions_names[@]}") #namesarray is a global variable that is used for showing the options
check_or_get_int_option "interaction type" ${interactions}
interactions=$value
unset namesarray #unset the global variable


check_or_get_float_option "minimal theta value (in degrees, dont ask me why...)" ${theta_min}
theta_min=$value

check_or_get_int_option "events per sample" ${evts_per_sample}
evts_per_sample=$value

check_or_get_int_option "start sample index" ${start_sample_index}
start_sample_index=$value

check_or_get_int_option "end sample index" ${end_sample_index}
end_sample_index=$value

if [ ! ${filename_prefix} ]; then
  #construct default filename prefix
  filename_prefix=${evts_per_sample}_dpm_${interactions_names[${interactions}]}_plab_${lab_momentum}GeV_thmin_${theta_min}deg
fi

check_or_get_dir_option "filename prefix" ${filename_prefix}
filename_prefix=$value

echo "using following options:"
printf "lab momentum: \t\t %8s \n" ${lab_momentum}
printf "elastic mode: \t\t %8s \n" ${interactions}
printf "minimal theta value: \t\t %8s \n" ${theta_min}
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

parallel -j8 'starti={}; qsub -t $starti-$(echo $(($(($starti+'$(($maxjobs-1))'))<'${end_sample_index}'?$(($starti+'$(($maxjobs-1))')):'${end_sample_index}'))) -N runDPMGen_'${filename_prefix}' -l nodes=1:ppn=1,walltime=00:30:00 -j oe -o /home/pflueger/himster_output_logs/runDPMGen_'${filename_prefix}' -v var1="'${lab_momentum}'",var2="'${evts_per_sample}'",var3="'${interactions}'",var4="'${theta_min}'",var5="'${filename_prefix}'" -V ./runDPMGen.sh' ::: `seq ${start_sample_index} $maxjobs ${end_sample_index}`
