#include "PndGapEventBuilderTask.h"

#include <iostream>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "FairTimeStamp.h"

PndGapEventBuilderTask::PndGapEventBuilderTask() :
	FairTask("Pnd Gap Event Builder"), fTimeGap(10), fMainBranchName(), fPersistence(kTRUE)
{
}

PndGapEventBuilderTask::~PndGapEventBuilderTask()
{
}

void PndGapEventBuilderTask::SetParContainers()
{
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
//  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

InitStatus PndGapEventBuilderTask::ReInit()
{
  InitStatus stat=kSUCCESS;
  return stat;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndGapEventBuilderTask::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndGapEventBuilderTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  TString outputName = fMainBranchName;
  outputName.Append("_event");
  fMainHitArray = (TClonesArray*) FairRootManager::Instance()->GetObject(fMainBranchName);
  std::cout << "ClassName of HitArray: " << fMainHitArray->ClassName() << std::endl;
  fMainEventHitArray = ioman->Register(outputName.Data(), fMainHitArray->GetClass()->GetName(), "EventData", fPersistence);

  for (int i = 0; i < fAddBranches.size(); i++){
	  outputName = fAddBranches[i].first;
	  fAddHitArray.push_back((TClonesArray*)ioman->GetObject(outputName.Data()));
	  outputName.Append("_event");
	  fAddEventHitArray.push_back(ioman->Register(outputName.Data(), fAddHitArray[i]->GetClass()->GetName(), "EventData", fPersistence));
  }

  fStartFunctor = new StopTime();
  fStopFunctor= new StopTime();
  fTimeGapFunctor = new TimeGap();

  std::cout << "-I- PndGapEventBuilderTask: Initialisation successfull" << std::endl;
  fInitDone = kTRUE;
  return kSUCCESS;
}


// -----   Public method Exec   --------------------------------------------
void PndGapEventBuilderTask::Exec(Option_t* opt)
{
	fMainHitArray->Delete();
	fMainEventHitArray->Delete();
	for (int i = 0; i < fAddHitArray.size(); i++){
		fAddEventHitArray[i]->Delete();
	}
	fMainHitArray = FairRootManager::Instance()->GetData(fMainBranchName, fTimeGapFunctor, fTimeGap);
	for(int i = 0; i < fMainHitArray->GetEntriesFast(); i++)
	{
		FairTimeStamp* temp = (FairTimeStamp*)fMainHitArray->At(i);
		std::cout << i << " :" << temp->GetTimeStamp();
		if (temp->GetPointerToLinks() > 0) {
			std::cout << " " << *(temp->GetPointerToLinks())  << std::endl;
		} else std::cout  << std::endl;
	}
	fMainEventHitArray->AbsorbObjects(fMainHitArray);

	if (fMainEventHitArray->GetEntriesFast() > 0){

		FairTimeStamp* data = (FairTimeStamp*)fMainEventHitArray->At(0);
		Double_t startTime = data->GetTimeStamp() - 10;
		if (startTime < 0) startTime = 0;

		data = (FairTimeStamp*)fMainEventHitArray->At(fMainEventHitArray->GetEntriesFast() - 1);
		Double_t stopTime  = data->GetTimeStamp();

		for (int i = 0; i < fAddHitArray.size(); i++){
			TClonesArray* tempArray = FairRootManager::Instance()->GetData(fAddBranches[i].first, fStartFunctor, startTime, fStopFunctor, stopTime + fAddBranches[i].second);
			fAddEventHitArray[i]->AbsorbObjects(tempArray);
			std::cout << std::endl << "PndGapEventBuilderTask: selected Hits: " << fAddBranches[i].first << " for timegap: " << startTime << " _ " << stopTime + fAddBranches[i].second << std::endl;
			for (int j = 0; j < fAddEventHitArray[i]->GetEntriesFast(); j++){
				FairTimeStamp* tempdata = (FairTimeStamp*)fAddEventHitArray[i]->At(j);
				std::cout << j << " : " << tempdata->GetTimeStamp();
				if (tempdata->GetPointerToLinks() > 0)
					std::cout << " " << *(tempdata->GetPointerToLinks()) << std::endl;
				else std::cout << std::endl;
			}
		}
	}

}

void PndGapEventBuilderTask::FinishEvent()
{
//	fMainEventHitArray->Delete();
}


ClassImp(PndGapEventBuilderTask);

