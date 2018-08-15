/*
 * PndSciTHitSorterTask.cxx
 *
 *  Created on: May 13, 2015
 *      Author: Steinschaden Dominik
 *  Last update: 6.2015
 */

#include "PndSciTHitSorterTask.h"

#include "PndSciTHit.h"
#include "PndSciTHitRingSorter.h"

#include "TClonesArray.h"

ClassImp(PndSciTHitSorterTask);

PndSciTHitSorterTask::PndSciTHitSorterTask() {
	// TODO Auto-generated constructor stub

}

PndSciTHitSorterTask::~PndSciTHitSorterTask() {
	// TODO Auto-generated destructor stub
}


void PndSciTHitSorterTask::AddNewDataToTClonesArray(FairTimeStamp* data)
{
	 FairRootManager* ioman = FairRootManager::Instance();
	 TClonesArray* myArray = ioman->GetTClonesArray(fOutputBranch);
	 if (fVerbose > 1){
		 std::cout << "-I- PndSciTHitSorterTask::AddNewDataToTClonesArray Data: " ;
		 std::cout <<  *(PndSciTHit*)(data) << std::endl;
	 }
	 new ((*myArray)[myArray->GetEntries()]) PndSciTHit(*(PndSciTHit*)(data));
}

FairRingSorter* PndSciTHitSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const{
	return new PndSciTHitRingSorter(numberOfCells, widthOfCells);
}
