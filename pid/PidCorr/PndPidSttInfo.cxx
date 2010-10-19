#include "PndDetectorList.h"
#include "PndPidCorrelator.h"
#include "PndPidCandidate.h"
#include "PndTrack.h"
#include "PndTrackID.h"
#include "PndSttHelixHit.h"

#include "FairTrackParH.h"
#include "FairMCApplication.h"
#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"

#include "TObjArray.h"
#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TGeoManager.h"

#include <cmath>

#include "PndPidCorrelator.h"

//_________________________________________________________________
Bool_t PndPidCorrelator::GetSttInfo(PndTrack* track, PndPidCandidate* pidCand) {
 
  std::vector<Double_t> dedxvec;
  dedxvec.clear();
  
  Int_t sttCounts = 0;
  PndTrackCand trackCand = track->GetTrackCand();
  for (Int_t ii=0; ii<trackCand.GetNHits(); ii++)
    {
      PndSttHelixHit *sttHit = NULL;
      PndTrackCandHit candHit = trackCand.GetSortedHit(ii);
      if (candHit.GetDetId()!=kSttHelixHit) continue;
      sttHit = (PndSttHelixHit*)fSttHit->At(candHit.GetHitId());
      if (sttHit==0) continue;
      if (sttHit->GetdEdx() != 0.) 
	{
	  dedxvec.push_back(sttHit->GetdEdx());
	  sttCounts++;
	}
    }
  
  if( sttCounts > 0) {
    // truncated mean
    Double_t perc = 0.60;
    // sort
    std::sort(dedxvec.begin(), dedxvec.end());
    
    //truncated mean
    Double_t sum = 0;
    Int_t endnum = int(floor(sttCounts * perc));
     
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

  pidCand->SetSttHits(sttCounts);
}

ClassImp(PndPidCorrelator)
