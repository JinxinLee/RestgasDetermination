// -------------------------------------------------------------------------
// -----                PndSorterTaskT source file             -----
// -------------------------------------------------------------------------

#include "PndSorterTask.h"
#include "TClonesArray.h"

#include "FairRootManager.h"




 InitStatus PndSorterTask::ReInit()
{
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
 InitStatus PndSorterTask::Init()
{

  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman )
  {
    std::cout << "-E- PndSorterTaskT::Init: "
    << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fSorter = new PndRingSorter(fNumberOfCells, fWidthOfCells);

  // Create and register output array
  fInputArray = FairRootManager::Instance()->GetTClonesArray(fInputBranch);

  if(fVerbose>1) Info("Init","Registering this branch: %s/%s","PndMvd",fOutputBranch.Data());
  fOutputArray = ioman->Register(fOutputBranch, fInputArray->GetClass()->GetName(), fFolder, fPersistance);
  
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
 void PndSorterTask::Exec(Option_t* opt)
{


  
  fInputArray = FairRootManager::Instance()->GetTClonesArray(fInputBranch);
  std::cout << "-I- PndSorterTaskT: Size PixelArray: " << fInputArray->GetEntriesFast() << std::endl;
  for (int i = 0; i < fInputArray->GetEntriesFast(); i++){
	  FairTimeStamp* myData = (FairTimeStamp*)fInputArray->At(i);
	  myData->SetEntryNr(FairLink(0, fEntryNr, fInputBranch, i));
	  if (fVerbose > 2) std::cout << "Sorter filled with: " << *myData << std::endl;
	  fSorter->AddElement(myData, myData->GetTimeStamp());
  }
  if (fVerbose > 2)fSorter->Print();

  std::vector<FairTimeStamp*> sortedData = fSorter->GetOutputData();
  fSorter->DeleteOutputData();

  fOutputArray = FairRootManager::Instance()->GetEmptyTClonesArray(fOutputBranch);
  for (int i = 0; i < sortedData.size(); i++) {
	  WriteDataToTClonesArray(sortedData[i]);
  }
  fEntryNr++;
}

// -------------------------------------------------------------------------

 void PndSorterTask::FinishEvent(){
	  fOutputArray->Delete();
}

 void PndSorterTask::FinishTask()
{
	fInputArray = FairRootManager::Instance()->GetTClonesArray(fInputBranch);
	if (fVerbose > 2) std::cout << "-I- PndSorterTaskT::FinishTask Size InputArray: " << fInputArray->GetEntriesFast() << std::endl;
	for (int i = 0; i < fInputArray->GetEntriesFast(); i++) {
		FairTimeStamp* myDigi = (FairTimeStamp*) fInputArray->At(i);
		fSorter->AddElement(myDigi, ((FairTimeStamp*)myDigi)->GetTimeStamp());
	}
	fSorter->WriteOutAll();
	std::vector<FairTimeStamp*> sortedData = fSorter->GetOutputData();
	fSorter->DeleteOutputData();

	FairRootManager* ioman = FairRootManager::Instance();
	fOutputArray = ioman->GetEmptyTClonesArray(fOutputBranch);
	for (int i = 0; i < sortedData.size(); i++) {
		WriteDataToTClonesArray(sortedData[i]);
	}
	ioman->SetLastFill();
}

ClassImp(PndSorterTask);
