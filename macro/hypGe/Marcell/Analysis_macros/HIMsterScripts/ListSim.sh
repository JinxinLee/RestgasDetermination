#!/bin/bash

####script should be executed via "ListSim" command to use the right folder

subdir=Gamma
#events=10000000
events1=2860000
events2=2580000
events3=2330000
events4=2110000
#geo=4
SimListFile=${SIMDATADIR}/${subdir}/txtfiles/GammaFilesToAnaList.txt 
rm $SimListFile
#echo ls Sim_Geo36_E*${events}*ST?${geo}/Sim*.root | grep -v Simpara >> $SimListFile
#wc -l $SimListFile
#ls Sim_Geo36_E1.000*${events}*ST${geo}/Sim*.root | grep -v Simpara >> $SimListFile
#wc -l $SimListFile

for setup in 43
do
	for i in 12
	do
		#for j in 0.050 0.100 0.150 0.200 0.250 0.284 0.300
		#do
			ls -d *Geo$setup*Evts*ST$i*mu* | wc -w
			ls -f *Geo$setup*Evts${events1}*ST$i*mu*/*.root | grep -v Simparams | grep root >> $SimListFile
			ls -f *Geo$setup*Evts${events2}*ST$i*mu*/*.root | grep -v Simparams | grep root >> $SimListFile
			ls -f *Geo$setup*Evts${events3}*ST$i*mu*/*.root | grep -v Simparams | grep root >> $SimListFile
			ls -f *Geo$setup*Evts${events4}*ST$i*mu*/*.root | grep -v Simparams | grep root >> $SimListFile
		#done
	done
done
wc -l $SimListFile
