#!/bin/bash
#
# mergetrees.sh : Merges trees stored in root files (given by INPUT)

# argument 1: run identification number
# argument 2: tree name
# argument 3: name of the root files to be merged
# argument 4: output root filename
#
# An example line in the job description file could look like:
#
# JOB 1 $HOME/scripts/mergetrees.sh $HOME/input $HOME/output 3 cbmsim sim_emc.root sim_emc_merge.root 
#
#
# Obtain overview of the available input files
# (later we delete them)
#
ls -c > inputstuff
#
# Setup the environmental variables to run ROOT.
# For this run the config.sh in the build directory of PandaRoot.
# Please set the path according to your setup
#
PANDAROOTHOME=$VMCWORKDIR
PANDAROOTBUILD=$VMCWORKDIR/../build
#
source $PANDAROOTBUILD/config.sh > logfile
#
# Copy treeMerge.C root-script
#
cp $PANDAROOTHOME/tools/mpiTools/macros/tools/treeMerge.C .
#
# Create list of root files to merge
#
find ./ -name "${3}*" > inputlist
#
# Run the root script
#
root -l -b -q "treeMerge.C(\"$2\",\"inputlist\",\"$4\")" >> logfile 2>&1
#
# Remove input files (we dont want to copy those)
#
rm -rf `cat inputstuff`
#
# Validate the output and return the appropiate value
#
for ofile in "logfile" "inputlist" "$4" ; do  
  [ -f $ofile ]  || error="$error $ofile doesn't exist,";
done

for message in "Segmentation violation" "Segmentation fault" "Abort" "Bus error" "Floating point exception" "root: command not found" "cp: cannot stat" "Error opening Input file" ; do
 grep -i "$message" logfile &&  error="$error and $message"
done
#
# Success
#
if [ -z "$error"  ] ; then 

  rm -f inputstuff
  rm -f *.C
  rm -f *.sh

  exit 0
fi
#
# Failure
#
echo "<E> Validation failed with $error"
exit -1
