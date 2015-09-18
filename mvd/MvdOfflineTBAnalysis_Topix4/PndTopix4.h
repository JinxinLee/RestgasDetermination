/*
 * PndTopix4.h
 *
 *  Created on: 16.09.2015
 *      Author: Stockmanns
 */

#ifndef MVD_MVDOFFLINETBANALYSIS_TOPIX4_PNDTOPIX4_H_
#define MVD_MVDOFFLINETBANALYSIS_TOPIX4_PNDTOPIX4_H_

#include "Rtypes.h"

namespace ToPix4{
	struct frameHeader{
		frameHeader():fChipAddress(0), fFrameCount(0), fECC(0){};

		UInt_t fChipAddress;
		UInt_t fFrameCount;
		UInt_t fECC;

	};

	struct frameTrailer{
		frameTrailer():fNEvents(0), fFrameCRC(0), fECC(0){};

		UInt_t fNEvents;
		UInt_t fFrameCRC;
		UInt_t fECC;
	};

	struct pixel{
		pixel():fPixelAddress(0), fLeadingEdge(0), fTrailingEdge(0){};

		UInt_t fPixelAddress;
		UInt_t fLeadingEdge;
		UInt_t fTrailingEdge;
	};
}

class PndTopix4 {
public:
	PndTopix4();
	virtual ~PndTopix4();

	ULong64_t ConvertToPix4HammingToStandardHamming(ULong64_t topixhamming);

	int GetHeader(ULong64_t data);
	int GetFrameCount(ULong64_t data);
	ToPix4::pixel BitAnalyzePixelData(ULong64_t& data);
	ToPix4::frameHeader BitAnalyzeHeader(ULong64_t& header);
	ToPix4::frameTrailer BitAnalyzeTrailer(ULong64_t& trailer);

	std::pair<UInt_t, UInt_t> PixeladdressToMatrixAddress(UInt_t pixelglobaladdress);


};

#endif /* MVD_MVDOFFLINETBANALYSIS_TOPIX4_PNDTOPIX4_H_ */
