#!/bin/bash
if [ $FAIRSOFT_VERSION=="may16p1" ] && [ $FAIRROOT_VERSION=="v-17.10b" ]; then
   export SIMPATH=<path to the FairSoft may16p1 build folder>
   export FAIRROOTPATH=<path to the FairRoot v-17.10b install folder>
elif [ $FAIRSOFT_VERSION=="oct17" ] && [ $FAIRROOT_VERSION=="dev" ]; then
   export SIMPATH=<path to the FairSoft oct17 build folder>
   export FAIRROOTPATH=<path to the FairRoot dev install folder>
else
   echo "No valid FAIRSOFT_VERSION: " $FAIRSOFT_VERSION
   echo "of FAIRROOT_VERSION: " $FAIRROOT_VERSION
fi

