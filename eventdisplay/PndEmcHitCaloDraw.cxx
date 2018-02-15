// -------------------------------------------------------------------------
// -----                        FairTPCRecoPointDraw source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------

#include "PndEmcHitCaloDraw.h"
#include "PndEmcHit.h"
#include <iostream>

// -------------------------------------------------------------------------

TVector3 PndEmcHitCaloDraw::GetVector(TObject* obj)
{
	PndEmcHit* hit = (PndEmcHit*)obj;
	TVector3 pos;
	hit->Position(pos);
	return pos;
}

Double_t PndEmcHitCaloDraw::GetEnergy(TObject* obj)
{
	PndEmcHit* hit = (PndEmcHit*)obj;
	return (hit->GetEnergy()); //< Energy in GeV
}


ClassImp(PndEmcHitCaloDraw)
