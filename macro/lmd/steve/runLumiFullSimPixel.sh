#!/bin/bash      

env

cd $PBS_O_WORKDIR

#include some helper functions
. bashFunctions.sh

cd ..

if [ $var1 ] && [ $var2 ] && [ $var3 ] && [ $var4 ] && [ $var5 ]; then
          pathname=$var5
          dirname=$var4
          if [ ! -d $pathname ]; then
            mkdir $pathname
          fi

	  num_evts=$var1
	  mom=$var2
	  filename_index=$PBS_ARRAYID
	  gen_input_filename="${var3}_${PBS_ARRAYID}.root"

	  beamX0=0.0
	  beamY0=0.0
	  beam_widthX=0.0
	  beam_widthY=0.0
	  beam_gradX=0.0
	  beam_gradY=0.0
	  beam_grad_sigmaX=0.0
	  beam_grad_sigmaY=0.0

	  if [ $var7 ]; then
	    beamX0=$var7
	  fi
	  if [ $var8 ]; then
	    beamY0=$var8
	  fi
	  if [ $var9 ]; then
	    beam_widthX=$var9
	  fi
	  if [ ${var10} ]; then
	    beam_widthY=${var10}
	  fi
	  if [ ${var11} ]; then
	    beam_gradX=${var11}
	  fi
	  if [ ${var12} ]; then
	    beam_gradY=${var12}
	  fi
	  if [ ${var13} ]; then
	    beam_grad_sigmaX=${var13}
	  fi
	  if [ ${var14} ]; then
	    beam_grad_sigmaY=${var14}
	  fi

      verbositylvl=0
      start_evt=$((${num_evts}*${filename_index})) #number of events * filename index is startevt
      #switch on "missing plane" search algorithm
	    misspl=true
      # #add multiples scattering error estimation for hits (needed only for trk fit with Minuit). false=no
      useMSerr=false
      #merge hits on sensors from different sides. true=yes
      mergedHits=true

      
      check_stage_success "$pathname/runLumiPixel1Digi_${start_evt}.log"
      if [ 0 -eq "$?" ]; then
        root -l -b -q 'runLumiPixel0SimDPM.C('${num_evts}','${start_evt}','${mom}',"'${gen_input_filename}'", "'${pathname}'",'$beamX0', '$beamY0', '${beam_widthX}', '${beam_widthY}', '${beam_gradX}', '${beam_gradY}', '${beam_grad_sigmaX}', '${beam_grad_sigmaY}','$verbositylvl')' 2>&1 >> /dev/null #$pathname/runLumiPixel0SimDPM_${start_evt}.log
        root -l -b -q 'runLumiPixel1Digi.C('${num_evts}','${start_evt}',"'${pathname}'",'$verbositylvl')' 2>&1 >> $pathname/runLumiPixel1Digi_${start_evt}.log
      fi

      check_stage_success "$pathname/runLumiPixel2Reco_${start_evt}.log"
      if [ 0 -eq "$?" ]; then
        root -l -b -q 'runLumiPixel2Reco.C('${num_evts}','${start_evt}',"'${pathname}'",'$verbositylvl','${useMSerr}',false)' 2>&1 >> $pathname/runLumiPixel2Reco_${start_evt}.log
      fi

      #merge hits
      check_stage_success "$pathname/runLumiPixel2bHitMerge_${start_evt}.log"
      if [ 0 -eq "$?" ]; then
        root -l -b -q 'runLumiPixel2bHitMerge.C('${num_evts}','${start_evt}',"'${pathname}'",'$verbositylvl')' 2>&1 >> $pathname/runLumiPixel2bHitMerge_${start_evt}.log
      fi

      ### change "CA" --> "Follow" if you want to use Trk-Following as trk-search algorithm
      ### NB: CA can use merged or single(not merged) hits, Trk-Following can't
      check_stage_success "$pathname/runLumiPixel3Finder_${start_evt}.log"
      if [ 0 -eq "$?" ]; then
        root -l -b -q 'runLumiPixel3Finder.C('${num_evts}','${start_evt}',"'${pathname}'",'$verbositylvl',"Follow",'${misspl}','${mergedHits}')' 2>&1 >> $pathname/runLumiPixel3Finder_${start_evt}.log
      fi

      # ### Track fit: Miniut
      # ### NB: useMSerr should be _true_ in this case !
      # root -l -b -q runLumiPixel4Fitter.C\(${num_evts},0,"\"${pathname}\"",0,${mergedHits}\)
      # ### or Kalman Fillter with GEANE trk representation
      # ### change "GEANE" --> "RK" if you want to use Runge-Kutta trk.rep.
      # ### NB: useMSerr should be _false_ in this case !
      # the 1 (argument 5) stands for pixel 0 would mean strip sensors...
      check_stage_success "$pathname/runLumi4KalmanFitter_${start_evt}.log"
      if [ 0 -eq "$?" ]; then
        root -l -b -q 'runLumi4KalmanFitter.C('${num_evts}','${start_evt}',"'${pathname}'",'$verbositylvl',1,'${mergedHits}',"GEANE")' 2>&1 >> $pathname/runLumi4KalmanFitter_${start_evt}.log
      fi

      # back-propgation GEANE
      check_stage_success "$pathname/runLumiPixel5BackProp_${start_evt}.log"
      if [ 0 -eq "$?" ]; then
        root -l -b -q 'runLumiPixel5BackProp.C('${num_evts}', '${start_evt}', "'${pathname}'", '$verbositylvl', "RK", true, '${mom}')' 2>&1 >> $pathname/runLumiPixel5BackProp_${start_evt}.log
        #root -l -b -q '~/lmd/runLumi5Geane.C('${num_evts}','${mom}','${start_evt}',"'${pathname}'",'$verbositylvl','${mergedHits}')' 2>&1 >> $pathname/runLumi5Geane_${start_evt}.log
      fi
      # ### or Runge-Kutta for back-propagation
      #root -l -b -q runLumi5RungeKutta.C\(${num_evts},${mom},0,"\"${pathname}\"",0,${mergedHits}\)

      # combine MC and reco information
      check_stage_success "$pathname/runLumiPixel7TrksQA_${start_evt}.log"
      if [ 0 -eq "$?" ]; then
        root -l -b -q '~/lmd/runLumiPixel7TrksQA.C('${num_evts}','${start_evt}',"'${pathname}'",'0','${mom}',false)' 2>&1 >> $pathname/runLumiPixel7TrksQA_${start_evt}.log
      fi
else
  echo "reconstructMCData [start event number] [number of events] [beam momentum] [generator input filename] [numbering index] [dirname] [beamX0] [beamY0] [beam width X] [beam width Y] [beam grad X] [beam grad Y] [beam grad sigma X] [beam grad sigma Y]"
fi
