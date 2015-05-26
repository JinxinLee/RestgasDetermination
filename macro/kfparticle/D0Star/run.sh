#!/bin/sh

NEVENTS=10000

rm -rf *.root *.log *.dat *.txt

root -l -b -q "sim_complete.C("$NEVENTS")"      2>&1 | tee sim.log
root -l -b -q "digi_complete.C" 2>&1 | tee digi.log
root -l -b -q "recoideal_complete.C" 2>&1 | tee reco.log
root -l -b -q "pid_complete.C" 2>&1 | tee pid.log
root -l -b -q "kfparticle.C" 2>&1 | tee kfparticle.log

