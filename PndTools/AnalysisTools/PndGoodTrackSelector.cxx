#include "PndGoodTrackSelector.h"
#include "PndPidCandidate.h"
ClassImp(PndGoodTrackSelector)



void PndGoodTrackSelector::SetCriterion ( const char* c )
{
  // Set criteria accroding to PANDA specifications.

  TString crit ( c );

  if ( crit=="veryLoose" ) {
    SetHits ( 4 );
    SetPrimaryVertex ( 1.5,-10.,10. );
    SetMomentum ( 0,10. );
    SetPt ( 0.0 );
    SetFit ( 0.0 );
    SetPhi ( 0, 2.*3.14159265358979323846 );
    SetTheta ( 0, 2.*3.14159265358979323846 );
    SetLength ( 0,1.E8 );
    SetMvdHits(0);
    SetSttHits(0);
    SetGemHits(0);
    SetFtsHits(0);
    SetShowerEnergy(0);
  } else if ( crit=="loose" ) {
    SetHits ( 11 );
    SetPrimaryVertex ( 1.5,-10.,10. );
    SetMomentum ( 0,10. );
    SetPt ( 0.1 );
    SetFit ( 0.0 );
    SetPhi ( 0, 2.*3.14159265358979323846 );
    SetTheta ( 0, 2.*3.14159265358979323846 );
    SetLength ( 0,1.E8 );
    SetMvdHits(3);
    SetSttHits(0);
    SetGemHits(0);
    SetFtsHits(0);
    SetShowerEnergy(0);
  } else {
    SetHits ( 19 );
    SetPrimaryVertex ( 1.0,-3.,3. );
    SetMomentum ( 0,10. );
    SetPt ( 0.1 );
    SetFit ( 0.0 );
    SetPhi ( 0, 2.*3.14159265358979323846 );
    SetTheta ( 0, 2.*3.14159265358979323846 );
    SetLength ( 0,1.E8 );
  }

  RhoParticleSelectorBase::SetCriterion ( c );
}





Bool_t PndGoodTrackSelector::Accept(FairRecoCandidate& frecoc)
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

// Bool_t PndGoodTrackSelector::Accept(RhoCandidate& )
// {
//
// }

