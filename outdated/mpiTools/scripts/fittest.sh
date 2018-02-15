#!/bin/bash
#
# fittest.sh:
#            
#
# argument 1: run identification number
# argument 2: number of iterations
# argument 3: number of statistics
# argument 4: peak to background
# argument 5: beam resolution
# argument 6: resonance width
# argument 7: silent (kTRUE/kFALSE)
# argument 8: fit options ("LQN")
#
PANDAROOTBUILD=$VMCWORKDIR/../build
#
source $PANDAROOTBUILD/config.sh > logfile
#
# Copy fittest.C
#
cp $VMCWORKDIR/tools/mpiTools/macros/tests/fittest.C .
#
# Run the scripts
#
root -l -b -q "fittest.C+($2,\"$8\",$3,$4,$5,$6,3.526,$1,$7)" >> logfile 2>&1
#
# Validate the output and return the appropiate value
#
for ofile in "logfile" "fittest.C" ; do  
  [ -f $ofile ]  || error="$error $ofile doesn't exist,";
done

for message in "Segmentation violation" "Segmentation fault" "Abort" "Bus error" "Floating point exception" "root: command not found" "cp: cannot stat" "Error opening Input file" ; do
 grep -i "$message" logfile &&  error="$error and $message"
done
#
# Success
#
if [ -z "$error"  ] ; then 
  rm -f logfile
  rm -f fittest.C
  rm -f *.so
  rm -f fittest.sh
  exit 0
fi
#
# Failure
#
cat logfile
echo "<E> Validation failed with $error"
exit -1
