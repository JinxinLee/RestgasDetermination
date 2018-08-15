/**
 @class PndSmpCand

 @author Klaus Goetzen <k [dot] goetzen (at) gsi [dot] de>

 @brief Simple particle candidate to perform simple combinatorics and particle counting for event filtering
 
 This class defines a simple particle candidate, which allows simple combinatoric with overlap check 
 and provides access to its internal 4-vector.

 */
 
#include "PndSmpCand.h"
 
// -------------------------------------------------------------------------

PndSmpCand::PndSmpCand(TLorentzVector lv, Float_t chrg, Int_t pdg, Int_t uid) : 
	fLV(lv), fCharge(chrg), fPdgCode(pdg), fUniqueID(uid), fNDau(0)
{
	for (int i=0;i<5;++i) fDaughter[i] = 0;
	fMarker = 1<<fUniqueID;
}

// -------------------------------------------------------------------------

PndSmpCand::PndSmpCand(TLorentzVector *lv, Float_t chrg, Int_t pdg, Int_t uid) : 
	fLV(*lv), fCharge(chrg), fPdgCode(pdg), fUniqueID(uid), fNDau(0)
{
	for (int i=0;i<5;++i) fDaughter[i] = 0;
	fMarker = 1<<fUniqueID;
}

// -------------------------------------------------------------------------

void PndSmpCand::Print()
{
	printf("uid:%3d  lv:(%7.3f,%7.3f,%7.3f,%7.3f ; m:%7.3f, p:%7.3f, pt:%7.3f, tht:%6.1f, phi:%7.1f)  ch:%2.0f  pdg:%5d  marker:%8lu (",
		fUniqueID, fLV.Px(), fLV.Py(), fLV.Pz(), fLV.E(), fLV.M(), fLV.P(), fLV.Pt(), fLV.Theta()*57.296, fLV.Phi()*57.296, fCharge, fPdgCode, fMarker);
	PrintBinary(fMarker);
	printf(")\n");
}

// -------------------------------------------------------------------------

void PndSmpCand::CombCand(Int_t pdg, PndSmpCand *c0, PndSmpCand *c1, PndSmpCand *c2, PndSmpCand *c3, PndSmpCand *c4)
{
	fPdgCode     = pdg;
	fNDau        = 2;

	fDaughter[0] = c0;
	fDaughter[1] = c1;
	
	fMarker      = c0->fMarker  | c1->fMarker;
	fLV          = c0->fLV      + c1->fLV;
	fCharge      = c0->fCharge  + c1->fCharge;
	fUniqueID    = -1;
	
	if (0!=c2) { fDaughter[fNDau++] = c2; fLV += c2->fLV; fCharge += c2->fCharge; fMarker |= c2->fMarker; }
	if (0!=c3) { fDaughter[fNDau++] = c3; fLV += c3->fLV; fCharge += c3->fCharge; fMarker |= c3->fMarker; }
	if (0!=c4) { fDaughter[fNDau++] = c4; fLV += c4->fLV; fCharge += c4->fCharge; fMarker |= c4->fMarker; }
}

// -------------------------------------------------------------------------

ClassImp(PndSmpCand)
