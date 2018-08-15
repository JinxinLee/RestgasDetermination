#!/bin/bash

. /home/konctbel/Downloads/fairsoft/fairsoft_mar15p6/pandaroot/buildPanda/config.sh

export run
run=$1
echo $run
#cp -v simparams.root simparams_v0_r$run.root
root -l -b -q -w sim_complete_runs.C\($run,\"TGeant4\"\)
root -l -b -q -w digi_complete_runs.C\($run\)
root -l -b -q -w reco_complete_runs.C\($run\)
root -l -b -q -w pid_complete_runs.C\($run\)
