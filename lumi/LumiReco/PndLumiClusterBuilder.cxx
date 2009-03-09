/*
 * PndLumiClusterBuilder.cxx
 *
 *  Created on: Nov 18, 2008
 *      Author: tsito
 */
#include "PndLumiClusterBuilder.h"

PndLumiClusterBuilder::PndLumiClusterBuilder()
{

}

PndLumiClusterBuilder::PndLumiClusterBuilder(Double_t pitch,
		Double_t orient, Double_t W, Double_t L,
		Double_t thr, Double_t noise, Double_t sigma)
{
	fPitch          = pitch;
	fOrient         = orient;
	fWidth  		= W;
	fLength 		= L;
	fThreshold      = thr;
	fNoise          = noise;
	fSigma          = sigma;
	//fSide           = side;
}

PndLumiClusterBuilder::~PndLumiClusterBuilder()
{

}

InitStatus PndLumiClusterBuilder::Init()
{
	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman ){
		cout << "-E- PndLumiClusterBuilder::Init: "
		<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}

	// Get input array
	fLumiPoint = (TClonesArray*) ioman->GetObject("LumiPoint");
	if (!fLumiPoint){
		cout << "-W- PndLumiClusterBuilder::Init: "
		<< "No LumiPoint collection!" << endl;
		return kERROR;
	}

	// Create and register output array
	fLumiClusterFront = new TClonesArray("PndLumiCluster");
	fLumiClusterBack = new TClonesArray("PndLumiCluster");
	ioman->Register("LumiClusterFront", "Lumi", fLumiClusterFront, kTRUE);
	ioman->Register("LumiClusterBack", "Lumi", fLumiClusterBack, kTRUE);

	return kSUCCESS;

}

void PndLumiClusterBuilder::Exec(Option_t* opt)
{
	// Reset output array
    if (!fLumiClusterFront || !fLumiClusterBack)
    	Fatal("Exec", "No hit collection");

    fLumiClusterFront->Clear();
    fLumiClusterBack->Clear();

    PndLumiPoint *point = NULL;

    std::vector<PndLumiStrip> digi_front, digi_back;  //collect digi strip
    std::map<Int_t, PndLumiStrip> cluster_front, cluster_back;
    std::map<Int_t, PndLumiStrip>::iterator it, it1;
    TVector3 pos_strip_front, pos_strip_back;
    TVector3 glo;
    TVector2 stripzeroId_front, stripzeroId_back;
    TVector3 entryPos, exitPos, MCPosition;

    //Loop over LumiPoint
    Int_t nPoints = fLumiPoint->GetEntriesFast();

    for (Int_t iPoint = 0 ; iPoint< nPoints ; iPoint++){
    	point = (PndLumiPoint*) fLumiPoint->At(iPoint);

    	//Variable definition

    	TString detname = point->GetDetName();
    	Double_t eLoss = point->GetEnergyLoss()* 1E9;

    	entryPos = point->GetEntryPoint(),
    	exitPos = point->GetExitPoint();
    	MCPosition = 0.5*(entryPos + exitPos);

    	//cout << "Entry in GCS : ( "<< entryPos.X()<< " , "<<entryPos.Y() <<" , "<<entryPos.Z()<< " )"<<endl;
    	//cout << "Exit in GCS : ( "<< exitPos.X()<< " , "<<exitPos.Y() <<" , "<<exitPos.Z()<< " )"<<endl;
    	//cout << "Mean : ( "<< MCPosition.X() <<" , " <<MCPosition.Y()<<" , "<< MCPosition.Z()<< " )"<< endl;

    	//Make some transformations of the coord syst
		FairGeoVector posInL, posOutL, A;
		TVector3 B;
		PndLumiTransposition trans(fVerboseLevel);
		trans.GetLocalHitPoints(point, posInL, posOutL);
		//cout << "Entry in LCS : ( "<< posInL.X()<< " , "<<posInL.Y()<< " , "<< posInL.Z()<<" )"<<endl;
		//cout << "Exit in LCS : ( "<< posOutL.X()<< " , "<<posOutL.Y()<< " , "<< posOutL.Z()<<" )"<<endl;

		pos_strip_front= trans.LocalToStripOrientation(fOrient, posInL);
		//cout << " Entry in SCS : "<<pos_strip_front.X() <<" , "<< pos_strip_front.Y() <<" , "<< pos_strip_front.Z()<<endl;
		A.setXYZ(pos_strip_front.X(), pos_strip_front.Y(), pos_strip_front.Z());
		B = trans.LocalToStripOrientation(((2*TMath::Pi())-fOrient), A);
		stripzeroId_front.Set(fWidth, 0.0);

		pos_strip_back= trans.LocalToStripOrientation(-((TMath::Pi()/2)-fOrient), posOutL);
		//cout << " Exit in SCS : "<<pos_strip_back.X() <<" , "<< pos_strip_back.Y() <<" , "<< pos_strip_back.Z()<<endl;
		stripzeroId_back.Set(0.0, 0.0);

		PndLumiCalcStripDigi StripDigiFront(fPitch, fOrient, fWidth,
				fLength, fThreshold, fNoise, fSigma, stripzeroId_front);
		digi_front = StripDigiFront.GetStripsDigi(posInL, posOutL, eLoss);
		cluster_front = StripDigiFront.GetClusters(digi_front);

		PndLumiCalcStripDigi StripDigiBack(fPitch, -((TMath::Pi()/2)-fOrient), fWidth,
				fLength, fThreshold, fNoise, fSigma, stripzeroId_back);
		digi_back = StripDigiBack.GetStripsDigi(posInL, posOutL, eLoss);
		cluster_back = StripDigiBack.GetClusters(digi_back);

		TGeoHMatrix transMatrix = trans.GetTransformation(detname.Data());
		TVector3 offset = trans.GetSensorDimensions(detname.Data());

		new ((*fLumiClusterFront)[iPoint]) PndLumiCluster( detname, 1.,
				pos_strip_front, MCPosition, cluster_front, transMatrix, offset,
				nPoints, stripzeroId_front, fPitch, fOrient);
		new ((*fLumiClusterBack)[iPoint]) PndLumiCluster( detname, -1.,
				pos_strip_back, MCPosition, cluster_back, transMatrix, offset,
				nPoints, stripzeroId_back, fPitch, -((TMath::Pi()/2)-fOrient));
    }
}
ClassImp(PndLumiClusterBuilder)
