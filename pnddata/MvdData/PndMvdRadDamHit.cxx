/*
 * PndMvdRadDamHit.cxx
 *
 *  Created on: Dec 16, 2008
 *      Author: stockman
 */

#include "PndMvdRadDamHit.h"

PndMvdRadDamHit::PndMvdRadDamHit() :
  fTrackID(0),
	fHitID(0),
  fDetId(0),
	fPdgCode(0),
	fEnergy(0.),
	fMom(0.,0.,0.),
	fRadDam(0.)
{
}

PndMvdRadDamHit::PndMvdRadDamHit(Int_t trackID, Int_t hitID, Int_t detId, Int_t pdgCode, Double_t energy,
  	    TVector3 pos, TVector3 mom, Double_t radDam):
	  	    	fTrackID(trackID), fHitID(hitID), fDetId(detId), fPdgCode(pdgCode), fEnergy(energy),
	  	    	fMom(mom), fRadDam(radDam)
{
	SetPosition(pos);
}

ClassImp(PndMvdRadDamHit);
