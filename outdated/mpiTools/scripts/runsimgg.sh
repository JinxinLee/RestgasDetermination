#!/bin/bash
#
# argument 1: run identification number
# argument 2: number of events
# argument 3: momentum (GeV/c)
# argument 4: dec file
# argument 5: transport model (TGeant3, TGeant4, TFluka)
#
#
export KVIHOME=/home/rugkvi04/kvi
export PANDAHOME=/home/rugkvi04/panda
export PANDAROOTHOME=$HOME/pandaroot
export CERN=$KVIHOME/cern
export CERN_ROOT=$CERN/2005

source $PANDAROOTHOME/build/config.sh > logfile

export LD_LIBRARY_PATH=$CERN_ROOT/lib:$KVIHOME/subversion/lib:$LD_LIBRARY_PATH
export PATH=$KVIHOME/subversion/bin:$HOME/bin:$PATH

svn info $VMCWORKDIR >> logfile 2>&1
#
revline=`cat logfile | grep "Revision" `
revnumber=`echo $revline | awk '{print $2}'` 
#
# Copy ROOT scripts to local path
#
cp $PANDAROOTHOME/trunk/macro/SimulationGG/EvtGen/DECAY.DEC .
cp $PANDAROOTHOME/trunk/macro/SimulationGG/EvtGen/evt.pdl .
cp $HOME/bin/ggEvtGen .
cp $PANDAROOTHOME/trunk/macro/SimulationGG/EvtGen/dec/$4 .
#
cp $PANDAROOTHOME/trunk/macro/SimulationGG/SimulationMacros/sim_gg.C .
cp $PANDAROOTHOME/trunk/macro/SimulationGG/SimulationMacros/full_gg.C .
#
# Run evtgen
#
./ggEvtGen $3 $2 $4 $1 >> logfile 2>&1
#
# Run the scripts
#
root -l -b -q "sim_gg.C(\"sim_rev${revnumber}.root\",\"output.evt\",\"simparams_rev${revnumber}.root\",$2,\"$5\",$3,$1)" >> logfile 2>&1
root -l -b -q "full_gg.C(\"sim_rev${revnumber}.root\",\"full_rev${revnumber}.root\",\"simparams_rev${revnumber}.root\")" >> logfile 2>&1

cnt=1
for FILENAME in "`find . -name "sim_rev${revnumber}.root_*" -print`" ; do
 if [ -n "$FILENAME" ]; then
    root -l -b -q "full_gg.C(\"$FILENAME\",\"full_rev${revnumber}_$cnt.root\",\"simparams_rev${revnumber}.root\")" >> logfile 2>&1
    let cnt=cnt+1
 fi
done
#
# Validate the output and return the appropiate value
#
for ofile in "ggEvtGen" "logfile" "sim_gg.C" "full_gg.C" "sim_rev${revnumber}.root" "simparams_rev${revnumber}.root" "full_rev${revnumber}.root" "output.evt" ; do  
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
  rm -f ggEvtGen
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
