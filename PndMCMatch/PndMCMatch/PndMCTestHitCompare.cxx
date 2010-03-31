// -------------------------------------------------------------------------
// -----                PndMCTestHitCompare source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
#include "PndMCTestHitCompare.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairLinkedData.h"

#include "PndMvdMCPoint.h"
#include "PndMvdHit.h"
#include "PndMCEntry.h"

#include "PndDetectorList.h"


// -----   Default constructor   -------------------------------------------
PndMCTestHitCompare::PndMCTestHitCompare()
	: FairTask("Creates PndMC test")
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMCTestHitCompare::~PndMCTestHitCompare()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCTestHitCompare::Init()
{


//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");



  FairRootManager* ioman = FairRootManager::Instance();
  	if (!ioman) {
  		std::cout << "-E- PndMCTestHitCompare::Init: "
  				<< "RootManager not instantiated!" << std::endl;
  		return kFATAL;
  	}

  	fMCMatch = (PndMCMatch*)ioman->GetObject("MCMatch");

  	fStripHit = (TClonesArray*)ioman->GetObject("MVDHitsStrip");
  	fMCPoint = (TClonesArray*)ioman->GetObject("MVDPoint");

	std::cout << "-I- PndMCTestHitCompare::Init: Initialization successfull" << std::endl;


  return kSUCCESS;
}


// -------------------------------------------------------------------------
void PndMCTestHitCompare::SetParContainers()
{
  // Get Base Container
//  FairRun* ana = FairRun::Instance();
//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void PndMCTestHitCompare::Exec(Option_t* opt)
{
	//fMCMatch->CreateArtificialStage(kMCTrack, "", "");

	PndMCResult myResult = fMCMatch->GetMCInfo(kMVDHitsStrip, kMVDPoint);
	std::cout << myResult;
	for (int i = 0; i < myResult.GetNEntries(); i++){
		PndMCEntry myLinks = myResult.GetMCLink(i);
		PndMvdHit* myHit = (PndMvdHit*)fStripHit->At(i);

		std::cout << *myHit;

		for (int j = 0; j < myLinks.GetNLinks(); j++){
			if (myLinks.GetLink(j).GetType() == kMVDPoint){
				std::cout << "MCPoint " << myLinks.GetLink(j).GetIndex() << std::endl;
				PndMvdMCPoint* myMCPoint = (PndMvdMCPoint*)fMCPoint->At(myLinks.GetLink(j).GetIndex());
				//myMCTrack->Print(myLinks.GetFairLink(j).GetIndex());
				std::cout << *myMCPoint;
				std::cout << "--------------------------------" << std::endl;
			}
		}
		std::cout << std::endl;
	}
}

void PndMCTestHitCompare::Finish()
{
}


ClassImp(PndMCTestHitCompare);
