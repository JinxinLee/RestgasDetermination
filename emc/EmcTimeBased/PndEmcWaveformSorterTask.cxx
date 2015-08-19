/*
 * PndEmcWaveformSorterTask.cxx
 */

#include <PndEmcWaveformSorterTask.h>

#include "PndEmcWaveform.h"
#include "PndEmcWaveformRingSorter.h"
#include "TClonesArray.h"

ClassImp(PndEmcWaveformSorterTask);

PndEmcWaveformSorterTask::PndEmcWaveformSorterTask() {
}

PndEmcWaveformSorterTask::~PndEmcWaveformSorterTask() {
}


void PndEmcWaveformSorterTask::AddNewDataToTClonesArray(FairTimeStamp* data)
{
	 FairRootManager* ioman = FairRootManager::Instance();
	 TClonesArray* myArray = ioman->GetTClonesArray(fOutputBranch);
	 if (fVerbose > 1){
		 std::cout << "-I- PndEmcWaveformSorterTask::AddNewDataToTClonesArray Data: " ;
		 std::cout <<  *(PndEmcWaveform*)(data) << std::endl;
	 }
	 new ((*myArray)[myArray->GetEntries()]) PndEmcWaveform(*(PndEmcWaveform*)(data));
}

FairRingSorter* PndEmcWaveformSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const{
	return new PndEmcWaveformRingSorter(numberOfCells, widthOfCells);
}
