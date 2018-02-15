/*
 * PndMQGapEventBuilderFTS.h
 *
 *  Created on: 14.10.2015
 *      Author: Stockmanns
 */

#ifndef PndMQGapEventBuilderFTS_H_
#define PndMQGapEventBuilderFTS_H_

#include "FairTimeStamp.h"

class PndMQGapEventBuilderFTS {
public:
	PndMQGapEventBuilderFTS();
	PndMQGapEventBuilderFTS(double timeGap);
	virtual ~PndMQGapEventBuilderFTS();

	void FillData(std::vector<FairTimeStamp*> data);
	std::vector<std::vector<FairTimeStamp*> > GetSeparatedData();
	std::vector<std::vector<FairTimeStamp*> > GetLastData();

private:
	double fTimeGap;
	std::vector<FairTimeStamp*> fData;
	std::vector<FairTimeStamp*> fPreviousData;
};

#endif /* PndMQGapEventBuilderFTS_H_ */
