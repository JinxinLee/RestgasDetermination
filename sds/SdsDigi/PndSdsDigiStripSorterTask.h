/*
 * PndSdsDigiStripSorterTask.h
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#ifndef PNDSDSDigiStripSORTERTASK_H_
#define PNDSDSDigiStripSORTERTASK_H_

#include <PndSorterTask.h>

class PndSdsDigiStripSorterTask: public PndSorterTask {
public:
	PndSdsDigiStripSorterTask();
	PndSdsDigiStripSorterTask(const char* name):PndSorterTask(name){};
	PndSdsDigiStripSorterTask(Int_t numberOfCells, Double_t widthOfCells, TString inputBranch, TString outputBranch, TString folderName):
		PndSorterTask(numberOfCells, widthOfCells, inputBranch, outputBranch, folderName){};

	virtual ~PndSdsDigiStripSorterTask();

	virtual void AddNewDataToTClonesArray(FairTimeStamp* data);
	virtual PndRingSorter* InitSorter(Int_t numberOfCells, Double_t widthOfCells);

	ClassDef(PndSdsDigiStripSorterTask, 1);
};

#endif /* PNDSDSDigiStripSORTERTASK_H_ */
