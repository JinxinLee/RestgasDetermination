// -------------------------------------------------------------------------
// -----                PndMvdPixelClusterTasksource file             -----
// -------------------------------------------------------------------------


#include "PndMvdPixelClusterTask.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "PndSdsPixelDigiPar.h"
// -----   Default constructor   -------------------------------------------
PndMvdPixelClusterTask::PndMvdPixelClusterTask() :
PndSdsPixelClusterTask("MVD Clustertisation Task")
{
  fPersistance = kTRUE;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdPixelClusterTask::~PndMvdPixelClusterTask()
{
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdPixelClusterTask::SetParContainers()
{
  // Get Base Container
	FairRun* ana = FairRun::Instance();
	FairRuntimeDb* rtdb=ana->GetRuntimeDb();
	fDigiPar = (PndSdsPixelDigiPar*)(rtdb->getContainer("MVDPixelDigiPar"));
	rtdb->getContainer("MVDPixelTotDigiPar");
	PndSdsPixelClusterTask::SetParContainers();
}

void PndMvdPixelClusterTask::SetBackMapping()
{
  // at Init() stage we already have fGeoH filled with the parameters
	fBackMapping = new PndMvdChargeWeightedPixelMapping(fGeoH, fVerbose);
}

void PndMvdPixelClusterTask::SetClusterFinder()
{
	fClusterFinder = new PndMvdSimplePixelClusterFinder(fVerbose);
}

// -----   Manula I/O folders/branches   ----------------------------------------------------
void PndMvdPixelClusterTask::SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName)
{
  fInBranchName = inBranchname;
  fOutBranchName = outHitBranchname;
  fClustBranchName = outClustBranchname;
  fFolderName = folderName;
}

// -----   Default I/O folder/branches   ----------------------------------------------------
void PndMvdPixelClusterTask::SetBranchNames()
{
  fInBranchName = "MVDPixelDigis";
  fOutBranchName = "MVDHitsPixel";
  fClustBranchName = "MVDPixelClusterCand";
  fFolderName = "PndMvd";
}

ClassImp(PndMvdPixelClusterTask);

