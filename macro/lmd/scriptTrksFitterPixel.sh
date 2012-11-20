#!/bin/bash
pathPixel=${VMCWORKDIR}/macro/lmd/PixelSensors
addMS=false
mergedHits=false
for pbeam in 15 11.91 8.9 4.06 1.5
do
 #### CA

    mergedHits=true
    echo ${T} ": Now for "${pbeam} " GeV/c with merged hits, CA and Minuit" >> logPixelFitter
    addMS=true
    time root -l -b -q TrksFitterResults.C\("\"${pathPixel}/Lumi_out_MC_and_REC_trks_matches_with_IDs0_CA_1trks_mergedHits_${mergedHits}_addMS_${addMS}_MinuitFit_${pbeam}.root\""\) >> logPixelFitter

    echo ${T} ": Now for "${pbeam} " GeV/c with merged hits, CA and Kalman Fillter" >> logPixelFitter
    addMS=false
    time root -l -b -q TrksFitterResults.C\("\"${pathPixel}/Lumi_out_MC_and_REC_trks_matches_with_IDs0_CA_1trks_mergedHits_${mergedHits}_addMS_${addMS}_KalmanFillter_${pbeam}.root\""\) >> logPixelFitter
   
    mergedHits=false
    echo ${T} ": Now for "${pbeam} " GeV/c with single (not merged hits), CA and Minuit" >> logPixelFitter
    addMS=true
    time root -l -b -q TrksFitterResults.C\("\"${pathPixel}/Lumi_out_MC_and_REC_trks_matches_with_IDs0_CA_1trks_mergedHits_${mergedHits}_addMS_${addMS}_MinuitFit_${pbeam}.root\""\) >> logPixelFitter

    echo ${T} ": Now for "${pbeam} " GeV/c with with single (not merged hits), CA and Kalman Fillter" >> logPixelFitter
    addMS=false
    time root -l -b -q TrksFitterResults.C\("\"${pathPixel}/Lumi_out_MC_and_REC_trks_matches_with_IDs0_CA_1trks_mergedHits_${mergedHits}_addMS_${addMS}_KalmanFillter_${pbeam}.root\""\) >> logPixelFitter
   

 #### Follow
   mergedHits=true
    echo ${T} ": Now for "${pbeam} " GeV/c with merged hits, Follow and Minuit" >> logPixelFitter
    addMS=true
    time root -l -b -q TrksFitterResults.C\("\"${pathPixel}/Lumi_out_MC_and_REC_trks_matches_with_IDs0_Follow_1trks_mergedHits_${mergedHits}_addMS_${addMS}_MinuitFit_${pbeam}.root\""\) >> logPixelFitter

    echo ${T} ": Now for "${pbeam} " GeV/c with merged hits, Follow and Kalman Fillter" >> logPixelFitter
    addMS=false
    time root -l -b -q TrksFitterResults.C\("\"${pathPixel}/Lumi_out_MC_and_REC_trks_matches_with_IDs0_Follow_1trks_mergedHits_${mergedHits}_addMS_${addMS}_KalmanFillter_${pbeam}.root\""\) >> logPixelFitter



    # echo ${T} ": Now for "${pbeam} " GeV/c with merged hits, Follow and Minuit" >> logPixelFitter
    # time root -l -b -q TrksFitterResults.C\("\"${pathPixel}/Lumi_out_MC_and_REC_trks_matches_with_IDs0_Follow_1trks_mergedHits_MinuitFit_${pbeam}.root\""\) >> logPixelFitter
    # echo ${T} ": Now for "${pbeam} " GeV/c with merged hits, Follow and Kalman Fillter" >> logPixelFitter
    # time root -l -b -q TrksFitterResults.C\("\"${pathPixel}/Lumi_out_MC_and_REC_trks_matches_with_IDs0_Follow_1trks_mergedHits_KalmanFillter_${pbeam}.root\""\) >> logPixelFitter
       
done
exit
