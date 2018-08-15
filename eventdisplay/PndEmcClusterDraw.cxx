// -------------------------------------------------------------------------
// -----                        FairTPCRecoPointDraw source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------

#include "PndEmcClusterDraw.h"
#include "PndEmcCluster.h"
#include <iostream>

// -------------------------------------------------------------------------
TVector3 PndEmcClusterDraw::GetVector(TObject* obj)
{
	PndEmcCluster* Cluster = (PndEmcCluster*)obj;
	return (Cluster->where());
}


Int_t PndEmcClusterDraw::GetValue(TObject* obj, Int_t ) // i //[R.K.03/2017] unused variable(s)
{
	PndEmcCluster* Cluster = (PndEmcCluster*)obj;
	return (Cluster->energy() * 1000); //< Energy in MeV
}


ClassImp(PndEmcClusterDraw)
