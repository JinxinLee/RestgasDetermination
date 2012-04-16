// -------------------------------------------------------------------------
// -----                PndMCTestDataCrawler source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
#include "PndMCTestDataCrawler.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairMultiLinkedData.h"
#include "FairEventHeader.h"

#include "PndMCTrack.h"
#include "PndTrack.h"
#include "PndMCEntry.h"
#include "PndSdsMCPoint.h"
#include "PndSdsHit.h"

// -----   Default constructor   -------------------------------------------
PndMCTestDataCrawler::PndMCTestDataCrawler() :
	FairTask("Creates PndMC data crawler test"), fEventNr(0), fStartBranch(""), fStopBranch(""), fStartVal(-20), fStopVal(40), fBins(601) {
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMCTestDataCrawler::~PndMCTestDataCrawler() {
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCTestDataCrawler::Init() {

	//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");

	fCrawler = new PndMCDataCrawler();
	fCrawler->Init();
	fCrawler->SetStoreIntermediate(kFALSE);

	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- PndMCTestDataCrawler::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	fInputData = (TClonesArray*) ioman->GetObject(fStartBranch.Data());

	if (fInputData == 0){
		std::cout << "-E- PndMCTestDataCrawler::Init() no InputData for " << fStartBranch.Data() << std::endl;
		return kFATAL;
	}


	fTimeResHisto = new TH1D("fTimeResHisto", "fTimeResHisto", fBins, fStartVal, fStopVal);

	TTree* dataTree = ioman->GetInTree();

	fEventHeaderBranch = dataTree->GetBranch("EventHeader.");

	if (fEventHeaderBranch == 0){
		std::cout << "-E- PndMCTestDataCrawler::Init() no EventHeaderBranch" << std::endl;
		return kFATAL;
	}

	std::cout
			<< "-I- PndMCTestDataCrawler::Init: Initialization successfull"
			<< std::endl;

	return kSUCCESS;
}

// -------------------------------------------------------------------------
void PndMCTestDataCrawler::SetParContainers() {
	// Get Base Container
	//  FairRun* ana = FairRun::Instance();
	//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}

// -----   Public method Exec   --------------------------------------------
void PndMCTestDataCrawler::Exec(Option_t* opt) {
	std::cout << "PndMCTestDataCrawler::Exec eventNr: " << fEventNr << std::endl;
	for (int i = 0; i < fInputData->GetEntriesFast(); i++){
		FairTimeStamp* hit = (FairTimeStamp*)fInputData->At(i);
		std::cout << std::endl;
		std::cout << "HitTime: " << hit->GetTimeStamp() << std::endl;

		FairMultiLinkedData* linkData = (FairMultiLinkedData*)hit; //fInputData->At(i);
		FairMultiLinkedData result = fCrawler->GetInfo(FairMultiLinkedData(*linkData), fStopBranch.Data());

		std::cout << "StartLinks: " << *linkData << std::endl;
		std::cout << "result: " << result << std::endl;

		FairEventHeader* evtHeader;

		for (int j = 0; j < result.GetNLinks(); j++){
//			std::cout << "BranchName of Result: " << FairRootManager::Instance()->GetBranchName(result.GetLink(j).GetType()) << std::endl;
			if (result.GetNLinks() == 1){
				TObject* data = fCrawler->GetEntry(result.GetLink(j));
				if ( data != 0){
					if (result.GetLink(j).GetType() == FairRootManager::Instance()->GetBranchId("EventHeader.")){
						 evtHeader = (FairEventHeader*)data;
					}
					else {
						Int_t entryNr = result.GetLink(j).GetEntry();
						TObject* timeArray = (TClonesArray*)FairRootManager::Instance()->GetObject("EventHeader.");
						fEventHeaderBranch->GetEntry(entryNr);
						evtHeader = (FairEventHeader*)timeArray;
					}

	//				PndSdsMCPoint* myPoint = (PndSdsMCPoint*)fCrawler->GetEntry(result.GetLink(j));
	//				//std::cout << j << " : " << myPoint->GetTimeStamp() << std::endl;
					fTimeResHisto->Fill(hit->GetTimeStamp() - evtHeader->GetEventTime());
//					if ((hit->GetTimeStamp() - evtHeader->GetEventTime()) > 10){
//						std::cout << "TimeDifference: " << hit->GetTimeStamp() << " - " << evtHeader->GetEventTime() << " = " << hit->GetTimeStamp() - evtHeader->GetEventTime() << std::endl;
//						std::cout << *linkData << std::endl;
//						for (int k = 0; k < linkData->GetNLinks(); k++){
//							if (linkData->GetLink(k).GetType() != FairRootManager::Instance()->GetBranchId("EventHeader.")){
//								PndSdsMCPoint* myTSData = (PndSdsMCPoint*)FairRootManager::Instance()->GetLinkData(linkData->GetLink(k));
//								if (myTSData)
//									std::cout << *myTSData << std::endl;
//							}
//						}
//					}
				}
			}
		}

	}
	fEventNr++;
}

void PndMCTestDataCrawler::Finish() {
	fTimeResHisto->Draw();
}

ClassImp( PndMCTestDataCrawler);
