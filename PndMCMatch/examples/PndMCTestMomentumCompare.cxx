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
	: FairTask("Creates PndMC test"), fEventNr(0)
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

  	fTrack = (TClonesArray*)ioman->GetObject("MVDRiemannTrackCand");
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
	FairRootManager* ioman = FairRootManager::Instance();
	PndMCResult myResult = fMCMatch->GetMCInfo("MVDRiemannTrackCand", "MCTrack");
	std::cout << myResult;

	std::cout << "----- Event " << fEventNr << " ------" << std::endl;
	fEventNr++;
	for (int i = 0; i < myResult.GetNEntries(); i++){
		PndMCEntry myLinks = myResult.GetMCLink(i);
		PndTrackCand* myTrack = (PndTrackCand*)fTrack->At(i);
		std::cout << "TrackMatch for Track " << i << std::endl;
		std::cout << "P: " << myTrack->getPosSeed().Mag() << std::endl;
		std::cout << "Belongs to: " << std::endl;
		for (int j = 0; j < myLinks.GetNLinks(); j++){
			if (myLinks.GetLink(j).GetType() == ioman->GetBranchId("MCTrack")){
				std::cout << "MCTrack " << myLinks.GetLink(j).GetIndex() << std::endl;
				if (myLinks.GetLink(j).GetIndex() < fMCTrack->GetEntries()){
					PndMCTrack* myMCTrack = (PndMCTrack*)fMCTrack->At(myLinks.GetLink(j).GetIndex());
					std::cout << "P: " << myMCTrack->GetMomentum().Mag() << " PID: " << myMCTrack->GetPdgCode() << std::endl;
					std::cout << "--------------------------------" << std::endl;
				}
				else std::cout << "Index out of bounds: Index : " << myLinks.GetLink(j).GetIndex() << " Bounds: " << fMCTrack->GetEntries() << std::endl;
			}
		}
		std::cout << std::endl;
	}
}

void PndMCTestMomentumCompare::Finish()
{
}


ClassImp(PndMCTestMomentumCompare);
