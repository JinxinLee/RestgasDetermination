#! /bin/bash

filepath=${GEN_DATA}
random_num=$RANDOM.$RANDOM
filename=`echo $var5 | sed -e 's/\./o/g'`_$PBS_ARRAYID

if [ ! -f $filepath/$var3/$filename.root ]; then
  root -l -b -q 'standaloneBoxGen.C('$var1', '$var2', '$var3', '$var4', "'$filepath/$var5/$filename.root'", '${random_num}')' >> $filepath/$var5/$filename.log
fi

sleep 10;
exit 0;
