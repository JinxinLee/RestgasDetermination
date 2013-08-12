#!/bin/bash

while getopts "m:n:a:g:" OPTION
do
    case $OPTION in
    m)
        lab_momentum="$OPTARG"
    ;;
    n)
        num_events="$OPTARG"
    ;;
    a)
        acceptance_directory="$OPTARG"
    ;;
    g)
        generated_luminosity="$OPTARG"
    ;;
    [?])
        echo "Usage: $0 [-m lab_momentum] [-n number_of_events] [-a acceptance_path] [-g generated_luminosity]" >&2
        exit 1
    ;;
    esac
done
shift $(($OPTIND-1))

parallel -j8 qsub -N lumiFit -l nodes=1:ppn=1,walltime=01:00:00 -j oe -o {}/fit.log -v var1=${lab_momentum},var2=${num_events},var3={},var4=${acceptance_directory},var5=${generated_luminosity} -V ~/lmd/runLumiFit ::: $@

exit 0;
