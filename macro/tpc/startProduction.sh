#!/bin/bash

macro=$1

for p in {0.2,0.4,0.6,0.8,1.0,1.2,1.5,2.0}
do
    echo $p
    if [[ "$1" =~ "MC.sh" ]]
    then
	qsub -t 5-160:5 -v MOM=$p -N MC_MOM_$p $1 
    fi
    if [[ "$1" =~ "Digi.sh" ]]
    then
	#create DEPENDENT job
	qsub -t 5-160:5 -v MOM=$p -N Digi_MOM_$p -hold_jid MC_MOM_$p $1 
    fi
    if [[ "$1" =~ "Reco.sh" ]]
    then
	#create DEPENDENT job
	qsub -t 5-160:5 -v MOM=$p -N Reco_MOM_$p -hold_jid Digi_MOM_$p $1 
	#qsub -t 5-160:5 -v MOM=$p -N Reco_MOM_$p $1 
    fi
done

