/*
 * PndSciTHitSorterTask.h
 *
 *  Created on: May 13, 2015
 *      Author: Steinschaden Dominik
 *  Last update: 6.2015
 */

#ifndef PNDSCITHITSORTERTASK_H
#define PNDSCITHITSORTERTASK_H

#include "FairRingSorterTask.h"
#include "FairRingSorter.h"

class PndSciTHitSorterTask: public FairRingSorterTask {
public:
	PndSciTHitSorterTask();
	PndSciTHitSorterTask(const char* name):FairRingSorterTask(name){};
	PndSciTHitSorterTask(Int_t numberOfCells, Double_t widthOfCells, TString inputBranch, TString outputBranch, TString folderName):
		FairRingSorterTask(numberOfCells, widthOfCells, inputBranch, outputBranch, folderName){};

	virtual ~PndSciTHitSorterTask();

	virtual void AddNewDataToTClonesArray(FairTimeStamp* data);
	virtual FairRingSorter* InitSorter(Int_t numberOfCells, Double_t widthOfCells) const;

	ClassDef(PndSciTHitSorterTask, 1);
};

#endif /* PNDSCITHITSORTERTASK_H_ */
