/*
 * PndSdsDigiPixelWriteoutBuffer.h
 *
 *  Created on: May 10, 2011
 *      Author: stockman
 */

#ifndef PNDSDSDIGIPIXELWRITEOUTBUFFER_H_
#define PNDSDSDIGIPIXELWRITEOUTBUFFER_H_

#include "PndWriteoutBufferT.h"
#include "PndSdsDigiPixel.h"

class PndSdsDigiPixelWriteoutBuffer : public PndWriteoutBufferT<PndSdsDigiPixel>{
public:
	PndSdsDigiPixelWriteoutBuffer();
	PndSdsDigiPixelWriteoutBuffer(TString branchName);

	virtual ~PndSdsDigiPixelWriteoutBuffer();

	virtual std::vector<std::pair<double, PndSdsDigiPixel> > Modify(std::pair<double, PndSdsDigiPixel> oldData, std::pair<double, PndSdsDigiPixel> newData);

	ClassDef(PndSdsDigiPixelWriteoutBuffer, 1);
};

#endif /* PNDSDSDIGIPIXELWRITEOUTBUFFER_H_ */
