// -------------------------------------------------------------------------
// -----                PndMCTestMomentumCompare source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
#include "PndMCTestMomentumCompare.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairLinkedData.h"

#include "PndMCTrack.h"
#include "PndTrack.h"
#include "PndMCEntry.h"


// -----   Default constructor   -------------------------------------------
PndMCTestMomentumCompare::PndMCTestMomentumCompare()
	: FairTask("Creates PndMC test")
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMCTestMomentumCompare::~PndMCTestMomentumCompare()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCTestMomentumCompare::Init()
{


//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");



  FairRootManager* ioman = FairRootManager::Instance();
  	if (!ioman) {
  		std::cout << "-E- PndMCTestMomentumCompare::Init: "
  				<< "RootManager not instantiated!" << std::endl;
  		return kFATAL;
  	}

  	fMCMatch = (PndMCMatch*)ioman->GetObject("MCMatch");

  	fTrack = (TClonesArray*)ioman->GetObject("LheGenTrack");
  	fMCTrack = (TClonesArray*)ioman->GetObject("MCTrack");

	std::cout << "-I- PndMCTestMomentumCompare::Init: Initialization successfull" << std::endl;


  return kSUCCESS;
}


// -------------------------------------------------------------------------
void PndMCTestMomentumCompare::SetParContainers()
{
  // Get Base Container
//  FairRun* ana = FairRun::Instance();
//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void PndMCTestMomentumCompare::Exec(Option_t* opt)
{
	//fMCMatch->CreateArtificialStage(kMCTrack, "", "");

	PndMCResult myResult = fMCMatch->GetMCInfo(kTrack, kMCTrack);
	std::cout << myResult;
	for (int i = 0; i < myResult.GetNEntries(); i++){
		PndMCEntry myLinks = myResult.GetMCLink(i);
		PndTrack* myTrack = (PndTrack*)fTrack->At(i);
		std::cout << "TrackMatch for Track " << i << std::endl;
		//myTrack->Print();
		std::cout << "P: " << myTrack->GetParamFirst().GetSDMomentum().Mag() << std::endl;
		std::cout << "Belongs to: " << std::endl;
		for (int j = 0; j < myLinks.GetNLinks(); j++){
			if (myLinks.GetLink(j).GetType() == kMCTrack){
				std::cout << "MCTrack " << myLinks.GetLink(j).GetIndex() << std::endl;
				PndMCTrack* myMCTrack = (PndMCTrack*)fMCTrack->At(myLinks.GetLink(j).GetIndex());
				//myMCTrack->Print(myLinks.GetFairLink(j).GetIndex());
				std::cout << "P: " << myMCTrack->GetMomentum().Mag() << " PID: " << myMCTrack->GetPdgCode() << std::endl;
				std::cout << "--------------------------------" << std::endl;
			}
		}
		std::cout << std::endl;
	}
}

void PndMCTestMomentumCompare::Finish()
{
}


ClassImp(PndMCTestMomentumCompare);
