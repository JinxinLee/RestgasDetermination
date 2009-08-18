/*
 * PndLumiRecoTask.cxx
 *
 *  Created on: Nov 10, 2008
 *      Author: tsito
 */

#include "PndLumiRecoTask.h"
PndLumiRecoTask::PndLumiRecoTask()
{
}

InitStatus PndLumiRecoTask::Init()
{
	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman ){
		cout << "-E- PndLumiRecoTask::Init: "
		     << "RootManager not instantiated!" << endl;
		return kFATAL;
	}

	// Get input array

	fLumiClusterArray = (TClonesArray*) ioman->GetObject("LumiCluster");
	if (!fLumiClusterArray){
		cout << "-W- PndLumiRecoTask::Init: "
		     << "No LumiCluster collection!" << endl;
		return kERROR;
	}

	//Get Output array
	fLumiRecoArray = new TClonesArray("PndLumiRecoHit");
	ioman->Register("LumiRecoHit", "Lumi", fLumiRecoArray, kTRUE);

	return kSUCCESS;
}

void PndLumiRecoTask::Exec(Option_t* opt)
{
	if (!fLumiRecoArray)
		Fatal("Exec", "No Reconstructed Array");
	fLumiRecoArray->Clear();

	//Variables declaration
	TGeoHMatrix transMatrix;
	Double_t yf, yb;
	Double_t of, ob;
	TVector2 RecoPoint;
	TVector3 RecoPosition, MCPosition;
	TVector3 Loc;
	TVector3 offset ;

	// Loop over PndLumiCluster
	Int_t nClusters = fLumiClusterArray->GetEntriesFast();

	for (Int_t iCluster = 0; iCluster < nClusters; iCluster++){

		PndLumiCluster *Cluster = (PndLumiCluster*) fLumiClusterArray->At(iCluster);

		yf = Cluster->GetClusterPositionFront();
		yb = Cluster->GetClusterPositionBack();
		of = Cluster->GetStripOrientFront();
		ob = Cluster->GetStripOrientBack();
		MCPosition = Cluster->GetMCPosition();
		transMatrix = Cluster->GetTransformationMatrix();
		offset = Cluster->GetSensorDimension();

		RecoPoint = CalcLineCross(yf, yb, of, ob);

		Loc.SetXYZ(RecoPoint.X(),RecoPoint.Y(),MCPosition.Z());

		Double_t local[3], global[3];

		local[0] = Loc.X()- offset.X();
		local[1] = Loc.Y()- offset.Y();
		local[2] = Loc.Z()- offset.Z();

		transMatrix.LocalToMaster(local , global);

		RecoPosition.SetXYZ(global[0], global[1], global[2]);

		new((*fLumiRecoArray)[iCluster]) PndLumiRecoHit(MCPosition, RecoPosition);

	}

	Print();
}

TVector2 PndLumiRecoTask::CalcLineCross(
              Double_t yf, Double_t yb,
              Double_t of, Double_t ob) const
{
	Double_t D, dx, dy, x, y;
	D = -cos(of)*sin(ob) + cos(ob)*sin(of);
	dy = -yf*sin(ob) + yb*sin(of);
	dx = yb*cos(of) - yf*cos(ob);
	x = dx/D;
	y = dy/D;
	TVector2 result(x,y);
	return result;
}

void PndLumiRecoTask::Print() const
{
	Int_t n = fLumiRecoArray->GetEntriesFast();
	for (Int_t i=0; i < n; i++){
		(*fLumiRecoArray)[i]->Print();
	}

}
ClassImp(PndLumiRecoTask)
