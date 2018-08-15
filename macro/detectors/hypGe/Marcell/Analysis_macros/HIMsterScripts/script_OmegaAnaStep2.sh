#!/bin/bash

path=${SIMDATADIR}/Gamma/Ana/CombinedData/
for mu in -2.0
do
	for Q in 0.0 -0.4 -0.8 -1.2 -1.6 -2.0 -2.4 -2.8 -3.2 -3.6 -4.0
	do
		for Psf in 1 1.5
		do
			echo $mu $Q  $Psf
			f1=`ls $path*286*$mu,*$Q*$Psf.root`
			f2=`ls $path*258*$mu,*$Q*$Psf.root`
			f3=`ls $path*233*$mu,*$Q*$Psf.root`
			f4=`ls $path*211*$mu,*$Q*$Psf.root`
			
			root -l -b -q '../OmegaQuadrupoleFourEnergiesCombination.C ("'${mu}'","'${Q}'","'${Psf}'","'${f1}'","'${f2}'","'${f3}'","'${f4}'")'
			#echo root -l -b -q '../OmegaQuadrupoleFourEnergiesCombination.C ("'${mu}'","'${Q}'","'${Psf}'")'
		done
	done
done 
