/*
 * PndFtsHitWriteoutBuffer.h
 *
 *  Created on: May 10, 2011
 *      Author: stockman
 */

#ifndef PndFtsHitWRITEOUTBUFFER_H_
#define PndFtsHitWRITEOUTBUFFER_H_

#include "FairWriteoutBuffer.h"
#include "PndFtsHit.h"

class FairTimeStamp;

class PndFtsHitWriteoutBuffer : public FairWriteoutBuffer{

public:
	PndFtsHitWriteoutBuffer();
	PndFtsHitWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance);
	void AddNewDataToTClonesArray(FairTimeStamp*);

	virtual ~PndFtsHitWriteoutBuffer();


	virtual double FindTimeForData(FairTimeStamp* data) ;
	virtual void FillDataMap(FairTimeStamp* data, double activeTime) ;
	virtual void EraseDataFromDataMap(FairTimeStamp* data);
protected:	

	std::map<PndFtsHit, double> fData_map;
	
	ClassDef(PndFtsHitWriteoutBuffer, 1);
};

#endif /* PndFtsHitWRITEOUTBUFFER_H_ */
