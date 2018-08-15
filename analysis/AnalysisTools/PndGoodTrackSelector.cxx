#include "PndGoodTrackSelector.h"
#include "PndPidCandidate.h"
ClassImp(PndGoodTrackSelector)



void PndGoodTrackSelector::SetCriterion ( const char* c )
{
  // Set criteria accroding to PANDA specifications.
//FIXME Insert sensible values!!!
  TString crit ( c );

  if ( crit=="veryLoose" ) {
//    SetNHitsCut ( 4 );
    //SetPrimaryVertexCut ( 1.5,-10.,10. );
    //SetMomentumCut ( 0,10. );
    //SetPtCut ( 0.0 );
    SetFitCut ( 1e8, 0.001 );
    SetMvdHits(0);
    SetSttHits(0);
    SetGemHits(0);
    SetFtsHits(0);
    SetShowerEnergy(0);
  } else if ( crit=="loose" ) {
//    SetNHitsCut ( 11 );
    SetPrimaryVertexCut ( 1.5,-10.,10. );
    SetMomentumCut ( 0,10. );
    SetPtCut ( 0.1 );
    SetFitCut ( 1e8, 0.01 );
    SetMvdHits(3);
    SetSttHits(0);
    SetGemHits(0);
    SetFtsHits(0);
    SetShowerEnergy(0);
  } else {
//    SetNHitsCut ( 19 );
    SetPrimaryVertexCut ( 1.0,-3.,3. ); //cm
    SetMomentumCut ( 0.1,10. ); //GeV/c
    //SetPtCut ( 0.1 ); //GeV/c
    SetFitCut ( 1e5,0.01 ); // ciqmax,probmin
    SetMvdHits(3);
    SetSttHits(0);
    SetGemHits(0);
    SetFtsHits(0);
    SetShowerEnergy(0);
  }

  RhoParticleSelectorBase::SetCriterion ( c );
}





Bool_t PndGoodTrackSelector::Accept(PndRecoCandidate& frecoc)
{
  Bool_t rc = RhoGoodTrackSelector::Accept(frecoc);
  if (rc==kFALSE) { return kFALSE; }
  PndPidCandidate* recocand = dynamic_cast<PndPidCandidate*> (&frecoc); //make use of full panda functionality

  if (recocand->GetMvdHits() < fMvdHitsMin) { return kFALSE; }
  if (recocand->GetMvdHits() > fMvdHitsMax) { return kFALSE; }
  if (recocand->GetSttHits() < fSttHitsMin) { return kFALSE; }
  if (recocand->GetSttHits() > fSttHitsMax) { return kFALSE; }
  //if(recocand->Get() < fGemHitsMin) return kFALSE;
  //if(recocand->Get() > fGemHitsMax) return kFALSE;
  //if(recocand->Get() < fFtsHitsMin) return kFALSE;
  //if(recocand->Get() > fFtsHitsMax) return kFALSE;
  if (recocand->GetEmcCalEnergy() < fEmcMin) { return kFALSE; }
  if (recocand->GetEmcCalEnergy() > fEmcMax) { return kFALSE; }
  return kTRUE;
}



