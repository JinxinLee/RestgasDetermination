#!/bin/bash
#
# addhisto.sh : Sums up the histograms stored in root files (INPUT)

# argument 1: run identification number
# argument 2: name of the root files to be summed up
# argument 3: output filename
#
# An example line in the job description file could look like:
#
# JOB 10 $HOME/scripts/addhisto.sh $HOME/input $HOME/output 2 output.root sum.root 
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
# Copy hadd.C root-script
#
cp $PANDAROOTHOME/tools/mpiTools/macros/tools/hadd.C .
#
# Create list of root files to merge
#
find ./ -name "$2" > inputlist
#
# Run the root script
#
root -l -b -q "hadd.C(\"inputlist\",\"$3\")" >> logfile 2>&1
#
# Remove input files (we dont want to copy those)
#
rm -rf `cat inputstuff`
#
# Validate the output and return the appropiate value
#
for ofile in "logfile" "inputlist" "$3" ; do  
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
