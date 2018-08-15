#!/bin/bash
#
# argument 1: run identification number
# argument 2: number of events
# argument 3: number of classes
# argument 4: number of features
# argument 5: number of neighbours
# argument 6: input filename
#
PANDAROOTHOME=$VMCWORKDIR
PANDAROOTBUILD=$VMCWORKDIR/../build
#
source $PANDAROOTBUILD/config.sh > logfile
#
# Copy programs to local path
#
cp $PANDAROOTHOME/tools/MVA/KNN/PndKnn* .
cp $PANDAROOTHOME/tools/MVA/KNN/train.cpp .
cp $PANDAROOTHOME/tools/MVA/KNN/classify.cpp .
cp $PANDAROOTHOME/tools/MVA/KNN/Makefile.jgm .
#
# Compile
#
make -f Makefile.jgm >> logfile 2>&1
#
export RANSEED=`date +%N`
./classify $2 $3 $4 $5 $6 $RANSEED >> logfile 2>&1
#
rm -f $6
rm -f train
rm -f classify
rm -f PndKnn*
rm -f *.cpp
rm -f *.h
rm -f Makefile.jgm
#
# Validate the output and return the appropiate value
#
for ofile in "logfile" "output_histo.root" ; do  
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
