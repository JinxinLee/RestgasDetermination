//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TEventSelector							//
//                                                                      //
// Selector classes for event selection                                 //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "RhoBase/TRho.h"
#include "RhoBase/VAbsReader.h"
#include "RhoSelector/TEventSelector.h"
#include "RhoSelector/TPidSelector.h"
#include "RhoBase/TCandidate.h"
#include "RhoConditions/TConditions.h"
#include "RhoConditions/TBeams.h"
#include "RhoBase/VAbsTag.h"
#include "RhoBase/VAbsBit.h"

ClassImp(TEventSelector)

TBuffer &operator>>(TBuffer &buf, TEventSelector *&obj)
{
   obj = (TEventSelector *) buf.ReadObject(TEventSelector::Class());
   return buf;
}

#include <iostream>
using namespace std;

void TEventSelector::PrintOn(std::ostream& o) const 
{
    o << GetName() << " settings:";
    if (fTight) o << " Tight mode";
    o << endl;
    o << "Number of Tracks        = "  << fNTracksMin << " ... " << fNTracksMax << endl; 
    if (fDocaMax < 1.E8)  o << "Primary vertex (d,-z,z) < ("  << fDocaMax << ',' << fZMin << ',' << fZMax << ')' << endl; 
    if (fTotChar < 65535) o << "Total charge            < " << fTotChar << endl; 
    if (fETotLow > 0.0)   o << "Total energy            = " << fETotLow   << " ... " << fETotHigh << " GeV" << endl; 
    if (fPTotLow > 0.0)   o << "Total momentum          = " << fPTotLow <<   " ... " << fPTotHigh << " GeV" << endl; 
    if (fELow > 0.0)      o << "Track energy            = " << fELow   << " ... " << fEHigh << " GeV" << endl; 
    if (fPLow > 0.0)      o << "Track momentum          = " << fPLow <<   " ... " << fPHigh << " GeV" << endl; 
    if (fPtLow > 0.0)     o << "Track Pt                = " << fPtLow <<   " ... " << fPtHigh << " GeV" << endl; 
    if (fEmcLow > 0.0)    o << "Track EMC energy        = " << fEmcLow << " ... " << fEmcHigh << " GeV" << endl; 
    if (fThetaLow > 0.0)  o << "Track theta window      = " << fThetaLow << " ... " << fThetaHigh << " Rad" << endl; 
    if (fPhiLow > 0.0)    o << "Track phi window        = " << fPhiLow << " ... " << fPhiHigh << " Rad" << endl; 
    o << endl;    
}

std::ostream&  operator << (std::ostream& o, const TEventSelector& a) { a.PrintOn(o); return o; }


ClassImp(TTagBaseSelector)

TBuffer &operator>>(TBuffer &buf, TTagBaseSelector *&obj)
{
   obj = (TTagBaseSelector *) buf.ReadObject(TTagBaseSelector::Class());
   return buf;
}

void TTagBaseSelector::PrintOn(std::ostream& o) const 
{
    TEventSelector::PrintOn(cout);
    if (fPTotalMass > 0.0)   o << "Total mass              > " << fPTotalMass << " GeV" << endl; 
    if (fPTotalCosTh > -1.0) o << "Total CosTheta P        > " << fPTotalCosTh << endl; 
    if (fENeutralCosTh > -1.0) o << "Total CosTheta E        > " << fENeutralCosTh << endl; 
    if (fP1MagLow > 0.0) o << "First particle P        = " << fP1MagLow << " ... " << fP1MagHigh << " GeV" << endl; 
    if (fP2MagLow > 0.0) o << "Secnd particle P        = " << fP2MagLow << " ... " << fP2MagHigh << " GeV" << endl; 
    if (fE1MagLow > 0.0) o << "First particle E        = " << fE1MagLow << " ... " << fE1MagHigh << " GeV" << endl; 
    if (fE2MagLow > 0.0) o << "Secnd particle E        = " << fE2MagLow << " ... " << fE2MagHigh << " GeV" << endl; 
    o << endl;
}

std::ostream&  operator << (std::ostream& o, const TTagBaseSelector& a) { a.PrintOn(o); return o; }


