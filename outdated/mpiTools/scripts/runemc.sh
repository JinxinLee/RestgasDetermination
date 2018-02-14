#!/bin/bash
#
# argument 1: run identification number
# argument 2: number of events
# argument 3: particle (gamma, e-, e+, pi-, pi+, etc.)
# argument 4: minimum momentum (GeV/c)
# argument 5: maximum momentum (GeV/c)
# argument 6: minimum theta (degrees)
# argument 7: maximum theta (degrees)
# argument 8: minimum phi (degrees)
# argument 9: maximum phi (degrees)
# argument 10: transport model (TGeant3, TGeant4, TFluka)
# argument 11: remove pandaroot simulation output (TRUE, FALSE)
# argument 12: remove logfile (TRUE, FALSE)
#
# An example line in the job description file could look like:
#
# JOB 10 $HOME/scripts/runemc.sh NULL $HOME/output 11 1000 gamma 0.0 2.0 0. 180. 0. 360. TGeant3 TRUE FALSE
#
# Setup the environmental variables to run Panda and Fair ROOT.
# For this run the config.sh in the build directory of PandaRoot.
# Please set the path according to your setup
#
PANDAROOTHOME=$VMCWORKDIR
PANDAROOTBUILD=$VMCWORKDIR/../build/
#
# Get the configuration files for G3 and G4 from own gconfig directory
#
export CONFIG_DIR=$PANDAROOTHOME/PndTools/mpiTools/macros/gconfig/
#
source $PANDAROOTBUILD/config.sh > logfile
#
# Copy ROOT scripts to local path
#
cp $PANDAROOTHOME/PndTools/mpiTools/macros/emc/emc.C .
cp $PANDAROOTHOME/PndTools/mpiTools/macros/emc/reco_analys.C .
cp $PANDAROOTHOME/PndTools/mpiTools/macros/emc/rootlogon.C .
#
# Run the scripts
#
root -l -b -q "emc.C($2,\"$3\",$4,$5,$6,$7,$8,$9,\"sim_emc.root\",\"simparams.root\",\"${10}\",$1,kFALSE,kTRUE,kFALSE,kTRUE)" >> logfile 2>&1
root -l -b -q "reco_analys.C+(\"sim_emc.root\",\"output_$3_${10}.root\")" >> logfile 2>&1

cnt=1
for FILENAME in "`find . -name "sim_emc.root_*" -print`" ; do
 if [ -n "$FILENAME" ]; then
    root -l -b -q "reco_analys.C+(\"$FILENAME\",\"output_$3_${10}.root_$cnt\")" >> logfile 2>&1
    let cnt=cnt+1
 fi
done
#
# Validate the output and return the appropiate value
#
for ofile in "logfile" "emc.C" "reco_analys.C" "rootlogon.C" "sim_emc.root" "simparams.root" "output_$3_${10}.root" ; do  
  [ -f $ofile ]  || error="$error $ofile doesn't exist,";
done

for message in "Segmentation violation" "Segmentation fault" "Abort" "Bus error" "Floating point exception" "root: command not found" "cp: cannot stat" "Error opening Input file" ; do
 grep -i "$message" logfile &&  error="$error and $message"
done

#
# Success
#
if [ -z "$error"  ] ; then 

#
# Remove files you dont want to keep
#

  rm -f *.C
  rm -f *.dat
  rm -f *.so
  rm -f *.sh
#
# Also remove the pandaroot results and logfile, in case requested...
#
 if [ ${11} = "TRUE" ]; then
  rm -f sim_emc.root*
  rm -f simparams.root
 fi
#
 if [ ${12} = "TRUE" ]; then
  rm logfile
 fi

  exit 0
fi
#
# Failure
#
cat logfile
echo "<E> Validation failed with $error"
exit -1
