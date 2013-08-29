#include "PndSttCellTrackFinderTask.h"

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

using std::cout;
using std::endl;

ClassImp(PndSttCellTrackFinderTask)
;

void PndSttCellTrackFinderTask::SetParContainers() {
	FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
	fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

//InitStatus PndSttCellTrackFinderTask::ReInit() {
//	InitStatus stat = kSUCCESS;
//	return stat;
//}

// -----   Public method Init   --------------------------------------------
InitStatus PndSttCellTrackFinderTask::Init() {
	FairRootManager* ioman = FairRootManager::Instance();

	if (!ioman) {
		std::cout << "-E- PndSttCellTrackFinderTask::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	fEventHeader = (TClonesArray*) ioman->GetObject("EventHeader.");
	if (!fEventHeader) {
		cout
				<< "-W-  PndJetteAnalysisTask::Init: No EventHeader array! Needed for EventNumber"
				<< endl;
		return kERROR;
	}

	// Get input array
//  if (FairRunAna::Instance()->IsTimeStamp()){
//	  if (fTimeBasedHitBranch.size() == 0){
//		  std::cout << "-W- PndSttCellTrackFinderTask::Init: " << "No Branch Names given with AddHitBranch(TString branchName)! Standard BranchNames taken!" << std::endl;
//		  fTimeBasedHitBranch.push_back(FairTSBufferParameters("MVDHitsPixel", new TimeGap(), 10));
//	  }
//  }
	if (fHitBranch.size() == 0) {
		std::cout << "-W- PndSttCellTrackFinderTask::Init: "
				<< "No Branch Names given with AddHitBranch(TString branchName)! Standard BranchNames taken!"
				<< std::endl;
		fHitBranch.push_back("STTHit");
	}

	for (int i = 0; i < (int) fHitBranch.size(); i++) {
		InitHitArray(fHitBranch[i]);
	}

	PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
	fTubeArray = mapper->FillTubeArray();

//	fSttStrawMap = new PndSttStrawMap();
//	fSttStrawMap->GenerateStrawMap(fTubeArray);
//	fSttGeometryMap = new PndSttGeometryMap(fTubeArray, 1);

	fTrackFinder.SetSttTubeArray(fTubeArray);
	fTrackFinder.SetCalcFirstTrackletInf(fAnalyseSteps);

	fFirstTrackCandArray = new TClonesArray("PndTrackCand");
	ioman->Register("FirstTrackCand", "STT", fFirstTrackCandArray,
			fPersistence);

	fFirstRiemannTrackArray = new TClonesArray("PndRiemannTrack");
	ioman->Register("FirstRiemannTrack", "STT", fFirstRiemannTrackArray,
			fPersistence);

	fCombiTrackCandArray = new TClonesArray("PndTrackCand");
	ioman->Register("CombiTrackCand", "STT", fCombiTrackCandArray,
			fPersistence);

	fCombiRiemannTrackArray = new TClonesArray("PndRiemannTrack");
	ioman->Register("CombiRiemannTrack", "STT", fCombiRiemannTrackArray,
			fPersistence);

	std::cout << "-I- PndSttCellTrackFinderTask: Initialisation successfull"
			<< std::endl;
	//fInitDone = kTRUE;
	return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndSttCellTrackFinderTask::Exec(Option_t* opt) {

	FairEventHeader* myEventHeader = (FairEventHeader*) fEventHeader;
	int eventNumber = myEventHeader->GetMCEntryNumber();

	if (fVerbose > 0) {
		cout
				<< "====================Begin PndSttCellTrackFinderTask::Exec======================="
				<< endl;

		cout << "Event #" << eventNumber << endl;
	}

	// Reset output array
	if (!fFirstTrackCandArray)
		Fatal("Exec", "No trackCandArray");

//	PndSttCellTrackFinder trackFinder;
//	trackFinder.SetStrawMap(fSttStrawMap);
//	trackFinder.SetGeometryMap(fSttGeometryMap);
//	trackFinder.SetSttTubeArray(fTubeArray);
//	trackFinder.SetVerbose(fVerbose);
//	trackFinder.SetCalcFirstTrackletInf(fAnalyseSteps);
//	trackFinder.SetStrawMap(fSttStrawMap);
//	trackFinder.SetGeometryMap(fSttGeometryMap);
//	trackFinder.SetSttTubeArray(fTubeArray);
	fTrackFinder.Reset();

	//FillHitArray();

	FairRootManager *ioman = FairRootManager::Instance();

	// std::cout << std::endl;
//  std::cout << "------------- event " << fEventNr << "----------------" << std::endl;

	for (int i = 0; i < (int) fHitBranch.size(); i++) {
		fTrackFinder.AddHits(fHitArray[i], ioman->GetBranchId(fHitBranch[i]));
	}

	fTrackFinder.FindTracks();

	if (fAnalyseSteps) {
		for (int i = 0; i < fTrackFinder.NumFirstTrackCands(); i++) {
			PndTrackCand* myCand =
					new ((*fFirstTrackCandArray)[i]) PndTrackCand(
							fTrackFinder.GetFirstTrackCand(i));
		}

//		for (int i = 0; i < fTrackFinder.NumFirstRiemannTracks(); ++i) {
//			PndRiemannTrack* myTrack =
//					new ((*fFirstRiemannTrackArray)[i]) PndRiemannTrack(
//							fTrackFinder.GetFirstRiemannTrack(i));
//		}
	}

	for (int i = 0; i < fTrackFinder.NumCombinedTracks(); ++i) {
		PndTrackCand* myCand = new ((*fCombiTrackCandArray)[i]) PndTrackCand(
				fTrackFinder.GetCombiTrackCand(i));

	}

//	for (int i = 0; i < fTrackFinder.NumCombinedRiemannTracks(); ++i) {
//
//		PndRiemannTrack* myRiemannTrack =
//				new ((*fCombiRiemannTrackArray)[i]) PndRiemannTrack(
//						fTrackFinder.GetCombiRiemannTrack(i));
//	}

	if (fVerbose > 0)
		cout << "#FirstTracklets: " << fTrackFinder.NumFirstTrackCands()
				<< ", #CombinedTracklets: " << fTrackFinder.NumCombinedTracks()
				<< endl;

	fFirstTrackCandArray->Sort();
	fCombiTrackCandArray->Sort();

}

void PndSttCellTrackFinderTask::FinishEvent() {
	fFirstTrackCandArray->Delete();
	fFirstRiemannTrackArray->Delete();
	fCombiTrackCandArray->Delete();
	fCombiRiemannTrackArray->Delete();

}

void PndSttCellTrackFinderTask::InitHitArray(TString branchName) {
	TClonesArray* tempArray =
			(TClonesArray*) FairRootManager::Instance()->GetObject(branchName);
	if (tempArray == 0) {
		std::cout << "-W- PndSttCellTrackFinderTask::Init: "
				<< "No hitArray for BranchName " << branchName.Data()
				<< std::endl;
	}
	fHitArray.push_back(tempArray);
}

//void PndSttCellTrackFinderTask::FillHitArray() {
//	Double_t eventTime = -1;
//	if (FairRunAna::Instance()->IsTimeStamp()){
//		fHitArray[0]->Delete();
//		fHitArray[1]->Delete();
//
//		fHitArray[0] = FairRootManager::Instance()->GetData("MVDHitsPixel", fTimeGapFunctor, 10); //FairRootManager::Instance()->GetEventTime() +
//		std::cout << "PixelHits: " << fHitArray[0]->GetEntriesFast() << std::endl;
//		if (fHitArray[0]->GetEntriesFast() > 1){
//			FairTimeStamp* data = (FairTimeStamp*)fHitArray[0]->At(1);
//			eventTime = data->GetTimeStamp();
//			std::cout << "EventTime: " << eventTime << std::endl;
//			fHitArray[1] = FairRootManager::Instance()->GetData("MVDHitsStrip", fStopFunctor, eventTime - 10, fStopFunctor, eventTime + 10);
//			std::cout << "StripHits: " << fHitArray[1]->GetEntriesFast() << std::endl;
//		}
//	}
//}

//void PndSttCellTrackFinderTask::AddHitBranch(TString branchName) {
//	if (fInitDone == kFALSE)
//		fHitBranch.push_back(branchName);
//	else
//		std::cout
//				<< "-W- AddHitBranch has to be called before the Init() of the task!"
//				<< std::endl;
//}

