#!/bin/bash

JobMax=1000


for SekTar in 12
#for SekTar in 43
#for SekTar in 12 22 32 42 52
do
	for Energy in 0.050 0.100 0.150 0.200 0.250 0.284 0.300 0.350 0.400 0.450 0.500 0.550 0.558 0.600 0.650 0.700 0.750 0.800 0.850 0.900 0.950 1.000 1.100 1.332 1.684 2.000 2.429 2.780 3.049 4.000 4.704 5.590 6.000 6.380 6.760 7.940 8.000 10.000
	#for Energy in 1.100 1.332 1.684 2.000 2.429 2.780 3.049 4.000 4.704 5.590 6.000 6.380 6.760 7.940 8.000 10.000
	#for Energy in 0.050 0.150 0.200 0.250 0.284 0.300 0.350 0.400 0.450 0.500 0.550 0.558 0.600 0.650 0.700 0.750 0.800 0.850 0.900 0.950 1.000
	#for Energy in 0.050 0.100 0.150 0.200 0.250 0.284 0.300
	#for Energy in 0.100
	do
		./script_Gamma_SimArrayMulti.sh -E $Energy -s $SekTar -n 250 -i 1 -e 10000000 -g 43
		x=$(qstat -t | grep -c ${USER})		
		while [ $x -gt $JobMax ]
		do
			echo "Still some jobs running: waiting to start new jobs"
			date
			sleep 30s
			#x=$(ls | grep -c ext)
			x=$(qstat -t | grep -c ${USER})
		done			### end of while "double queue" loop
	done
done
