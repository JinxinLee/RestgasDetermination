#! /bin/bash

filepath=${GEN_DATA}
cd ~/pandaroot/pgenerators/DpmEvtGen
random_num=$RANDOM.$RANDOM
filename=`echo $var5 | sed -e 's/\./o/g'`_$PBS_ARRAYID

if [ ! -f $filepath/$var5/$filename.root ]; then
  echo Running ./DPMGen -s ${random_num} -m $var1 -e $var3 -t $var4 -n $var2 -f $filepath/$var5/$filename.root
  ./DPMGen -s ${random_num} -m $var1 -e $var3 -t $var4 -n $var2 -f $filepath/$var5/$filename.root >> $filepath/$var5/$filename.log
fi

sleep 10;
exit 0;
