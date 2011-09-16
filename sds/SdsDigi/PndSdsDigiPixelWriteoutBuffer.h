/*
 * PndSdsDigiPixelWriteoutBuffer.h
 *
 *  Created on: May 10, 2011
 *      Author: stockman
 */

#ifndef PNDSDSDIGIPIXELWRITEOUTBUFFER_H_
#define PNDSDSDIGIPIXELWRITEOUTBUFFER_H_

#include "FairWriteoutBuffer.h"
#include "PndSdsDigiPixel.h"

class FairTimeStamp;

class PndSdsDigiPixelWriteoutBuffer : public FairWriteoutBuffer{
public:
	PndSdsDigiPixelWriteoutBuffer();
	PndSdsDigiPixelWriteoutBuffer(TString branchName);

	virtual ~PndSdsDigiPixelWriteoutBuffer();

	virtual std::vector<std::pair<double, PndSdsDigiPixel> > Modify(std::pair<double, PndSdsDigiPixel> oldData, std::pair<double, PndSdsDigiPixel> newData);

	ClassDef(PndSdsDigiPixelWriteoutBuffer, 1);

        void AddNewDataToTClonesArray(FairTimeStamp*){;}
        double FindTimeForData(FairTimeStamp*){;}
        void FillDataMap(FairTimeStamp*, double){;}
       void EraseDataFromDataMap(FairTimeStamp*){;}


};

#endif /* PNDSDSDIGIPIXELWRITEOUTBUFFER_H_ */
