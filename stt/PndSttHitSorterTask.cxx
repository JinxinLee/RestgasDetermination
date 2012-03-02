/*
 * PndSttHitSorterTask.cxx
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#include <PndSttHitSorterTask.h>

#include "PndSttHit.h"
#include "PndSttHitRingSorter.h"

ClassImp(PndSttHitSorterTask);

PndSttHitSorterTask::PndSttHitSorterTask() {
	// TODO Auto-generated constructor stub

}

PndSttHitSorterTask::~PndSttHitSorterTask() {
	// TODO Auto-generated destructor stub
}


void PndSttHitSorterTask::AddNewDataToTClonesArray(FairTimeStamp* data)
{
	 FairRootManager* ioman = FairRootManager::Instance();
	 TClonesArray* myArray = ioman->GetTClonesArray(fOutputBranch);
	 if (fVerbose > 1){
		 std::cout << "-I- PndSttHitSorterTask::AddNewDataToTClonesArray Data: " ;
		 std::cout <<  *(PndSttHit*)(data) << std::endl;
	 }
	 new ((*myArray)[myArray->GetEntries()]) PndSttHit(*(PndSttHit*)(data));
}

FairRingSorter* PndSttHitSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const{
	return new PndSttHitRingSorter(numberOfCells, widthOfCells);
}
