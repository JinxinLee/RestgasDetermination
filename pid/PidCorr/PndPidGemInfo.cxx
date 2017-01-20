#include "PndPidCorrelator.h"

//_________________________________________________________________
Bool_t PndPidCorrelator::GetGemInfo(PndTrack* track, PndPidCandidate* pidCand) {
 
  Int_t gemCounts = 0;
  PndTrackCand trackCand = track->GetTrackCand();
  for (size_t ii=0; ii<trackCand.GetNHits(); ii++)
    {
      PndTrackCandHit candHit = trackCand.GetSortedHit(ii);
     
      if ( ( candHit.GetDetId()!=FairRootManager::Instance()->GetBranchId("GEMHit") && fMixMode==kFALSE) ||
           ( candHit.GetDetId()!=FairRootManager::Instance()->GetBranchId("GEMHitMix") && fMixMode==kTRUE) ) continue;
     
      gemCounts++;
      
    }
 
  pidCand->SetGemHits(gemCounts);
  return kTRUE;
}

ClassImp(PndPidCorrelator)
