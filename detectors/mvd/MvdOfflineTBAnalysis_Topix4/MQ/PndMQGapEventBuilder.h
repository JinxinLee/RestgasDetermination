/*
 * PndMQGapEventBuilder.h
 *
 *  Created on: 14.10.2015
 *      Author: Stockmanns
 */

#ifndef MVD_MVDOFFLINETBANALYSIS_TOPIX4_MQ_PNDMQGAPEVENTBUILDER_H_
#define MVD_MVDOFFLINETBANALYSIS_TOPIX4_MQ_PNDMQGAPEVENTBUILDER_H_

#include "PndSdsDigiTopix4.h"

class PndMQGapEventBuilder {
public:
	PndMQGapEventBuilder();
	PndMQGapEventBuilder(double timeGap);
	virtual ~PndMQGapEventBuilder();

	void FillData(std::vector<PndSdsDigiTopix4> data);
	std::vector<std::vector<PndSdsDigiTopix4> > GetSeparatedData();
	std::vector<std::vector<PndSdsDigiTopix4> > GetLastData();

private:
	double fTimeGap;
	std::vector<PndSdsDigiTopix4> fData;
	std::vector<PndSdsDigiTopix4> fPreviousData;
};

#endif /* MVD_MVDOFFLINETBANALYSIS_TOPIX4_MQ_PNDMQGAPEVENTBUILDER_H_ */
