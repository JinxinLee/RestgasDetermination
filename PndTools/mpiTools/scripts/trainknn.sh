#!/bin/bash
#
# argument 1: run identification number
# argument 2: number of events
# argument 3: number of classes
# argument 4: number of features
# argument 5: output filename
#
PANDAROOTHOME=$VMCWORKDIR
PANDAROOTBUILD=$VMCWORKDIR/../build
#
source $PANDAROOTBUILD/config.sh > logfile
#
# Copy programs to local path
#
cp $PANDAROOTHOME/PndTools/MVA/KNN/PndKnn* .
cp $PANDAROOTHOME/PndTools/MVA/KNN/train.cpp .
cp $PANDAROOTHOME/PndTools/MVA/KNN/classify.cpp .
cp $PANDAROOTHOME/PndTools/MVA/KNN/Makefile.jgm .
#
# Compile
#
make -f Makefile.jgm >> logfile 2>&1
#
# Run the program
#
export RANSEED=`date +%N`
./train $2 $3 $4 $5 $RANSEED >> logfile 2>&1
#
# Validate the output and return the appropiate value
#
for ofile in "logfile" "train.cpp" "classify.cpp" "Makefile.jgm" "$7" ; do  
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
