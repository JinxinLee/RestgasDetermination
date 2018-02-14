// -------------------------------------------------------------------------
// -----                PndMvdIdealClusterTask source file             -----
// -------------------------------------------------------------------------

#include "PndMvdIdealClusterTask.h"

// -----   Default constructor   -------------------------------------------
PndMvdIdealClusterTask::PndMvdIdealClusterTask() :
  PndSdsIdealClusterTask()
{
}

PndMvdIdealClusterTask::PndMvdIdealClusterTask(Double_t radius, Int_t FEcolumns, Int_t FErows, TString geoFile) :
  PndSdsIdealClusterTask(radius, FEcolumns, FErows, geoFile)
{
}

// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMvdIdealClusterTask::~PndMvdIdealClusterTask()
{
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdIdealClusterTask::SetParContainers()
{
}
// -----   Manula I/O folders/branches   ----------------------------------------------------
void PndMvdIdealClusterTask::SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName)
{
  fInBranchName = inBranchname;
  fOutBranchName = outHitBranchname;
  fClustBranchName = outClustBranchname;
  fFolderName = folderName;
}

// -----   Default I/O folder/branches   ----------------------------------------------------
void PndMvdIdealClusterTask::SetBranchNames()
{
  fInBranchName = "MVDPixelDigis";
  fOutBranchName = "MVDHitsPixelIdeal";
  fClustBranchName = "MVDPixelClusterCandIdeal";
  fFolderName = "PndMvd";
}

ClassImp(PndMvdIdealClusterTask);

