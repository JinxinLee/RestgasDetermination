#!/bin/bash
#
# mergetrees.sh : Merges trees stored in root files (given by INPUT)

# argument 1: tree name
# argument 2: name of the root files to be merged
# argument 3: Path name to rootfiles
# argument 4: output root filename
#
#
PANDAROOTHOME=$VMCWORKDIR
PANDAROOTBUILD=$VMCWORKDIR/../buildPanda
#
source $PANDAROOTBUILD/config.sh > logfile
#
# Create list of root files to merge
#
find $3 -name "${2}*" > inputlist
# echo $inputlist
#
# Run the root script
#
root -l -b -q "treeMerge.C(\"$1\",\"inputlist\",\"$4\")" >> logfile 2>&1