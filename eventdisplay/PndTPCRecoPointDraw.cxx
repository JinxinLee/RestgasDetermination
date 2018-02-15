// -------------------------------------------------------------------------
// -----                        FairTPCRecoPointDraw source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------

#include "PndTPCRecoPointDraw.h"
#include "PndTpcCluster.h"
#include <iostream>

// -------------------------------------------------------------------------
TVector3 PndTPCRecoPointDraw::GetVector(TObject* obj)
{
	PndTpcCluster* cluster = (PndTpcCluster*)obj;
	return (cluster->pos());
}


ClassImp(PndTPCRecoPointDraw)


