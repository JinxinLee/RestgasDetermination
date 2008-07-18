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

#include "RhoSelector/TGoodPhotonSelector.h"

#include "RhoBase/TRho.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/VAbsMicroCandidate.h"

ClassImp(TGoodPhotonSelector)

TBuffer &operator>>(TBuffer &buf, TGoodPhotonSelector *&obj)
{
   obj = (TGoodPhotonSelector *) buf.ReadObject(TGoodPhotonSelector::Class());
   return buf;
}

#include <iostream>
using namespace std;

TGoodPhotonSelector::TGoodPhotonSelector(const char *name, Double_t emc, Int_t nCrys, Double_t lat, Bool_t qc) : 
VAbsPidSelector(name,"gamma") 
{
    SetShowerEnergy(emc,1.E8);
    SetCrystals(nCrys,100000);
    SetLAT(0.0,lat);
    SetPhi(0, 2.*3.14159265358979323846);
    SetTheta(0, 2.*3.14159265358979323846);
}

void TGoodPhotonSelector::SetCriterion(const char *c)
{
    // Set criteria accroding to 
    // http://babar-hn.slac.stanford.edu:5090/HyperNews/get/AnalTools/273.html
    TString crit(c);

    if (crit=="veryLoose" || crit=="loose") {
	SetShowerEnergy(0.03,1.E8);
	SetCrystals(0,100000);
	SetLAT(0.0,0.8);
	SetPhi(0, 2.*3.14159265358979323846);
	SetTheta(0, 2.*3.14159265358979323846);
    }
    else {
	SetShowerEnergy(0.1,1.E8);
	SetCrystals(0,100000);
	SetLAT(0.0,0.8);
	SetPhi(0, 2.*3.14159265358979323846);
	SetTheta(0, 2.*3.14159265358979323846);
    }

    VAbsPidSelector::SetCriterion(c);
}

Bool_t TGoodPhotonSelector::Accept(TCandidate& b) 
{
    // The TGoodPhotonSelector does a selection based on the
    // quality of a reconstructed charged track
    
    if (&b == 0) return kFALSE;
    if (!Accept(b.GetMicroCandidate())) return kFALSE;
    SetTypeAndMass(b);
    return kTRUE;
}

Bool_t TGoodPhotonSelector::Accept(VAbsMicroCandidate& cand) 
{
    // The TGoodPhotonSelector does a selection based on the
    // quality of a reconstructed charged track
    
    if (&cand == 0) return kFALSE;
    
    if (cand.GetCharge() != 0) return kFALSE;
    
    // Check EMC energy deposit    
    Float_t emc  = cand.GetEmcCalEnergy();
    if (emc <= emcMin) return kFALSE;
    if (emc >  emcMax) return kFALSE;
    
    // Cut on number of crystals    
    Int_t nc = cand.GetEmcNumberOfCrystals();
    if (nc <= ncMin) return kFALSE;
    if (nc >  ncMax) return kFALSE;
    
    // Check LAT    
    Float_t lat  = cand.GetEmcLateralShape();
    if (lat <= latMin) return kFALSE;
    if (lat >  latMax) return kFALSE;
    
    // Check angular acceptance
    
    TVector3 p = cand.GetMomentum();
    if (p.Theta() <  thetaMin) return kFALSE;
    if (p.Theta() >  thetaMax) return kFALSE;
    if (p.Phi()+2.*3.14159265358979323846   <  phiMin)   return kFALSE;
    if (p.Phi()+2.*3.14159265358979323846   >  phiMax)   return kFALSE;

    return kTRUE;
}

void TGoodPhotonSelector::PrintOn(std::ostream& o) const 
{
    o << GetName() << endl;
    if (emcMin > 0)        o << "EMC energy          = " << emcMin << " ... " << emcMax << " GeV" << endl;
    if (ncMin > 0)         o << "Number of Crystals  = " << ncMin << " ... " << ncMax << endl; 
    if (latMin > 0)        o << "LAT                 = " << latMin << " ... " << latMax << endl;
    if (thetaMin > 0)      o << "Theta [Rad]         = " << thetaMin << " ... " << thetaMax << endl; 
    if (phiMin > 0)        o << "Phi   [Rad]         = " << phiMin << " ... " << phiMax << endl; 
    o << endl;    
}

std::ostream&  operator << (std::ostream& o, const TGoodPhotonSelector& a) { a.PrintOn(o); return o; }

