#!/bin/bash 
################################################################
## script for running sim&rec for LMD with BOX or DPM generator
## [with pixels sensors set-up!]
## 13/11/2012 1st version
## 06/09/2013 updated version
## 23/03/2014 more or less final version
## A. Karavdina
## 24/4/2019 updated for current PandaRoot version
## R. Klasen
################################################################

stage() {
	echo -e "\e[93m"
	echo "# --------------------------------- "
	echo "# --- $1 "
	echo "# --------------------------------- "
	echo -e "\e[0m"
}
smallstage() {
	echo -e "\e[93m# --- $1 \e[0m"
}

#! ####### instructions ##############################################
manual(){
	echo "
	bash script to run the current full simulation chain with simple options.
	you can run it interactively (default, no command line arguments) and be prompted
	for all parameters, all of which have default values.

	This script can also be used to run non-interactive by specifying four command
	line arguments (in this order!):

	- momentum
	- number of events
	- number of tracks
	- start event

	So the script can be run headlessley as well. You can also run multiple
	instances of this script to background, so that the simulations
	happen concurrently like this:

	./runLumiPixelFullChain.sh 1.5 10000 10 0 	& > /dev/null	
	./runLumiPixelFullChain.sh 1.5 10000 10 10000 	& > /dev/null
	./runLumiPixelFullChain.sh 1.5 10000 10 20000 	& > /dev/null
	./runLumiPixelFullChain.sh 1.5 10000 10 30000 	& > /dev/null

	And then look at the process monitor and wait until all jobs have finished.

	ATTENTION! There still some issues with this, some jobs crash for unknown
	reasons. Maybe it's because of GEANT from the external packages which
	has trouble on 64 bit systems when compiler optimizations are turned on.

	Don't use for analysis! Use only for development/testing!
	"
}

#! ####### read user parameters #######################################
runInteractive(){
	
	read -p "Enter momentum from 1.5, 4.06, 8.9, 11.91, 15 [GeV/c] [1.5]: " mom
	mom=${mom:-1.5}
	echo "Beam mometum is: $mom!"

	read -p "Select number of events [1000]: " numEvents
	numEvents=${numEvents:-1000}

	read -p "Select number of tracks per event [1]: " numTracks
	numTracks=${numTracks:-1}

	read -p "Select start event [0]: " startEvt
	startEvt=${startEvt:-0}

	runSims $mom $numEvents $numTracks $startEvt
}

