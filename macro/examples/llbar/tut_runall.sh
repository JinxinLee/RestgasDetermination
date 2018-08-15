#!/bin/bash

nev=1000

if test "$1" != ""; then
  nev=$1
fi

root -l -b -q -w sim_complete.C\($nev\)
root -l -b -q -w digi_complete.C
root -l -b -q -w recoideal_complete.C
root -l -b -q -w pidideal_complete.C
