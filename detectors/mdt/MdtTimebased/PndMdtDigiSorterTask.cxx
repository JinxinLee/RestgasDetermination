/*
 * PndMdtDigiRingSorterTask.cxx
 */

#include "PndMdtDigiSorterTask.h"
#include "PndMdtDigiRingSorter.h"
#include "TClonesArray.h"
#include "FairTimeStamp.h"

ClassImp(PndMdtDigiSorterTask);

PndMdtDigiSorterTask::PndMdtDigiSorterTask() {
}

PndMdtDigiSorterTask::~PndMdtDigiSorterTask() {
}


void PndMdtDigiSorterTask::AddNewDataToTClonesArray(FairTimeStamp* data)
{
	FairRootManager* ioman = FairRootManager::Instance();
	TClonesArray* myArray = ioman->GetTClonesArray(fOutputBranch);
	if (fVerbose > 1){
		std::cout << "-I- PndMdtDigiSorterTask::AddNewDataToTClonesArray Data: " ;
		data->Print();
		std::cout<<std::endl;
	}
	(*myArray)[myArray->GetEntries()] = data;
}

FairRingSorter* PndMdtDigiSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const{
	return new PndMdtDigiRingSorter(numberOfCells, widthOfCells);
}
