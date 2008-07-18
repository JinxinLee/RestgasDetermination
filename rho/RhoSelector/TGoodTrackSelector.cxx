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

#include "RhoSelector/TGoodTrackSelector.h"
#include "RhoSelector/TPidQuality.h"

#include "RhoBase/TRho.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/VAbsMicroCandidate.h"

#include "RhoConditions/TConditions.h"
#include "RhoConditions/TBeams.h"

ClassImp(TGoodTrackSelector)

TBuffer &operator>>(TBuffer &buf, TGoodTrackSelector *&obj)
{
   obj = (TGoodTrackSelector *) buf.ReadObject(TGoodTrackSelector::Class());
   return buf;
}

#include <iostream>
using namespace std;

TGoodTrackSelector::TGoodTrackSelector(const char *name, Int_t nh, Double_t pt, Double_t chipm, Double_t d,Double_t z, Double_t emc,Bool_t qc) : 
VAbsPidSelector(name,"pi-") 
{
    SetHits(nh);
    SetDchHits(0);
    SetSvtHits(0);
    SetPrimaryVertex(d,-z,z);
    SetMomentum(0);
    SetPt(pt);
    SetFit(chipm);
    SetPhi(0, 2.*3.14159265358979323846);
    SetTheta(0, 2.*3.14159265358979323846);
    SetLength(0,1.E8);
    SetShowerEnergy(emc);

    if (qc) fQC = new TPidQuality(name);
}

TGoodTrackSelector::~TGoodTrackSelector() { delete fQC; }    

void TGoodTrackSelector::SetCriterion(const char* c)
{
    // Set criteria accroding to 
    // http://babar-hn.slac.stanford.edu:5090/HyperNews/get/AnalTools/273.html
    TString crit(c);

    if (crit=="veryLoose") {
	SetHits(4);
	SetPrimaryVertex(1.5,-10.,10.);
	SetDchHits(0);
	SetSvtHits(0);
	SetMomentum(0,10.);
	SetPt(0.0);
	SetFit(0.0);
	SetPhi(0, 2.*3.14159265358979323846);
	SetTheta(0, 2.*3.14159265358979323846);
	SetLength(0,1.E8);
	SetShowerEnergy(0.0);
    }
    else if (crit=="loose") {
	SetHits(11);
	SetPrimaryVertex(1.5,-10.,10.);
	SetDchHits(11);
	SetSvtHits(0);
	SetMomentum(0,10.);
	SetPt(0.1);
	SetFit(0.0);
	SetPhi(0, 2.*3.14159265358979323846);
	SetTheta(0, 2.*3.14159265358979323846);
	SetLength(0,1.E8);
	SetShowerEnergy(0.0);
    }
    else {
	SetHits(19);
	SetPrimaryVertex(1.0,-3.,3.);
	SetDchHits(19);
	SetSvtHits(0);
	SetMomentum(0,10.);
	SetPt(0.1);
	SetFit(0.0);
	SetPhi(0, 2.*3.14159265358979323846);
	SetTheta(0, 2.*3.14159265358979323846);
	SetLength(0,1.E8);
	SetShowerEnergy(0.0);
    }

    VAbsPidSelector::SetCriterion(c);
}

Bool_t TGoodTrackSelector::Accept(TCandidate& b) 
{
    // The GoodTrackSelector does a selection based on the
    // quality of a reconstructed charged track
    
    if (&b == 0) return kFALSE;
    if (!Accept(b.GetMicroCandidate())) return kFALSE;
    SetTypeAndMass(b);
    
    return kTRUE;
}

