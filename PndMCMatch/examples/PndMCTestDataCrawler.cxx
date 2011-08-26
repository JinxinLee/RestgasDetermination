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
#include "FairLinkedData.h"

#include "PndMCTrack.h"
#include "PndTrack.h"
#include "PndMCEntry.h"
#include "PndSdsMCPoint.h"
#include "PndSdsHit.h"

// -----   Default constructor   -------------------------------------------
PndMCTestDataCrawler::PndMCTestDataCrawler() :
	FairTask("Creates PndMC data crawler test"), fEventNr(0) {
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMCTestDataCrawler::~PndMCTestDataCrawler() {
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCTestDataCrawler::Init() {

	//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");

	fCrawler = new PndMCDataCrawler();
	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- PndMCTestDataCrawler::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	fInputData = (TClonesArray*) ioman->GetObject("MVDHitsPixel");
	ioman->GetObject("MVDPoint");
	ioman->GetObject("MVDPixelDigis");
	ioman->GetObject("MVDStripDigis");
	ioman->GetObject("MVDSortedPixelDigis");
	ioman->GetObject("MVDPixelClusterCand");
	ioman->GetObject("MVDStripClusterCand");
	ioman->GetObject("MVDHitsPixel");
	ioman->GetObject("MVDHitsStrip");
	ioman->GetObject("MVDRiemannTrackCand");
	ioman->GetObject("MVDDigiCorr");

	fTimeResHisto = new TH1D("fTimeResHisto", "fTimeResHisto", 601, -20, 40);

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
		PndSdsHit* hit = (PndSdsHit*)fInputData->At(i);
//		std::cout << "Hit MC EventTime: " << hit->GetTimeStamp() << std::endl;

		FairMultiLinkedData* linkData = (FairMultiLinkedData*)hit; //fInputData->At(i);
		FairMultiLinkedData result = fCrawler->GetInfo(FairMultiLinkedData(*linkData), "MVDPoint");
//		std::cout << "SourceHit: " << *linkData << std::endl;
//		std::cout << "result: " << result << std::endl;
		for (int j = 0; j < result.GetNLinks(); j++){
			if (fCrawler->GetEntry(result.GetLink(j)) != 0){
				TString className = fCrawler->GetEntry(result.GetLink(j))->ClassName();
				//std::cout << className.Data() << std::endl;
				if (className == "PndSdsMCPoint"){
					PndSdsMCPoint* myPoint = (PndSdsMCPoint*)fCrawler->GetEntry(result.GetLink(j));
					//std::cout << j << " : " << myPoint->GetTimeStamp() << std::endl;
					fTimeResHisto->Fill(hit->GetTimeStamp() - myPoint->GetTimeStamp());
					if ((hit->GetTimeStamp() - myPoint->GetTimeStamp()) > 10){
						std::cout << "Time Difference: " << (hit->GetTimeStamp() - myPoint->GetTimeStamp()) << std::endl;
						std::cout << "MCPoint: " << *myPoint << std::endl;
						std::cout << "MVDHit: " << *hit << std::endl;
					}
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
