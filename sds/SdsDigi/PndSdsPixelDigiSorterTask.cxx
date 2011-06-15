// -------------------------------------------------------------------------
// -----                PndSdsPixelDigiSorterTask source file             -----
// -------------------------------------------------------------------------

#include "PndSdsPixelDigiSorterTask.h"
#include "TClonesArray.h"

#include "FairRootManager.h"


// -----   Default constructor   -------------------------------------------
PndSdsPixelDigiSorterTask::PndSdsPixelDigiSorterTask() :
PndSdsTask("SDS Pixel Digi Sorter"), fNumberOfCells(1000), fWidthOfCells(10), fOverwriteParams(kFALSE)
{
}
// -------------------------------------------------------------------------

PndSdsPixelDigiSorterTask::PndSdsPixelDigiSorterTask(const char* name) :
PndSdsTask(name), fNumberOfCells(1000), fWidthOfCells(10), fOverwriteParams(kFALSE)
{
  if(fVerbose>0) Info("PndSdsPixelDigiSorterTask","%s created, Parameters will be taken from RTDB",name);
}
// -------------------------------------------------------------------------

PndSdsPixelDigiSorterTask::PndSdsPixelDigiSorterTask(Int_t numberOfCells, Double_t widthOfCells) :
PndSdsTask("SDS Pixel Digi Sorter (PndSdsPixelDigiSorterTask)"), fNumberOfCells(numberOfCells), fWidthOfCells(widthOfCells), fOverwriteParams(kTRUE)
{
  if(fVerbose>0) Info("PndSdsPixelDigiSorterTask","SDS Pixel Digi Sorter created, Parameters will be overwritten in RTDB");
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndSdsPixelDigiSorterTask::~PndSdsPixelDigiSorterTask()
{
	if (fSorter!= 0) delete fSorter;
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndSdsPixelDigiSorterTask::SetParContainers()
{
  if( ! fDigiPar) Fatal("SetParContainers","No digitiztion parameters specified.");
  if(fVerbose>1) Info("SetParContainers","done.");
  return;
}

InitStatus PndSdsPixelDigiSorterTask::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndSdsPixelDigiSorterTask::Init()
{
  SetBranchNames();

  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman )
  {
    std::cout << "-E- PndSdsPixelDigiSorterTask::Init: "
    << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fSorter = new PndRingSorterT<PndSdsDigiPixel>(fNumberOfCells, fWidthOfCells);
  fPixelArray = (TClonesArray*) ioman->GetObject(fInBranchName);
  if ( ! fPixelArray )
  {
    std::cout << "-W- PndSdsPixelDigiSorterTask::Init: "
    << "No SDSPoint array!" << std::endl;
    return kERROR;
  }
  

  // Create and register output array
  if(fVerbose>1) Info("Init","Registering this branch: %s/%s",fFolderName.Data(),fOutBranchName.Data());
  fSortedPixelArray = ioman->Register(fOutBranchName, "PndSdsDigiPixel", fFolderName, fPersistance);
  
  
  if(fOverwriteParams==kTRUE){
    fDigiPar->SetPixelSorterCellWidth(fWidthOfCells);
    fDigiPar->SetPixelSorterNumberOfCells(fNumberOfCells);
	  if(fVerbose>0) Info("Init","RTDB updated");
  }
  
  if(fVerbose>2) fDigiPar->Print();
  
  fWidthOfCells = fDigiPar->GetPixelSorterCellWidth();
  fNumberOfCells = fDigiPar->GetPixelSorterNumberOfCells();
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndSdsPixelDigiSorterTask::Exec(Option_t* opt)
{

  fSortedPixelArray->Delete();
  
  for (int i = 0; i < fPixelArray->GetEntriesFast(); i++){
	  PndSdsDigiPixel* myDigi = (PndSdsDigiPixel*)fPixelArray->At(i);
  	  fSorter->AddElement(*myDigi, myDigi->GetTimeStamp());
    }
    fSorter->Print();

    std::vector<PndSdsDigiPixel> sortedData = fSorter->GetOutputData();
    fSorter->DeleteOutputData();

    int nPix = 0;
    fSortedPixelArray = FairRootManager::Instance()->GetEmptyTClonesArray(fOutBranchName);
    for (int i = 0; i < sortedData.size(); i++){
  	  new ((*fSortedPixelArray)[nPix++])PndSdsDigiPixel(sortedData[i]);
    }

}

// -------------------------------------------------------------------------


void PndSdsPixelDigiSorterTask::FinishTask()
{
	  fSorter->WriteOutAll();
	  std::vector<PndSdsDigiPixel> sortedData = fSorter->GetOutputData();
	  fSorter->DeleteOutputData();

	  FairRootManager* ioman = FairRootManager::Instance();
	  int nPix = 0;
	  fSortedPixelArray = ioman->GetEmptyTClonesArray(fOutBranchName);
	  for (int i = 0; i < sortedData.size(); i++){
		  new ((*fSortedPixelArray)[nPix++])PndSdsDigiPixel(sortedData[i]);
	  }

	//ioman->SaveAllContainers();
	ioman->ForceFill();

}

ClassImp(PndSdsPixelDigiSorterTask);
