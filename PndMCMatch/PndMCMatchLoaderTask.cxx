// -------------------------------------------------------------------------
// -----                PndMCMatchLoaderTask source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "TVector3.h"


// framework includes
#include "FairRootManager.h"
#include "PndMCMatchLoaderTask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairMultiLinkedData.h"
#include "PndMCTrack.h"
// PndMvd includes
#include "PndRiemannTrack.h"
#include "PndTrackCand.h"

#include "PndDetectorList.h"

#include <vector>
#include <map>


// -----   Default constructor   -------------------------------------------
PndMCMatchLoaderTask::PndMCMatchLoaderTask() : FairTask("Creates PndMCMatch"), fEventNr(0)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMCMatchLoaderTask::~PndMCMatchLoaderTask()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCMatchLoaderTask::Init()
{
	fMCMatch = new PndMCMatch("PndMCMatch", "PndMCMatch");

	fMCMatch->InitStage("MVDPoint");
	  fMCMatch->InitStage("MVDStripDigis");
	  fMCMatch->InitStage("MVDPixelDigis");
	  fMCMatch->InitStage("MVDHitsPixel");
	  fMCMatch->InitStage("MVDHitsStrip");
	  fMCMatch->InitStage("MVDPixelClusterCand");
	  fMCMatch->InitStage("MVDStripClusterCand");

	  fMCMatch->InitStage("EmcCluster");
	  fMCMatch->InitStage("EmcBump");
	  fMCMatch->InitStage("EmcDigi");
	  fMCMatch->InitStage("EmcHit");
	  fMCMatch->InitStage("EmcPoint");
	  fMCMatch->InitStage("EmcRecoHit");
	  fMCMatch->InitStage("EmcSharedDigi");
	  fMCMatch->InitStage("EmcWaveform");



	  fMCMatch->InitStage("STTPoint");
	  fMCMatch->InitStage("STTHit");
	  fMCMatch->InitStage("SttHelixHit");
	  fMCMatch->InitStage("STTTrack");
	  fMCMatch->InitStage("STTTrackCand");

	  fMCMatch->InitStage("PndTpcPoint");
	  fMCMatch->InitStage("PndTpcPrimaryCluster");
	  fMCMatch->InitStage("PndTpcDriftedElectron");
	  fMCMatch->InitStage("PndTpcAvalanche");
	  fMCMatch->InitStage("PndTpcSignal");
	  fMCMatch->InitStage("PndTpcDigi");
	  fMCMatch->InitStage("PndTpcCluster");


	  fMCMatch->InitStage("GEMPoint");
	  fMCMatch->InitStage("GEMDigi");
	  fMCMatch->InitStage("GEMHit");

	  fMCMatch->InitStage("PndDchPoint");
	  fMCMatch->InitStage("PndDchDigi");
	  fMCMatch->InitStage("PndDchCylinderHit");

	  fMCMatch->InitStage("MdtPoint");
	  fMCMatch->InitStage("MdtHit");
	  fMCMatch->InitStage("MdtTrk");

	  fMCMatch->InitStage("LheHit");
	  fMCMatch->InitStage("LheCandidate");
	  fMCMatch->InitStage("LheTrack");
	  fMCMatch->InitStage("MVDIdealTrackCand");
	  fMCMatch->InitStage("MVDRiemannTrackCand");
	  //fMCMatch->InitStage("PndTrack");
	  fMCMatch->InitStage("LheGenTrack");
	  fMCMatch->InitStage("PidChargedCand");
	  fMCMatch->InitStage("PidNeutralCand");

  FairRootManager* ioman = FairRootManager::Instance();
  	if (!ioman) {
  		std::cout << "-E- PndMCMatchLoaderTask::Init: "
  				<< "RootManager not instantiated!" << std::endl;
  		return kFATAL;
  	}

	fMCLink = (TClonesArray*)ioman->GetObject("MCLink");
	ioman->Register("MCMatch", "MCMatch", fMCMatch, kFALSE);

  return kSUCCESS;
}


// -------------------------------------------------------------------------
void PndMCMatchLoaderTask::SetParContainers()
{
  // Get Base Container
//  FairRun* ana = FairRun::Instance();
//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void PndMCMatchLoaderTask::Exec(Option_t* opt)
{
/*	for (int trackIndex = 0; trackIndex < fTrack->GetEntriesFast(); trackIndex++){
		PndTrack* myTrack = (PndTrack*)fTrack->At(trackIndex);
		fMCMatch->AddElement(kTrack, trackIndex, kTrackCand, myTrack->GetRefIndex());
	}
*/

	if (!fMCLink) Fatal("Exec", "No fMCLink");
//	fMCLinkDet->Delete();
//	fMCLinkHit->Delete();
	fMCMatch->ClearMCList();

	fMCMatch->LoadInMCLists(fMCLink);
	fMCMatch->CreateArtificialStage("MCTrack");

	fMCMatch->Print();
	std::cout << std::endl;
}

void PndMCMatchLoaderTask::Finish()
{
}


ClassImp(PndMCMatchLoaderTask);
