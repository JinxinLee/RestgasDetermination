#include "PndMvdReadInTBDataTask.h"

#include <iostream>
#include <fstream>


// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "PndSdsDigiTopix4.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"

// PndMvd includes



PndMvdReadInTBDataTask::PndMvdReadInTBDataTask() :
	FairTask("MVDReadInTBDataTask"), fClockFrequency(50), fEvent(0)
{
}

PndMvdReadInTBDataTask::~PndMvdReadInTBDataTask()
{
	for (int i = 0; i < fReader.size(); i++){
		delete(fReader[i]);
	}
	fReader.clear();
}

void PndMvdReadInTBDataTask::SetParContainers()
{
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
}

InitStatus PndMvdReadInTBDataTask::ReInit()
{
 fInitDone = kTRUE;
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdReadInTBDataTask::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();
  std::cout << "PndMvdReadInTBDataTask::Init start of Init" << std::endl;

  if ( ! ioman )
    {
      std::cout << "-E- PndMvdReadInTBDataTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  for (int i = 0; i < fFileNames.size(); i++) {
    std::cout << "PndMvdReadInTBDataTask::Init FileName: " << fFileNames[i] << std::endl;
	  fReader.push_back(new PndMvdReadInTBData());
	  fEndOfFile.push_back(kFALSE);
	  fReader[i]->SetFileName(fFileNames[i]);
	  fReader[i]->SetClockFrequency(fClockFrequency);
	  fReader[i]->SetVerbose(fVerbose);
	  fReader[i]->SetFE(i+1);
	  std::cout << "PndMvdReadInTBDataTask::Init before Reader Init" << std::endl;
	  fReader[i]->Init();
  }

  fDigiArray = ioman->Register("ToPix4Hits", "PndSdsDigiTopix4", "MVD", kTRUE);
  // fFrameHeaderArray = ioman->Register("ToPix4FrameHeader","frameHeader","MVD", kTRUE);

  std::cout << "-I- PndMvdReadInTBDataTask: Initialisation successfull" << std::endl;
  fInitDone = kTRUE;
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdReadInTBDataTask::Exec(Option_t* opt)
{
	if (fEvent % 10000 == 0) {
	  std::cout << "PndMvdReadInTBDataTask::Exec called - Event " <<std::dec<<  fEvent << std::endl;
	}
	fEvent++;

	for(int i = 0; i < fReader.size(); i++){
		if (fEndOfFile[i] != kTRUE){
			fEndOfFile[i] = fReader[i]->ReadInData(fDigiArray);
		}
	}
	Bool_t endOfFiles = kTRUE;
	for (int j = 0; j < fEndOfFile.size(); j++){
		endOfFiles &= fEndOfFile[j];
	}
	if (endOfFiles == kTRUE){
		std::cout << "Number of non sequential frame counters: " << std::endl;
		for(int i = 0; i < fReader.size(); i++){
			UInt_t nonSequential = fReader[i]->GetNonSequenctialFC();
			std::cout << i << " : " << nonSequential
					<< " double header " << fReader[i]->GetDoubleHeader()
					<< " double trailer " << fReader[i]->GetDoubleTrailer() << std::endl;
			std::cout << i << "SuperFrameCount: " << fReader[i]->GetSuperFrameCount() << std::endl;
			std::cout << i << "WrongHammingCount: " << fReader[i]->GetWrongHammingCodeCount() << std::endl;
		}
		FairRootManager::Instance()->SetFinishRun(kTRUE);
	}

}

void PndMvdReadInTBDataTask::FinishEvent()
{
	fDigiArray->Delete();
	//fFrameHeaderArray->Delete();
}

ClassImp(PndMvdReadInTBDataTask);
