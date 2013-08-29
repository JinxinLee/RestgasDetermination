/*
 * PndEmcDigiSorterTask.cxx
 */

#include <PndEmcDigiSorterTask.h>

#include "PndEmcDigi.h"
#include "PndEmcDigiRingSorter.h"

#include "TClonesArray.h"

ClassImp(PndEmcDigiSorterTask);

PndEmcDigiSorterTask::PndEmcDigiSorterTask() {
}

PndEmcDigiSorterTask::~PndEmcDigiSorterTask() {
}


void PndEmcDigiSorterTask::AddNewDataToTClonesArray(FairTimeStamp* data)
{
	 FairRootManager* ioman = FairRootManager::Instance();
	 TClonesArray* myArray = ioman->GetTClonesArray(fOutputBranch);
	 if (fVerbose > 1){
		 std::cout << "-I- PndEmcDigiSorterTask::AddNewDataToTClonesArray Data: " ;
		 std::cout <<  *(PndEmcDigi*)(data) << std::endl;
	 }
	 new ((*myArray)[myArray->GetEntries()]) PndEmcDigi(*(PndEmcDigi*)(data));
}

FairRingSorter* PndEmcDigiSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const{
	return new PndEmcDigiRingSorter(numberOfCells, widthOfCells);
}
