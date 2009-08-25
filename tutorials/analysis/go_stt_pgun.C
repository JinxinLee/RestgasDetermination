# This script runs PandaRoot macros for the full Sim with
# STT as Tracker
# usage: ./go_stt_pgun.sh <nEvents> <pid> <momentum1> <momentum2>
# (06/2009 R. Kliemt)

# make data directory, if not there. all data file go there, makes them easier to move around
if [ ! -d "data" ]; then 
  mkdir data
fi

nEvts=10
pid=11
mom1=1.0
mom2=1.01

if test "$1" != "" ; then 
  nEvts=$1
fi

if test "$2" != "" ; then 
  pid=$2
fi

if test "$3" != ""; then
  mom1=$2
fi

if test "$4" != ""; then
  mom2=$2
fi


# let's go
echo "Start PandaRoot Simulation with $nEvts events."
root -l -q run_sim_sttcombi_pgun.C\($nEvts,$pid,$mom1,$mom2\) &> data/1-sim_pgun.log
echo "Start Digitization"
root -l -q run_digi_sttcombi.C\($nEvts\) &> data/2-digi_pgun.log
echo "Start LHE Trackfinding, Prefits and Pid."
root -l -q run_reco_sttcombi.C\($nEvts\) &> data/3-reco_pgun.log
echo "Start Kalman Filter."
root -l -q run_kalman_stt.C\($nEvts\) &> data/4-kalman_pgun.log
echo "Writing TCandas for Analysis."
root -l -q makeTCands_stt.C\($nEvts\) &> data/5-microwriter_pgun.log

# The analysis pops up a root window and remains in the root shell
echo "Starting a sample analysis."
root -l ana_example.C
