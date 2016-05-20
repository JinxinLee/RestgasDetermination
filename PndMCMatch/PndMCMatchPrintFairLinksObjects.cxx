// -------------------------------------------------------------------------
// -----                PndMCMatchPrintFairLinksObjects source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>
#include <exception>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "TChain.h"
#include "TChainElement.h"

// framework includes
#include "FairRootManager.h"
#include "PndMCMatchPrintFairLinksObjects.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairMultiLinkedData_Interface.h"

#include "PndDetectorList.h"
#include "PndMCEntry.h"


// -----   Default constructor   -------------------------------------------
PndMCMatchPrintFairLinksObjects::PndMCMatchPrintFairLinksObjects() : FairTask("Creates PndMCMatch"), fSelectedBranches(new TList())
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMCMatchPrintFairLinksObjects::~PndMCMatchPrintFairLinksObjects()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCMatchPrintFairLinksObjects::Init()
{
	  FairRootManager* ioman = FairRootManager::Instance();
	  	if (!ioman) {
	  		std::cout << "-E- PndMCMatchPrintFairLinksObjects::Init: "
	  				<< "RootManager not instantiated!" << std::endl;
	  		return kFATAL;
	  	}

	  	TList* branchNames;
	  	if (fSelectedBranches->GetEntries() > 0){
	  		branchNames = fSelectedBranches;
	  	} else {
	  		branchNames = ioman->GetBranchNameList();
	  	}

	  	InitBranchList(branchNames);

	  	PrintBranchNameList(ioman->GetBranchNameList());

  return kSUCCESS;
}


void PndMCMatchPrintFairLinksObjects::InitBranchList(TList* branches)
{
	FairRootManager* ioman = FairRootManager::Instance();
	for (int i = 0; i < branches->GetEntries(); i++) {
		TObjString* branchName = (TObjString*) branches->At(i);
		if (branchName->String().Contains("_link")) continue;
		if (branchName->String().Contains("MCTrack")) continue;
		if (branchName->String().Contains(".")) continue;
		if (branchName->String().Contains("GeoTracks")) continue;
		if (branchName->String().Contains("Header")) continue;
		if (branchName->String().Contains("Info")) continue;
		if (branchName->String().Contains("ID")) continue;
		if (branchName->String().Contains("PidAlgo")) continue;
		if (branchName->String().Contains("Riemann")) continue;

		std::cout << "PndMCMatchPrintFairLinksObjects::Init() branches: " << branchName->String() << std::endl;
		if ((TClonesArray*)ioman->GetObject(branchName->String()) != 0){
			fBranches[ioman->GetBranchId(branchName->String())] = (TClonesArray*)ioman->GetObject(branchName->String());
		} else {
			std::cout << "-E- PndMCMatchPrintFairLinksObjects " << branchName->String().Data() << " is not a valid branch name!" << std::endl;
		}
	}
}


void PndMCMatchPrintFairLinksObjects::PrintBranchNameList(TList* branches)
{
	std::cout << "-I- PndMCMatchPrintFairLinksObjects Branches:" << std::endl;

	for (int i = 0; i < branches->GetEntries(); i++) {
		TObjString* branchName = (TObjString*) branches->At(i);
		std::cout << i << " : " << branchName->String().Data() << std::endl;
	}
	std::cout << std::endl;
}

// -------------------------------------------------------------------------
void PndMCMatchPrintFairLinksObjects::SetParContainers()
{
  // Get Base Container
//  FairRun* ana = FairRun::Instance();
//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void PndMCMatchPrintFairLinksObjects::Exec(Option_t* opt)
{
	std::cout << std::endl << "--------------------- Event " << FairRootManager::Instance()->GetEntryNr() <<
			" at " << FairRootManager::Instance()->GetEventTime() << " ns ----------------------" << std::endl;
	for (std::map<Int_t, TClonesArray*>::iterator iter = fBranches.begin(); iter != fBranches.end(); iter++){
		std::cout << std::endl << iter->first << " : " << FairRootManager::Instance()->GetBranchName(iter->first) << " Entries: " << iter->second->GetEntriesFast() << std::endl;
		for (int i = 0; i < iter->second->GetEntriesFast(); i++){
			FairMultiLinkedData_Interface* myLinks = (FairMultiLinkedData_Interface*)iter->second->At(i);
			if (myLinks->GetPointerToLinks() != 0){
				std::cout << i << " : " << *myLinks << std::endl << std::endl;
			}
		}
	}
}

void PndMCMatchPrintFairLinksObjects::Finish()
{
}


ClassImp(PndMCMatchPrintFairLinksObjects);
