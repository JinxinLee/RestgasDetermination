#ifndef PNDGOODTRACKSELECTOR_H
#define PNDGOODTRACKSELECTOR_H

#include "RhoGoodTrackSelector.h"


class PndGoodTrackSelector : public RhoGoodTrackSelector
{

  public:
    virtual Bool_t Accept(FairRecoCandidate& );
    void SetCriterion ( const char* crit );

    //    virtual Bool_t Accept(RhoCandidate& ) ;

    void SetMvdHits ( Int_t min, Int_t max=65535 ) {
      fMvdHitsMin = min;
      fMvdHitsMax = max;
    }

    void SetSttHits ( Int_t min, Int_t max=65535 ) {
      fSttHitsMin = min;
      fSttHitsMax = max;
    }

    void SetGemHits ( Int_t min, Int_t max=65535 ) {
      fGemHitsMin = min;
      fGemHitsMax = max;
    }

    void SetFtsHits ( Int_t min, Int_t max=65535 ) {
      fFtsHitsMin = min;
      fFtsHitsMax = max;
    }

    void SetShowerEnergy ( Double_t min, Double_t max=1.E8 ) {
      fEmcMin = min;
      fEmcMax = max;
    }

    Int_t fMvdHitsMin, fMvdHitsMax;    // Number of Mvd hits
    Int_t fSttHitsMin, fSttHitsMax;    // Number of Mvd hits
    Int_t fGemHitsMin, fGemHitsMax;    // Number of Mvd hits
    Int_t fFtsHitsMin, fFtsHitsMax;    // Number of Mvd hits
    Double_t fEmcMin, fEmcMax;         // In addition: associated bump energy

    ClassDef(PndGoodTrackSelector,1);
};

#endif
