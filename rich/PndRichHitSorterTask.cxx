/*
 *      Author: stockman
 * PndRichHitSorterTask.cxx
 *
 */
// -------------------------------------------------------------------------
// -----                   PndRichHitSorterTask source file            -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------

#include <PndRichHitSorterTask.h>

#include "PndRichPDHit.h"
#include "PndRichHitRingSorter.h"

ClassImp(PndRichHitSorterTask);

PndRichHitSorterTask::PndRichHitSorterTask() {
	// TODO Auto-generated constructor stub

}

PndRichHitSorterTask::~PndRichHitSorterTask() {
	// TODO Auto-generated destructor stub
}


void PndRichHitSorterTask::AddNewDataToTClonesArray(FairTimeStamp* data)
{
	 FairRootManager* ioman = FairRootManager::Instance();
	 TClonesArray* myArray = ioman->GetTClonesArray(fOutputBranch);
	 if (fVerbose > 1){
		 std::cout << "-I- PndRichHitSorterTask::AddNewDataToTClonesArray Data: " ;
		 std::cout <<  *(PndRichPDHit*)(data) << std::endl;
	 }
	 new ((*myArray)[myArray->GetEntries()]) PndRichPDHit(*(PndRichPDHit*)(data));
}

FairRingSorter* PndRichHitSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const{
	return new PndRichHitRingSorter(numberOfCells, widthOfCells);
}
