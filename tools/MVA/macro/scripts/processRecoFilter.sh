#!/bin/bash

if [ "$1" == "" ]; then
  echo "Usage:"
  echo "./processRecoFilter.sh <directory> <pdg> <particle name>."
  exit 0
fi

if [ "$2" == "" ]; then
  echo "Usage:"
  echo "./processRecoFilter.sh <directory> <pdg> <particle name>."
  exit 0
fi

if [ "$3" == "" ]; then
  echo "Usage:"
  echo "./processRecoFilter.sh <directory> <pdg> <particle name>."
  exit 0
fi

. $HOME/ConfigurePanda.sh

FILES=$1*

for f in $FILES
do
  echo "Directory name is $f."
  cd $f
  cp $HOME/scripts/reco.C .
  cp $HOME/scripts/filterEvt.C .
  ls
  root -q -b -l reco.C"(\"digiFile.root\",\"simFile.root\",\"param.root\",\"recoFile.root\")"
  root -q -b -l runEvtSel.C"($2,\"$3\",\"param.root\",\"simFile.root\",\"digiFile.root\",\"recoFile.root\",\"evtData.root\")"
  echo "Processed."
done

exit 0