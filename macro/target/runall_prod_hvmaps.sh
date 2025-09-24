#!/bin/bash

# USAGE:
# sbatch -a<min>-<max> jobsim_kronos.sh <pref> <nevt> <dec> <mom> <saveall>
#
#SBATCH -J pndsim
#SBATCH --time=8:00:00
#SBATCH --get-user-env
#SBATCH -e data/slurmlog/slurm_%j_errout.log
#SBATCH -o data/slurmlog/slurm_%j_errout.log

prefix=9999
nEvts=1000
dec="pp_dd"
mom=8.9

sig=1

if [ $# -lt 4 ]; then
  echo -e "\nPerforms simulation of EvtGen/DPM/FTF/BOX events.\n"
  echo -e "USAGE: ./runall_prod.sh <prefix> <nevts> <gen> <pbeam>\n"
  echo -e " <prefix>  : Prefix of output files"
  echo -e " <nevts>   : Number of events to be simulated"
  echo -e " <gen>     : Name of EvtGen decay file 'xxx.dec:iniRes'. Keyword 'DPM/FTF/BOX' instead runs other generator"
  echo -e " <pbeam>   : Momentum of pbar-beam.\n"
  echo -e "Creates output files: <prefix>_sim.root, <prefix>_par.root, <prefix>_pid.root"
  echo -e "and corresponding log files.\n"
  exit 1
fi

if test "$1" != ""; then
  prefix=$1
fi

if test "$2" != ""; then
  nEvts=$2
fi

if test "$3" != ""; then
  dec=$3
fi

if test "$4" != ""; then
  mom=$4
fi

#. /lustre/panda/jili/dev/build/config.sh -p
echo "SIMPATH is "$SIMPATH
echo "FAIRROOTPATH is "$FAIRROOTPATH

outprefix="data/dpm/"$prefix"_"$SLURM_ARRAY_TASK_ID

root -l -q -b prod_sim_hvmaps.C\(\"$outprefix\",$nEvts,\"$dec\",$mom\) &> $outprefix"_sim.log" 
NUMEV=`grep 'Generated Events' $outprefix"_sim.log"`
root -l -b -q prod_aod_hvmaps.C\(\"$outprefix\"\) &> $outprefix"_digi.log"
echo $NUMEV >> $outprefix"_digi.log"
root -l -b -q reco_complete.C"($nEvts, \"$outprefix\")" &> $outprefix"_reco.log"
echo $NUMEV >> $outprefix"_reco.log"
root -l -b -q pid_complete.C"($nEvts, \"$outprefix\")" &> $outprefix"_pid.log"
echo $NUMEV >> $outprefix"_pid.log"
root -l -b -q ana_dpm.C"($nEvts, \"$outprefix\")" &> $outprefix"_ana.log"
echo $NUMEV >> $outprefix"_ana.log"

# Set the environment variable for the fit result file
export FIT_RESULT_FILE=${outprefix}_vtx_fit.json
echo "Fit result file is: $FIT_RESULT_FILE"

# Re-run reco and pid with the fit result
root -l -b -q reco_complete.C"($nEvts, \"$outprefix\", \"reco_from_fit\")" &> $outprefix"_reco_from_fit.log"
echo $NUMEV >> $outprefix"_reco_from_fit.log"
root -l -b -q pid_complete.C"($nEvts, \"$outprefix\", \"pid_from_fit\", \"reco_from_fit\")" &> $outprefix"_pid_from_fit.log"
echo $NUMEV >> $outprefix"_pid_from_fit.log"

