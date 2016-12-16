// -------------------------------------------------------------------------
// -----                PndMvdPixelClusterTasksource file             -----
// -------------------------------------------------------------------------


#include "PndMvdPixelClusterTask.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "PndSdsPixelDigiPar.h"
// -----   Default constructor   -------------------------------------------
PndMvdPixelClusterTask::PndMvdPixelClusterTask(TString parName, TString totParName) :
PndSdsPixelClusterTask("MVD Clustertisation Task"), 
      fParName(parName),
      fTotParName(totParName),
	    fTotDigiPar(NULL),
	    fSensorNamePar(NULL)
{
  fPersistance = kTRUE;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdPixelClusterTask::~PndMvdPixelClusterTask()
{
}
// -------------------------------------------------------------------------

void PndMvdPixelClusterTask::GetParList(TList* tempList) {
  fDigiPar = new PndSdsPixelDigiPar(fParName.Data());
  tempList->Add(fDigiPar);
  fTotDigiPar = new PndSdsTotDigiPar(fTotParName.Data());
  tempList->Add(fTotDigiPar);
  fSensorNamePar = new PndSensorNamePar("PndSensorNamePar");
  tempList->Add(fSensorNamePar);

  return;
}

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdPixelClusterTask::SetParContainers()
{
  // Get Base Container
	FairRun* ana = FairRun::Instance();
	FairRuntimeDb* rtdb=ana->GetRuntimeDb();
	fDigiPar = (PndSdsPixelDigiPar*)(rtdb->getContainer(fParName.Data()));
	fTotDigiPar = (PndSdsTotDigiPar*)(rtdb->getContainer(fTotParName.Data()));
	PndSdsPixelClusterTask::SetParContainers();
}

void PndMvdPixelClusterTask::SetBackMapping()
{
  // at Init() stage we already have fGeoH filled with the parameters
	fBackMapping = new PndMvdChargeWeightedPixelMapping(fGeoH, fVerbose);
}

void PndMvdPixelClusterTask::SetBackMappingMQ(TList* tempList)
{
  // at Init() stage we already have fGeoH filled with the parameters
	fDigiPar = (PndSdsPixelDigiPar*)tempList->FindObject(fParName.Data());
	fTotDigiPar = (PndSdsTotDigiPar*)tempList->FindObject(fTotParName.Data());
	fSensorNamePar = (PndSensorNamePar*)tempList->FindObject("PndSensorNamePar");
	fSensorNamePar->FillMap();
	if (fGeoH == 0)
		fGeoH = new PndGeoHandling(fSensorNamePar);
	fBackMapping = new PndMvdChargeWeightedPixelMapping(fGeoH, fDigiPar, fTotDigiPar);
}

void PndMvdPixelClusterTask::SetClusterFinder()
{
	fClusterFinder = new PndMvdSimplePixelClusterFinder(fParName, fTotParName, fVerbose);
}

void PndMvdPixelClusterTask::SetClusterFinderMQ(TList* tempList)
{
	fDigiPar = (PndSdsPixelDigiPar*)tempList->FindObject(fParName.Data());
	fTotDigiPar = (PndSdsTotDigiPar*)tempList->FindObject(fTotParName.Data());
	fSensorNamePar = (PndSensorNamePar*)tempList->FindObject("PndSensorNamePar");

	fClusterFinder = new PndMvdSimplePixelClusterFinder(fDigiPar, fTotDigiPar);
	fGeoH = new PndGeoHandling(fSensorNamePar);

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

