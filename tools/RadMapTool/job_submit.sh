#!/bin/bash

usage="\n\nThe script `basename $0` runs job on the SGE cluster\n
Usage: $0 [[-s|-n][-a|-b]|-h]\n\n
\t -b\t [b]asename   - [b]asename of the files to recognise (_Geant4.15GeV_ or Geant3.15GeV)\n
Simulation:\n
\t -s\t [s]simulation   - [s]imulation will be performed\n
\t -n\t [n]umber     - number of simulated events (default:2)\n\n
Analyis:
\t -a\t [a]nalysis - [a]nalysis will be performed\n
\n
\t -h\t [h]elp       - this help\n
The path of the files should be _absolute_\n\n"

#
LOG=/hera/panda/makonyi/Work/codes/log/
source /hera/panda/makonyi/PandaRoot/100514/built/config.sh

#
OPTIND=1 
SCR="Scripts"
# Initialize our own variables:
number=2 
process="analysis"
base=""
verbose=0

while getopts "sn:ab:h?" opt; do
    case $opt in
        s) process="simulation";;
        n) number=$OPTARG;;
        a) process="analysis";;
        b) base=$OPTARG;;
        h|\?)clear; echo -e $usage; exit 0;;
    esac
done

echo $process
echo $base

#if there is no '-d' given, use a default name 
if [[ "$base" == "" ]]; then
    base="Geant4.15GeV"
fi
if [ ! -d $base ]; then
    echo "Creating directory"
    mkdir $base;
fi

if [[ "$process" == simu* ]]; then
    if [ ! -d $base/1 ]; then
        echo $base/1 is not existing
        cd $base;
        $SCR/dirmaker.sh > /dev/null; 
        cd ..;    
    fi
    
    clear; echo "Simulation"
    cat $SCR/Run0.sh | sed "s/base=/base=$base/g" | sed "s@path=@path=`pwd`@g"  > temp.sh
    id=`qsub -t 1-1000 temp.sh $number| awk '{print $3}' | awk -F. '{print $1}'`
    sleep 10s
    ./wait.sh $id
fi


# if [[ "$process" != [Aa][Nn][Aa][Ll]* ]]; then
if [[ "$process" == anal* ]]; then
    echo "Analysis"
    clear; echo "Run"
    cat $SCR/Run1.sh | sed "s/base=/base=$base/g" | sed "s@path=@path=`pwd`@g"  > temp.sh
    # cat Run1.sh | sed "s/base=/base=$base/g" > temp.sh
    id=`qsub -t 1-1000 temp.sh | awk '{print $3}' | awk -F. '{print $1}'`
    sleep 10s
    ./wait.sh $id

    clear; echo "Hadd1"
    cat $SCR/Run2.sh | sed "s/base=/base=$base/g" | sed "s@path=@path=`pwd`@g"  > temp.sh
#    cat Run2.sh | sed "s/base=/base=$base/g" > temp.sh
    id=`qsub -t 1-100 temp.sh | awk '{print $3}' | awk -F. '{print $1}'`
    sleep 10s
    ./wait.sh $id

    clear; echo "Hadd2"
    cat $SCR/Run3.sh | sed "s/base=/base=$base/g" | sed "s@path=@path=`pwd`@g"  > temp.sh
#    cat Run3.sh | sed "s/base=/base=$base/g" > temp.sh
    id=`qsub -t 1-10 temp.sh | awk '{print $3}' | awk -F. '{print $1}'`
    sleep 10s
    ./wait.sh $id

    clear; echo "Final Hadd"
    cat $SCR/Run4.sh | sed "s/base=/base=$base/g" | sed "s@path=@path=`pwd`@g"  > temp.sh
#    cat Run4.sh | sed "s/base=/base=$base/g" > temp.sh
    id=`qsub -t 1-1 temp.sh | awk '{print $3}' | awk -F. '{print $1}'`
    sleep 10s
    ./wait.sh $id
fi

if [ ! -d log ]; then
    mkdir log
fi

find . -maxdepth 1 -name "job_submit.o*" | xargs -I {} mv {} log
    
# if [ -a temp.sh ]; then
#     rm temp.sh
# fi
