/*
 * PndSciTHitWriteoutBuffer.h
 *
 *  Created on: May 13, 2015
 *      Author: Steinschaden Dominik
 *  Last update: 6.2015
 */

#ifndef PNDSCITHITWRITEOUTBUFFER_H
#define PNDSCITHITWRITEOUTBUFFER_H

#include "FairWriteoutBuffer.h"

#include "PndSciTHit.h"

class FairTimeStamp;

class PndSciTHitWriteoutBuffer : public FairWriteoutBuffer{

public:
	PndSciTHitWriteoutBuffer();	
        PndSciTHitWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance);

	virtual ~PndSciTHitWriteoutBuffer();

	void AddNewDataToTClonesArray(FairTimeStamp*);

	virtual double FindTimeForData(FairTimeStamp* data) ;

	virtual void FillDataMap(FairTimeStamp* data, double activeTime) ;

	virtual void EraseDataFromDataMap(FairTimeStamp* data);

//	virtual std::vector<std::pair<double, FairTimeStamp*> > Modify(std::pair<double,        	FairTimeStamp*> oldData, std::pair<double, FairTimeStamp*> newData);

// Modify methode überschreiben falls Hits in der recovery time nich einfach ignoriert werden sollen sondern zu einem pile up etc. führen.

protected:	

	std::map<PndSciTHit, double> fData_map;
	
	ClassDef(PndSciTHitWriteoutBuffer, 1);
};

#endif /* PNDSCITHITWRITEOUTBUFFER_H */
