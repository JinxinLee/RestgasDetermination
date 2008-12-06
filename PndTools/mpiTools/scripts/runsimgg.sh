#!/bin/bash
#
# argument 1: run identification number
# argument 2: number of events
# argument 3: momentum (GeV/c)
# argument 4: dec file
# argument 5: transport model (TGeant3, TGeant4, TFluka)
#
#
source $HOME/johan/simgg.sh > logfile
#
# Copy ROOT scripts to local path
#
cp $HOME/johan/SimulationGG/EvtGen/DECAY.DEC .
cp $HOME/johan/SimulationGG/EvtGen/evt.pdl .
cp $HOME/johan/SimulationGG/EvtGen/pandaEvtGen .
cp $HOME/johan/SimulationGG/EvtGen/dec/$4 .
#
cp $HOME/johan/SimulationGG/SimulationMacros/sim.C .
cp $HOME/johan/SimulationGG/SimulationMacros/full.C .
#
# Run evtgen
#
./pandaEvtGen $3 $2 $4 $1 >> logfile 2>&1
#
# Run the scripts
#
root -l -b -q "sim.C(\"sim.root\",\"output.evt\",\"simparams.root\",$2,\"$5\",$1)" >> logfile 2>&1
root -l -b -q "full.C(\"sim.root\",\"full.root\",\"simparams.root\")" >> logfile 2>&1

cnt=1
for FILENAME in "`find . -name "sim.root_*" -print`" ; do
 if [ -n "$FILENAME" ]; then
    root -l -b -q "full.C(\"$FILENAME\",\"full_$cnt.root\",\"simparams.root\")" >> logfile 2>&1
    let cnt=cnt+1
 fi
done
#
# Validate the output and return the appropiate value
#
for ofile in "logfile" "sim.C" "full.C" "sim.root" "simparams.root" "full.root" "output.evt" ; do  
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
  rm -f pandaEvtGen
  rm -f $4
  rm -f DECAY.DEC
  rm -f evt.pdl
  rm logfile

  exit 0
fi
#
# Failure
#
cat logfile
echo "<E> Validation failed with $error"
exit -1
