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
#include "FairMultiLinkedData.h"

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

  fMCMatch->InitStage("MVDPoint");
  fMCMatch->InitStage("MVDStripDigis");
  fMCMatch->InitStage("MVDPixelDigis");
  fMCMatch->InitStage("MVDPixelClusterCand");
  fMCMatch->InitStage("MVDStripClusterCand");
  fMCMatch->InitStage("MVDSortedPixelDigis");
  fMCMatch->InitStage("MVDSortedStripDigis");
  fMCMatch->InitStage("MVDHitsPixel");
  fMCMatch->InitStage("MVDHitsStrip");

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
  fMCMatch->InitStage("MVDTrack");
  fMCMatch->InitStage("LheGenTrack");
  fMCMatch->InitStage("PidChargedCand");
  fMCMatch->InitStage("PidNeutralCand");

  InitStatus status = InitBranches();


  FairRootManager* ioman = FairRootManager::Instance();
  	if (!ioman) {
  		std::cout << "-E- PndMCMatchCreatorTask::Init: "
  				<< "RootManager not instantiated!" << std::endl;
  		return kFATAL;
  	}

	fMCLink = new TClonesArray("PndMCEntry");
	ioman->Register("MCLink", "MCInfo", fMCLink, kTRUE);

	ioman->Register("MCMatch", "MCInfo", fMCMatch, kFALSE);

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

	//std::cout << "NMCStages: " << fMCMatch->GetNMCStages() << std::endl;
	for (int i = 0; i < fMCMatch->GetNMCStages(); i++){
		if (fMCMatch->GetMCStage(i)->GetFill() == kTRUE && fMCMatch->GetMCStage(i)->GetLoaded() == kFALSE){
			//std::cout << i << ": ";
			//std::cout << "BranchName: " << fMCMatch->GetMCStage(i)->GetBranchName() << std::endl;
			TClonesArray* clArray = fBranches[fMCMatch->GetMCStage(i)->GetBranchName()];
			for (int j = 0; j < clArray->GetEntries(); j++){
				FairMultiLinkedData* myData = (FairMultiLinkedData*)clArray->At(j);
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
