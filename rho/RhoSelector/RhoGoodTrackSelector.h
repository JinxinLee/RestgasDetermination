#ifndef RHOGOODTRACKSELECTOR_H
#define RHOGOODTRACKSELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoGoodTrackSelector                                                 //
//                                                                      //
// Selector classes for track quality selections                        //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/RhoParticleSelectorBase.h"

class RhoGoodTrackSelector : public RhoParticleSelectorBase
{

  public:
    // Default constructor installs the currently accepted general setting
    RhoGoodTrackSelector ( const char* name="RhoGoodTrackSelector" );
    //Destructor
    virtual ~RhoGoodTrackSelector();
    virtual void PrintOn ( std::ostream& o=std::cout ) const;


//    void SetNHitsCut ( Int_t min, Int_t max=65535 ) {
//      fNHitsMin = min;
//      fNHitsMax = max;
//      fCutHits=kTRUE;
//    }
    void SetMomentumCut ( Double_t pmin, Double_t pmax=1.E8 ) {
      fPMin = pmin;
      fPMax = pmax;
      fCutMom=kTRUE;
    }
    void SetPtCut ( Double_t ptmin, Double_t ptmax=1.E8 ) {
      fPtMin = ptmin;
      fPtMax = ptmax;
      fCutPt=kTRUE;
    }
    void SetFitCut (  Double_t chi=1e9, Double_t chipm=0.0) {
      fChisqProbMin = chipm;
      fChisqMax = chi;
      fCutFit=kTRUE;
    }
    void SetPrimaryVertexCut ( Double_t d, Double_t zmin, Double_t zmax ) {
      fDocaMax = d;
      fZMin = zmin;
      fZMax = zmax;
      fCutVtx=kTRUE;
    }
  
  protected:
    //operations
    virtual void SetCriterion ( const char*  ) {}; // crit //[R.K.03/2017] unused variable(s)
    virtual Bool_t Accept ( RhoCandidate& b );
    virtual Bool_t Accept ( FairRecoCandidate& b );
 
  private:
//    Int_t fNHitsMin;  // Number of hits: total
//    Int_t fNHitsMax;  // Number of hits: total
    Double_t fPMin, fPMax, fPtMin, fPtMax;    // Momentum cuts (abs. value)
    Double_t fChisqProbMin, fChisqMax;    // Fit chi-square probability
    Double_t fDocaMax;        // Distance of closest approach
    Double_t fZMin, fZMax;      // Closest z approach to beam spot
    Bool_t fCutFit;
    Bool_t fCutVtx;
    Bool_t fCutMom;
    Bool_t fCutPt;
//    Bool_t fCutHits;

  public:
    ClassDef( RhoGoodTrackSelector,1 )  // Track selector
};

// standalone print
std::ostream&  operator << ( std::ostream& o, const RhoGoodTrackSelector& );

#endif
