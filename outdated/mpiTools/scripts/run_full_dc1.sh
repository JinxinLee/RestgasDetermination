#!/bin/bash
#
# Input Parameters:
# -----------------
# argument 1: run identification number
# argument 2: number of events
# argument 3: momentum of beam (GeV/c)
#
# Output files:
# -------------
# fullmc.root     - output transport simulation
# testparams.root - database parameters
#
# An example line in the job description file could look like:
#
# JOB 10 $HOME/scripts/run_full_dc1.sh NULL $HOME/output 2 1000 2.0
#
# Setup the environmental variables to run Panda and Fair ROOT.
# For this run the config.sh in the build directory of PandaRoot.
# Please set the path according to your setup
#
PANDAROOTHOME=$VMCWORKDIR
PANDAROOTBUILD=$VMCWORKDIR/../build
#
source $PANDAROOTBUILD/config.sh > logfile
#
# Copy ROOT scripts to local path
#
cp $PANDAROOTHOME/macro/dc1/full/run_sim.C .
#
# Run the scripts
#
export RANSEED=`date +%N`
#
root -b -q "run_sim.C($2,$3,$RANSEED)" >> logfile 2>&1
#
# Validate the output and return the appropiate value
#
for ofile in "logfile" "run_sim.C" "testparams.root" "fullmc.root" ; do  
  [ -f $ofile ]  || error="$error $ofile doesn't exist,";
done

for message in "Segmentation violation" "Segmentation fault" "Abort" "Bus error" "Floating point exception" "root: command not found" "cp: cannot stat" "Error opening Input file" ; do
 grep -i "$message" logfile &&  error="$error and $message"
done
#
# Success
#
if [ -z "$error"  ] ; then 
  exit 0
fi
#
# Failure
#
cat logfile
echo "<E> Validation failed with $error"
exit -1