ClassImp(TAodBaseSelector)

TBuffer &operator>>(TBuffer &buf, TAodBaseSelector *&obj)
{
   obj = (TAodBaseSelector *) buf.ReadObject(TAodBaseSelector::Class());
   return buf;
}

TAodBaseSelector::TAodBaseSelector(const char *name) :
TEventSelector(name), 
fSelector(new TGoodTrackSelector("TGoodTrackSelector",20,0.1,0.0,1.0,4.0,-1.0))
{
    fOwnSelector = fSelector; 
}

TAodBaseSelector::~TAodBaseSelector() 
{ 
    delete fOwnSelector; 
}

void TAodBaseSelector::PrintOn(std::ostream& o) const 
{
    TEventSelector::PrintOn(o);
    o << *fSelector;  // Show the settings of the associated track selector
}
std::ostream&  operator << (std::ostream& o, const TAodBaseSelector& a) { a.PrintOn(o); return o; }

// TTagSelector is used to define a cut based on a TTreeFormula

ClassImp(TTagSelector)

TBuffer &operator>>(TBuffer &buf, TTagSelector *&obj)
{
   obj = (TTagSelector *) buf.ReadObject(TTagSelector::Class());
   return buf;
}

TTagSelector::TTagSelector(const char *name, const char* formula) :
TTagBaseSelector(name)
{
    SetFormula(formula);
}
 
Bool_t TTagSelector::Accept(TEventManager& e) 
{
/*
    if (fTreeFormula == 0) {
	fTreeFormula = new TTreeFormula(fName,fFormula,e.GetReader()->GetStream(0)->GetTree());
	fTreeFormula->Print();
    }
    fTreeFormula->SetTree(e.GetReader()->GetStream(0)->GetTree());
    Float_t result = fTreeFormula->EvalInstance();
    return (result>0.0);
*/
    return 0.0;
}

void TTagSelector::PrintOn(std::ostream& o) const 
{
    o << GetName() << " settings: ";
    o << fFormula;
    o << endl;    
}

std::ostream&  operator << (std::ostream& o, const TTagSelector& a) { a.PrintOn(o); return o; }


// The TTwoprongTagSelector looks for a ninimum number of tracks
// and neutrals. It then make sure that two prongs are opposite
// in the CMS. The selection is based on information stored in the
// TAG.

ClassImp(TTwoprongTagSelector)

TBuffer &operator>>(TBuffer &buf, TTwoprongTagSelector *&obj)
{
   obj = (TTwoprongTagSelector *) buf.ReadObject(TTwoprongTagSelector::Class());
   return buf;
}

TTwoprongTagSelector::TTwoprongTagSelector(const char *name) :
TTagBaseSelector(name)
{
    SetLoose(); 
    SetNumberOfTracks(2,2);
    SetDeltaPhi(11.);
    SetDeltaCosTheta(0.1);
}
 
Bool_t TTwoprongTagSelector::Accept(TEventManager& e) 
{
    VAbsTag &tag = e.GetTag();	    // Shortcut to access TAG

    // Check the number of good tracks

    if (!tight && tag.GetNumberOfGoodTracksLoose() < fNTracksMin) return kFALSE;
    if ( tight && tag.GetNumberOfGoodTracksTight() < fNTracksMin) return kFALSE;
    
    // Preselect the interesting events in CMS 
    // according to angular constraints

    Double_t p1Phi, p2Phi, p1CosTh, p2CosTh;
    TVector3 t1 = tag.GetPrimaryTrackMomentum();    // CosTheta, Phi, Magnitude
    TVector3 t2 = tag.GetSecondaryTrackMomentum();  // CosTheta, Phi, Magnitude

    p1CosTh = t1.X();
    p2CosTh = t2.X();
    Double_t dcosth = TMath::Abs(p1CosTh + p2CosTh);	    // Ideally cancels to zero
    if (dcosth > fDCosTh) return kFALSE;			    // Does it match ?

    p1Phi = t1.Y();
    p2Phi = t2.Y();
    Double_t dphi = TMath::Abs(p1Phi - p2Phi);
    dphi = TMath::Abs(dphi - 3.14159265358979323846);			    // Those are close to 180 deg.
    if (dphi/3.14159265358979323846*180. > fDPhi) return kFALSE;		    // Does it match ?

    return kTRUE;
}

