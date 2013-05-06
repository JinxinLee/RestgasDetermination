#ifndef RHOGOODPHOTONSELECTOR_H
#define RHOGOODPHOTONSELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoGoodPhotonSelector                                                //
//                                                                      //
// Selector classes for photon quality selections                       //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/RhoParticleSelectorBase.h"
#include "PndPidCandidate.h"

class RhoGoodPhotonSelector : public RhoParticleSelectorBase
{

  public:
    // Default constructor installs the currently accepted general setting
    RhoGoodPhotonSelector ( const char* name="RhoGoodPhotonSelector", Double_t e=0.1, Int_t nCrys=0, Double_t lat=1.0, Bool_t qc=kFALSE );

    //Destructor
    virtual ~RhoGoodPhotonSelector() {}

    //operations
    virtual void SetCriterion ( const char* crit );
    virtual Bool_t Accept ( RhoCandidate& b );
    virtual Bool_t Accept ( PndPidCandidate& b );
    virtual void PrintOn ( std::ostream& o=std::cout ) const;
    void SetCrystals ( Int_t min, Int_t max ) {
      fNcMin = min;
      fNcMax = max;
    }
//    void SetLAT ( Double_t min, Double_t max ) {
//        fLatMin = min;
//        fLatMax = max;
//    }
//    void SetPhi ( Double_t min, Double_t max ) {
//        fPhiMin = min;
//        fPhiMax = max;
//    }
    void SetTheta ( Double_t min, Double_t max ) {
      fThetaMin = min;
      fThetaMax = max;
    }
    void SetShowerEnergy ( Double_t min, Double_t max=1.E8 ) {
      fEmcMin = min;
      fEmcMax = max;
    }

  private:
    Int_t    fNcMin, fNcMax;      // Number of crystals
    //Double_t fLatMin, fLatMax;      // Cut // Angular cuts at poca on LAT
    //Double_t fPhiMin, fPhiMax;          // Angular cuts at poca
    Double_t fThetaMin, fThetaMax;      // Angular cuts at poca
    Double_t fEmcMin, fEmcMax;      // In addition: associated bump energy

  public:
    ClassDef ( RhoGoodPhotonSelector,1 )  // Track selector
};

// standalone print
std::ostream&  operator << ( std::ostream& o, const RhoGoodPhotonSelector& );

#endif
