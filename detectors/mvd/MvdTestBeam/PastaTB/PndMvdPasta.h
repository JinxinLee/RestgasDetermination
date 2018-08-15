/*
 * PndMvdPasta.h
 *
 *  Created on: 23.09.2017
 *      Author: Stockmanns
 */

#ifndef MVD_MVDTESTBEAM_PASTATB_PNDMVDPASTA_H_
#define MVD_MVDTESTBEAM_PASTATB_PNDMVDPASTA_H_

#include "PndMvdPastaDigi.h"
#include "TObject.h"

class PndMvdPasta : public TObject {
public:
	PndMvdPasta();
	virtual ~PndMvdPasta();

	ThresholdDataFullMode AnalyzeThresholdWordFull(ULong64_t word);
	FrameHeader AnalyzeHeader(ULong64_t word);
	std::vector<char> ConvertData(std::vector<ULong64_t> frame);

	ClassDef(PndMvdPasta,1);
};

#endif /* MVD_MVDTESTBEAM_PASTATB_PNDMVDPASTA_H_ */