void TTwoprongTagSelector::PrintOn(std::ostream& o) const 
{
    TTagBaseSelector::PrintOn(cout);
    o << "Delta CosTheta    < "  << fDCosTh << endl;
    o << "Delta Phi [deg]   < "  << fDPhi << endl;
    o << endl;    
}

std::ostream&  operator << (std::ostream& o, const TTwoprongTagSelector& a) { a.PrintOn(o); return o; }


// Define rough cuts for dimuon selection to be applied at the TAG level
// See http://www.slac.stanford.edu/~rfaccini/phys/Y4Sscan/selections.html

ClassImp(TDimuonTagSelector)

TBuffer &operator>>(TBuffer &buf, TDimuonTagSelector *&obj)
{
   obj = (TDimuonTagSelector *) buf.ReadObject(TDimuonTagSelector::Class());
   return buf;
}

TDimuonTagSelector::TDimuonTagSelector(const char *name) :
TTagBaseSelector(name)
{
    SetName(name);
    SetPrimaryMomentum(4.0);
    SetSecondaryMomentum(2.0);
    SetTheta(2.8,3.5);
}

Bool_t TDimuonTagSelector::Accept(TEventManager& e) 
{
    VAbsTag &tag = e.GetTag();	    // Shortcut to access TAG

    Float_t p1Mag, p2Mag, p1CosTheta, p2CosTheta;
    tag.getFloat(p1Mag,"p1Mag"); 
    if (p1Mag <= fP1MagLow) return kFALSE;
    tag.getFloat(p2Mag,"p2Mag");
    if (p2Mag <= fP2MagLow) return kFALSE;
    tag.getFloat(p1CosTheta,"p1CosTheta"); 
    tag.getFloat(p2CosTheta,"p2CosTheta");
    Double_t theta1 = TMath::ACos(p1CosTheta);
    Double_t theta2 = TMath::ACos(p2CosTheta);
    Double_t theta = theta1 + theta2;
    if (theta <= fThetaLow || theta >= fThetaHigh) return kFALSE;

    return kTRUE;
}

void TDimuonTagSelector::PrintOn(std::ostream& o) const 
{
    TTagBaseSelector::PrintOn(o);
    o << endl;
}

std::ostream&  operator << (std::ostream& o, const TDimuonTagSelector& a) { a.PrintOn(o); return o; }

// Define rough cuts for gamma gamma selection to be applied at the TAG level

ClassImp(TGammaGammaTagSelector)

TBuffer &operator>>(TBuffer &buf, TGammaGammaTagSelector *&obj)
{
   obj = (TGammaGammaTagSelector *) buf.ReadObject(TGammaGammaTagSelector::Class());
   return buf;
}

TGammaGammaTagSelector::TGammaGammaTagSelector(const char *name) :
TTagBaseSelector(name)
{
    SetPrimaryEnergy(4.0);
    SetSecondaryEnergy(2.0);
    SetTheta(2.8,3.5);
}

Bool_t TGammaGammaTagSelector::Accept(TEventManager& e) 
{
    VAbsTag &tag = e.GetTag();	    // Shortcut to access TAG
    Float_t e1Mag, e2Mag, e1CosTheta, e2CosTheta;
    tag.getFloat(e1Mag,"e1Mag"); 
    if (e1Mag <= fE1MagLow) return kFALSE;
    tag.getFloat(e2Mag,"e2Mag");
    if (e2Mag <= fE2MagLow) return kFALSE;
    tag.getFloat(e1CosTheta,"e1CosTheta"); 
    tag.getFloat(e2CosTheta,"e2CosTheta");
    Double_t theta1 = TMath::ACos(e1CosTheta);
    Double_t theta2 = TMath::ACos(e2CosTheta);
    Double_t theta = theta1 + theta2;
    if (theta <= fThetaLow || theta >= fThetaHigh) return kFALSE;

    return kTRUE;
}

