/*
 * PndSdsTimeWalkCorrTask.cxx
 *
 *  Created on: Aug 27, 2010
 *      Author: Simone Esch
 */

#include "PndSdsTimeWalkCorrTask.h"


ClassImp(PndSdsTimeWalkCorrTask);

// -----   Default constructor   -------------------------------------------

PndSdsTimeWalkCorrTask::PndSdsTimeWalkCorrTask():
   PndSdsTask("SDS TimeWalkCorrection Task"), fPersistance(true)
{

}

// -----   Named constructor   ---------------------------------------------
PndSdsTimeWalkCorrTask::PndSdsTimeWalkCorrTask(const char* name):
   PndSdsTask(name), fPersistance(true)
{
fVerbose=1;
}


PndSdsTimeWalkCorrTask::~PndSdsTimeWalkCorrTask() {
}

// -----   Initialization  of Parameter Containers -------------------------
void PndSdsTimeWalkCorrTask::SetParContainers()
{
  if( ! fDigiPar) Fatal("SetParContainers","No digitiztiopn parameters specified");
  if( ! fDigiTotPar ) Fatal("SetParContainers","No tot digitiztiopn parameters specified");
  if(fVerbose>1) Info("SetParContainers","done.");
  return;
}


// -----   Public method Init   --------------------------------------------
InitStatus PndSdsTimeWalkCorrTask::Init()
{
  SetBranchNames();

  fTimeWalkCorr= new PndSdsTimeWalkCorrSimple(fDigiTotPar->GetChargingTime(), fDigiTotPar->GetConstCurrent(), fDigiPar->GetThreshold(),fDigiTotPar->GetClockFrequency(), fVerbose);

  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
  {
     std::cout << "-E- PndSdsTimeWalkCorrTask::Init: "
     << "RootManager not instantiated!" << std::endl;
     return kFATAL;
  }

  // Get input array
  fDigiArray = (TClonesArray*) ioman->GetObject(fInBranchName);

  if ( ! fDigiArray )
  {
	  std::cout << "-W- PndSdsTimeWalkCorrTask::Init: " << "No SDSDigi array!" << std::endl;
	  return kERROR;
  }

  // Create output array
  fDigiCorrArray = new TClonesArray("PndSdsDigiPixel");

  ioman->Register(fOutBranchName, fFolderName, fDigiCorrArray, fPersistance);

  SetInBranchId();

  fDigiPar->Print();
  fDigiTotPar->Print();

  std::cout << "-I- PndSdsTimeWalkCorrTask: Initialisation successfull" << std::endl;

  return kSUCCESS;
}
// -------------------------------------------------------------------------

void PndSdsTimeWalkCorrTask::Exec(Option_t* opt)
{
	std::vector<PndSdsDigiPixel> DigiPixelArray;
	// Reset output array
	if ( ! fDigiCorrArray ) Fatal("Exec", "No ClusterArray");
	fDigiCorrArray->Delete();

	Int_t nPoints = fDigiArray->GetEntriesFast();

	  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++)
	  {
		  PndSdsDigiPixel myDigi = *(PndSdsDigiPixel*)(fDigiArray->At(iPoint));
		  DigiPixelArray.push_back(myDigi);
	  }

	  for (Int_t ii=0; ii< nPoints; ii++)
	  {
		  fTimeCorrection = fTimeWalkCorr->CorrectionTimeWalk(DigiPixelArray[ii].GetCharge());
		  new((*fDigiCorrArray)[ii]) PndSdsDigiPixel(DigiPixelArray[ii].GetIndices(),DigiPixelArray[ii].GetDetID(),DigiPixelArray[ii].GetSensorID(),DigiPixelArray[ii].GetFE(),DigiPixelArray[ii].GetPixelColumn(),DigiPixelArray[ii].GetPixelRow(),DigiPixelArray[ii].GetCharge(),DigiPixelArray[ii].GetMCPointType(),DigiPixelArray[ii].GetTime()-fTimeCorrection);
	  }

	  return;
}
