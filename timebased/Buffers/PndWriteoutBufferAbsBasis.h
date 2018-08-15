/*
 * PndWriteoutBufferAbsBasis.h
 *
 *  Created on: Jul 18, 2011
 *      Author: stockman
 */

#ifndef PNDWRITEOUTBUFFERABSBASIS_H_
#define PNDWRITEOUTBUFFERABSBASIS_H_

#include "TObject.h"

class PndWriteoutBufferAbsBasis : public TObject{
public:
	virtual ~PndWriteoutBufferAbsBasis(){};

	virtual void WriteOutData(double time) = 0;
	virtual void WriteOutAllData() = 0;

};

#endif /* PNDWRITEOUTBUFFERABSBASIS_H_ */