void TGammaGammaTagSelector::PrintOn(std::ostream& o) const 
{
    TTagBaseSelector::PrintOn(o);
    o << endl;
}

std::ostream&  operator << (std::ostream& o, const TGammaGammaTagSelector& a) { a.PrintOn(o); return o; }


// The TBhabhaRatioTagSelector is a specialized TwoprongSelector
// which looks for Bhabha events.

ClassImp(TBhabhaRatioTagSelector)

TBuffer &operator>>(TBuffer &buf, TBhabhaRatioTagSelector *&obj)
{
   obj = (TBhabhaRatioTagSelector *) buf.ReadObject(TBhabhaRatioTagSelector::Class());
   return buf;
}

TBhabhaRatioTagSelector::TBhabhaRatioTagSelector(const char *name) :
TTwoprongTagSelector(name)
{ 
    SetAngularAcceptance(0.8);
    SetEnergyMomentumRatio(0.7);
}

Bool_t TBhabhaRatioTagSelector::Accept(TEventManager& e) 
{
    VAbsTag &tag = e.GetTag();	    // Shortcut to access TAG

    if (!TTwoprongTagSelector::Accept(e)) return kFALSE;

    // Now check, whether we hit the calorimeter with E ~ P

    Float_t p1CosTheta;
    tag.getFloat(p1CosTheta,"p1CosTheta"); 
    if (TMath::Abs(p1CosTheta) > _costh) return kFALSE;	    // Require angular constraint

    Float_t p2CosTheta;
    tag.getFloat(p2CosTheta,"p2CosTheta"); 
    if (TMath::Abs(p2CosTheta) > _costh) return kFALSE;	    // Require angular constraint

    Float_t p1Mag;
    tag.getFloat(p1Mag,"p1Mag"); 
    Double_t p1EmcCandEnergy = tag.GetPrimaryTrackShowerEnergy();
    if (p1EmcCandEnergy < _epratio * p1Mag) return kFALSE;

    Float_t p2Mag;
    tag.getFloat(p2Mag,"p2Mag"); 
    Double_t p2EmcCandEnergy = tag.GetSecondaryTrackShowerEnergy();
    if (p2EmcCandEnergy < _epratio * p2Mag) return kFALSE;

    return kTRUE;
}

void TBhabhaRatioTagSelector::PrintOn(std::ostream& o) const 
{
    TTwoprongTagSelector::PrintOn(o);
    o << "Energy/Momentum ratio = "  << _epratio << endl; 
    o << "Angular Acceptance    = -" << _costh << " ... " << _costh << endl; 
    o << endl;
}

std::ostream&  operator << (std::ostream& o, const TBhabhaRatioTagSelector& a) { a.PrintOn(o); return o; }


// The TBhabhaTagSelector looks for Bhabha events.

ClassImp(TBhabhaTagSelector)

TBuffer &operator>>(TBuffer &buf, TBhabhaTagSelector *&obj)
{
   obj = (TBhabhaTagSelector *) buf.ReadObject(TBhabhaTagSelector::Class());
   return buf;
}

TBhabhaTagSelector::TBhabhaTagSelector(const char *name) :
TTagBaseSelector(name)
{ 
    SetMomentum(3.0);
    SetTotalEnergy(3.0);
    SetShowerEnergy(2.0);
}

Bool_t TBhabhaTagSelector::Accept(TEventManager& e) 
{
    VAbsTag &tag = e.GetTag();	    // Shortcut to access TAG

    if (tag.GetTotalEnergy() <= fETotLow) return kFALSE;

    Float_t p1Mag;
    tag.getFloat(p1Mag,"p1Mag"); 
    if (p1Mag <= fPLow) return kFALSE;

    Int_t n = 0;

    Double_t p1EmcCandEnergy = tag.GetPrimaryTrackShowerEnergy();
    if (p1EmcCandEnergy > fEmcLow) n++;

    Double_t p2EmcCandEnergy = tag.GetSecondaryTrackShowerEnergy();
    if (p2EmcCandEnergy > fEmcLow) n++;

    if (n == 0) return kFALSE; // We require at least one energetic photon 

    return kTRUE;
}

