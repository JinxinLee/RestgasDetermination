# This script runs Lumi macros for the full Sim
################################################################
# Script for production and merge results of BkgStudies_MCmatch_withSignal_exe.C (Background\Signal study)
# !need all files from full reconstruction chain (results from GEANE, MC etc.)
# author: Anastasia Karavdina
# date: April, 2012
#
# to run it print and follow instuctions: 
# ./HIMsterDATA.sh
#!NB: Storagepath is a directory in ${path}
#change ${path} to right value in your case
################################################################

# reads number of Events, name of run and verbose Level
nEvts=10
nJobs=1
path="test"
verbose=0
mom=0

echo Enter number of Events:
read nEvts 
echo "Number of Events: $nEvts!"

echo Enter number of Jobs:
read nJobs
echo "Number of Jobs: $nJobs!"

step=`expr $nEvts / $nJobs`

echo Enter storage-folder:
read folder
echo "Storagepath: $folder!"

echo Enter verbosity Level:
read verbose
echo "Verbosity Level is: $verbose!"

echo Enter momentum from "1.5, 4.06, 8.9, 11.91, 15 [GeV/c]":
read mom
echo "Beam momentum is: $mom!"

momStr=`echo $mom | sed -e '{ s/\./_/g; }'`

path="/data/FAIRsorf/pandaroot/trunk/macro/lmd/tmpHIMster/"${folder}
# data directory. all data file go there and read from there, makes them easier to move around

# let's go
cd /home/akaravdina/lmdMacro
source config.sh

last=`expr $nEvts - 1`
for start in `seq 0 $step $last` ; do
cd /data/FAIRsorf/pandaroot/build/bin
#./bkg_rec_mc_match -s ${start} -n ${step} -mom  ${mom} -path ${path} -v 1  &> ${path}/bkgStudy_${start}.log
./bkg_rec_mc_match -s ${start} -n ${step} -mom  ${mom} -path ${path} -v 1  >> ${path}/bkgStudy_ALL.log
if [ $start == "0" ]; then
cp ${path}/compMC_and_REC_withSignal0.root ${path}/sumAll.root
fi
if [ $start != "0" ]; then 
  $ROOTSYS/bin/hadd -f ${path}/sumAll_tot.root ${path}/sumAll.root ${path}/compMC_and_REC_withSignal${start}.root
  mv ${path}/sumAll_tot.root ${path}/sumAll.root
fi
done
cd /home/akaravdina/lmdMacro
time root -l -b -q sumBkgStudies_MCmatch_withSignal.C\("\"${path}\""\)



