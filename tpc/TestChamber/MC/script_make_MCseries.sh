#!/bin/bash

for SPREAD in 006 007 008 009 010 011 012 013 014 015 016 017 018 019 020 021 023 024 025 026 027 028 029 030 031; do

    for GAIN in 4500 5000 5500 6000 6500; do
	FILENAME=TCmc_gain$GAIN\_spread$SPREAD.root
#do digi then rename dreggn2.root to $i.root or so
	echo doing $FILENAME;
	sed  -i.bak.$GAIN.$SPREAD "s/Spread:d 0\..*/Spread:d 0.$SPREAD/" ../pandaroot/tpc/tpc.TestChamber.par
	sed -i.bak.$GAIN.$SPREAD.1 "s/Gain:d .*/Gain:d $GAIN/" ../pandaroot/tpc/tpc.TestChamber.par
	rm dreggn2.root
	root -q -l ../pandaroot/tpc/TestChamber/MC/runDigi.C
	mv dreggn2.root $FILENAME
    done

done


