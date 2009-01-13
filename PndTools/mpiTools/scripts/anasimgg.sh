#!/bin/bash
#
# argument 1: run identification number
# argument 2: input directory
# argument 3: minimum runnumber
# argument 4: maximum runnumber
# argument 5: output filename
#
#
export PANDAHOME=/home/panda
export PANDAROOTHOME=$HOME/pandaroot
source $PANDAROOTHOME/build/config.sh > logfile
#
# Copy ROOT scripts to local path
#
cp $VMCWORKDIR/macro/SimulationGG/SimulationMacros/full.C . >> logfile 2>&1
#
#
#
let tel=$3
let max=$4+1

while [ $tel != $max ]
do
 echo "<I> Copying $2/$tel" >> logfile 2>&1
 scp -B -r $2/$tel/sim*.root . >> logfile 2>&1

 echo "<I> Analyzing $2/$tel" >> logfile 2>&1
 if [ -s sim.root ]; then
     root -l -b -q "full.C(\"sim.root\",\"full_$5.root\",\"simparams.root\")" >> logfile 2>&1 
     cnt=1
     for FILENAME in "`find . -name "sim.root_*" -print`" ; do
	 if [ -n "$FILENAME" ]; then
	     root -l -b -q "full.C(\"$FILENAME\",\"full_$5_$cnt.root\",\"simparams.root\")" >> logfile 2>&1
	     let cnt=cnt+1
	 fi
     done
 fi

 echo "<I> Copying full_$5.root to $2/$tel/" >> logfile 2>&1
 if [ -s full_$5.root ]; then
     scp -B -r full_$5.root $2/$tel/ >> logfile 2>&1
 fi
 rm -f sim*.root >> logfile 2>&1
 rm -f full*.root >> logfile 2>&1

 scp -B -r logfile $2/$tel/logfile_$5 > /dev/null 2>&1
 let tel=tel+1
done

exit 0
