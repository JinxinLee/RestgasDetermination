#ifndef TGOODTRACKSELECTOR_H
#define TGOODTRACKSELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGoodTrackSelector						        //
//                                                                      //
// Selector classes for particle selection                              //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/VAbsPidSelector.h"

class TGoodTrackSelector : public VAbsPidSelector {
    
public:
    // Default constructor installs the currently accepted general setting
    TGoodTrackSelector(const char *name="TGoodTrackSelector", Int_t nh=19, Double_t pt=0.1, Double_t chipm=0.0, Double_t d=1.0,Double_t z=3.0, Double_t emc=-1.0,Bool_t qc=kFALSE); 
    
    //Destructor
    virtual ~TGoodTrackSelector();
    
    //operations
    virtual void SetCriterion(const char *crit);
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    virtual void PrintOn( std::ostream& o=std::cout ) const;
    void SetHits(Int_t min, Int_t max=65535) { nHitsMin = min; nHitsMax = max; }
    void SetDchHits(Int_t min, Int_t max=65535) { nHitsDchMin = min; nHitsDchMax = max; }
    void SetSvtHits(Int_t min, Int_t max=65535) { nHitsSvtMin = min; nHitsSvtMax = max; }
    void SetMomentum(Double_t pmin, Double_t pmax=1.E8) { pMin = pmin; pMax = pmax; }
    void SetPt(Double_t ptmin, Double_t ptmax=1.E8) { ptMin = ptmin; ptMax = ptmax; }
    void SetFit(Double_t chipm, Double_t chi=0.0, Int_t n=0) { chisqProbMin = chipm; chisquare = chi; nDof = n; }
    void SetPrimaryVertex(Double_t d, Double_t zmin, Double_t zmax) { docaMax = d; zMin = zmin; zMax = zmax; }
    void SetPhi(Double_t min, Double_t max) { phiMin = min; phiMax = max; }
    void SetTheta(Double_t min, Double_t max) { thetaMin = min; thetaMax = max; }
    void SetLength(Double_t min, Double_t max, Int_t l=0) { lengthMin = min; lengthMax = max; spanDch = l; }
    void SetShowerEnergy(Double_t min, Double_t max=1.E8) { emcMin = min; emcMax = max; }
    
private:
    Int_t nHitsMin, nHitsDchMin, nHitsSvtMin;	// Number of hits: total, Dch, Svt
    Int_t nHitsMax, nHitsDchMax, nHitsSvtMax;	// Number of hits: total, Dch, Svt
    Double_t pMin, pMax, ptMin, ptMax;		// Momentum cuts (abs. value)
    Double_t chisqProbMin, chisquare;		// Fit chi-square probability
    Int_t nDof;					// Number degrees of freedom
    Double_t docaMax;				// Distance of closest approach
    Double_t zMin, zMax;			// Closest z approach to beam spot
    Double_t phiMin, phiMax, thetaMin, thetaMax;// Angular cuts at poca
    Double_t lengthMin, lengthMax;		// Track length
    Int_t spanDch;				// Distance from innermost to outermost layer
    Double_t emcMin, emcMax;			// In addition: associated bump energy
    
public:    
    ClassDef(TGoodTrackSelector,1)  // Track selector
};      

// standalone print
std::ostream&  operator <<(std::ostream& o, const TGoodTrackSelector&);

#endif