Bool_t TGoodTrackSelector::Accept(VAbsMicroCandidate& cand) 
{
    // The GoodTrackSelector does a selection based on the
    // quality of a reconstructed charged track
    
    if (&cand == 0) return kFALSE;
    
    if (cand.GetCharge() == 0) return kFALSE;
    
    // Cut on number of hits and distance from inner to outer layer

    Int_t ndch = cand.GetDchHits();			// Get the number of DCH hits
    Int_t nsvt = cand.GetSvtHits();			// Get the number of SVT hits
    if (ndch+nsvt <= nHitsMin) return kFALSE;
    if (ndch+nsvt >  nHitsMax) return kFALSE;
    if (ndch      <= nHitsDchMin) return kFALSE;
    if (ndch      >  nHitsDchMax) return kFALSE;
    if (nsvt      <= nHitsSvtMin) return kFALSE;
    if (nsvt      >  nHitsSvtMax) return kFALSE;
    if (cand.GetDchOuterLayer() - cand.GetDchInnerLayer() <= spanDch) return kFALSE;
    
    // Cut on momenta
    
    TVector3 p = cand.GetMomentum();
    if (p.Mag()  <  pMin)  return kFALSE;
    if (p.Mag()  >  pMax)  return kFALSE;
    if (p.Perp() <  ptMin) return kFALSE;
    if (p.Perp() >  ptMax) return kFALSE;
    
    // Check angular acceptance
    
    if (p.Theta() <  thetaMin) return kFALSE;
    if (p.Theta() >  thetaMax) return kFALSE;
    if (p.Phi()+3.14159265358979323846   <  phiMin)   return kFALSE;
    if (p.Phi()+3.14159265358979323846   >  phiMax)   return kFALSE;
    
    // Check track length
    
    if (cand.GetTrackLength() < lengthMin) return kFALSE;
    if (cand.GetTrackLength() > lengthMax) return kFALSE;
    
    // Cut on quality of fit
    
    if (cand.GetProbability() < chisqProbMin) return kFALSE;
    if (cand.GetChiSquared()  < chisquare)    return kFALSE;
    if (cand.GetDegreesOfFreedom() < nDof)    return kFALSE;
    
    // Cut on beam spot position
    
    TVector3 v = cand.GetPosition();	// Get the starting point (spot corrected)

    TConditions *theConditions = TRho::Instance()->GetConditions();
    if (theConditions == 0) {
	cerr << "VAbsPidSelector: No handle to conditions manager." << endl;
	abort();
    }

    if (theConditions != 0) {
	TRhoTime *theTime = TRho::Instance()->GetTime();
	if (theTime!=0) 
	    theConditions->At(*theTime);	// Read the beam conditions DB
	else
	    theConditions->At(0);
	TBeams *beams = theConditions->GetBeams();
	v = v - beams->Interaction3Point();
    }

    if (v.Z()  > zMax) return kFALSE;
    if (v.Z()  < zMin) return kFALSE;
    if (v.Perp() > docaMax) return kFALSE;
    
    // Check matched EMC energy deposit
    
    Float_t emc  = cand.GetEmcRawEnergy();
    if (emc < emcMin) return kFALSE;
    
    return kTRUE;
}

void TGoodTrackSelector::PrintOn(std::ostream& o) const 
{
    o << GetName() << endl;
    if (nHitsMin > 0)     o << "Number of Hits       = " << nHitsMin << " ... " << nHitsMax << endl; 
    if (nHitsDchMin > 0)  o << "Number of DCH Hits   = " << nHitsDchMin << " ... " << nHitsDchMax << endl; 
    if (nHitsSvtMin > 0)  o << "Number of SVT Hits   = " << nHitsSvtMin << " ... " << nHitsSvtMax << endl; 
    if (pMin > 0)         o << "Momentum range       = " << pMin << " ... " << pMax << " GeV" << endl; 
    if (ptMin > 0)        o << "Momentum range (Pt)  = " << ptMin << " ... " << ptMax << " GeV" << endl; 
    if (chisqProbMin > 0) o << "Fit probability/chi2 = " << chisqProbMin << '/' << chisquare << endl;
    if (nDof > 0)         o << "nDoF                 > " << nDof << endl; 
    if (thetaMin > 0)     o << "Theta [Rad]          = " << thetaMin << " ... " << thetaMax << endl; 
    if (phiMin > 0)       o << "Phi   [Rad]          = " << phiMin << " ... " << phiMax << endl; 
    if (lengthMin > 0)    o << "Track length         = " << lengthMin << " ... " << lengthMax << endl; 
    if (spanDch > 0)      o << "Layer distance       > " << spanDch << endl; 
    if (zMin>-1.E8 || zMax < 1.E8) o << "z position           = " << zMin << " ... " << zMax << endl;
    if (docaMax < 1.E8)    o << "doca                 = " << docaMax << endl;
    if (emcMin > 0)     o << "Matched EMC energy   = " << emcMin << " ... " << emcMax << " GeV" << endl;
    o << endl;    
}

std::ostream&  operator << (std::ostream& o, const TGoodTrackSelector& a) { a.PrintOn(o); return o; }


