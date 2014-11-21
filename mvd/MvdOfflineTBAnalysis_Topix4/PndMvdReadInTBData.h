/*
 * PndMvdReadInTBData.h
 *
 *  Created on: 23.10.2014
 *      Author: Stockmanns
 */

#ifndef PNDMVDREADINTBDATA_H_
#define PNDMVDREADINTBDATA_H_

#include "Rtypes.h"
#include "TClonesArray.h"
#include "TString.h"

#include "PndSdsDigiTopix4.h"

#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

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

class PndMvdReadInTBData {
public:
	PndMvdReadInTBData();
	virtual ~PndMvdReadInTBData();
	void SetFileName(TString fileName){
		fFileName = fileName;
	}

	void Init();

	Bool_t ReadInData(TClonesArray* sdsDigiContainer);
	Bool_t ReadInRawData(std::ifstream* fileHandle, std::vector<ULong_t>& rawData);//<input is fileHandle, output vector of raw data, output is end of file
	void AnalyzeData(std::vector<ULong_t>& rawData, Double_t clockFrequency);

	pixel BitAnalyzePixelData(ULong64_t& data);
	frameHeader BitAnalyzeHeader(ULong64_t& header);
	frameTrailer BitAnalyzeTrailer(ULong64_t& trailer);

	std::pair<UInt_t, UInt_t> PixeladdressToMatrixAddress(UInt_t pixelglobaladdress);

	void SetClockFrequency(Double_t val) {fClockFrequency = val;}
	void SetFE(Int_t val){fFE = val;}
	UInt_t GetNonSequenctialFC() const {return fNonSequentialFC;}
	UInt_t GetDoubleHeader() const {return fDoubleHeader;}
	UInt_t GetDoubleTrailer() const {return fDoubleTrailer;}
	UInt_t GetSuperFrameCount() const {return fSuperFrameCount;}
	UInt_t GetWrongHammingCodeCount() const {return fWrongHammingCodeCount;}

	PndSdsDigiTopix4 ProcessData(ULong64_t& data, frameHeader& header, Double_t& clockFrequency);
	ULong64_t ReflectBitsStream(ULong64_t crc, int bitnum);
	ULong64_t CalculateCRCTableFast(std::vector<char> p, ULong64_t len);
	void GenerateCRCTable();
	UShort_t CheckHammingCode(ULong64_t dataword, int dataword_length);
	ULong64_t ConvertToPix4HammingToStandardHamming(ULong64_t topixhamming);
	void AnalyzeToPixFrame(Double_t clockFrequency);
	void SetVerbose(Int_t val){fVerbose = val;}

private:
	TString fFileName;
	std::ifstream* fFileHandle;
	std::map<Int_t, Int_t> fChipIdMap;
	Double_t fClockFrequency;
	TClonesArray* fOutputArray;
	//TClonesArray* fOutputArrayHeader;
	UInt_t fSuperFrameCount;
	UInt_t fOldFrameCount;
	UInt_t fNonSequentialFC;
	UInt_t fWrongHammingCodeCount;

	frameHeader fRecentFrameHeader;
	frameTrailer fRecentFrameTrailer;
	Bool_t fFirstHeader;
	Bool_t fHeaderPresent;
	Bool_t fTrailerPresent;
	UInt_t fDoubleHeader;
	UInt_t fDoubleTrailer;
	UInt_t fDataLostCount;
	Int_t fVerbose; // fVerbose==5 gives all detected errors from ToPix build in error detection
	Int_t fFE;

	std::vector<ULong64_t> fToPixFrame;

	TClonesArray* fDigiArray;
	// CRC parameters :

	const UInt_t order;
	const ULong64_t polynom;
	const UInt_t direct;
	const ULong64_t crcinit;
	const ULong64_t crcxor;
	const UInt_t refin;
	const UInt_t refout;

	ULong64_t crcmask;
	ULong64_t crchighbit;
	ULong64_t crcinit_direct;
	ULong64_t crcinit_nondirect;
	ULong64_t crctab[256];


};

#endif /* PNDMVDREADINTBDATA_H_ */
