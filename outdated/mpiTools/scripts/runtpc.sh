#!/bin/bash
#
# argument 1: run identification number
# argument 2: number of events
# argument 3: pT (GeV/c))
#
# An example line in the job description file could look like:
#
# JOB 10 $HOME/scripts/runtpc.sh NULL $HOME/output 2 1000 2.0
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
cp $PANDAROOTHOME/macro/dc1/tpc/run_sim_tpcmvd.C .
cp $PANDAROOTHOME/macro/dc1/tpc/run_rectrack_tpcmvd.C  .
cp $PANDAROOTHOME/macro/dc1/tpc/all.par .
#
# Run the scripts
#
root -l -b -q "run_sim_tpcmvd.C($2,$3,$1)" >> logfile 2>&1
root -l -b -q "run_rectrack_tpcmvd.C" >> logfile 2>&1

#
# Validate the output and return the appropiate value
#
for ofile in "logfile" "run_sim_tpcmvd.C" "run_rectrack_tpcmvd.C" "all.par" "points_tpcmvdemc.root" "tracks_tpcmvdemc.root" "testparams.root" ; do  
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
