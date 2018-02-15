// -------------------------------------------------------------------------
// -----                        FairTPCRecoPointDraw source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------

#include "PndEmcDigiCaloDraw.h"
#include "PndEmcDigi.h"
#include <iostream>

// -------------------------------------------------------------------------

TVector3 PndEmcDigiCaloDraw::GetVector(TObject* obj)
{
	PndEmcDigi* digi = (PndEmcDigi*)obj;
	TVector3 pos = digi->where();
	return pos;
}

Double_t PndEmcDigiCaloDraw::GetEnergy(TObject* obj)
{
	PndEmcDigi* digi = (PndEmcDigi*)obj;
	return (digi->GetEnergy()); //< Energy in GeV
}


ClassImp(PndEmcDigiCaloDraw)