#! ####### run simulations with 4 arguments! #######################################
runSims(){

	if [ "$#" -ne 4 ]; then
		echo "Internal script error. This should never happen. Exiting!"
		exit 1
	fi

	# these four parameters MUST be present
	pbeam=$1
	numEv=$2
	numTrks=$3
	startEvt=$4


	momStr=$(echo $pbeam | sed -e '{ s/\./_/g; }')
	path=${VMCWORKDIR}/macro/detectors/lmd/testFullChain/mom-${momStr}/
	mkdir -p ${path}

	# use box generator
	generator=box
	#generator=dpm
	#generator=ftf

	# misalignmment and alginment matrix files
	misalignMatrices=""
	alignMatrices=""

	# true: shift data, false: shift geometry
	usePointTransform=false

	# produce noise hits, doesn't work properly yet
	hitNoise=false

	# merge hits on sensors from different sides, sould always be true
	mergedHits=true

	# Skip kinematic filter (before back-propagation)
	SkipFilt=false

	# switch on "missing plane" search algorithm
	misspl=true

	# select track finder
	trackFinder="CA"
	#trackFinder="Follow"

	# select Fitter
	trackFitter="Minuit"
	#trackFitter="KalmanGeane"
	#trackFitter="KalmanRK"

	# use cuts during trk seacrh with "CA". Should be 'false' if sensors missaligned!
	trkcut=true

	# if SkipFilt=false (XThetaCut or YPhiCut) or BoxCut should be true:
	# X-Theta kinematic cut before back-propagation, add constant shift dX[cm] if beam is shifted
	XThetaCut=true
	dX=0.

	# Y-Phi kinematic cut before back-propagation, add constant shift dY[cm] if beam is shifted
	YPhiCut=true
	dY=0.

	# BOX cut before back-propagation
	BoxCut=false

	# select back propagator
	backPropagator="Geane"
	#backPropagator="RK"

	# Clean after back-propagation (M-cut: momentum or MVA cut)
	CleanSig=true

	# Combined filter (X&Y and M-cut)
	CombFilt=false

	# Write all MC info in TrkQA array
	WrAllMC=true

	#! ====================================
	stage "Monte Carlo Simulation"
	# =====================================

	if [ $generator = "box" ]; then
		
		# box generator
		smallstage "using box generator"
		root -l -b -q 'runLumiPixel0SimBox.C('${numEv}','${startEvt}',"'${path}'",'0',2112,'${pbeam}','${numTrks}','0')' > /dev/null 2>&1

	elif [ $generator = "dpm" ]; then
		
		# DPM generator
		smallstage "using DPM direct"
		# TODO: use SimDPM.C instead of SimDPMDirect, also account for misalignment
		modeDPM=0 #inel
		modeDPM=1 #el+inel
		modeDPM=2 #el
		root -l -b -q runLumiPixel0SimDPMDirect.C\(${numEv},${startEvt},${pbeam},"\"${path}\"",0,0,$modeDPM,0.12\)

	elif [ $generator = "ftf" ]; then
		
		# FTF Generator
		smallstage "simulation with FTF"
		# Attention! For inelastic events only add "#noElastics" /pgenerators/FtfEvtGen/PbarP.mac !
		root -l -b -q runLumiPixel0SimFTFDirect.C\(${numEv},${startEvt},${pbeam},"\"${path}\"",0,0\)

	else
		echo "ERROR! Invalid Generator specified! Exiting."
		exit 1
	fi

	#! ====================================
	stage "Hit Digitization"
	# =====================================

	root -l -b -q 'runLumiPixel1Digi.C('${numEv}','${startEvt}',"'${path}'", "'${misalignMatrices}'", '${usePointTransform}', '0')'

	if $hitNoise; then
		smallstage "applying noise"
		# generate noise
		root -l -b -q 'runLumiPixel1bDigiNoise.C('${numEv}','${startEvt}',"'${path}'",'0', '0')'
		# generate signal and noise
		root -l -b -q 'runLumiPixel1cDigiSigNoise.C('${numEv}','${startEvt}',"'${path}'",'0', '0')'
	fi

	#! ====================================
	stage "Hit Reconstruction, Merge"
	# =====================================

	# --- hit reco
	smallstage "reco hits"
	root -l -b -q 'runLumiPixel2Reco.C('${numEv}','${startEvt}',"'${path}'", "'${alignMatrices}'", "'${misalignMatrices}'", '${usePointTransform}', '0')'

	if $mergedHits; then
		# --- hit merge
		smallstage "merging hits"
		root -l -b -q 'runLumiPixel2bHitMerge.C('${numEv}','${startEvt}',"'${path}'",'0')'
	fi

	#! ====================================
	stage "PairFinder, Matrix Finder"
	# =====================================

	smallstage "finding pairs"
	root -l -b -q 'runLumiPixel2ePairFinder.C('${numEv}','${startEvt}',"'${path}'", '0')'

	smallstage "finding matrices"
	# TODO: python script here, this is external!

	#! ====================================
	stage "Track Finder, Fitter, Filter"
	# =====================================

	smallstage "track finder"
	# NB: CA can use merged or single(not merged) hits, Trk-Following can't
	root -l -b -q 'runLumiPixel3Finder.C('${numEv}', '${startEvt}', "'${path}'",0 ,"'$trackFinder'",'${misspl}', '${mergedHits}', '${trkcut}', '${pbeam}')'

	smallstage "track fitter"
	# radLen = average effective thickness X/X0[%]
	radLen=0.32
	root -l -b -q 'runLumiPixel4Fitter.C('${numEv}', '${startEvt}', "'${path}'", 0, "'$trackFitter'", '${mergedHits}', '${radLen}')'

	smallstage "track filter"
	# track filter (on number of hits and chi2)
	root -l -b -q 'runLumiPixel4aFilter.C('${numEv}', '${startEvt}', "'${path}'", 0, '${mergedHits}', '${SkipFilt}', '${XThetaCut}', '${YPhiCut}', '${BoxCut}', '${dX}', '${dY}')' 

	# save filtered results as standart Track array
	mv ${path}/Lumi_Track_${startEvt}.root ${path}/Lumi_TrackNotFiltered_${startEvt}.root 
	cp ${path}/Lumi_TrackFiltered_${startEvt}.root ${path}/Lumi_Track_${startEvt}.root

	# doesn't work
	if false; then
		smallstage "noise track filter"
		#noise tracks suppression (NTS)
		root -l -b -q runLumiPixel4bNoiseTrkFilter.C\(${numEv},${startEvt},"\"${path}\"",0,${pbeam}\) 
		#save filtered results as standart Track array
		mv ${path}/Lumi_Track_${startEvt}0.root ${path}/Lumi_TrackNotNoiseSuppressed_${startEvt}.root 
		cp ${path}/Lumi_TrackNoiseSuppressed_${startEvt}.root ${path}/Lumi_Track_${startEvt}.root
	fi

	#! ====================================
	stage "Back Propagation"
	# =====================================

	root -l -b -q 'runLumiPixel5BackProp.C('${numEv}', '${startEvt}', "'${path}'", '0', "'${backPropagator}'", '${mergedHits}', '${pbeam}')'

	#clean signal from background (momentum cut or MVA cut)
	if $CleanSig; then
		smallstage "cleaning signal"
		root -l -b -q 'runLumiPixel5bCleanSig.C('${numEv}', '${startEvt}', "'${path}'", '0', '${pbeam}', '${dX}', '${dY}')'

	fi
	#combined filter (X&Y and M cuts)
	if $CombFilt; then
		smallstage "applying combined filter"
		root -l -b -q runLumiPixel5cCombFilt.C\(${numEv},${startEvt},"\"${path}\"",0,${pbeam},${dX},${dY}\)
	fi

	#! ====================================
	stage "Track Quality Assurance"
	# =====================================

	if $CombFilt; then 
		smallstage "using comboned filter"
		# PixelQA doesn't work at the moment
		#root -l -b -q runLumiPixel7QA.C\(${numEv},0,"\"${path}\"",0,${pbeam},${CombFilt}\)
		root -l -b -q runLumiPixel7TrksQA.C\(${numEv},${startEvt},"\"${path}\"",0,${pbeam},${WrAllMC},${CombFilt}\)
	else
		smallstage "using cleaned signal"
		# PixelQA doesn't work at the moment
		#root -l -b -q runLumiPixel7QA.C\(${numEv},0,"\"${path}\"",0,${pbeam},${CleanSig}\)
		root -l -b -q runLumiPixel7TrksQA.C\(${numEv},${startEvt},"\"${path}\"",0,${pbeam},${WrAllMC},${CleanSig}\)
	fi

	# TODO: collect individual stage successes before proclaiming a successful run
	if true; then
		stage "Macro finished successfully!"
	fi
}

runMulti(){

	echo "
	Running 8 threads of mc data. This is experimental and should not be used.
	
	I mean it.
	
	You have been warned! You'll see no output from SimBox.
	"

	stage "Multihreaded, this will produce A LOT OF unordered output!"

	# TODO: maybe change this to take a variable number of threads

	# run 8 threads
	runSims 1.5 100000 10      0 & 
	runSims 1.5 100000 10 100000 & 
	runSims 1.5 100000 10 200000 & 
	runSims 1.5 100000 10 300000 &
	runSims 1.5 100000 10 400000 &
	runSims 1.5 100000 10 500000 & 
	runSims 1.5 100000 10 600000 &
	runSims 1.5 100000 10 700000 &

	stage "And now, just wait."
	exit 0
}

#! ####### main function #######################################

if [ "$#" -eq 0 ]; then
	runInteractive
	exit 0
elif [ "$#" -eq 1 ]; then
	if [ $1 = "MULTI" ]; then
		runMulti
		exit 0
	else
		echo "Unknown option."
		exit 1
	fi
elif [ "$#" -eq 4 ]; then
	runSims $1 $2 $3 $4
	exit 0
else
	echo "Invalid number of parameters, use none or four."
	manual
	exit 1
fi