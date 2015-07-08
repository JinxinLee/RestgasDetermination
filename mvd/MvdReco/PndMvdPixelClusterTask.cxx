// -------------------------------------------------------------------------
// -----                PndMvdPixelClusterTasksource file             -----
// -------------------------------------------------------------------------


#include "PndMvdPixelClusterTask.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "PndSdsPixelDigiPar.h"
// -----   Default constructor   -------------------------------------------
PndMvdPixelClusterTask::PndMvdPixelClusterTask(TString parName, TString totParName) :
PndSdsPixelClusterTask("MVD Clustertisation Task"), fParName(parName), fTotParName(totParName)
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
	fDigiPar = (PndSdsPixelDigiPar*)(rtdb->getContainer(fParName.Data()));
	rtdb->getContainer(fTotParName.Data());
	PndSdsPixelClusterTask::SetParContainers();
}

void PndMvdPixelClusterTask::SetBackMapping()
{
  // at Init() stage we already have fGeoH filled with the parameters
	fBackMapping = new PndMvdChargeWeightedPixelMapping(fGeoH, fVerbose);
}

void PndMvdPixelClusterTask::SetClusterFinder()
{
	fClusterFinder = new PndMvdSimplePixelClusterFinder(fParName, fTotParName, fVerbose);
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
	if (fInBranchName.Length() == 0){
		if (FairRunAna::Instance()->IsTimeStamp())
			fInBranchName = "MVDSortedPixelDigis";
		else
			fInBranchName = "MVDPixelDigis";
	}
	if (fOutBranchName.Length() == 0)
		fOutBranchName = "MVDHitsPixel";
	if (fClustBranchName.Length() == 0)
		fClustBranchName = "MVDPixelClusterCand";
	if (fFolderName.Length() == 0)
		fFolderName = "PndMvd";
}

ClassImp(PndMvdPixelClusterTask);

