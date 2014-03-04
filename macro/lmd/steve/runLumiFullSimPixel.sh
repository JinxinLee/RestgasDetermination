#!/bin/bash      

cd ${VMCWORKDIR}/macro/lmd/steve

#include some helper functions
. bashFunctions.sh

cd ..

if [ $# -eq '16' ]; then
  num_evts=$1
  mom=$2
  gen_input_file_stripped=$3
  dirname=$4
  pathname=$5
  filename_index=$6

  beamX0=$7
  beamY0=$8
  targetZ0=$9
  beam_widthX=${10}
  beam_widthY=${11}
  target_widthZ=${12}
  beam_gradX=${13}
  beam_gradY=${14}
  beam_grad_sigmaX=${15}
  beam_grad_sigmaY=${16}
fi

if [ ${PBS_ARRAYID} ]; then
  filename_index=${PBS_ARRAYID}
fi

gen_input_filename="${gen_input_file_stripped}_${filename_index}.root"

if [ ! -d $pathname ]; then
  mkdir $pathname
fi

verbositylvl=0
start_evt=$((${num_evts}*${filename_index})) #number of events * filename index is startevt
#switch on "missing plane" search algorithm
misspl=true
#use cuts during trk seacrh with "CA". Should be 'false' if sensors missaligned!
trkcut=true
#merge hits on sensors from different sides. true=yes
mergedHits=true
#Skip kinematic filter (before back-propagation)
SkipFilt=false
## if SkipFilt=false (XThetaCut or YPhiCut) or BoxCut should be true:
## X-Theta kinematic cut before back-propagation
XThetaCut=true
YPhiCut=true
## BOX cut before back-propagation
BoxCut=false

#simulation
check_stage_success "$pathname/Lumi_MC_${start_evt}.root"
if [ 0 -eq "$?" ]; then
  root -l -b -q 'runLumiPixel0SimDPM.C('${num_evts}','${start_evt}','${mom}',"'${gen_input_filename}'", "'${pathname}'",'$beamX0', '$beamY0', '${targetZ0}', '${beam_widthX}', '${beam_widthY}', '${target_widthZ}', '${beam_gradX}', '${beam_gradY}', '${beam_grad_sigmaX}', '${beam_grad_sigmaY}','$verbositylvl')'
fi

#digitization
check_stage_success "$pathname/Lumi_digi_${start_evt}.root"
if [ 0 -eq "$?" ]; then
  root -l -b -q 'runLumiPixel1Digi.C('${num_evts}','${start_evt}',"'${pathname}'",'$verbositylvl')'
fi

check_stage_success "$pathname/Lumi_reco_${start_evt}.root"
if [ 0 -eq "$?" ]; then
  root -l -b -q 'runLumiPixel2Reco.C('${num_evts}','${start_evt}',"'${pathname}'",'$verbositylvl',false)'
fi

#merge hits
check_stage_success "$pathname/Lumi_recoMerged_${start_evt}.root"
if [ 0 -eq "$?" ]; then
  root -l -b -q 'runLumiPixel2bHitMerge.C('${num_evts}','${start_evt}',"'${pathname}'",'$verbositylvl')'
fi

### change "CA" --> "Follow" if you want to use Trk-Following as trk-search algorithm
### NB: CA can use merged or single(not merged) hits, Trk-Following can't
check_stage_success "$pathname/Lumi_TCand_${start_evt}.root"
if [ 0 -eq "$?" ]; then
  #root -l -b -q 'runLumiPixel3Finder.C('${num_evts}','${start_evt}',"'${pathname}'",'$verbositylvl',"CA",'${misspl}','${mergedHits}','${trkcut}','${mom}')' 2>&1 >> $pathname/runLumiPixel3Finder_${start_evt}.log
  root -l -b -q 'runLumiPixel3Finder.C('${num_evts}','${start_evt}',"'${pathname}'",'$verbositylvl',"Follow",'${misspl}','${mergedHits}','${trkcut}','${mom}')'
fi

#track fit:
### Possible options: "Minuit", "KalmanGeane", "KalmanRK"
check_stage_success "$pathname/Lumi_TrackNotFiltered_${start_evt}.root"
if [ 0 -eq "$?" ]; then
  root -l -b -q 'runLumi4KalmanFitter.C('${num_evts}', '${start_evt}', "'${pathname}'", '$verbositylvl', 1, '${mergedHits}', "GEANE")'
  #this script output a Lumi_Track_... file. Rename that to the NotFiltered..
  mv $pathname/Lumi_Track_${start_evt}.root $pathname/Lumi_TrackNotFiltered_${start_evt}.root
fi

#track filter (on number of hits and chi2)
check_stage_success "$pathname/Lumi_TrackFiltered_${start_evt}.root"
if [ 0 -eq "$?" ]; then
  #this macro needs Lumi_Track_... file as input so we need to link the unfiltered file
  cd ${pathname}
  ln -sf Lumi_TrackNotFiltered_${start_evt}.root Lumi_Track_${start_evt}.root
  cd -
  root -l -b -q 'runLumiPixel4aFilter.C('${num_evts}', '${start_evt}', "'${pathname}'", '$verbositylvl', '${mergedHits}', '${SkipFilt}', '${XThetaCut}', '${YPhiCut}', '${BoxCut}')'
   
  #now overwrite the Lumi_Track_ sym link with the filtered version
  cd ${pathname}
  ln -sf Lumi_TrackFiltered_${start_evt}.root Lumi_Track_${start_evt}.root
  cd -
fi

# back-propgation GEANE
### Possible options: "Geane", "RK"
check_stage_success "$pathname/Lumi_Geane_${start_evt}.root"
if [ 0 -eq "$?" ]; then
  root -l -b -q 'runLumiPixel5BackProp.C('${num_evts}', '${start_evt}', "'${pathname}'", '$verbositylvl', "RK", '${mergedHits}', '${mom}')'
fi

# # Quality assurance task(s)
# combine MC and reco information
# the last parameter is mc all write flag and needs to be true
# so that all mc events are written even if geometrically missing the sensors
# this is required for the acceptance calculation
check_stage_success "$pathname/Lumi_TrksQA_${start_evt}.root"
if [ 0 -eq "$?" ]; then
  root -l -b -q 'runLumiPixel7TrksQA.C('${num_evts}','${start_evt}',"'${pathname}'",'0','${mom}',true)'
fi
