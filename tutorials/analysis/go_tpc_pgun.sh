# This script runs PandaRoot macros for the full Sim with
# STT as Tracker
# usage: ./go_tpc_evtgen.sh <momentum> <nEvents>
# (06/2009 R. Kliemt)

# make data directory, if not there. all data file go there, makes them easier to move around
#if [ ! -d "data" ]; then 
#  mkdir data
#fi

nEvts=5

if test "$1" != ""; then
  nEvts=$1
fi

# let's go
echo "Start PandaRoot Simulation with $nEvts events."
root -l -q run_sim_tpccombi_pgun.C\($nEvts,211,1.,1.2\) &> 1-sim.log
echo "Start Digitization"
root -l -q run_digi_tpccombi.C &> 2-digi.log
echo "Start LHE Trackfinding, Prefits, and Kalman."
root -l -q run_reco_tpccombi.C &> 3-reco.log
echo "Start PID"
root -l -q run_pid_tpc.C &> 4-pid.log

# The analysis pops up a root window and remains in the root shell
#echo "Starting a sample analysis."
#root -l ana_example.C
