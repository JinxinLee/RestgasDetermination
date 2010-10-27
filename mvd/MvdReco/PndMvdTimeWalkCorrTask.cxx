/*
 * PndMvdTimeWalkCorrectionTask.cxx
 *
 *  Created on: Aug 27, 2010
 *      Author: Simone Esch
 */

#include "PndMvdTimeWalkCorrTask.h"


// -----   Default constructor   -------------------------------------------

PndMvdTimeWalkCorrTask::PndMvdTimeWalkCorrTask() :
PndSdsTimeWalkCorrTask("SDS TimeWalkCorrection Task")
{
fPersistance=true;

}



PndMvdTimeWalkCorrTask::~PndMvdTimeWalkCorrTask() {

}

// -----   Default I/O folder/branches   ----------------------------------------------------
void PndMvdTimeWalkCorrTask::SetBranchNames()
{
  fInBranchName = "MVDPixelDigis";                 // Name in digi_complete
  fOutBranchName = "MVDDigisCorr";                 // Name in reco_complete
  fFolderName = "PndMvd";
}

void PndMvdTimeWalkCorrTask::SetParContainers(){

	FairRun* ana = FairRun::Instance();
	FairRuntimeDb* rtdb=ana->GetRuntimeDb();
	fDigiPar = (PndSdsPixelDigiPar*)(rtdb->getContainer("MVDPixelDigiPar"));
	fDigiTotPar = (PndSdsTotDigiPar*)(rtdb->getContainer("MVDPixelTotDigiPar"));
	PndSdsTimeWalkCorrTask::SetParContainers();

}
