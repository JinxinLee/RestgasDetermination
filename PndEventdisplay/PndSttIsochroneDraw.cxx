// -------------------------------------------------------------------------
// -----                        PndSttIsochroneDraw source file                  -----
// -----                  Created 10/12/07  by M. Al-Turany            -----
// -------------------------------------------------------------------------

#include "PndSttIsochroneDraw.h"
#include "TEveTrack.h"
#include <iostream>
#include "TEveTrackPropagator.h"
#include "TGeoTrack.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TEveManager.h"
#include "FairEventManager.h"
#include "PndTrack.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TEveBoxSet.h"
#include "TEveGeoShape.h"
#include "TGeoMatrix.h"
#include "TGeoTube.h"
#include "TGeoMatrix.h"
#include "PndSttMapCreator.h"
#include "PndSttHit.h"
#include "PndSttTube.h"
#include "PndSttSingleStraw.h"
#include "PndSttPoint.h"
#include "FairRuntimeDb.h"
#include "FairEventHeader.h"
#include <string>
#include <sstream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndSttIsochroneDraw::PndSttIsochroneDraw(Bool_t propagate)
   : fTimeWindowPlus(0.), fTimeWindowMinus(0.), fStartTime(-1), fUseEventTime(kTRUE), fUseIsochroneTime(kFALSE),
	 fSttHitArray(0), fSttTubeArray(0), fSttParameters(0), fEventManager(0), fListOfIsochrones(0), fListOfTiltedIsochrones(0),
	 fStartFunctor(0), fStopFunctor(0), fCrawler(0), fEveTrList(0), fEventHeaderBranch(0)
{
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
PndSttIsochroneDraw::PndSttIsochroneDraw(const char* name, Bool_t propagate, Int_t iVerbose)
  : FairTask(name, iVerbose), fTimeWindowPlus(0.), fTimeWindowMinus(0.), fStartTime(-1), fUseEventTime(kTRUE), fUseIsochroneTime(kFALSE),
	 fSttHitArray(0), fSttTubeArray(0), fSttParameters(0), fEventManager(0), fListOfIsochrones(0), fListOfTiltedIsochrones(0),
	 fStartFunctor(0), fStopFunctor(0), fCrawler(0), fEveTrList(0), fEventHeaderBranch(0)
{
	 // fPro = new FairGeanePro();

}


// -------------------------------------------------------------------------
InitStatus PndSttIsochroneDraw::Init()
{
   if (fVerbose > 1)
		cout << "PndSttIsochroneDraw::Init()" << endl;
   if (fSttHitArray == 0){
		FairRootManager* fManager = FairRootManager::Instance();
		fSttHitArray = (TClonesArray *) fManager->GetObject(GetName());
		if (fSttHitArray == 0) {
			cout << "PndSttIsochroneDraw::Init()  branch " << GetName()
					<< " Not found! Task will be deactivated " << endl;
			SetActive(kFALSE);
		}

		fEventManager = FairEventManager::Instance();
		if (fVerbose > 2)
			cout << "PndSttIsochroneDraw::Init() get instance of FairEventManager "	<< endl;
		PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
		fSttTubeArray = mapper->FillTubeArray();

   }
   fStartFunctor = new StopTime();
   fStopFunctor = new StopTime();
   fCrawler = new PndMCDataCrawler();
   fCrawler->Init();
   fCrawler->SetStoreIntermediate(kFALSE);

   if (IsActive())
		return kSUCCESS;
	else
		return kERROR;
}

// -----   Destructor   ----------------------------------------------------
PndSttIsochroneDraw::~PndSttIsochroneDraw()
{
	if (fStartFunctor != 0)
		delete (fStartFunctor);
	if (fStopFunctor != 0)
		delete (fStopFunctor);
}
// -------------------------------------------------------------------------
void PndSttIsochroneDraw::Exec(Option_t* option)
{

	if (IsActive()) {

		if (fVerbose > 1)
			cout << " PndSttIsochroneDraw::Exec " << endl;
		Reset();

		fListOfIsochrones = new TEveBoxSet("SttIsochrones");
		fListOfTiltedIsochrones = new TEveBoxSet("SttTiltedIsochrones");
		fListOfParallelIsochrones = new TEveBoxSet("SttParallelIsochrones");
		//fListOfTracks->DestroyElements();

		gEve->AddElement(fListOfIsochrones, fEventManager);
		fListOfIsochrones->AddElement(fListOfTiltedIsochrones);
		fListOfIsochrones->AddElement(fListOfParallelIsochrones);
		Double_t eventTime = FairRootManager::Instance()->GetEventTime();

		if (FairRunAna::Instance()->IsTimeStamp()) {
			std::cout << "Inside TimeStamp" << std::endl;
			if (fUseEventTime) {
				fStartTime = eventTime - fTimeWindowMinus;
			}
			cout << "EventTime: " << eventTime << " TimeWindow: " << fStartTime
					<< " - " << eventTime + fTimeWindowPlus << std::endl;

			fSttHitArray = FairRootManager::Instance()->GetData(GetName(),
					fStartFunctor, fStartTime, fStopFunctor, eventTime + fTimeWindowPlus); //FairRootManager::Instance()->GetEventTime() +
			std::cout << fStartTime << " " << eventTime + fTimeWindowPlus
					<< " FoundTracks: " << fSttHitArray->GetEntriesFast()
					<< std::endl;
		}

		std::cout << "SttHitArray: " << fSttHitArray->GetEntriesFast()
				<< std::endl;

		for (Int_t i = 0; i < fSttHitArray->GetEntriesFast(); i++) {
			if (fVerbose > 2)
				cout << "PndSttIsochroneDraw::Exec " << i << endl;

			PndSttHit* myHit = (PndSttHit*) fSttHitArray->At(i);
			std::cout << "STTHits: " << myHit->GetX() << " " << myHit->GetY()
					<< " " << myHit->GetZ() << std::endl;
			if (0 == myHit) {
				std::cout << "-E- PndSttIsochroneDraw Hit does not exist! " << i
						<< std::endl;
				continue;
			}
			if (myHit->GetTubeID() < fSttTubeArray->GetEntriesFast()) {
				PndSttTube* myTube = (PndSttTube*) fSttTubeArray->At(
						myHit->GetTubeID());

				if (0 == myTube) {
					std::cout << "-E- PndSttIsochroneDraw Tube does not exist! "
							<< myHit->GetTubeID() << std::endl;
					continue;
				}

//				std::cout << "Tube: " << myTube->GetHalfLength() << std::endl;

				Double_t tubeLengthHalf = myTube->GetHalfLength();
				Double_t radius = 0;
				Double_t radiusError = 0;
				if (fUseIsochroneTime == kTRUE) {
					PndSttSingleStraw straw;
					Double_t driftTime = myHit->GetTimeStamp() - eventTime;
					FairMultiLinkedData* linkData = (FairMultiLinkedData*)myHit; //fInputData->At(i);

					fCrawler->Init();
					fCrawler->SetStoreIntermediate(kFALSE);
					std::cout << "StartLink: " << *linkData << std::endl;
					FairMultiLinkedData result = fCrawler->GetInfo(FairMultiLinkedData(*linkData), "STTPoint");
					std::cout << "Links: " << result << std::endl;
					if (result.GetNLinks() == 1 && result.GetLink(0).GetType() == FairRootManager::Instance()->GetBranchId("STTPoint")){
						PndSttPoint* mcPoint = (PndSttPoint*)FairRootManager::Instance()->GetCloneOfLinkData(result.GetLink(0));
						FairLink evtHeaderLink(-1, result.GetLink(0).GetEntry(), "EventHeader.", -1);
						FairEventHeader* evtHeader = (FairEventHeader*)FairRootManager::Instance()->GetCloneOfLinkData(evtHeaderLink);
						if (evtHeader != 0)
							std::cout << "MCTime: " << evtHeader->GetEventTime() << std::endl;
						std::cout << "MCRadius: " << mcPoint->GetTrueDistance() << std::endl;// << " MCTime: " << evtHeader->GetEventTime() << std::endl;
						delete mcPoint;
						delete evtHeader;
					}
					std::cout << "-I- PndSttIsochroneDraw Pulse: " << myHit->GetPulse() << " TimeStamp: " << myHit->GetTimeStamp() << " EventTime " << eventTime << " DriftTime: " << driftTime << std::endl;
					std::cout << "-I- PndSttIsochroneDraw Radius: " << myHit->GetIsochrone() << " RecoRadius: " << straw.TimnsToDiscm(driftTime) << std::endl;

					if (driftTime > 0) {
						radius = straw.TimnsToDiscm(driftTime);
					} else {
						std::cout << "-I- PndSttIsochrone Drift time <= 0: " << driftTime << std::endl;
						continue;
					}
					if (radius > myTube->GetRadIn()) {
						std::cout << "-I- PndSttIsochrone Isochrone with " << radius << " larger than tube radius of " << myTube->GetRadIn() << std::endl;
						continue;
					}
					radiusError = myHit->GetIsochroneError();
				} else {
					radius = myHit->GetIsochrone();
					radiusError = myHit->GetIsochroneError();
				}
//				std::cout << "RadiusError " << radiusError << std::endl;

				TEveGeoShape* myEveShape;
				myEveShape = new TEveGeoShape("SttTube");
				radiusError = 0.05;
				myEveShape->SetShape(
						new TGeoTube(radius - radiusError, radius + radiusError,
								tubeLengthHalf));
//				std::cout << "Wire direction: " << myTube->GetWireDirection().X() << " "  << myTube->GetWireDirection().Y() << " "  << myTube->GetWireDirection().Z() << std::endl;
				if (myTube->GetWireDirection().Pt() > 0) {
					myEveShape->SetMainColor(kYellow);
					myEveShape->SetMainTransparency(80);
				} else {
					myEveShape->SetMainColor(kCyan);
				}
				TMatrixT<double> myRotMat = myTube->GetRotationMatrix();
				Double_t rotArray[9];
				rotArray[0] = myRotMat(0, 0);
				rotArray[1] = myRotMat(0, 1);
				rotArray[2] = myRotMat(0, 2);
				rotArray[3] = myRotMat(1, 0);
				rotArray[4] = myRotMat(1, 1);
				rotArray[5] = myRotMat(1, 2);
				rotArray[6] = myRotMat(2, 0);
				rotArray[7] = myRotMat(2, 1);
				rotArray[8] = myRotMat(2, 2);

				if (fVerbose > 1) {
					std::cout << "RotMat NElements: "
							<< myRotMat.GetNoElements() << std::endl;

					std::cout << myRotMat(0, 0) << " ";
					std::cout << myRotMat(0, 1) << " ";
					std::cout << myRotMat(0, 2) << std::endl;
					std::cout << myRotMat(1, 0) << " ";
					std::cout << myRotMat(1, 1) << " ";
					std::cout << myRotMat(1, 2) << std::endl;
					std::cout << myRotMat(2, 0) << " ";
					std::cout << myRotMat(2, 1) << " ";
					std::cout << myRotMat(2, 2) << std::endl;
				}

				TGeoHMatrix geoTrans;
				geoTrans.SetRotation(rotArray);
				Double_t trans[3];
				trans[0] = myTube->GetPosition().X();
				trans[1] = myTube->GetPosition().Y();
				trans[2] = myTube->GetPosition().Z();
				geoTrans.SetTranslation(trans);

				myEveShape->SetTransMatrix(geoTrans);
				if (radius > 0) {
					if (myTube->GetWireDirection().Pt() > 0) {
						fListOfTiltedIsochrones->AddElement(myEveShape);
					}
					else {
						fListOfParallelIsochrones->AddElement(myEveShape);
					}
				}
			}
		}
		gEve->Redraw3D(kFALSE);
//		if (fListOfIsochrones) fListOfIsochrones->Delete();
//		if (fListOfTiltedIsochrones) fListOfTiltedIsochrones->Delete();
	}
}




// -------------------------------------------------------------------------
void PndSttIsochroneDraw::SetParContainers()
{
	  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
	  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

// -------------------------------------------------------------------------
void PndSttIsochroneDraw::Finish()
{

}
// -------------------------------------------------------------------------
void PndSttIsochroneDraw::Reset()
{

	   if (fListOfTiltedIsochrones != 0) {
	//		fListOfTiltedIsochrones->Reset();
//		   gEve->PreDeleteElement(fListOfTiltedIsochrones);
//			gEve->RemoveElement(fListOfTiltedIsochrones, fListOfIsochrones);
			fListOfTiltedIsochrones->DestroyElements();
			fListOfTiltedIsochrones->Destroy();
		}

	if (fListOfParallelIsochrones != 0){
//	   fListOfIsochrones->Reset();
//	   gEve->PreDeleteElement(fListOfParallelIsochrones);
//	   gEve->RemoveElement(fListOfParallelIsochrones, fListOfIsochrones);
	   fListOfParallelIsochrones->DestroyElements();
	   fListOfParallelIsochrones->Destroy();

   }
	if (fListOfIsochrones != 0){
//	   fListOfIsochrones->Reset();
//	   gEve->PreDeleteElement(fListOfIsochrones);
//	   gEve->RemoveElement(fListOfIsochrones, fEventManager);
	   fListOfIsochrones->Destroy();
   }

}


void PndSttIsochroneDraw::SetTimeWindowMinus(Double_t val)
{
  fTimeWindowMinus = val;
}

void PndSttIsochroneDraw::SetTimeWindowPlus(Double_t val)
{
  fTimeWindowPlus = val;
}


ClassImp(PndSttIsochroneDraw)


