/*
 * PndMvdRadDamHit.cxx
 *
 *  Created on: Dec 16, 2008
 *      Author: stockman
 */

#include "PndMvdRadDamHit.h"

PndMvdRadDamHit::PndMvdRadDamHit()
{
}

PndMvdRadDamHit::PndMvdRadDamHit(Int_t trackID, Int_t hitID, Int_t detId, Int_t pdgCode, Double_t energy,
  	    TVector3 pos, TVector3 mom, Double_t radDam):
	  	    	fTrackID(trackID), fHitID(hitID), fDetId(detId), fPdgCode(pdgCode), fEnergy(energy),
	  	    	fPos(pos), fMom(mom), fRadDam(radDam)
{
}

ClassImp(PndMvdRadDamHit);
