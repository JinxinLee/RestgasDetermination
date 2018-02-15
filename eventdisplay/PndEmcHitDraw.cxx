// -------------------------------------------------------------------------
// -----                        FairTPCRecoPointDraw source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------

#include "PndEmcHitDraw.h"
#include "PndEmcHit.h"
#include <iostream>

// -------------------------------------------------------------------------

Int_t PndEmcHitDraw::GetValue(TObject* obj, Int_t ) // i //[R.K.03/2017] unused variable(s)
{
	PndEmcHit* Hit = (PndEmcHit*)obj;
	return (Hit->GetEnergy() * 1000); //< Energy in MeV
}


ClassImp(PndEmcHitDraw)
