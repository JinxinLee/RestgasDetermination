#include "PndSttSkewedCombineTask.h"

#include <iostream>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "FairEventHeader.h"

// PndMvd includes
#include "PndTrackCand.h"
#include "PndRiemannTrack.h"
#include "PndSttMapCreator.h"
#include "PndSttCellTrackFinder.h"

ClassImp(PndSttSkewedCombineTask)
;

void PndSttSkewedCombineTask::SetParContainers() {
	FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
	fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

//InitStatus PndSttSkewedCombineTask::ReInit() {
//	InitStatus stat = kSUCCESS;
//	return stat;
//}

// -----   Public method Init   --------------------------------------------
InitStatus PndSttSkewedCombineTask::Init() {
	FairRootManager* ioman = FairRootManager::Instance();

	if (!ioman) {
		std::cout << "-E- PndSttSkewedCombineTask::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	fEventHeader = (TClonesArray*) ioman->GetObject("EventHeader.");
	if (!fEventHeader) {
		std::cout << "-W-  PndSttSkewedCombineTask::Init: No EventHeader array! Needed for EventNumber"	<< endl;
		return kERROR;
	}

	// Get input array
//  if (FairRunAna::Instance()->IsTimeStamp()){
//	  if (fTimeBasedHitBranch.size() == 0){
//		  std::cout << "-W- PndSttSkewedCombineTask::Init: " << "No Branch Names given with AddHitBranch(TString branchName)! Standard BranchNames taken!" << std::endl;
//		  fTimeBasedHitBranch.push_back(FairTSBufferParameters("MVDHitsPixel", new TimeGap(), 10));
//	  }
//  }

	fSttHits = (TClonesArray*) FairRootManager::Instance()->GetObject("STTHit");

	PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
	fTubeArray = mapper->FillTubeArray();
	fStrawMap.GenerateStrawMap(fTubeArray);
	fGeometryMap = new PndSttGeometryMap(fTubeArray, 1);

	fCombinedSkewedHits = ioman->Register("STTCombinedSkewedHits", "FairHit", "Stt", kTRUE);


	std::cout << "-I- PndSttSkewedCombineTask: Initialisation successfull"
			<< std::endl;
	//fInitDone = kTRUE;
	return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndSttSkewedCombineTask::Exec(Option_t* opt) {

	FairEventHeader* myEventHeader = (FairEventHeader*) fEventHeader;
	int eventNumber = myEventHeader->GetMCEntryNumber();

//	if (fVerbose > 0) {
		std::cout << "====================Begin PndSttSkewedCombineTask::Exec=======================" << endl;

		std::cout << "Event #" << eventNumber << endl;
//	}



	FairRootManager *ioman = FairRootManager::Instance();

	std::map<int, PndSttHit*> tubeMap;
	for (int i = 0; i < fSttHits->GetEntriesFast(); i++){
		PndSttHit* myHit = (PndSttHit*)fSttHits->At(i);
		myHit->SetEntryNr(FairLink(-1, eventNumber, ioman->GetBranchId("STTHit"),i));
		tubeMap[myHit->GetTubeID()] = myHit;
	}

	for (int i = 0; i < fSttHits->GetEntriesFast(); i++){
		PndSttHit* myHit = (PndSttHit*)fSttHits->At(i);
		int strawRow = fStrawMap.GetRow(myHit->GetTubeID());
		std::cout << "PndSttSkewedCombineTask::Exec: TubeID " << myHit->GetTubeID() << " RowNr " << strawRow << std::endl;
		if (strawRow > 6 && strawRow < 16 && strawRow % 2 == 1) {
			TArrayI tmp = fGeometryMap->GetNeighboringsByMap(myHit->GetTubeID());
			for (int j = 0; j < tmp.GetSize(); j++){
				if (fStrawMap.GetRow(tmp.At(j)) == strawRow + 1) {
					if (tubeMap.find(tmp.At(j)) != tubeMap.end()) {
						TVector3 poca, pocaError;
						std::cout << "PndSttSkewedCombineTask::Exec: FoundNeighbourHit " << tubeMap[tmp.At(j)]->GetTubeID() << std::endl;

						double distance = fGeometryMap->CalculateStrawPoca(myHit, tubeMap[tmp.At(j)], poca);
						std::cout << "PndSttSkewedCombineTask::Exec: poca: " << poca.x() << "/" << poca.y() << "/" << poca.z() << " " << distance << std::endl;
						FairHit* myFairHit = new ((*fCombinedSkewedHits)[fCombinedSkewedHits->GetEntriesFast()]) FairHit(-1, poca, pocaError,-1);
						myFairHit->AddLink(myHit->GetEntryNr());
						myFairHit->AddLink(tubeMap[tmp.At(j)]->GetEntryNr());
					}
				}
			}
		}
	}
}

void PndSttSkewedCombineTask::FinishEvent() {
	fCombinedSkewedHits->Delete();
}



