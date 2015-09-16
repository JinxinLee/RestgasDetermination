/*
 * PndHammingEncoder.h
 *
 *  Created on: 16.09.2015
 *      Author: Stockmanns
 */

#ifndef MVD_MVDOFFLINETBANALYSIS_TOPIX4_PNDHAMMINGDECODER_H_
#define MVD_MVDOFFLINETBANALYSIS_TOPIX4_PNDHAMMINGDECODER_H_

#include "Rtypes.h"

class PndHammingDecoder {
public:
	PndHammingDecoder();
	virtual ~PndHammingDecoder();

	UShort_t CheckHammingCode(ULong64_t dataword, int dataword_length);
	ULong64_t CalculateCRCTableFast(std::vector<char> p, ULong64_t len);
	ULong64_t ReflectBitsStream(ULong64_t crc, int bitnum);

protected:
	void GenerateCRCTable();

private:
	// CRC parameters :

	const UInt_t fOrder;
	const ULong64_t fPolynom;
	const ULong64_t fCRCXor;
	const UInt_t fRefIn;
	const UInt_t fRefOut;

	ULong64_t fCRCMask;
	ULong64_t fCRCHighBit;
	ULong64_t fCRCInit_direct;
	ULong64_t fCRCTab[256];
};

#endif /* MVD_MVDOFFLINETBANALYSIS_TOPIX4_PNDHAMMINGDECODER_H_ */
