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
#include "FairLinkedData.h"
#include "PndMCTrack.h"
// PndMvd includes
#include "PndMvdMCPoint.h"
#include "PndMvdDigiPixel.h"
#include "PndMvdDigiStrip.h"
#include "PndMvdHit.h"
#include "PndMvdCluster.h"
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

//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");
  fMCMatch->InitStage(kMVDPoint, "", "MVDPoint");
  fMCMatch->InitStage(kMVDDigiStrip, "", "MVDStripDigis");
  fMCMatch->InitStage(kMVDDigiPixel, "", "MVDPixelDigis");
  fMCMatch->InitStage(kMVDHitsPixel, "", "MVDHitsPixel");
  fMCMatch->InitStage(kMVDHitsStrip, "", "MVDHitsStrip");
  fMCMatch->InitStage(kMVDClusterPixel, "", "MVDPixelClusterCand");
  fMCMatch->InitStage(kMVDClusterStrip, "", "MVDStripClusterCand");

  fMCMatch->InitStage(kSttPoint, "", "STTPoint");
  fMCMatch->InitStage(kSttHit, "", "STTHit");
  fMCMatch->InitStage(kSttHelixHit, "", "SttHelixHit");
  fMCMatch->InitStage(kSttTrack, "", "STTTrack");
  fMCMatch->InitStage(kSttTrackCand, "", "STTTrackCand");

  fMCMatch->InitStage(kTpcPoint, "", "PndTpcPoint");
  fMCMatch->InitStage(kTpcPrimaryCluster, "", "PndTpcPrimaryCluster");
  fMCMatch->InitStage(kTpcDriftedElectron, "", "PndTpcDriftedElectron");
  fMCMatch->InitStage(kTpcAvalanche, "", "PndTpcAvalanche");
  fMCMatch->InitStage(kTpcSignal, "", "PndTpcSignal");
  fMCMatch->InitStage(kTpcDigi, "", "PndTpcDigi");
  fMCMatch->InitStage(kTpcCluster, "", "PndTpcCluster");

  fMCMatch->InitStage(kGemPoint, "", "GEMPoint");
  fMCMatch->InitStage(kGemDigi, "", "GEMDigi");
  fMCMatch->InitStage(kGemHit, "", "GEMHit");

 // fMCMatch->InitStage(kEmcPoint,"","EmcPoint");
  fMCMatch->InitStage(kEmcHit, "", "EmcHit");
  fMCMatch->InitStage(kEmcDigi, "", "EmcDigi");
  fMCMatch->InitStage(kEmcCluster, "", "EmcCluster");
  fMCMatch->InitStage(kEmcBump, "", "EmcBump");
  fMCMatch->InitStage(kEmcRecoHit, "", "EmcRecoHit");

  //fMCMatch->InitStage(kLheHit, "", "LheHit");
  fMCMatch->InitStage(kLheCand, "", "LheCandidate");
  fMCMatch->InitStage(kLheTrack,"", "LheTrack");
  //fMCMatch->InitStage(kTrackCand,"","MVDIdealTrackCand");
  //fMCMatch->InitStage(kTrackCand,"","MVDRiemannTrackCand");
  //fMCMatch->InitStage(kTrack,"","PndTrack");
  fMCMatch->InitStage(kTrack,"","LheGenTrack");
  fMCMatch->InitStage(kPidChargedCandidate,"","PidChargedCand");
  fMCMatch->InitStage(kPidNeutralCandidate,"","PidNeutralCand");


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
	fMCMatch->CreateArtificialStage(kMCTrack, "", "");

	fMCMatch->Print();
	std::cout << std::endl;
}

void PndMCMatchLoaderTask::Finish()
{
}


ClassImp(PndMCMatchLoaderTask);
