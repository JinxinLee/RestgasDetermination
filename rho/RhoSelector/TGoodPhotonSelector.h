#ifndef TGOODPHOTONSELECTOR_H
#define TGOODPHOTONSELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGoodPhotonSelector						        //
//                                                                      //
// Selector classes for particle selection                              //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/VAbsPidSelector.h"

class TGoodPhotonSelector : public VAbsPidSelector {
    
public:
    // Default constructor installs the currently accepted general setting
    TGoodPhotonSelector(const char *name="TGoodPhotonSelector", Double_t e=0.1, Int_t nCrys=0, Double_t lat=1.0, Bool_t qc=kFALSE); 
    
    //Destructor
    virtual ~TGoodPhotonSelector() {}
    
    //operations
    virtual void SetCriterion(const char *crit);
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    virtual void PrintOn( std::ostream& o=std::cout ) const;
    void SetCrystals(Int_t min, Int_t max) { ncMin = min; ncMax = max; }
    void SetLAT(Double_t min, Double_t max) { latMin = min; latMax = max; }
    void SetPhi(Double_t min, Double_t max) { phiMin = min; phiMax = max; }
    void SetTheta(Double_t min, Double_t max) { thetaMin = min; thetaMax = max; }
    void SetShowerEnergy(Double_t min, Double_t max=1.E8) { emcMin = min; emcMax = max; }
    
private:
    Int_t    ncMin, ncMax;			// Number of crystals
    Double_t latMin, latMax;			// Cut on LAT
    Double_t phiMin, phiMax, thetaMin, thetaMax;// Angular cuts at poca
    Double_t emcMin, emcMax;			// In addition: associated bump energy
    
public:    
    ClassDef(TGoodPhotonSelector,1)  // Track selector
};      

// standalone print
std::ostream&  operator <<(std::ostream& o, const TGoodPhotonSelector&);

#endif