void TBhabhaTagSelector::PrintOn(std::ostream& o) const 
{
    TTagBaseSelector::PrintOn(o);
}

std::ostream&  operator << (std::ostream& o, const TBhabhaTagSelector& a) { a.PrintOn(o); return o; }


// The TRadiativeBhabhaTagSelector is a specialized TwoprongSelector
// which looks for rediative Bhabha events. Needs completion...

ClassImp(TRadiativeBhabhaTagSelector)

TBuffer &operator>>(TBuffer &buf, TRadiativeBhabhaTagSelector *&obj)
{
   obj = (TRadiativeBhabhaTagSelector *) buf.ReadObject(TRadiativeBhabhaTagSelector::Class());
   return buf;
}

Bool_t TRadiativeBhabhaTagSelector::Accept(TEventManager& e) 
{
    VAbsBit &bit = e.GetBit();		 // Shortcut to access TAG
    return bit.GetBool("IsRadiativeBhabha");
}

void TRadiativeBhabhaTagSelector::PrintOn(std::ostream& o) const 
{
    o << GetName() << " settings:" << endl;
    o << "Works in tag bit mode." << endl;
    o << endl;
}

std::ostream&  operator << (std::ostream& o, const TRadiativeBhabhaTagSelector& a) { a.PrintOn(o); return o; }


// The THadronTagSelector looks for hadronic events
// on the TAG level (Require just more than 2 tracks)

ClassImp(THadronTagSelector)

TBuffer &operator>>(TBuffer &buf, THadronTagSelector *&obj)
{
   obj = (THadronTagSelector *) buf.ReadObject(THadronTagSelector::Class());
   return buf;
}

THadronTagSelector::THadronTagSelector(const char *name) :
TTagBaseSelector(name)
{
    SetNumberOfTracks(3); 
}

Bool_t THadronTagSelector::Accept(TEventManager& e) 
{
    VAbsTag &tag = e.GetTag();		 // Shortcut to access TAG

    Int_t nGoodTrk;
    if (!tight) nGoodTrk = tag.GetNumberOfGoodTracksLoose();
    if ( tight) nGoodTrk = tag.GetNumberOfGoodTracksTight();
    if (nGoodTrk < fNTracksMin) return kFALSE;

    return kTRUE;
}

void THadronTagSelector::PrintOn(std::ostream& o) const 
{
    TTagBaseSelector::PrintOn(o);
}

std::ostream&  operator << (std::ostream& o, const THadronTagSelector& a) { a.PrintOn(o); return o; }


// The THadronAodSelector looks for hadronic events
// on the AOD level

ClassImp(THadronAodSelector)

TBuffer &operator>>(TBuffer &buf, THadronAodSelector *&obj)
{
   obj = (THadronAodSelector *) buf.ReadObject(THadronAodSelector::Class());
   return buf;
}

THadronAodSelector::THadronAodSelector(const char *name) :
TAodBaseSelector(name)
{
    SetNumberOfTracks(3);	 // Require more than two tracks
    SetTotalCharge(4);		 // Approx. charge balance
    SetShowerEnergy(0.1,1.E8);   // Matched EMC energy
    SetHighPt(0.5);		 // At least one track with high Pt
    SetPrimaryVertex(1.,-4.,4.); // Within (doca, -z, z)
}

