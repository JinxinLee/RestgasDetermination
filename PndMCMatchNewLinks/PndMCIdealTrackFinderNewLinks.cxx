/*
 * PndMCIdealTrackFinderNewLinkss.cpp
 *
 *  Created on: Apr 12, 2010
 *      Author: stockman
 */

#include "PndMCIdealTrackFinderNewLinks.h"

#include "FairRootManager.h"
#include "FairMCPoint.h"
#include "PndTrackCand.h"

ClassImp(PndMCIdealTrackFinderNewLinks);

PndMCIdealTrackFinderNewLinks::PndMCIdealTrackFinderNewLinks() {
	// TODO Auto-generated constructor stub

}

PndMCIdealTrackFinderNewLinks::~PndMCIdealTrackFinderNewLinks() {
	// TODO Auto-generated destructor stub
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCIdealTrackFinderNewLinks::Init()
{

	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- PndMCTestHitCompare::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	if (fBranchNames.size() == 0){
		AddBranchName("MVDHitsPixel");
		AddBranchName("MVDHitsStrip");
		AddBranchName("STTHit");
		AddBranchName("GEMHit");
	}

	for (int i = 0; i < fBranchNames.size(); i++){
		if (ioman->GetObject(fBranchNames[i]) != 0){
			fBranchMap[fBranchNames[i]] = (TClonesArray*)ioman->GetObject(fBranchNames[i]);
		}
	}

  	fMCTrack = (TClonesArray*)ioman->GetObject("MCTrack");
  	ioman->GetObject("MVDPoint");

  	 fTrackCand = new TClonesArray("PndTrackCand");
  	 ioman->Register("IdealTrackCand", "MC", fTrackCand, kTRUE);

  	 return kSUCCESS;
}

void PndMCIdealTrackFinderNewLinks::Exec(Option_t* opt)
{
	fTrackCand->Delete();
	fTrackCandMap.clear();

	std::cout << "Event #" << FairRootManager::Instance()->GetEntryNr() << std::endl;
	CreateTrackCands();

	std::cout << "PndMCIdealTrackFinderNewLinks:Found Tracks:" << std::endl;
	for (std::map<FairLink, PndTrackCand>::iterator iter = fTrackCandMap.begin(); iter != fTrackCandMap.end(); iter++){
		PndTrackCand* myTrackCand = new((*fTrackCand)[fTrackCand->GetEntriesFast()]) PndTrackCand(iter->second);
		myTrackCand->setMcTrackId(iter->first.GetIndex());
		myTrackCand->AddLink(iter->first);
		myTrackCand->SetTimeStamp(FairRootManager::Instance()->GetEventTime());
		//std::cout << myTrackCand->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack")) << " : " << std::endl;
		myTrackCand->Print();
		//std::cout << *myTrackCand << std::endl;
	}
}

void PndMCIdealTrackFinderNewLinks::CreateTrackCands()
{
	fHitCount = 0;
	for (std::map<TString, TClonesArray*>::iterator iter = fBranchMap.begin(); iter != fBranchMap.end(); iter++){
		//std::cout << "AddLinks from Branch: " << iter->first << std::endl;
		for (int i = 0; i < iter->second->GetEntriesFast(); i++){
			FairMultiLinkedData_Interface* links = (FairMultiLinkedData_Interface*)iter->second->At(i);
			std::cout << *links << std::endl;
			FairMultiLinkedData mctracks = links->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack"));
			FairMultiLinkedData mvdpoints = links->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MVDPoint"));
			std::cout << "MvdPoints: " << mvdpoints << std::endl;
			for (int ipnt = 0; ipnt < mvdpoints.GetNLinks(); ipnt++){
				FairMCPoint *point = (FairMCPoint *) FairRootManager::Instance()->GetCloneOfLinkData(mvdpoints.GetLink(ipnt));
				if (point != 0)
					std::cout << "PointFound!" << std::endl;
				else
					std::cout << "Point not found!" << std::endl;
			}
			for (int trackIndex = 0; trackIndex < mctracks.GetNLinks(); trackIndex++){
				if (!fTrackCandMap.count(mctracks.GetLink(trackIndex))){
					fTrackCandMap[mctracks.GetLink(trackIndex)] = PndTrackCand();
					fTrackCandMap[mctracks.GetLink(trackIndex)].SetInsertHistory(kFALSE);
				}
				FairLink link(-1, FairRootManager::Instance()->GetEntryNr(),FairRootManager::Instance()->GetBranchId(iter->first),i);
				//std::cout << "CreateTrackCands " << mctracks.GetLink(trackIndex) << " : " << link << std::endl;
				fTrackCandMap[mctracks.GetLink(trackIndex)].SetInsertHistory(kFALSE);
				fTrackCandMap[mctracks.GetLink(trackIndex)].AddHit(link, fHitCount++);			//todo Rho is not properly calculated!
				fTrackCandMap[mctracks.GetLink(trackIndex)].AddLinks(*(links->GetPointerToLinks()));
			}
		}
	}
}
