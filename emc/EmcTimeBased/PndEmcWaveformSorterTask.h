/*
 * PndEmcWaveformSorterTask.h
 */

#ifndef PNDEMCWAVEFORMSORTERTASK_H_
#define PNDEMCWAVEFORMSORTERTASK_H_

#include <FairRingSorterTask.h>
#include <FairRingSorter.h>

class PndEmcWaveformSorterTask: public FairRingSorterTask {
public:
	PndEmcWaveformSorterTask();
	PndEmcWaveformSorterTask(const char* name):FairRingSorterTask(name){};
	PndEmcWaveformSorterTask(Int_t numberOfCells, Double_t widthOfCells, TString inputBranch, TString outputBranch, TString folderName):
		FairRingSorterTask(numberOfCells, widthOfCells, inputBranch, outputBranch, folderName){};

	virtual ~PndEmcWaveformSorterTask();

	virtual void AddNewDataToTClonesArray(FairTimeStamp* data);
	virtual FairRingSorter* InitSorter(Int_t numberOfCells, Double_t widthOfCells) const;

	ClassDef(PndEmcWaveformSorterTask, 1);
};

#endif /* PNDEMCWAVEFORMSORTERTASK_H_ */
