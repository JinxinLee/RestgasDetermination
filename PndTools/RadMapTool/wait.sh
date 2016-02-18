#!/bin/bash
ID=$1;
function wait {
    #if [[ $2 -eq 1 ]]; then
    printf "+--------------------------------------------------+\n"
    printf "|                    Number  of                    |\n"
    printf "|                                                  |\n" 
    printf "+------------------+-------------------------------+\n"
    printf "|                  |               All             |\n" 
    printf "+------------------+----------------+--------------|\n"
    printf "| submitted jobs   | submitted jobs |  all jobs    |\n"
    printf "|[all running jobs]|                |              |\n"
    printf "|==================+================+==============+\n"
    #fi

#    while [[ $(qstat | grep $USER | wc -l) != 0 ]]; do 
#        ALL=`qstat | grep $USER | wc -l`;
    while [[ $(qstat | grep $ID | wc -l) != 0 ]]; do 
        ALL=`qstat | grep $USER | wc -l`;
        NOW=`qstat | grep $ID | wc -l`;
        RUN=`qstat | awk '{print $5}' | grep r | wc -l`;
        printf "|      %4d        |      %4d      |     %4d     |\r"  $NOW $RUN $ALL

        sleep $1
    done
    printf "\n\n\n";
}



wait 1m


