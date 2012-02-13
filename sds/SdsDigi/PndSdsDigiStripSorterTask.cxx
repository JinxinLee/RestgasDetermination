/*
 * PndSdsDigiStripSorterTask.cxx
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#include <PndSdsDigiStripSorterTask.h>

#include "PndSdsDigiStrip.h"
#include "PndSdsDigiStripRingSorter.h"

ClassImp(PndSdsDigiStripSorterTask);

PndSdsDigiStripSorterTask::PndSdsDigiStripSorterTask() {
	// TODO Auto-generated constructor stub

}

PndSdsDigiStripSorterTask::~PndSdsDigiStripSorterTask() {
	// TODO Auto-generated destructor stub
}


void PndSdsDigiStripSorterTask::AddNewDataToTClonesArray(FairTimeStamp* data)
{
	 FairRootManager* ioman = FairRootManager::Instance();
	 TClonesArray* myArray = ioman->GetTClonesArray(fOutputBranch);
	 if (fVerbose > 1){
		 std::cout << "-I- PndSdsDigiStripSorterTask::AddNewDataToTClonesArray Data: " ;
		 std::cout <<  *(PndSdsDigiStrip*)(data) << std::endl;
	 }
	 new ((*myArray)[myArray->GetEntries()]) PndSdsDigiStrip(*(PndSdsDigiStrip*)(data));
}

FairRingSorter* PndSdsDigiStripSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const{
	return new PndSdsDigiStripRingSorter(numberOfCells, widthOfCells);
}
