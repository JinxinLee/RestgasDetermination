/*
 * PndGemDigiSorterTask.cxx
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#include <PndGemDigiSorterTask.h>

#include "PndGemDigi.h"
#include "PndGemDigiRingSorter.h"

ClassImp(PndGemDigiSorterTask);

PndGemDigiSorterTask::PndGemDigiSorterTask() {
	// TODO Auto-generated constructor stub

}

PndGemDigiSorterTask::~PndGemDigiSorterTask() {
	// TODO Auto-generated destructor stub
}


void PndGemDigiSorterTask::AddNewDataToTClonesArray(FairTimeStamp* data)
{
	 FairRootManager* ioman = FairRootManager::Instance();
	 TClonesArray* myArray = ioman->GetTClonesArray(fOutputBranch);
	 if (fVerbose > 1){
		 std::cout << "-I- PndGemDigiSorterTask::AddNewDataToTClonesArray Data: " ;
		 std::cout <<  *(PndGemDigi*)(data) << std::endl;
	 }
	 new ((*myArray)[myArray->GetEntries()]) PndGemDigi(*(PndGemDigi*)(data));
}

FairRingSorter* PndGemDigiSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const{
	return new PndGemDigiRingSorter(numberOfCells, widthOfCells);
}
