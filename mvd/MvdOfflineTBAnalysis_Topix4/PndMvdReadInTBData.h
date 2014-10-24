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
	pixel():fPixelAddress(-1), fLeadingEdge(0), fTrailingEdge(0){};

	Int_t fPixelAddress;
	UInt_t fLeadingEdge;
	UInt_t fTrailingEdge;
};

class PndMvdReadInTBData {
public:
	PndMvdReadInTBData();
	virtual ~PndMvdReadInTBData();
	void SetFileNames(std::vector<TString> fileNames){
		fFileNames = fileNames;
		std::cout << "PndMvdReadInTBData::SetFileNames " << std::endl;
		for (int i = 0; i < fFileNames.size(); i++) std::cout << fFileNames[i] << std::endl;
	}
	void Init();
	Bool_t ReadInData(TClonesArray* sdsDigiContainer);
	Bool_t ReadInRawData(std::ifstream* fileHandle, std::vector<ULong_t>& rawData);//<input is fileHandle, output vector of raw data, output is end of file
	void AnalyzeData(std::vector<ULong_t>& rawData, Double_t clockFrequency);

	pixel BitAnalyzePixelData(ULong_t& data);
	frameHeader BitAnalyzeHeader(ULong_t& header);
	frameTrailer BitAnalyzeTrailer(ULong_t& trailer);

	std::pair<UInt_t, UInt_t> PixeladdressToMatrixAddress(UInt_t pixelglobaladdress);

	PndSdsDigiTopix4 ProcessData(ULong_t& data, frameHeader& header, Double_t& clockFrequency);
private:
	std::vector<TString> fFileNames;
	std::vector<std::ifstream*> fFileHandles;
	std::map<Int_t, Int_t> fChipIdMap;
	Double_t fClockFrequency;
	TClonesArray* fOutputArray;

	TClonesArray* fDigiArray;
};

#endif /* PNDMVDREADINTBDATA_H_ */
