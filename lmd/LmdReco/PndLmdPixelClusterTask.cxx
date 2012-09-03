// -------------------------------------------------------------------------
// -----                PndLmdPixelClusterTasksource file             -----
// -------------------------------------------------------------------------


#include "PndLmdPixelClusterTask.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "PndSdsPixelDigiPar.h"
// -----   Default constructor   -------------------------------------------
PndLmdPixelClusterTask::PndLmdPixelClusterTask() :
PndSdsPixelClusterTask("LMD Clustertisation Task")
{
  fPersistance = kTRUE;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndLmdPixelClusterTask::~PndLmdPixelClusterTask()
{
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndLmdPixelClusterTask::SetParContainers()
{
  // Get Base Container
	FairRun* ana = FairRun::Instance();
	FairRuntimeDb* rtdb=ana->GetRuntimeDb();
	fDigiPar = (PndSdsPixelDigiPar*)(rtdb->getContainer("SDSPixelDigiPar"));
	rtdb->getContainer("SDSPixelTotDigiPar");
	PndSdsPixelClusterTask::SetParContainers();
}

void PndLmdPixelClusterTask::SetBackMapping()
{
  // at Init() stage we already have fGeoH filled with the parameters
	fBackMapping = new PndLmdChargeWeightedPixelMapping(fGeoH, fVerbose);
}

void PndLmdPixelClusterTask::SetClusterFinder()
{
	fClusterFinder = new PndLmdSimplePixelClusterFinder(fVerbose);
}

// -----   Manula I/O folders/branches   ----------------------------------------------------
void PndLmdPixelClusterTask::SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName)
{
  fInBranchName = inBranchname;
  fOutBranchName = outHitBranchname;
  fClustBranchName = outClustBranchname;
  fFolderName = folderName;
}

// -----   Default I/O folder/branches   ----------------------------------------------------
void PndLmdPixelClusterTask::SetBranchNames()
{
	if (FairRunAna::Instance()->IsTimeStamp())
		fInBranchName = "LMDSortedPixelDigis";
	else
		fInBranchName = "LMDPixelDigis";
  fOutBranchName = "LMDHitsPixel";
  fClustBranchName = "LMDPixelClusterCand";
  fFolderName = "cbmsim";
}

ClassImp(PndLmdPixelClusterTask);

