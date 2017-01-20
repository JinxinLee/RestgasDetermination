// -------------------------------------------------------------------------
// -----                PndMCMatchSelectorTask source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
#include "PndMCMatchSelectorTask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairMultiLinkedData.h"
#include "FairRootManager.h"

#include "PndMCEntry.h"


// -----   Default constructor   -------------------------------------------
PndMCMatchSelectorTask::PndMCMatchSelectorTask()
	: FairTask("Creates PndMCMatch"), fStart(kUnknown), fStop(kUnknown)
{
}
// -------------------------------------------------------------------------

PndMCMatchSelectorTask::PndMCMatchSelectorTask(TString start, TString stop)
	: FairTask("Creates PndMCMatch"), fStartString(start), fStopString(stop), fStart(-1), fStop(-1)
{
}

PndMCMatchSelectorTask::PndMCMatchSelectorTask(Int_t start, Int_t stop)
	: FairTask("Creates PndMCMatch"), fStart(start), fStop(stop), fStartString(""), fStopString("")
{
}

// -----   Destructor   ----------------------------------------------------
PndMCMatchSelectorTask::~PndMCMatchSelectorTask()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCMatchSelectorTask::Init()
{


//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");



  FairRootManager* ioman = FairRootManager::Instance();
  	if (!ioman) {
  		std::cout << "-E- PndMCMatchSelectorTask::Init: "
  				<< "RootManager not instantiated!" << std::endl;
  		return kFATAL;
  	}

  	fMCMatch = (PndMCMatch*)ioman->GetObject("MCMatch");
  	if (fStart < 0 && fStop < 0){
  		fStart = ioman->GetBranchId(fStartString);
  		fStop = ioman->GetBranchId(fStopString);
  		std::cout << fStartString << ": " << fStart << " / " << fStopString << ": " << fStop << std::endl;
  	}

	std::cout << "-I- PndMCMatchSelectorTask::Init: Initialization successfull" << std::endl;


  return kSUCCESS;
}


// -------------------------------------------------------------------------
void PndMCMatchSelectorTask::SetParContainers()
{
  // Get Base Container
//  FairRun* ana = FairRun::Instance();
//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void PndMCMatchSelectorTask::Exec(Option_t* opt)
{
	std::cout << "Output Selector: " << std::endl;
	SetWeights();
	fMCMatch->CreateArtificialStage("MCTrack");
	std::cout << fMCMatch->GetMCInfo(fStart, fStop);
	std::cout << std::endl;
}

void PndMCMatchSelectorTask::SetWeights()
{
//	std::cout << "SetWeights: CommonWeight " << fCommonWeight << " NStageWeights " << fStageWeights.size() << std::endl;
	fMCMatch->SetCommonWeightStages(fCommonWeight);
	for (size_t i = 0; i < fStageWeights.size();i++){
		fMCMatch->GetMCStageType(fStageWeights[i].first)->SetWeight(fStageWeights[i].second);
	}
}

void PndMCMatchSelectorTask::Finish()
{
}


ClassImp(PndMCMatchSelectorTask);
