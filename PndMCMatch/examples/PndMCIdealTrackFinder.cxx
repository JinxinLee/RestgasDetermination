/*
 * PndMCIdealTrackFinder.cpp
 *
 *  Created on: Apr 12, 2010
 *      Author: stockman
 */

#include "PndMCIdealTrackFinder.h"

#include "FairRootManager.h"
#include "PndTrackCand.h"

ClassImp(PndMCIdealTrackFinder);

PndMCIdealTrackFinder::PndMCIdealTrackFinder() {
	// TODO Auto-generated constructor stub

}

PndMCIdealTrackFinder::~PndMCIdealTrackFinder() {
	// TODO Auto-generated destructor stub
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCIdealTrackFinder::Init()
{

	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- PndMCTestHitCompare::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

  	fMCMatch = (PndMCMatch*)ioman->GetObject("MCMatch");
  	fMCTrack = (TClonesArray*)ioman->GetObject("MCTrack");

  	 fTrackCand = new TClonesArray("PndTrackCand");
  	 ioman->Register("PndIdealTrackCand", "MC", fTrackCand, kTRUE);

  	 return kSUCCESS;
}

void PndMCIdealTrackFinder::Exec(Option_t* opt)
{
	//fMCMatch->CreateArtificialStage(kMCTrack, "", "");

	fMCMatch->CreateArtificialStage("MCTrack");
	PndMCResult myResult = fMCMatch->GetMCInfo("MCTrack", "GEMHit");
	std::cout << myResult;

	fTrackCand->Delete();

	for (int trackIndex = 0; trackIndex < myResult.GetNEntries(); trackIndex++){
		PndMCEntry myEntry = myResult.GetEntry(trackIndex);
		myEntry.RemoveType(FairRootManager::Instance()->GetBranchId("MCTrack"));
		if (myEntry.GetNLinks() > 0){
			PndTrackCand* myTrackCand = new((*fTrackCand)[fTrackCand->GetEntriesFast()]) PndTrackCand;
			for (int entryIndex = 0; entryIndex < myEntry.GetNLinks(); entryIndex++)
				if (myEntry.GetLink(entryIndex).GetType() > 0)
					myTrackCand->AddHit(myEntry.GetLink(entryIndex), entryIndex);
			myTrackCand->setMcTrackId(trackIndex);
			myTrackCand->SetTimeStamp(FairRootManager::Instance()->GetEventTime());
		}
	}

}
