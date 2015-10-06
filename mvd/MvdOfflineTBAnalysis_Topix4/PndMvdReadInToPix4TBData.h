/*
 * PndMvdReadInToPix4TBData.h
 *
 *  Created on: 23.10.2014
 *      Author: Stockmanns
 */

#ifndef PNDMVDREADINTOPIX4TBDATA_H_
#define PNDMVDREADINTOPIX4TBDATA_H_

#include "Rtypes.h"
#include "TClonesArray.h"
#include "TString.h"

#include "PndSdsDigiTopix4.h"
#include "PndSdsDigiTopix4Header.h"

#include "PndTopix4.h"
#include "PndHammingDecoder.h"

#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

//#include <unordered_map>

//#include "FairMQDevice.h

class TMrfData_8b;


class PndMvdReadInToPix4TBData {
public:
	PndMvdReadInToPix4TBData();
	virtual ~PndMvdReadInToPix4TBData();
	void SetFileName(std::vector<TString> fileName){
	  
	  std::cout << "number of entires " << fileName.size() << std::endl;
	  for(int i=0; i < fileName.size();i++)
	    {
	      std::cout << i<< " " << fileName[i] << std::endl;
	    }
	  fFileNames= fileName;
	}

	void SetFileName(TString fileName){
		std::vector<TString> names;
		names.push_back(fileName);
		SetFileName(names);
	}

	void Init();

	Bool_t ReadInData(TClonesArray* sdsDigiContainer, TClonesArray* headerContainer, TClonesArray* allheaderContainer);
	Bool_t ReadInRawData(std::ifstream* fileHandle, std::vector<ULong64_t>& rawData);//<input is fileHandle, output vector of raw data, output is end of file
	virtual Bool_t ReadInDataFromFile(TMrfData_8b*& data);
	std::vector<ULong64_t> GetRawData(TMrfData_8b* data);
	std::vector<std::vector<PndSdsDigiTopix4> > AnalyzeData(std::vector<ULong64_t>& rawData, Double_t clockFrequency);
	bool BuildFrame(ULong64_t& rawData);

	Int_t GetDeltaFrameCount();


	virtual void WriteoutToPix4Digi(PndSdsDigiTopix4& data);
	virtual void WriteoutToPix4Frames(std::vector<std::vector<PndSdsDigiTopix4> > &frames);

	PndSdsDigiTopix4 ProcessData(ULong64_t& data, ToPix4::frameHeader& header, Double_t& clockFrequency);
	std::vector<PndSdsDigiTopix4> AnalyzeToPixFrame(Double_t clockFrequency);
	bool CheckDataIntegrity(std::vector<ULong64_t> topix4Frame);

	void SetClockFrequency(Double_t val) {fClockFrequency = val;}
	void SetFE(Int_t val){fFE = val;}
	UInt_t GetNonSequenctialFC() const {return fNonSequentialFC;}
	UInt_t GetDoubleHeader() const {return fDoubleHeader;}
	UInt_t GetDoubleTrailer() const {return fDoubleTrailer;}
	UInt_t GetSuperFrameCount() const {return fSuperFrameCount;}


	UInt_t GetTotalHitCount() const {return fTotalHitCount;}
	UInt_t GetCorrectHitCount() const {return fCorrectHitCount;}
	UInt_t GetPreFrameLossHitCount() const {return fPreFrameLossHitCount;}
	UInt_t GetHammingLossHitCount() const {return fHammingLossHitCount;}
	UInt_t GetCRCLossHitCount() const {return fCRCLossHitCount;}

	UInt_t GetTotalFrameCount() const {return fTotalFrameCount;}
	UInt_t GetCorrectFrameCount() const {return fCorrectFrameCount;}
	UInt_t GetHammingLossFrameCount() const {return fHammingLossFrameCount;}
	UInt_t GetCRCLossFrameCount() const {return fCRCLossFrameCount;}
	
	UInt_t GetTotalHeaderCount() const {return fTotalHeaderCount;}
	UInt_t GetTotalTrailerCount() const {return fTotalTrailerCount;}
	

	void SetVerbose(Int_t val){fVerbose = val;}

private:
	std::vector<TString> fFileNames;
	std::ifstream* fFileHandle;
	Double_t fClockFrequency;
	TClonesArray* fOutputArray;
	TClonesArray* fOutputArrayHeader;
	TClonesArray* fOutputArrayAllHeader;
	UInt_t fSuperFrameCount;
	UInt_t fOldFrameCount;
	UInt_t fOldAllHeaderCount;
	UInt_t fNonSequentialFC;
	UInt_t fHammingLossFrameCount;
	UInt_t fCRCLossFrameCount;

	UInt_t fTotalHitCount;
	UInt_t fPreFrameLossHitCount;
	UInt_t fHammingLossHitCount;
	UInt_t fCRCLossHitCount;
	UInt_t fCorrectHitCount;

	ToPix4::frameHeader fRecentFrameHeader;
	ToPix4::frameHeader fRecentAllFrameHeader;
	ToPix4::frameTrailer fRecentFrameTrailer;
	Bool_t fFirstHeader;
	Bool_t fHeaderPresent;
	Bool_t fTrailerPresent;
	UInt_t fDoubleHeader;
	UInt_t fDoubleTrailer;

	UInt_t fTotalHeaderCount;
	UInt_t fTotalTrailerCount;

	Int_t fVerbose; // fVerbose==5 gives all detected errors from ToPix build in error detection
	Int_t fFE;
	Int_t fFileCounter;
	UInt_t fDataCount;
	UInt_t fTotalFrameCount;
	UInt_t fCorrectFrameCount;
	std::vector<ULong64_t> fToPixFrame;
	PndTopix4 fTopix;
	PndHammingDecoder fHamming;

	TClonesArray* fDigiArray;

};

#endif /* PNDMVDREADINTBDATA_H_ */
