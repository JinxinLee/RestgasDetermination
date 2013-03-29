#include "PndPidCorrelator.h"
#include "PndFtsHit.h"

#include <cmath>

//_________________________________________________________________
Bool_t PndPidCorrelator::GetFtsInfo(PndTrack* track, PndPidCandidate* pidCand) {
  
  std::vector<Double_t> dedxvec;
  dedxvec.clear();

  Double_t tuberadius = fFtsParameters->GetTubeInRad(); 
  
  Int_t ftsCounts = 0, ftsRawCounts = 0;
  PndTrackCand trackCand = track->GetTrackCand();
  for (Int_t ii=0; ii<trackCand.GetNHits(); ii++)
    {
      PndTrackCandHit candHit = trackCand.GetSortedHit(ii);
      Double_t dedx = 0.;

      if ( ( candHit.GetDetId()!=FairRootManager::Instance()->GetBranchId("FTSHit") && fMixMode==kFALSE) ||
           ( candHit.GetDetId()!=FairRootManager::Instance()->GetBranchId("FTSHitMix") && fMixMode==kTRUE) ) continue;
      PndFtsHit *ftsHit = (PndFtsHit*) fFtsHit->At(candHit.GetHitId());
      if(!ftsHit) continue;
      ftsRawCounts++;
      
      // compute dE/dx
      //dedx = ftsHit->ComputedEdx(track, tuberadius);
      
      if(dedx != 0)
	{
	  dedxvec.push_back(dedx);
	  ftsCounts++;
	  }
    }
  
  if( ftsCounts > 0) {
    // truncated mean
    Double_t perc = 0.70;
    // sort
    std::sort(dedxvec.begin(), dedxvec.end());
    
    //truncated mean
    Double_t sum = 0;
    Int_t endnum = int(floor(ftsCounts * perc));
     
    // ****************************************
    // CUT on n of hits: to have a meaningful
    // truncated mean we require not to have less 
    // than 5 hits (in the already truncated list)
    // ****************************************
    if(endnum > 5) {
      for(Int_t m = 0; m < endnum; m++) sum += dedxvec[m];
      pidCand->SetSttMeanDEDX(sum/(Double_t) endnum); // else default in pidCand is SttDEDXMean = 0
    }
  } 

  pidCand->SetSttHits(ftsRawCounts);
  return kTRUE;
}

ClassImp(PndPidCorrelator)
