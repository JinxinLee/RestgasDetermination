#!/bin/bash

for SekTar in 13 43
#for SekTar in 12 22 32 42 52
do
	for Energy in 0.050 0.100 0.150 0.200 0.250 0.284 0.300 0.350 0.400 0.450 0.500 0.550 0.558 0.600 0.650 0.700 0.750 0.800 0.850 0.900 0.950 1.000
	#for Energy in 0.558
	do
		./script_Gamma_Sim.sh -E $Energy -s $SekTar 
		#sleep 5m
	done
done
