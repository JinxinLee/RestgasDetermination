#!/bin/bash

JobMax=751

mu=-2.02
Quadru=(-4.0 -3.6 -3.2 -2.8 -2.4 -2.0 -1.6 -1.2 -0.8 -0.4 0.0) 
Energy1=(0.519267 0.51926 0.519253 0.519247 0.51924 0.519234 0.519227 0.519221 0.519214 0.519208 0.519201)
Energy2=(0.519823 0.519832 0.519841 0.51985 0.519859 0.519868 0.519877 0.519886 0.519895 0.519904 0.519913)
Energy3=(0.520486 0.520492 0.520498 0.520505 0.520511 0.520517 0.520523 0.52053 0.520536 0.520542 0.520548)
Energy4=(0.5212 0.52119 0.521181 0.521172 0.521162 0.521153 0.521144 0.521134 0.521125 0.521116 0.521106)


echo  ${#Quadru[@]} ${#Energy1[@]} ${#Energy2[@]} ${#Energy3[@]} ${#Energy4[@]}

for ((i=0 ; i<${#Quadru[@]}; i ++))
do
	echo $i
	 ./script_Gamma_SimArrayMulti.sh -E ${Energy1[$i]} -s 12 -n 250 -i 1 -e 2860000 -g 43 -O 1 -m $mu -Q ${Quadru[$i]}
	 x=$(qstat -t | grep -c ${USER})
	while [ $x -gt $JobMax ]
	do
		echo "Still some jobs running: waiting to start new jobs"
		date
		sleep 30s
		#x=$(ls | grep -c ext)
		x=$(qstat -t | grep -c ${USER})
	done			### end of while "double queue" loop
	 ./script_Gamma_SimArrayMulti.sh -E ${Energy2[$i]} -s 12 -n 250 -i 1 -e 2580000 -g 43 -O 1 -m $mu -Q ${Quadru[$i]}
	 x=$(qstat -t | grep -c ${USER})
	while [ $x -gt $JobMax ]
	do
		echo "Still some jobs running: waiting to start new jobs"
		date
		sleep 30s
		#x=$(ls | grep -c ext)
		x=$(qstat -t | grep -c ${USER})
	done			### end of while "double queue" loop
	 ./script_Gamma_SimArrayMulti.sh -E ${Energy3[$i]} -s 12 -n 250 -i 1 -e 2330000 -g 43 -O 1 -m $mu -Q ${Quadru[$i]}
	 x=$(qstat -t | grep -c ${USER})
	while [ $x -gt $JobMax ]
	do
		echo "Still some jobs running: waiting to start new jobs"
		date
		sleep 30s
		#x=$(ls | grep -c ext)
		x=$(qstat -t | grep -c ${USER})
	done			### end of while "double queue" loop
	 ./script_Gamma_SimArrayMulti.sh -E ${Energy4[$i]} -s 12 -n 250 -i 1 -e 2110000 -g 43 -O 1 -m $mu -Q ${Quadru[$i]}
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
exit 0
echo foo
for SekTar in 12

do
	#gamma line (n=11,l=10,j=23/2) -> (n=10,l=9,j=21/2)
	for Energy in 0.5189
	do
		./script_Gamma_SimArrayMulti.sh -E $Energy -s $SekTar -n 250 -i 1 -e 2860000 -g 43 -O 1 -m -2.5 -Q -2.8
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
	#gamma line (n=11,l=10,j=21/2) -> (n=10,l=9,j=19/2)
	for Energy in 0.5198
	do
		#./script_Gamma_SimArrayMulti.sh -E $Energy -s $SekTar -n 250 -i 1 -e 2580000 -g 43 -O 1 -m -2.5 -Q -2.8
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
	#gamma line (n=11,l=10,j=19/2) -> (n=10,l=9,j=17/2)
	for Energy in 0.5208
	do
		#./script_Gamma_SimArrayMulti.sh -E $Energy -s $SekTar -n 250 -i 1 -e 2330000 -g 43 -O 1 -m -2.5 -Q -2.8
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
	#gamma line (n=11,l=10,j=17/2) -> (n=10,l=9,j=15/2)
	for Energy in 0.5215
	do
		#./script_Gamma_SimArrayMulti.sh -E $Energy -s $SekTar -n 250 -i 1 -e 2110000 -g 43 -O 1 -m -2.5 -Q -2.8
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
