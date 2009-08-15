#!/bin/bash

ALIGNMENT=alignments/2alignment`printf %03i 1`.txt
ALIGNMENT_SED=alignments\\\/2alignment`printf %03i 1`.txt

for i in `seq 1 10` ; do
    TRACKHIST=tracks/2trackHist`printf %03i $i`.root

    echo calling tracker with alignmnet $ALIGNMENT, histograms in $TRACKHIST
    ./tracker run-1044.root $ALIGNMENT brute $TRACKHIST

    ALIGNINPUT=tracks/2track`printf %03i $i`.root
    ALIGNINPUT_SED=tracks\\\/2track`printf %03i $i`.root

    echo copying tracker output to $ALIGNINPUT
    mv tracks.root $ALIGNINPUT

    CONFIGFILE=`mktemp /tmp/alingment.config.XXXXXX`
    NEXTALIGNMENT=alignments/2alignment`printf %03i $((i+1))`.txt
    NEXTALIGNMENT_SED=alignments\\\/2alignment`printf %03i $((i+1))`.txt
    ALIGNHIST=alignments/2alignHist`printf %03i $i`.root
    ALIGNHIST_SED=alignments\\\/2alignHist`printf %03i $i`.root
    sed -e "s/^Alignment input.*$/Alignment input = $ALIGNMENT_SED/" ../../../alignment/config.config > $CONFIGFILE
    sed -i -e "s/^Alignment output.*$/Alignment output = $NEXTALIGNMENT_SED/" $CONFIGFILE
    sed -i -e "s/^Track file.*$/Track file = $ALIGNINPUT_SED/" $CONFIGFILE
    sed -i -e "s/^HistogramFile.*$/HistogramFile = $ALIGNHIST_SED/" $CONFIGFILE

    echo calling alignment with $CONFIGFILE
    LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../../.. ../../../alignment/alignment $CONFIGFILE

    rm -f $CONFIGFILE
    ALIGNMENT=$NEXTALIGNMENT
    ALIGNMENT_SED=$NEXTALIGNMENT_SED
done
