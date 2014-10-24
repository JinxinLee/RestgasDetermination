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
	FairTask("MVDReadInTBDataTask")
{
}

PndMvdReadInTBDataTask::~PndMvdReadInTBDataTask()
{
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

  if ( ! ioman )
    {
      std::cout << "-E- PndMvdReadInTBDataTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  fReader.SetFileNames(fFileNames);

  fReader.Init();

  fDigiArray = ioman->Register("ToPix4Hits", "PndSdsDigiTopix4", "MVD", kTRUE);

  std::cout << "-I- PndMvdReadInTBDataTask: Initialisation successfull" << std::endl;
  fInitDone = kTRUE;
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdReadInTBDataTask::Exec(Option_t* opt)
{
	std::cout << "PndMvdReadInTBDataTask::Exec called!" << std::endl;
	Bool_t endOfFiles = fReader.ReadInData(fDigiArray);
	if (endOfFiles == kTRUE){
		FairRootManager::Instance()->SetFinishRun(kTRUE);
	}

}

void PndMvdReadInTBDataTask::FinishEvent()
{
	fDigiArray->Delete();
}

ClassImp(PndMvdReadInTBDataTask);

