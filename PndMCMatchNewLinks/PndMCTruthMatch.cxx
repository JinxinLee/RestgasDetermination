/*
 * PndIdealTrackFinders.cpp
 *
 *  Created on: Apr 12, 2010
 *      Author: stockman
 */

#include "PndMCTruthMatch.h"

#include "FairRootManager.h"
#include "FairMultiLinkedData.h"
#include "FairMultiLinkedData_Interface.h"

#include "PndMCTrack.h"

ClassImp(PndMCTruthMatch);

PndMCTruthMatch::PndMCTruthMatch() :
  fInBranchName("")
{
	// TODO Auto-generated constructor stub
}


PndMCTruthMatch::PndMCTruthMatch(TString inBranchName) :
  fInBranchName(inBranchName)
{
}

PndMCTruthMatch::~PndMCTruthMatch() {
	// TODO Auto-generated destructor stub
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCTruthMatch::Init()
{

	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- PndMCTruthMatch::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	if (fInBranchName == ""){
	    // Use hits of all tracking subsystems if nothing is given
		std::cout << "-E- PndMCTruthMatch::Init no InBranchName given" << std::endl;
	}

	fInBranch = (TClonesArray*)ioman->GetObject(fInBranchName);

  	fMCTrack = (TClonesArray*)ioman->GetObject("MCTrack");

  	return kSUCCESS;
}

void PndMCTruthMatch::Exec(Option_t* opt)
{
	std::cout << "------------ EventNr " << FairRootManager::Instance()->GetEntryNr() << " ------------" << std::endl;
	std::cout << FairRootManager::Instance()->GetBranchId(fInBranchName) << " : " << fInBranchName << " Entries: " << fInBranch->GetEntriesFast() << std::endl;

	for (int i = 0; i < fInBranch->GetEntriesFast(); i++)
	{
		FairMultiLinkedData_Interface* myData = (FairMultiLinkedData_Interface*)fInBranch->At(i);
		std::cout << i << " : " << *myData << std::endl;
		FairMultiLinkedData myLinks = myData->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack"));
		for (int trackIndex = 0; trackIndex < myLinks.GetNLinks(); trackIndex++)
		{
			PndMCTrack* myTrack = (PndMCTrack*)FairRootManager::Instance()->GetCloneOfLinkData(myLinks.GetLink(trackIndex));
			std::cout << myLinks.GetLink(trackIndex) << " : " << *myTrack;
		}
		std::cout << std::endl;
	}
}

