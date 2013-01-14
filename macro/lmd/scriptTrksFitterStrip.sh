#!/bin/bash
##pathStrip=${VMCWORKDIR}/macro/lmd/StripSensors/
SIMEVENTS=1000000
pathStrip=/home/karavdin/datastorage/TrackPerformanceStrip/${SIMEVENTS}
for pbeam in 15 11.91 8.9 4.06 1.5
do
 #### CA
    echo ${T} ": Now for "${pbeam}" GeV/c with CA and Minuit " >> logStripFitter
    time root -l -b -q TrksFitterResults.C\("\"${pathStrip}/Lumi_out_MC_and_REC_trks_matches_with_IDs0_CA_1trks_MinuitFit_${pbeam}.root\""\) >> logStripFitter
    echo ${T} ": Now for "${pbeam} " GeV/c with CA and Kalman Fillter " >> logStripFitter
    time root -l -b -q TrksFitterResults.C\("\"${pathStrip}/Lumi_out_MC_and_REC_trks_matches_with_IDs0_CA_1trks_KalmanFillter_${pbeam}.root\""\) >> logStripFitter
 
 #### Follow
 echo ${T} ": Now for "${pbeam}" GeV/c with Follow and Minuit " >> logStripFitter
    time root -l -b -q TrksFitterResults.C\("\"${pathStrip}/Lumi_out_MC_and_REC_trks_matches_with_IDs0_Follow_1trks_MinuitFit_${pbeam}.root\""\) >> logStripFitter
    echo ${T} ": Now for "${pbeam} " GeV/c with Follow and Kalman Fillter " >> logStripFitter
    time root -l -b -q TrksFitterResults.C\("\"${pathStrip}/Lumi_out_MC_and_REC_trks_matches_with_IDs0_Follow_1trks_KalmanFillter_${pbeam}.root\""\) >> logStripFitter
 
   
done
exit
