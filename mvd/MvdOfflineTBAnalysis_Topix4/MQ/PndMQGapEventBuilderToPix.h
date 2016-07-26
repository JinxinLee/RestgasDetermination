/*
 * PndMQGapEventBuilder.h
 *
 *  Created on: 14.10.2015
 *      Author: Stockmanns
 */

#ifndef MVD_MVDOFFLINETBANALYSIS_TOPIX4_MQ_PNDMQGAPEVENTBUILDERTOPIX_H_
#define MVD_MVDOFFLINETBANALYSIS_TOPIX4_MQ_PNDMQGAPEVENTBUILDERTOPIX_H_

#include "PndSdsDigiTopix4.h"

class PndMQGapEventBuilderToPix {
public:
	PndMQGapEventBuilderToPix();
	PndMQGapEventBuilderToPix(double timeGap);
	virtual ~PndMQGapEventBuilderToPix();

	void FillData(std::vector<PndSdsDigiTopix4> data);
	std::vector<std::vector<PndSdsDigiTopix4> > GetSeparatedData();
	std::vector<std::vector<PndSdsDigiTopix4> > GetLastData();

private:
	double fTimeGap;
	std::vector<PndSdsDigiTopix4> fData;
	std::vector<PndSdsDigiTopix4> fPreviousData;
};

#endif /* MVD_MVDOFFLINETBANALYSIS_TOPIX4_MQ_PNDMQGAPEVENTBUILDERTOPIX_H_ */
