#!/bin/bash

if [ "$1" == "" ]; then
  echo ""
  echo "   Script for fetching weight file to be used with PndPidMvaAssociatorTask"
  echo "   and PndPidEmcAssociatorTask. For now these tasks use only KNN for classification."
  echo "   A prototype based algorithm (LVQ) will be included in the near future."
  echo ""
  echo "   Usage: ./fetchWeights.sh <directory>."
  echo "   <directory> is the path to wchich the file is stored."
  echo ""
  exit 0
fi

# Print directory name where the file is going to be stored.
echo ""
echo "<INFO>"
echo "Storing the weight file EmcMvaWeightOld.root in "$1
echo ""
echo ""

# Fetch the file
/usr/bin/wget -c --output-document=$1/EmcMvaWeightOld.root http://kvit13.kvi.nl/~babai/EmcMvaWeightOld.root

exit 0