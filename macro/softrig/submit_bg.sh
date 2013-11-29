#!/bin/bash

# simple script, which allows you to submit a bunch of jobs to prometheus
# Parameters:
#   <energy>: 24 (2.4GeV), 38 (3.77 GeV), 45 (4.5GeV), 55 (5.5GeV)
#   <bgmim> : Minimum job number
#   <bgmax> : Maximum job number; if not given, jobs array submitted from 0 - <bgmin>
#   <nevt>  : Number of events per job, default = 2000

energy=24
bgmin=1
bgmax=0
nevt=2000

if test "$1" != ""; then
  energy=$1
fi

if test "$2" != ""; then
  bgmin=$2
fi

if test "$3" != ""; then
  bgmax=$3
fi

if test "$4" != ""; then
  nevt=$4
fi

if [ "$bgmax" -lt "$bgmin" ]; then
  bgmax=$bgmin
  bgmin=1
fi


echo "BKG jobs: "$bgmin"-"$bgmax

case $energy in
# E_cm = 2.4 GeV, pbarmom = 1.913547
24)
	qsub -t $bgmin-$bgmax job_sof.sge 24900 $nevt DPM 1.913547 
	;;

# E_cm = 3.77 GeV, pbarmom = 6.56903
38)
	qsub -t $bgmin-$bgmax job_sof.sge 38900 $nevt DPM 6.56903 
	;;

# E_cm = 4.5 GeV, pbarmom = 9.808065
45)
	qsub -t $bgmin-$bgmax job_sof.sge 45900 $nevt DPM 9.808065
	;;

# E_cm = 5.5 GeV, pbarmom = 15.152765
55)
	qsub -t $bgmin-$bgmax job_sof.sge 55900 $nevt DPM 15.152765
	;;

*)
	echo "Unknown energy!"
esac

