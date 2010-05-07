// -------------------------------------------------------------------------
// -----                PndMCMatchCreatorTask source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
#include "PndMCMatchCreatorTask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairLinkedData.h"

#include "PndDetectorList.h"
#include "PndMCEntry.h"


// -----   Default constructor   -------------------------------------------
PndMCMatchCreatorTask::PndMCMatchCreatorTask() : FairTask("Creates PndMCMatch"), fEventNr(0), fPersistance(kTRUE)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMCMatchCreatorTask::~PndMCMatchCreatorTask()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCMatchCreatorTask::Init()
{


//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");

	fMCMatch = new PndMCMatch("PndMCMatch", "PndMCMatch");

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

  fMCMatch->InitStage(kDchPoint, "", "PndDchPoint");
  fMCMatch->InitStage(kDchDigi, "", "PndDchDigi");
  fMCMatch->InitStage(kDchHit, "", "PndDchCylinderHit");

  fMCMatch->InitStage(kMdtPoint, "", "MdtPoint");
  fMCMatch->InitStage(kMdtHit, "","MdtHit");
  fMCMatch->InitStage(kMdtTrack, "", "MdtTrk");

 // fMCMatch->InitStage(kEmcPoint,"","EmcPoint");
  fMCMatch->InitStage(kEmcHit, "", "EmcHit");
  fMCMatch->InitStage(kEmcDigi, "", "EmcDigi");
  fMCMatch->InitStage(kEmcCluster, "", "EmcCluster");
  fMCMatch->InitStage(kEmcBump, "", "EmcBump");
  fMCMatch->InitStage(kEmcRecoHit, "", "EmcRecoHit");

  fMCMatch->InitStage(kLheHit, "", "LheHit");
  fMCMatch->InitStage(kLheCand, "", "LheCandidate");
  fMCMatch->InitStage(kLheTrack,"", "LheTrack");
  fMCMatch->InitStage(kTrackCand,"","MVDIdealTrackCand");
  //fMCMatch->InitStage(kTrackCand,"","MVDRiemannTrackCand");
  //fMCMatch->InitStage(kTrack,"","PndTrack");
  fMCMatch->InitStage(kTrack,"","LheGenTrack");
  fMCMatch->InitStage(kPidChargedCandidate,"","PidChargedCand");
  fMCMatch->InitStage(kPidNeutralCandidate,"","PidNeutralCand");

  InitStatus status = InitBranches();


  FairRootManager* ioman = FairRootManager::Instance();
  	if (!ioman) {
  		std::cout << "-E- PndMCMatchCreatorTask::Init: "
  				<< "RootManager not instantiated!" << std::endl;
  		return kFATAL;
  	}

	fMCLink = new TClonesArray("PndMCEntry");
	ioman->Register("MCLink", "MCInfo", fMCLink, kTRUE);

	ioman->Register("MCMatch", "MCMatch", fMCMatch, kFALSE);

	std::cout << "-I- PndMCMatchCreatorTask::Init: Initialization successfull" << std::endl;


  return status;
}

InitStatus PndMCMatchCreatorTask::InitBranches()
{

	 // Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- PndMCMatchCreatorTask::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	int NStages = fMCMatch->GetNMCStages();
	for (int i = NStages-1; i > -1; i--){
		TClonesArray* myBranch = (TClonesArray*)ioman->GetObject(fMCMatch->GetMCStage(i)->GetBranchName().c_str());
		if (!myBranch)	{
			//std::cout << "NMCStages: " << fMCMatch->GetNMCStages() << std::endl;
			std::cout << "-W- PndMCMatchCreatorTask::Init: "<< "No "<<fMCMatch->GetMCStage(i)->GetBranchName() << " array!" << std::endl;
			fMCMatch->GetMCStage(i)->SetFill(kFALSE); //RemoveStage(fMCMatch->GetMCStage(i)->GetStageId());

			continue;
		}
		else fMCMatch->GetMCStage(i)->SetFill(kTRUE);
		fBranches[fMCMatch->GetMCStage(i)->GetBranchName()] = myBranch;
	}
	return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndMCMatchCreatorTask::SetParContainers()
{
  // Get Base Container
//  FairRun* ana = FairRun::Instance();
//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void PndMCMatchCreatorTask::Exec(Option_t* opt)
{

	if (!fMCLink) Fatal("Exec", "No fMCLinkDet");
	fMCLink->Delete();
	fMCMatch->ClearMCList();

	fMCMatch->LoadInMCLists(fMCLink);

	std::cout << "NMCStages: " << fMCMatch->GetNMCStages() << std::endl;
	for (int i = 0; i < fMCMatch->GetNMCStages(); i++){
		if (fMCMatch->GetMCStage(i)->GetFill() == kTRUE && fMCMatch->GetMCStage(i)->GetLoaded() == kFALSE){
			std::cout << i << ": ";
			std::cout << "BranchName: " << fMCMatch->GetMCStage(i)->GetBranchName() << std::endl;
			TClonesArray* clArray = fBranches[fMCMatch->GetMCStage(i)->GetBranchName()];
			for (int j = 0; j < clArray->GetEntries(); j++){
				FairLinkedData* myData = (FairLinkedData*)clArray->At(j);
				fMCMatch->SetElements(fMCMatch->GetMCStage(i)->GetStageId(), j, myData);
			}
			if (fMCMatch->GetMCStage(i)->GetNEntries() > 0)
				fMCMatch->GetMCStage(i)->SetLoaded(kTRUE);
		}
	}

	int i = 0;
	if (fPersistance){
		for (int index = 0; index < fMCMatch->GetNMCStages(); index++){
			PndMCStage myStage(*(fMCMatch->GetMCStage(index)));

			for (int indStage = 0; indStage < myStage.GetNEntries(); indStage++){

				PndMCEntry myLink(myStage.GetMCLink(indStage));
				//std::cout << "myLink: " << myStage.GetMCLink(indStage).GetSource() << "/" << myStage.GetMCLink(indStage).GetPos() << std::endl;
				new((*fMCLink)[i]) PndMCEntry(myLink.GetLinks(), myLink.GetSource(), myLink.GetPos());
				i++;
			}
		}
	}

	if (fVerbose > 0){
		fMCMatch->Print();
		std::cout << std::endl;
	}
}

void PndMCMatchCreatorTask::Finish()
{
}


ClassImp(PndMCMatchCreatorTask);
