#!/bin/bash
#
# Dependencies
# -------------
# fairsoft_jan10/feb11
# pandaroot/trunk/stable
#
# input parameters:
# -----------------
# $1-runid
# $2-number of events
# $3-momentum (GeV/c)
# $4-mode (tpc/stt)  
#
# Output:
# -------
# params_$4combi.root  
# digi_$4combi.root  
# points_$4combi.root  
# reco_$4combi.root
#
# JGM, Feb. 2011
#
#
# Run the scripts
#
export ranseed=`date +%N`

echo "This is a script to run the full simulation and digitization"
echo "with DPM events for STT/TPC"
echo 
echo "Starting the simulation:"
echo
echo "Momentum   = " $3
echo "Nevents    = " $2
echo "Seed       = " $ranseed 
echo 

source $HOME/buildPanda/config.sh > /dev/null 2>&1
echo "Copying macros from dc4 directory...."
cp $VMCWORKDIR/macro/run/dc4/*.C .

echo "-----------------------------------------------------------------------"
echo "starting simulation"

root -b -q "run_sim_${4}combi_dpm.C($2,$3,1,$ranseed)"  >> /dev/null 2>&1

echo ""
if [ -s "params_${4}combi.root" -a -s "points_${4}combi.root" ]; then
  echo "run_sim_${4}combi_dpm finished successfully"
else
  echo "ERROR: run_sim_${4}combi_dpm.C finished without producing MC data!"
  exit 12
fi	

root -b -q "run_digi_${4}combi.C"  >> /dev/null 2>&1

echo ""
if [ -s "params_${4}combi.root" -a -s "digi_${4}combi.root" ]; then
  echo "run_digi_${4}combi.C finished successfully"
else
  echo "ERROR: run_digi_${4}combi.C finished without producing digi data!"
  exit 13
fi	

root -b -q "run_reco_${4}combi.C"  >> /dev/null 2>&1

echo ""
if [ -s "params_${4}combi.root" -a -s "reco_${4}combi.root" ]; then
  echo "run_reco_${4}combi.C finished successfully"
else
  echo "ERROR: run_reco_${4}combi.C finished without producing reco data!"
  exit 14
fi	

root -b -q "run_pid_${4}.C"  >> /dev/null 2>&1

echo ""
if [ -s "params_${4}combi.root" -a -s "pid_${4}combi.root" ]; then
  echo "run_pid_${4} finished successfully"
else
  echo "ERROR: run_pid_${4} finished without producing pid data!"
  exit 15
fi	

rm -f *.C
rm -f *.dat
rm -f rundpm.sh

echo ""
echo "------------------------- YUHUU --------------------------------"

exit 0