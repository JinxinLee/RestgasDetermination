/*
 * PndMvdDigiPixelDraw.cpp
 *
 *  Created on: Apr 20, 2009
 *      Author: stockman
 */

#include "PndMvdDigiPixelDraw.h"

#include "PndSdsCalcFePixel.h"
#include "PndSdsHit.h"
#include "PndSdsCluster.h"

#include "FairRootManager.h"

#include "TGeoMatrix.h"
#include "TClonesArray.h"
#include "TEveTrans.h"
#include "TEveManager.h"
#include "TEveBoxSet.h"

PndMvdDigiPixelDraw::PndMvdDigiPixelDraw() : fPixelSize(0.01), fBoxSize(1)
{}

PndMvdDigiPixelDraw::PndMvdDigiPixelDraw(const char* name, Int_t iVerbose) : FairBoxSetDraw(name, iVerbose), fPixelSize(0.01), fBoxSize(1)
{}

PndMvdDigiPixelDraw::~PndMvdDigiPixelDraw()
{
	delete(fGeoH);
}

InitStatus PndMvdDigiPixelDraw::Init()
{
	FairBoxSetDraw::Init();
	fGeoH = new PndGeoHandling();
	fClusterCands = (TClonesArray *)fManager->GetObject("MVDPixelClusterCand");
	fRecoHits     = (TClonesArray *)fManager->GetObject("MVDHitsPixel");

	if(fClusterCands != 0 && fRecoHits != 0) fUseCluster = kTRUE;
}

void PndMvdDigiPixelDraw::Exec(Option_t* option)
{
	PndSdsCalcFePixel calc(104,104,10);
	Int_t col, row, fe;
	Reset();
	if (fUseCluster == kTRUE){
		for (int hitNr = 0; hitNr < fRecoHits->GetEntriesFast(); hitNr++){
			PndSdsHit*		myRecoHit = (PndSdsHit*)fRecoHits->At(hitNr);
			TVector3 recoVector;
			myRecoHit->Position(recoVector);
			TVector3 recoLocal = fGeoH->MasterToLocalId(recoVector, myRecoHit->GetDetName());
			TVector3 SensDim = fGeoH->GetSensorDimensionsId(myRecoHit->GetDetName());
			TString detName = myRecoHit->GetDetName();
			if (fModules[detName] == 0)
				fModules[detName] = CreateNewBoxSet(detName);

			PndSdsCluster* myCluster = (PndSdsCluster*)fClusterCands->At(myRecoHit->GetRefIndex());
			for (int clusterNr = 0; clusterNr < myCluster->GetClusterSize(); clusterNr++){
				PndSdsDigiPixel* p = (PndSdsDigiPixel*)fList->At(myCluster->GetDigiIndex(clusterNr));
				col = p->GetPixelColumn();
				row = p->GetPixelRow();
				fe  = p->GetFE();
				std::cout << "Fe, Col, Row: " << fe << " " << col << "/" << row << std::endl;
				calc.CalcSensorColRow(col, row, p->GetFE());
				std::cout << "Col, Row " << p->GetDetName() << " : " << col << "/" << row << std::endl;
				TVector3 digiLocal(((col+0.5) - (SensDim.X()*100))*fPixelSize, ((row+0.5)-(SensDim.Y()*100))*fPixelSize, 0);
				std::cout << "DigiLocal: " << digiLocal.X() << " " << digiLocal.Y() << " " << digiLocal.Z() << std::endl;
				std::cout << "RecoLocal: " << recoLocal.X() << " " << recoLocal.Y() << " " << recoLocal.Z() << std::endl;
				TVector3 diff = digiLocal - recoLocal;
				std::cout << "Diff: " << diff.X() << " " << diff.Y() << " " << diff.Z() << std::endl;
				TVector3 final = diff * (fBoxSize/fPixelSize);
				final += recoLocal;
				std::cout << "Final: " << final.X() << " " << final.Y() << " " << final.Z() << std::endl;

				fModules[p->GetDetName()]->AddBox(final.X(), final.Y(), final.Z());
				fModules[p->GetDetName()]->DigitValue(p->GetCharge());
			}
		}
	}
	else {
		for (Int_t i=0; i<fList->GetEntriesFast(); ++i) {
			PndSdsDigiPixel* p = (PndSdsDigiPixel*)fList->At(i);
			TString detName = p->GetDetName();

			if (fModules[detName] == 0)
				fModules[detName] = CreateNewBoxSet(detName);

			col = p->GetPixelColumn();
			row = p->GetPixelRow();
			fe  = p->GetFE();
			std::cout << "Fe, Col, Row: " << fe << " " << col << "/" << row << std::endl;
			calc.CalcSensorColRow(col, row, p->GetFE());
			std::cout << "Col, Row " << p->GetDetName() << " : " << col << "/" << row << std::endl;
			TVector3 SensDim = fGeoH->GetSensorDimensionsId(p->GetDetName());
			std::cout << "SensDim: " << SensDim.X() << " " << SensDim.Y() << " " << SensDim.Z() << std::endl;
			fModules[p->GetDetName()]->AddBox(((col+0.5) - (SensDim.X()*100))*0.01, ((row+0.5)-(SensDim.Y()*100))*0.01, 0.01);
			fModules[p->GetDetName()]->DigitValue(p->GetCharge());
		}
	}

	for (boxSetMapIter it = fModules.begin(); it != fModules.end(); it++){
		   TGeoHMatrix testMatrix = *(fGeoH->GetMatrixId(it->first));
		   //TGeoHMatrix invMatrix = testMatrix.Inverse();
		   Double_t scale[] = {0.01,0.01,0.01};		//Dim is 0.1 mm
		   //testMatrix.SetScale(scale);
		   TEveTrans& t = it->second->RefMainTrans();
		   t.SetFrom(testMatrix);
		   TEveElement* el = (TEveElement*)it->second;
		   TEveElement* man = (TEveElement*)fEventManager;
		   gEve->AddElement(it->second, fEventManager);
	}

	gEve->Redraw3D(kFALSE);
}

TEveBoxSet* PndMvdDigiPixelDraw::CreateNewBoxSet(TString& name)
{
	TEveBoxSet* bs = new TEveBoxSet(name);
	bs->Reset(TEveBoxSet::kBT_AABoxFixedDim, kFALSE, 32);
	bs->SetDefWidth(fBoxSize);
	bs->SetDefHeight(fBoxSize);
	bs->SetDefDepth(fBoxSize);

	return bs;
}

void PndMvdDigiPixelDraw::Reset()
{
	for (boxSetMapIter it = fModules.begin(); it != fModules.end(); it++){
		TEveElement* el = (TEveElement*)it->second;
		gEve->RemoveElement(it->second, fEventManager);
	}
	fModules.clear();
}

ClassImp(PndMvdDigiPixelDraw);

