#!/bin/bash      

if [ $1 ] && [ $2 ] && [ $3 ] && [ $4 ] && [ $5 ] && [ $6 ]; then
          pathname=$6
          dirname=$5
          if [ ! -d $pathname ]; then
            mkdir $pathname
          fi

          #if [ ! -d /tmp/$dirname ]; then
          #  mkdir /tmp/$dirname
          #fi

          #start_evt=$1
	  #num_evts=$2
	  #mom=$3
	  #gen_input_filename=$4
	  #filename_index=$5

	  num_evts=$1
	  mom=$2
	  gen_input_filename=$3
	  filename_index=$4

          #get input file from storage server
          #if [ ! -d /tmp/inputfiles ]; then
          #  mkdir /tmp/inputfiles
          #fi
          #if [ ! -d /tmp/inputfiles/$dirname ]; then
          #  mkdir /tmp/inputfiles/$dirname
          #fi
          #cp ${gen_input_filename} /tmp/inputfiles/$dirname/.

	  beamX0=0.0
	  beamY0=0.0
	  beam_widthX=0.0
	  beam_widthY=0.0
	  beam_gradX=0.0
	  beam_gradY=0.0
	  beam_grad_sigmaX=0.0
	  beam_grad_sigmaY=0.0

	  if [ $7 ]; then
	    beamX0=$7
	  fi
	  if [ $8 ]; then
	    beamY0=$8
	  fi
	  if [ $9 ]; then
	    beam_widthX=$9
	  fi
	  if [ ${10} ]; then
	    beam_widthY=${10}
	  fi
	  if [ ${11} ]; then
	    beam_gradX=${11}
	  fi
	  if [ ${12} ]; then
	    beam_gradY=${12}
	  fi
	  if [ ${13} ]; then
	    beam_grad_sigmaX=${13}
	  fi
	  if [ ${14} ]; then
	    beam_grad_sigmaY=${14}
	  fi

          verbositylvl=0
          start_evt=0

          #ok run these jobs fully on a local machine (also save data there)

	  #time root -l -b -q '~/lmd/runLumi0Gen.C('${num_evts}','${mom}',"'${gen_input_filename}'","'${filename_index}'", '${start_evt}', "'/tmp/$dirname'",'$beamX0', '$beamY0', '${beam_widthX}', '${beam_widthY}', '${beam_gradX}', '${beam_gradY}', '${beam_grad_sigmaX}', '${beam_grad_sigmaY}')' 2>&1 >> $pathname/runLumi0Gen_${filename_index}.log
thname
          ##time root -l -b -q 'MCinfo.C($1,$2)'
          #time root -l -b -q '~/lmd/runLumi1Digi.C('${num_evts}',"'${filename_index}'",'${start_evt}',"'/tmp/$dirname'", '$verbositylvl')' 2>&1 >> /tmp/$dirname/runLumi1Digi_${filename_index}.log
          #time root -l -b -q '~/lmd/runLumi2Reco.C('${num_evts}',"'${filename_index}'",'${start_evt}',"'/tmp/$dirname'", true, '$verbositylvl')' 2>&1 >> /tmp/$dirname/runLumi2Reco_${filename_index}.log
          ##time root -l -b -q 'runLumi3Finder.C('$1',0,"tmpOutput", 0, "CA",false)'
          #time root -l -b -q '~/lmd/runLumi3Finder.C('${num_evts}',"'${filename_index}'",'${start_evt}',"'/tmp/$dirname'", '$verbositylvl', "Follow",true)' 2>&1 >> /tmp/$dirname/runLumi3Finder_${filename_index}.log
          #time root -l -b -q '~/lmd/runLumi4Fitter.C('${num_evts}',"'${filename_index}'",'${start_evt}',"'/tmp/$dirname'", '$verbositylvl')' 2>&1 >> /tmp/$dirname/runLumi4Fitter_${filename_index}.log
          ##time root -l -b -q 'MC1stHitandRecLINTrkmatches.C('$1')' >> logCA
          #time root -l -b -q '~/lmd/runLumi5Geane.C('${num_evts}','${mom}',"'${filename_index}'",'${start_evt}',"'/tmp/$dirname'", '$verbositylvl')' 2>&1 >> /tmp/$dirname/runLumi5Geane_${filename_index}.log
	 
          ##ok if everything finished just mv all files to data directory
          #mv /tmp/$dirname/* $pathname/$dirname/.

          #rm -rf /tmp/inputfiles/$dirname
 
          time root -l -b -q '~/lmd/runLumi0Gen.C('${num_evts}','${mom}',"'${gen_input_filename}'","'${filename_index}'", '${start_evt}', "'$pathname'",'$beamX0', '$beamY0', '${beam_widthX}', '${beam_widthY}', '${beam_gradX}', '${beam_gradY}', '${beam_grad_sigmaX}', '${beam_grad_sigmaY}')' 2>&1 >> $pathname/runLumi0Gen_${filename_index}.log
          #time root -l -b -q 'MCinfo.C($1,$2)'
          time root -l -b -q '~/lmd/runLumi1Digi.C('${num_evts}',"'${filename_index}'",'${start_evt}',"'$pathname'", '$verbositylvl')' 2>&1 >> $pathname/runLumi1Digi_${filename_index}.log
          time root -l -b -q '~/lmd/runLumi2Reco.C('${num_evts}',"'${filename_index}'",'${start_evt}',"'$pathname'", true, '$verbositylvl')' 2>&1 >> $pathname/runLumi2Reco_${filename_index}.log
          #time root -l -b -q 'runLumi3Finder.C('$1',0,"tmpOutput", 0, "CA",false)'
          time root -l -b -q '~/lmd/runLumi3Finder.C('${num_evts}',"'${filename_index}'",'${start_evt}',"'$pathname'", '$verbositylvl', "Follow",true)' 2>&1 >> $pathname/runLumi3Finder_${filename_index}.log
          time root -l -b -q '~/lmd/runLumi4Fitter.C('${num_evts}',"'${filename_index}'",'${start_evt}',"'$pathname'", '$verbositylvl')' 2>&1 >> $pathname/runLumi4Fitter_${filename_index}.log
          #time root -l -b -q 'MC1stHitandRecLINTrkmatches.C('$1')' >> logCA
          time root -l -b -q '~/lmd/runLumi5Geane.C('${num_evts}','${mom}',"'${filename_index}'",'${start_evt}',"'$pathname'", '$verbositylvl')' 2>&1 >> $pathname/runLumi5Geane_${filename_index}.log
          #time root -l -b -q 'MCandGEANTTrkmatchesBgk.C('$1'0,0,'$2')'
          #time root -l -b -q 'HitsInfoNEW.C('$1')'
          #time root -l -b -q 'MCandRECTrkmatches.C('$1')'
else
  echo "reconstructMCData [start event number] [number of events] [beam momentum] [generator input filename] [numbering index] [dirname] [beamX0] [beamY0] [beam width X] [beam width Y] [beam grad X] [beam grad Y] [beam grad sigma X] [beam grad sigma Y]"
fi
