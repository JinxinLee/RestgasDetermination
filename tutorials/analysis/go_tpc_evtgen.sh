# This script runs PandaRoot macros for the full Sim with
# STT as Tracker
# usage: ./go_tpc_evtgen.sh <momentum> <nEvents>
# (06/2009 R. Kliemt)

# make data directory, if not there. all data file go there, makes them easier to move around
#if [ ! -d "data" ]; then 
#  mkdir data
#fi

file="output.evt"
mom=5.0
nEvts=5

if test "$1" != ""; then
  file=$1
fi

mom=`head $file -n10 |awk '/^  0/ {p=$10} END {print p}'`;

if test "$2" != ""; then
  nEvts=$2
fi

echo "Reading file $file with mom $mom for $nEvts events.";

# let's go
echo "Simulation with p=$mom file=$file nev=$nEvts"
echo "Start PandaRoot Simulation with $nEvts events."
root -l -q run_sim_tpccombi_evtgen.C\($mom,\"$file\",$nEvts\) &> 1-sim.log
echo "Start Digitization"
root -l -q run_digi_tpccombi.C &> 2-digi.log
echo "Start LHE Trackfinding, Prefits, and Kalman."
root -l -q run_reco_tpccombi.C &> 3-reco.log
echo "Start PID"
root -l -q run_pid_tpc.C &> 4-pid.log

# The analysis pops up a root window and remains in the root shell
#echo "Starting a sample analysis."
#root -l ana_example.C
