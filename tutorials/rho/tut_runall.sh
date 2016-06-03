#!/bin/bash

nev=1000

if test "$1" != ""; then
  nev=$1
fi

root -l -b -q  ../../macro/master/sim_complete.C\($nev\)
root -l -b -q  ../../macro/master/full_complete.C
