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
    RhoGoodTrackSelector ( const char* name="RhoGoodTrackSelector", Int_t nh=6400, Double_t pt=0., Double_t chipm=0.0, Double_t d=0., Double_t z=0., Bool_t qc=kFALSE );

    //Destructor
    virtual ~RhoGoodTrackSelector();

    //operations
    virtual void SetCriterion ( const char* crit ) = 0;
    virtual Bool_t Accept ( RhoCandidate& b );
    virtual Bool_t Accept ( FairRecoCandidate& b );
    virtual void PrintOn ( std::ostream& o=std::cout ) const;
    void SetHits ( Int_t min, Int_t max=65535 ) {
      fNHitsMin = min;
      fNHitsMax = max;
    }
    void SetMomentum ( Double_t pmin, Double_t pmax=1.E8 ) {
      fPMin = pmin;
      fPMax = pmax;
    }
    void SetPt ( Double_t ptmin, Double_t ptmax=1.E8 ) {
      fPtMin = ptmin;
      fPtMax = ptmax;
    }
    void SetFit ( Double_t chipm, Double_t chi=0.0, Int_t n=0 ) {
      fChisqProbMin = chipm;
      fChisqMax = chi;
      fNDof = n;
    }
    void SetPrimaryVertex ( Double_t d, Double_t zmin, Double_t zmax ) {
      fDocaMax = d;
      fZMin = zmin;
      fZMax = zmax;
    }
    void SetPhi ( Double_t min, Double_t max ) {
      fPhiMin = min;
      fPhiMax = max;
    }
    void SetTheta ( Double_t min, Double_t max ) {
      fThetaMin = min;
      fThetaMax = max;
    }
    void SetLength ( Double_t min, Double_t max ) {
      fLengthMin = min;
      fLengthMax = max;
    }
  private:
    Int_t fNHitsMin;  // Number of hits: total
    Int_t fNHitsMax;  // Number of hits: total
    Double_t fPMin, fPMax, fPtMin, fPtMax;    // Momentum cuts (abs. value)
    Double_t fChisqProbMin, fChisqMax;    // Fit chi-square probability
    Int_t fNDof;          // Number degrees of freedom
    Double_t fDocaMax;        // Distance of closest approach
    Double_t fZMin, fZMax;      // Closest z approach to beam spot
    Double_t fPhiMin, fPhiMax, fThetaMin, fThetaMax;// Angular cuts at poca
    Double_t fLengthMin, fLengthMax;    // Track length

  public:
    ClassDef( RhoGoodTrackSelector,1 )  // Track selector
};

// standalone print
std::ostream&  operator << ( std::ostream& o, const RhoGoodTrackSelector& );

#endif
