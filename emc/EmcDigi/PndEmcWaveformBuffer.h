/*
 * PndEmcWaveformBuffer.h
 */

#ifndef PNDEMCWAVEFORMBUFFER_H_
#define PNDEMCWAVEFORMBUFFER_H_

#include "FairWriteoutBuffer.h"

class FairTimeStamp;
class PndEmcWaveformData;

class TClonesArray;

class PndEmcWaveformBuffer : public FairWriteoutBuffer {

public:
	PndEmcWaveformBuffer();
	PndEmcWaveformBuffer(TString branchName, TString className, TString folderName, Bool_t persistance);
	void AddNewDataToTClonesArray(FairTimeStamp*);

	virtual ~PndEmcWaveformBuffer();

	virtual double FindTimeForData(FairTimeStamp* data) ;
	virtual void FillDataMap(FairTimeStamp* data, double activeTime) ;
	virtual void EraseDataFromDataMap(FairTimeStamp* data);

	virtual void DeleteOldData();
	virtual void StoreWaveformData(TString branchName, TString folderName, bool persistance);

	virtual void FillNewData(PndEmcWaveformData*);

protected:	

	std::map<PndEmcWaveformData, double> fData_map;
	std::vector<std::pair<double, FairTimeStamp*> > Modify(std::pair<double, FairTimeStamp*> oldData, std::pair<double, FairTimeStamp* > newData);

	ClassDef(PndEmcWaveformBuffer, 0);

	bool fStoreWaveformData;
	TClonesArray* fWfDataArray;
	TString fWfDataBranchName;
};

#endif /* PNDEMCWAVEFORMBUFFER_H*/
