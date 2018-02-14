#!/bin/bash
#
# argument 1: run identification number
# argument 2: input directory
# argument 3: minimum runnumber
# argument 4: maximum runnumber
# argument 5: output filename
#
#
export STUFF=/opt/exp_soft
export PANDAHOME=/home/panda
export PANDAROOTHOME=$HOME/pandaroot
source $PANDAROOTHOME/build/config.sh > logfile
#
export PATH=$HOME/bin:$STUFF/bin:$PATH
export LD_LIBRARY_PATH=$STUFF/lib:$LD_LIBRARY_PATH
#
# Copy ROOT scripts to local path
#
cp $VMCWORKDIR/macro/SimulationGG/SimulationMacros/full.C . >> logfile 2>&1
svn info $VMCWORKDIR >> logfile 2>&1
#
#
#
revline=`cat logfile | grep "Revision" `
revnumber=`echo $revline | awk '{print $2}'` 

let tel=$3
let max=$4+1

arg=$2
isxrd=${arg%%:*}
  
while [ $tel != $max ]
do

#
# Check existence of output file
#

 if [ $isxrd = "root" ]; then
  dir=/${arg#*///}
  host=${arg%///*}
  machineport=${host#*//}
  port=${machineport#*:}
  machine=${machineport%:*}	  
  checkcmd="ssh ${machine} ls $dir/$tel/full_rev${revnumber}_$5.root"
 else
  dir=${arg#*:}
  host=${arg%:*}
  checkcmd="ssh $host ls $dir/$tel/full_rev${revnumber}_$5.root"
 fi

 echo $checkcmd >> logfile 2>&1
 $checkcmd > /dev/null 2>&1
 retval=$?

 if [ $retval != 0 ]; then
  movefiles $2/$tel/sim.root . 1 0 > /dev/null 2>&1
  movefiles $2/$tel/simparams.root . 1 0 > /dev/null 2>&1

  if [ -s sim.root ]; then
     echo "<I> Analyzing $2/$tel" >> logfile 2>&1
     root -l -b -q "full.C(\"sim.root\",\"full_rev${revnumber}_$5.root\",\"simparams.root\")" >> logfile 2>&1 

     echo "<I> Copying full_rev${revnumber}_$5.root to $2/$tel/" >> logfile 2>&1
     if [ -s full_rev${revnumber}_$5.root ]; then
	 movefiles full_rev${revnumber}_$5.root $2/$tel/ 1 0 >> logfile 2>&1
     fi
     rm -f sim*.root >> logfile 2>&1
     rm -f full*.root >> logfile 2>&1
     rm -f logfile
     touch logfile
  fi
 fi
 let tel=tel+1  
done

exit 0
