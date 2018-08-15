/*
 * PndEmcDigiSorterTask.h
 */

#ifndef PNDEMCDIGISORTERTASK_H_
#define PNDEMCDIGISORTERTASK_H_

#include <FairRingSorterTask.h>
#include <FairRingSorter.h>

class PndEmcDigiSorterTask: public FairRingSorterTask {
public:
	PndEmcDigiSorterTask();
	PndEmcDigiSorterTask(const char* name):FairRingSorterTask(name){};
	PndEmcDigiSorterTask(Int_t numberOfCells, Double_t widthOfCells, TString inputBranch, TString outputBranch, TString folderName):
		FairRingSorterTask(numberOfCells, widthOfCells, inputBranch, outputBranch, folderName){};

	virtual ~PndEmcDigiSorterTask();

	virtual void AddNewDataToTClonesArray(FairTimeStamp* data);
	virtual FairRingSorter* InitSorter(Int_t numberOfCells, Double_t widthOfCells) const;

	ClassDef(PndEmcDigiSorterTask, 1);
};

#endif /* PNDEMCDIGISORTERTASK_H_ */
