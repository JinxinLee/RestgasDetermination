/*
 *      Author: stockman
 * PndDrcDigiSorterTask.h
 *
 */
// -------------------------------------------------------------------------
// -----                   PndDrcDigiSorterTask header file            -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------

#ifndef PNDDRCDigiSORTERTASK_H_
#define PNDDRCDigiSORTERTASK_H_

#include "TClonesArray.h"

#include <FairRingSorterTask.h>
#include <FairRingSorter.h>

class PndDrcDigiSorterTask: public FairRingSorterTask {
public:
	PndDrcDigiSorterTask();
	PndDrcDigiSorterTask(const char* name):FairRingSorterTask(name){};
	PndDrcDigiSorterTask(Int_t numberOfCells, Double_t widthOfCells, TString inputBranch, TString outputBranch, TString folderName):
		FairRingSorterTask(numberOfCells, widthOfCells, inputBranch, outputBranch, folderName){};

	virtual ~PndDrcDigiSorterTask();

	virtual void AddNewDataToTClonesArray(FairTimeStamp* data);
	virtual FairRingSorter* InitSorter(Int_t numberOfCells, Double_t widthOfCells) const;

	ClassDef(PndDrcDigiSorterTask, 1);
};

#endif /* PNDDrcDigiSORTERTASK_H_ */