Bool_t THadronAodSelector::Accept(TEventManager& e) 
{
    Float_t energy = 0.0;
    Int_t nGoodTrks = 0, nCharge = 0;
    Bool_t  highpt = kFALSE;
    Int_t i;
	    	    
    // Check all tracks
	    
    Int_t n = e.GetNumberOfChargedTracks();
    for (i=0; i<n; i++) {
		
	VAbsMicroCandidate &cand = e.GetMicroCandidate(i);// Use shortcut to access Micro
		
	Int_t charge = cand.GetCharge();		// Get electrical charge
	Double_t mom = cand.GetMomentum().Mag();	// Get the momentum
	Float_t emc  = cand.GetEmcRawEnergy();		// Get the energy deposit in EMC
		
	// Check the EMC signal and the charge 
		
	if (emc > fEmcLow) energy += emc;		// Accumulate energy deposit
	if (emc <  fEmcLow || emc > fEmcHigh) continue;	// Is the track matched ?
	if (mom <= fPLow   || mom > fPHigh)   continue;	// Momentum is out of range
		
	TCandidate candidate(cand,i);			// Decide on track quality
	if (!fSelector->Accept(candidate)) continue;
		
	// Prepare quantities for the final selection
		
	Double_t pt  = cand.GetMomentum().Perp();       // Get the transverse momentum
	if (pt > fHighPt) highpt = kTRUE;                // Check track momentum
		
	nGoodTrks++;                                    // Count the good tracks
	nCharge += charge;                              // Accumulate charge
		
    }

    n = e.GetNumberOfTracks();			// This second loop accumulates
    for (i=0; i<n; i++) {				// the neutral energy
	if (e.GetMicroCandidate(i).GetCharge()!=0) continue;
	Float_t emc  = e.GetMicroCandidate(i).GetEmcRawEnergy();
	if (emc > fEmcLow) energy += emc;
    }
		

    // Make a final decision on number of tracks, charge and energy balance
	    
    if (energy <= fELow || energy >= fEHigh) return kFALSE; // Require minimum energy deposit
    if (nGoodTrks < fNTracksMin) return kFALSE;             // Require min. number of tracks
    if (!highpt) return kFALSE;                             // Require Pt > HIGHPT (at least one track)
    if (abs(nCharge) >= fTotChar) return kFALSE;            // Require charge balance
	    
    return kTRUE;
}

void THadronAodSelector::PrintOn(std::ostream& o) const 
{
    TAodBaseSelector::PrintOn(o);
    o << "High Pt required      > " << fHighPt << " GeV" << endl; 
    o << endl;
}

std::ostream&  operator << (std::ostream& o, const THadronAodSelector& a) { a.PrintOn(o); return o; }


// The TDimuonAodSelector looks for dimuon events
// on the AOD level

ClassImp(TDimuonAodSelector)

TBuffer &operator>>(TBuffer &buf, TDimuonAodSelector *&obj)
{
   obj = (TDimuonAodSelector *) buf.ReadObject(TDimuonAodSelector::Class());
   return buf;
}

TDimuonAodSelector::TDimuonAodSelector(const char *name) :
TAodBaseSelector(name)
{
    SetNumberOfTracks(2,2);
    SetShowerEnergy(0.05,1.0);
    SetDeltaPhi(8.1);
    SetDeltaCosTheta(0.1);
}

Bool_t TDimuonAodSelector::Accept(TEventManager& e) 
{
    Int_t nGoodTrks = 0;

    tracklist.Reset();			// This will hold the selected candidates
	    	    
    // Check all tracks
	    
    Int_t n = e.GetNumberOfTracks();
    for (Int_t i=0; i<n; i++) {
		
	VAbsMicroCandidate &cand = e.GetMicroCandidate(i);// Use shortcut to access Micro
		
	Int_t charge = cand.GetCharge();		// Get electrical charge
		
	if (charge == 0) continue;			// Skip neutrals for "Rome" selection              
		
	TCandidate candidate(cand,i);			// Decide on track quality
	if (!fSelector->Accept(candidate)) continue;
		
	// Prepare quantities for the final selection
		
	tracklist.Put(candidate);			// Put the candidate into a list

	nGoodTrks++;                                    // Count the good tracks
		
    }
	    
    // Make a final decision on number of tracks
	    
    if (nGoodTrks < fNTracksMin) return kFALSE;              // Require 2 tracks
    if (nGoodTrks > fNTracksMax) return kFALSE;              // Require 2 tracks

    // Are the two tracks back-to-back in the CMS (in theta and phi) ?
    // Note, there is also a very small perpendicular boost due to the beam crossing

    TVector3 cmFrame(0.0,0.0,0.486976);
    if (TRho::Instance()->GetConditions()!=0) {
	TConditions *theConditions = TRho::Instance()->GetConditions();
        TRhoTime theTime(e.GetTime());
	theConditions->At(theTime);		    // Read the beam conditions DB
	TBeams *beams = theConditions->GetBeams();
	Double_t ecms = beams->EnergyCM();
	cmFrame = beams->BoostCMtoLab();
    }

    tracklist.Boost(-1.0*cmFrame);			    // Transform into CMS
    TCandidate *c1 = tracklist.Get(0);
    TCandidate *c2 = tracklist.Get(1);
    Double_t phi1 = c1->P3().Phi();			    // Calculate distance in phi
    Double_t phi2 = c2->P3().Phi();
    Double_t dphi = TMath::Abs(phi1 - phi2);
    dphi = TMath::Abs(dphi - 3.14159265358979323846);			    // Those are close to 180 deg.
    if (dphi/3.14159265358979323846*180. > fDPhi) return kFALSE;		    // Does it match ?

    Double_t cosTh1 = c1->P3().CosTheta();		    // Calculate cos(theta)
    Double_t cosTh2 = c2->P3().CosTheta();
    Double_t dcosth = TMath::Abs(cosTh1 + cosTh2);	    // Ideally cancels to zero
    if (dcosth > fDCosTh) return kFALSE;			    // Does it match ?

    return kTRUE;
}

