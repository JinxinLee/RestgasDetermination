/*
 * PndEmcDigiWriteoutBuffer.h
 */

#ifndef PNDEMCDIGIWRITEOUTBUFFER_H_
#define PNDEMCDIGIWRITEOUTBUFFER_H_

#include "FairWriteoutBuffer.h"
#include "PndEmcDigi.h"

class FairTimeStamp;

class PndEmcDigiWriteoutBuffer : public FairWriteoutBuffer{

public:
	PndEmcDigiWriteoutBuffer();
	PndEmcDigiWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance);
	void AddNewDataToTClonesArray(FairTimeStamp*);

	virtual ~PndEmcDigiWriteoutBuffer();

	virtual double FindTimeForData(FairTimeStamp* data) ;
	virtual void FillDataMap(FairTimeStamp* data, double activeTime) ;
	virtual void EraseDataFromDataMap(FairTimeStamp* data);
protected:	

	std::map<PndEmcDigi, double> fData_map;
	
	ClassDef(PndEmcDigiWriteoutBuffer, 1);
};

#endif /* PNDEMCDIGIWRITEOUTBUFFER_H_ */
