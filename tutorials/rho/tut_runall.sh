#!/bin/bash

nev=1000

if test "$1" != ""; then
  nev=$1
fi

root -l -b -q -w tut_sim.C\($nev\)
root -l -b -q -w tut_dig.C
root -l -b -q -w tut_rec.C
root -l -b -q -w tut_pid.C
