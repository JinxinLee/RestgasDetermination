/*
 * PndSdsDigiStripSorterTask.h
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#ifndef PNDSDSDigiStripSORTERTASK_H_
#define PNDSDSDigiStripSORTERTASK_H_

#include <FairRingSorterTask.h>

class PndSdsDigiStripSorterTask: public FairRingSorterTask {
public:
	PndSdsDigiStripSorterTask();
	PndSdsDigiStripSorterTask(const char* name):FairRingSorterTask(name){};
	PndSdsDigiStripSorterTask(Int_t numberOfCells, Double_t widthOfCells, TString inputBranch, TString outputBranch, TString folderName):
		FairRingSorterTask(numberOfCells, widthOfCells, inputBranch, outputBranch, folderName){};

	virtual ~PndSdsDigiStripSorterTask();

	virtual void AddNewDataToTClonesArray(FairTimeStamp* data);
	virtual FairRingSorter* InitSorter(Int_t numberOfCells, Double_t widthOfCells) const;

	ClassDef(PndSdsDigiStripSorterTask, 1);
};

#endif /* PNDSDSDigiStripSORTERTASK_H_ */
