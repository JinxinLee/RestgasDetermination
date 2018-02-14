/*
 *      Author: stockman
 * PndDrcDigiSorterTask.cxx
 *
 */
// -------------------------------------------------------------------------
// -----                   PndDrcDigiSorterTask source file            -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------

#include <PndDrcDigiSorterTask.h>

#include "PndDrcDigi.h"
#include "PndDrcDigiRingSorter.h"

ClassImp(PndDrcDigiSorterTask);

PndDrcDigiSorterTask::PndDrcDigiSorterTask() {
	// TODO Auto-generated constructor stub

}

PndDrcDigiSorterTask::~PndDrcDigiSorterTask() {
	// TODO Auto-generated destructor stub
}


void PndDrcDigiSorterTask::AddNewDataToTClonesArray(FairTimeStamp* data)
{
	 FairRootManager* ioman = FairRootManager::Instance();
	 TClonesArray* myArray = ioman->GetTClonesArray(fOutputBranch);
	 if (fVerbose > 1){
		 std::cout << "-I- PndDrcDigiSorterTask::AddNewDataToTClonesArray Data: " ;
		 std::cout <<  *(PndDrcDigi*)(data) << std::endl;
	 }
	 new ((*myArray)[myArray->GetEntries()]) PndDrcDigi(*(PndDrcDigi*)(data));
}

FairRingSorter* PndDrcDigiSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const{
	return new PndDrcDigiRingSorter(numberOfCells, widthOfCells);
}
