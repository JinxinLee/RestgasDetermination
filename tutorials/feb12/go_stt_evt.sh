# This script runs PandaRoot macros for the full Sim with
# STT as Tracker
# usage: ./go_tpc_evtgen.sh <momentum> <nEvents>
# (06/2009 R. Kliemt)

# make data directory, if not there. all data file go there, makes them easier to move around
#if [ ! -d "data" ]; then 
#  mkdir data
#fi

nEvts=5
res="psi(2S)"
dec="psi2s_jpsi2pi.dec"
mom=-1.0

if test "$1" != ""; then
  nEvts=$1
fi

if test "$2" != ""; then
  res=$2
fi

if test "$3" != ""; then
  dec=$3
fi

if test "$4" != ""; then
  mom=$4
fi


# let's go
echo "Start PandaRoot Simulation with $nEvts events."
root -l -q -b run_sim_sttcombi_evtgen.C\($nEvts,\"$res\",\"$dec\",$mom\) &> 1-sim.log
echo "Start Digitization"
root -l -q -b run_digi_sttcombi.C &> 2-digi.log
echo "Start LHE Trackfinding, Prefits, and Kalman."
root -l -q -b run_reco_sttcombi.C &> 3-reco.log
echo "Start PID"
root -l -q -b run_pid_stt.C &> 4-pid.log

# The analysis pops up a root window and remains in the root shell
#echo "Starting a sample analysis."
#root -l ana_example.C