void TDimuonAodSelector::PrintOn(std::ostream& o) const 
{
    TAodBaseSelector::PrintOn(o);
    o << "Delta CosTheta        < "  << fDCosTh << endl; 
    o << "Delta Phi [deg]       < "  << fDPhi << endl;
    o << endl;
}

std::ostream&  operator << (std::ostream& o, const TDimuonAodSelector& a) { a.PrintOn(o); return o; }


// The TBhabhaAodSelector looks for Bhabha events
// on the AOD level

ClassImp(TBhabhaAodSelector)

TBuffer &operator>>(TBuffer &buf, TBhabhaAodSelector *&obj)
{
   obj = (TBhabhaAodSelector *) buf.ReadObject(TBhabhaAodSelector::Class());
   return buf;
}

TBhabhaAodSelector::TBhabhaAodSelector(const char *name) :
TAodBaseSelector(name)
{
    SetTotalEnergy(4.0);
    SetNumberOfTracks(1,2);
    SetShowerEnergy(1.0,1.E8);
}

Bool_t TBhabhaAodSelector::Accept(TEventManager& e) 
{
    Int_t  nTracks = 0;
    Float_t energy = 0.0;
    Bool_t  matched = kFALSE;
    UInt_t  above1 = 0, above2 = 0;

    // Check all tracks
	    
    UInt_t n = e.GetNumberOfChargedTracks();
    for (UInt_t i=0; i<n; i++) {
		
	VAbsMicroCandidate &cand = e.GetMicroCandidate(i);  // Use shortcut to access Micro
		
	Int_t charge = cand.GetCharge();	// Get electrical charge
	Float_t emc  = cand.GetEmcRawEnergy();	// Get the energy deposit in EMC
	energy += emc;

	if (emc > 2.0) 
	    above2++;				// Count the energetic bumps
	else if (emc > 1.0) 
	    above1++;

	if (charge == 0) continue;		// Skip neutrals

	TCandidate candidate(cand,i);		// Decide on track quality
	if (!fSelector->Accept(candidate)) continue;
	nTracks++;

	if (emc > 0.1) matched = kTRUE;		// Is the track matched ?
    }
	    
    // Make a decision

    if (nTracks < fNTracksMin) return kFALSE;    // Require at least 1 track
    if (nTracks > fNTracksMax) return kFALSE;    // Not more than 2 tracks
    if (energy < fETotLow) return kFALSE;	// Require emc energy
    if (above2 < 1) return kFALSE;		// and bumps above
    if (above1+above2 < 2) return kFALSE;	// a certain threshold
    if (!matched) return kFALSE;			// One track should be matched

    return kTRUE;
}

void TBhabhaAodSelector::PrintOn(std::ostream& o) const 
{
    TAodBaseSelector::PrintOn(o);
    o << endl;
}

std::ostream&  operator << (std::ostream& o, const TBhabhaAodSelector& a) { a.PrintOn(o); return o; }

