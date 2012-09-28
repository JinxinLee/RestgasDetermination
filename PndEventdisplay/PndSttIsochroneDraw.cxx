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
#include "FairRuntimeDb.h"
#include <string>
#include <sstream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndSttIsochroneDraw::PndSttIsochroneDraw(Bool_t propagate)
   : fTimeWindowPlus(0.), fTimeWindowMinus(0.), fStartTime(-1), fUseEventTime(kTRUE),
	 fSttHitArray(0), fSttTubeArray(0), fSttParameters(0), fEventManager(0), fListOfIsochrones(0),
	 fStartFunctor(0), fStopFunctor(0)
{
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
PndSttIsochroneDraw::PndSttIsochroneDraw(const char* name, Bool_t propagate, Int_t iVerbose)
  : FairTask(name, iVerbose), fTimeWindowPlus(0.), fTimeWindowMinus(0.), fStartTime(-1), fUseEventTime(kTRUE),
	 fSttHitArray(0), fSttTubeArray(0), fSttParameters(0), fEventManager(0), fListOfIsochrones(0),
	 fStartFunctor(0), fStopFunctor(0)
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
		//fListOfTracks->DestroyElements();

		gEve->AddElement(fListOfIsochrones, fEventManager);

	if (FairRunAna::Instance()->IsTimeStamp()) {
		std::cout << "Inside TimeStamp" << std::endl;
		Double_t eventTime = FairRootManager::Instance()->GetEventTime();
		if (fUseEventTime) {
			fStartTime = eventTime - fTimeWindowMinus;
		}
		cout << "EventTime: " << eventTime << " TimeWindow: " << fStartTime
				<< " - " << eventTime + fTimeWindowPlus << std::endl;

		fSttHitArray = FairRootManager::Instance()->GetData(GetName(),
				fStartFunctor, fStartTime, fStopFunctor,
				eventTime + fTimeWindowPlus); //FairRootManager::Instance()->GetEventTime() +
		std::cout << fStartTime << " " << eventTime + fTimeWindowPlus << " FoundTracks: " << fSttHitArray->GetEntriesFast() << std::endl;
	}

		std::cout << "SttHitArray: " << fSttHitArray->GetEntriesFast() << std::endl;

		for (Int_t i = 0; i < fSttHitArray->GetEntriesFast(); i++) {
			if (fVerbose > 2)
				cout << "PndSttIsochroneDraw::Exec " << i << endl;

			PndSttHit* myHit = (PndSttHit*) fSttHitArray->At(i);
			std::cout << "STTHits: " << myHit->GetX() << " " << myHit->GetY() << " " << myHit->GetZ() << std::endl;
			if (0 == myHit) {
				std::cout << "-E- PndSttIsochroneDraw Hit does not exist! " << i
						<< std::endl;
				continue;
			}
			if (myHit->GetTubeID() < fSttTubeArray->GetEntriesFast()) {
				PndSttTube* myTube = (PndSttTube*)fSttTubeArray->At(myHit->GetTubeID());

				if (0 == myTube) {
					std::cout << "-E- PndSttIsochroneDraw Tube does not exist! "
							<< myHit->GetTubeID() << std::endl;
					continue;
				}


//				std::cout << "Tube: " << myTube->GetHalfLength() << std::endl;


				Double_t tubeLengthHalf = myTube->GetHalfLength();
				Double_t radius = myHit->GetIsochrone();
				Double_t radiusError = myHit->GetIsochroneError();
//				std::cout << "RadiusError " << radiusError << std::endl;

				TEveGeoShape* myEveShape;
				myEveShape = new TEveGeoShape("SttTube");
				myEveShape->SetShape(new TGeoTube(radius-radiusError, radius+radiusError, tubeLengthHalf));
//				std::cout << "Wire direction: " << myTube->GetWireDirection().X() << " "  << myTube->GetWireDirection().Y() << " "  << myTube->GetWireDirection().Z() << std::endl;
				if (myTube->GetWireDirection().Pt() > 0){
					myEveShape->SetMainColor(kYellow);
					myEveShape->SetMainTransparency(80);
				} else {
					myEveShape->SetMainColor(kCyan);
				}
				TMatrixT<double> myRotMat = myTube->GetRotationMatrix();
				Double_t rotArray[9];
				rotArray[0] = myRotMat(0,0);
				rotArray[1] = myRotMat(0,1);
				rotArray[2] = myRotMat(0,2);
				rotArray[3] = myRotMat(1,0);
				rotArray[4] = myRotMat(1,1);
				rotArray[5] = myRotMat(1,2);
				rotArray[6] = myRotMat(2,0);
				rotArray[7] = myRotMat(2,1);
				rotArray[8] = myRotMat(2,2);

				if (fVerbose > 1) {
					std::cout << "RotMat NElements: " << myRotMat.GetNoElements() << std::endl;

					std::cout << myRotMat(0,0) << " ";
					std::cout << myRotMat(0,1) << " ";
					std::cout << myRotMat(0,2) << std::endl;
					std::cout << myRotMat(1,0) << " ";
					std::cout << myRotMat(1,1) << " ";
					std::cout << myRotMat(1,2) << std::endl;
					std::cout << myRotMat(2,0) << " ";
					std::cout << myRotMat(2,1) << " ";
					std::cout << myRotMat(2,2) << std::endl;
				}

				TGeoHMatrix geoTrans;
				geoTrans.SetRotation(rotArray);
				Double_t trans[3];
				trans[0] = myTube->GetPosition().X();
				trans[1] = myTube->GetPosition().Y();
				trans[2] = myTube->GetPosition().Z();
				geoTrans.SetTranslation(trans);

				myEveShape->SetTransMatrix(geoTrans);

				fListOfIsochrones->AddElement(myEveShape);

			}
		}
		gEve->Redraw3D(kFALSE);
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
   if (fListOfIsochrones != 0){
	   gEve->RemoveElement(fListOfIsochrones, fEventManager);
	   //delete(fListOfTracks);
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


