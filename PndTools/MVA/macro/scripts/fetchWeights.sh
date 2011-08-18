#!/bin/bash

echo ""
echo "Please do NOT use this script. It is not working any more."
echo ""
exit 0

if [ "$1" == "" ]; then
  echo ""
  echo "   Script for fetching weight file to be used with PndPidMvaAssociatorTask"
  echo "   and PndPidEmcAssociatorTask. For now these tasks use only KNN for classification."
  echo "   A prototype based algorithm (LVQ) will be included in the near future."
  echo ""
  echo "   Usage: ./fetchWeights.sh <directory>."
  echo "   <directory> is the path to wchich the file is stored."
  echo ""
  echo ""
  exit 0
fi

if [ "$2" == "" ]; then
 echo ""
 echo " You need to specify which version of the file to fetch."
 echo " Possible options: Old: Old weight file containing emc data."
 echo "                   current: current not normalized weight file containing emc data."
 echo "                   currentNorm: current normalized weight file containing emc data."
 exit 0
fi
# Print directory name where the file is going to be stored.
echo ""
echo "<INFO>"
echo "Storing the weight file in "$1
echo ""
echo ""

if [ "$1" == "Old" ]; then
# Fetch the file
/usr/bin/wget -c --output-document=$1/EmcMvaWeightOld.root http://kvit13.kvi.nl/~babai/Weights/EmcMvaWeightOld.root
fi

if [ "$1" == "current" ]; then
# Fetch the file
/usr/bin/wget -c --output-document=$1/EmcMvaWeightOld.root http://kvit13.kvi.nl/~babai/Weights/jun23_10_EvtFeaturesSmallSet.root
fi

if [ "$1" == "currentNorm" ]; then
# Fetch the file
/usr/bin/wget -c --output-document=$1/EmcMvaWeightOld.root http://kvit13.kvi.nl/~babai/Weights/NormalizedJun23_10_EvtFeatSmallSet.root
fi

exit 0
