# This script runs PandaRoot macros for the full Sim with
# STT as Tracker
# usage: ./go_stt.sh <nEvents>
# (06/2009 R. Kliemt)

# make data directory, if not there. all data file go there, makes them easier to move around
if [ ! -d "data" ]; then 
  mkdir data
fi

# reads number of events from first parameter
nEvts=0
if test "$1" != "" ; then 
  nEvts=$1
fi

# let's go
echo "Start PandaRoot Simulation with $nEvts events."
root -l -q run_sim_sttcombi_pgun.C\($nEvts,211,1.,1.2\) &> data/1-sim.log
echo "Start Digitization"
root -l -q run_digi_sttcombi.C\($nEvts\) &> data/2-digi.log
echo "Start LHE Trackfinding, Prefits and Pid."
root -l -q run_reco_sttcombi.C\($nEvts\) &> data/3-reco.log
echo "Start Kalman Filter."
root -l -q run_kalman_stt.C\($nEvts\) &> data/4-kalman.log
echo "Writing TCandas for Analysis."
root -l -q makeTCands_stt.C\($nEvts\) &> data/5-microwriter.log

# The analysis pops up a root window and remains in the root shell
echo "Starting a sample analysis."
root -l ana_example.C
