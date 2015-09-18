/*
 * PndMvdReadInToPix4TBData.cxx
 *
 *  Created on: 23.10.2014
 *      Author: Stockmanns
 */

#include <MvdOfflineTBAnalysis_Topix4/PndMvdReadInToPix4TBData.h>

#include "mrfdata_8b.h"
#include "mrftools.h"
#include "boost/archive/binary_oarchive.hpp"
#include "boost/archive/binary_iarchive.hpp"
#include "boost/serialization/binary_object.hpp"
#include <boost/archive/archive_exception.hpp>

using namespace ToPix4;

PndMvdReadInToPix4TBData::PndMvdReadInToPix4TBData() : fDigiArray(0), fClockFrequency(0), fSuperFrameCount(0),
					   fOldFrameCount(0), fOldAllHeaderCount(0), fFirstHeader(kTRUE), fFE(-1),
					   fNonSequentialFC(0), fHammingLossFrameCount(0), fCRCLossFrameCount(0),
					   fTotalHitCount(0),fPreFrameLossHitCount(0), fHammingLossHitCount(0), fCRCLossHitCount(0), fCorrectHitCount(0),
					   fHeaderPresent(kFALSE), fTrailerPresent(kFALSE), fDoubleHeader(0), fDoubleTrailer(0), fVerbose(0),
					   fDataCount(0), fFileCounter(0), fTotalFrameCount(0), fTotalHeaderCount(0), fTotalTrailerCount(0) {

}

PndMvdReadInToPix4TBData::~PndMvdReadInToPix4TBData() {
  fFileHandle->close();
  delete(fFileHandle);
}


void PndMvdReadInToPix4TBData::Init(){
	//std::cout << "PndMvdReadInToPix4TBData::Init called" << std::endl;
//	for (int i = 0; i < fFileName.size(); i++){
	 std::ifstream* ifs = new std::ifstream(fFileNames[fFileCounter], std::ios::binary);
	 std::cout << "File: " << fFileNames[fFileCounter] << " is good: " << ifs->good() << std::endl;
	 fFileCounter++;
	 fFileHandle=ifs;
//	}
}

Bool_t PndMvdReadInToPix4TBData::ReadInData(TClonesArray* sdsDigiContainer, TClonesArray* headerContainer, TClonesArray* allheaderContainer){
	TMrfData_8b* tempdata;
	tempdata = new TMrfData_8b;
	ULong_t dataword=0;
	Bool_t endOfFile = kFALSE;

	fOutputArray = sdsDigiContainer;
	fOutputArrayHeader = headerContainer;
	fOutputArrayAllHeader = allheaderContainer;
	std::vector<ULong64_t> rawArray;
	endOfFile |= ReadInRawData(fFileHandle, rawArray);
	AnalyzeData(rawArray, fClockFrequency);
	return endOfFile;
}

Bool_t PndMvdReadInToPix4TBData::ReadInRawData(std::ifstream* fileHandle, std::vector<ULong64_t>& rawData) {
	TMrfData_8b* tempdata;
	tempdata = new TMrfData_8b;
	ULong_t dataword = 0;
	Bool_t endOfFile = kFALSE;
	fVerbose = 3;
	if (fileHandle->good()) {
		if (fVerbose > 2) {
			std::cout << std::endl;
			std::cout << "PndMvdReadInToPix4TBData::ReadInRawData reading file "
					<< std::endl;
		}
		try {
			boost::archive::binary_iarchive iar(*fileHandle); //this line causes an "Invalid Signature Error" at the end of the file but the file is still good
			iar >> tempdata;
		} catch (boost::archive::archive_exception& exception) {
			if (fVerbose > 1) {
				std::cout << "PndMvdReadInToPix4TBData::ReadInRawData: Error found in reading file "
						<< " : " << fileHandle->good() << " "
						<< fileHandle->eof() << " Exception: " << exception.code << " " << exception.what()
						<< std::endl;
			}

			if (exception.code == 3) {
				if (fFileCounter < fFileNames.size()) {
					std::cout << fFE << " open new file " << fFileNames[fFileCounter] << std::endl;
					fileHandle->close();
					delete (fFileHandle);
					std::ifstream* ifs = new std::ifstream(fFileNames[fFileCounter], std::ios::binary);
					fFileCounter++;
					return endOfFile;
				} else {
					std::cout << fFE << " All files read! Finishing FE " << std::endl;
					endOfFile = kTRUE;
					return endOfFile;
				}
			}
		}
		if (fVerbose > 2)
			std::cout << fFE << " PndMvdReadInToPix4TBData::ReadInRawData: NWords: " << tempdata->getNumWords() << std::endl;
		for (UInt_t i = 0; i < tempdata->getNumWords(); i += 5) {
			dataword = 0;
			for (uint j = 0; j < 5; j++) {
				dataword = dataword << 8;
				dataword += tempdata->getWord(i + j);
			}
			rawData.push_back(dataword);

			if (fVerbose > 2) {
				ULong_t frameCount = -1;
				ULong64_t header = fTopix.GetHeader(dataword);
				if (header == 1)
					frameCount = fTopix.GetFrameCount(dataword);

				std::cout << std::dec << "dataword No " << i / 5 << "/"	<< tempdata->getNumWords() / 5 << ": " << std::hex
						<< dataword << " " << std::dec << header << " : " << frameCount << std::endl;
			}
		}
	} else {
		std::cout << fFE << " An error occured " << std::endl;
		std::cout << fFE << " fileHandle->good() " << fileHandle->good() << std::endl;
		std::cout << fFE << " fileHandle->eof()  " << fileHandle->eof()	<< std::endl;
		std::cout << fFE << " fileHandle->fail() " << fileHandle->fail() << std::endl;
		std::cout << fFE << " fileHandle->bad()  " << fileHandle->bad()	<< std::endl;

		endOfFile = kFALSE;
		return endOfFile;
	}
	return endOfFile;
}

bool PndMvdReadInToPix4TBData::BuildFrame(ULong64_t& rawData)
{
	int header = fTopix.GetHeader(rawData);
	if (fFirstHeader) {	// if data stream does not start with a header all data is thrown away until a header is present
		if (header == 1) {
			fFirstHeader = kFALSE;
		} else {
			return false;
		}
	}

	if (header == 1) // header word found
	{
		fTotalHeaderCount++;
		fRecentAllFrameHeader = fTopix.BitAnalyzeHeader(rawData);

		Int_t deltaAllFrameCount = ((int) (fRecentAllFrameHeader.fFrameCount - fOldAllHeaderCount) < 0 ?
						((fRecentAllFrameHeader.fFrameCount	- fOldAllHeaderCount) + 256) :
						(fRecentAllFrameHeader.fFrameCount - fOldAllHeaderCount));
		new ((*fOutputArrayAllHeader)[fOutputArrayAllHeader->GetEntriesFast()]) PndSdsDigiTopix4Header(fRecentAllFrameHeader.fFrameCount, fFE,
				fRecentAllFrameHeader.fChipAddress, fRecentAllFrameHeader.fECC, fTotalHeaderCount, deltaAllFrameCount, 0, 0);
		if (deltaAllFrameCount > 1)
			std::cout << "-W- deltaAllFrameCount > 1: "	<< deltaAllFrameCount << std::endl;
		fOldAllHeaderCount = fRecentAllFrameHeader.fFrameCount;

		if (fHeaderPresent == kTRUE) {
			// double header found, cannot check previous data without trailer, clear vector
			fDoubleHeader++;

			if (fVerbose > 1) {
				std::cout << "Double Header Found! count: " << fDoubleHeader << "| FE: " << fFE << std::hex << " last ToPixFrame element: "
						<< fToPixFrame.back() << " new frame header " << rawData << std::endl;
			}
			fPreFrameLossHitCount += fToPixFrame.size() - 1;
			fToPixFrame.clear();
			// load new header into vector
			fToPixFrame.push_back(rawData);
		} else {
			// header found, start recording topix frame
			fTrailerPresent = kFALSE;
			fHeaderPresent = kTRUE;
			fToPixFrame.push_back(rawData);
		}
	}

	else if (header == 2) // trailer word found
	{
		fTotalTrailerCount++;
		if (fTrailerPresent == kTRUE) {
			// double trailer found, cannot give the hits a valid timestamp without the header, clear vector

			fToPixFrame.clear();
			fDoubleTrailer++;
			if (fVerbose > 1) {
				std::cout << "Double Trailer Found! Double header counter: " << fDoubleTrailer << std::endl;
			}
		} else {
			if (fHeaderPresent == kTRUE) {
				// one topix frame found! Go and analyze the vector...
				fHeaderPresent = kFALSE;
				fTrailerPresent = kTRUE;
				fToPixFrame.push_back(rawData);
				//  if (fVerbose > 1)
				//  {
				//	std::cout << "ToPix Frame found! Go and analyze this amount of data: " << fToPixFrame.size() << std::endl;
				//}

				fTotalFrameCount++;
				//AnalyzeToPixFrame(clockFrequency);
				fToPixFrame.clear();
				return true;
			} else {
				if (fVerbose > 1) {
					std::cout << "Trailer without header found! Double header counter: " << fDoubleHeader << std::endl;
				}
				// trailer without header, can happen at the beginning of the file or the header was not detected correctly
				// this case is in principle impossible to enter
				fPreFrameLossHitCount += fToPixFrame.size() - 1;
				fDoubleTrailer++;
				fToPixFrame.clear();
			}
		}
	} else if (header == 3) // data word found
			{
		fTotalHitCount++;
		if (fHeaderPresent == kTRUE) { // found data while a active header is present, go and save the data
			fToPixFrame.push_back(rawData);
		} else { // found data without a valid header, may happen at the beginning of the file or the header was detected
			fPreFrameLossHitCount++;
		}
	}
	return false;
}

void PndMvdReadInToPix4TBData::AnalyzeData(std::vector<ULong64_t>& rawData,	Double_t clockFrequency) {
	if (fVerbose > 2)
		std::cout << "PndMvdReadInToPix4TBData::AnalyzeData rawData.size(): " << rawData.size() << std::endl;
	for (int i = 0; i < rawData.size(); i++) {
		int header = fTopix.GetHeader(rawData[i]);
		if (fFirstHeader) {	// if datastream does not start with a header all data is thrown away until a header is present
			if (header == 1) {
				fFirstHeader = kFALSE;
			} else {
				continue;
			}
		}

		if (header == 1) // header word found
		{
			fTotalHeaderCount++;
			fRecentAllFrameHeader = fTopix.BitAnalyzeHeader(rawData.at(i));

			Int_t deltaAllFrameCount = ((int) (fRecentAllFrameHeader.fFrameCount - fOldAllHeaderCount) < 0 ?
							((fRecentAllFrameHeader.fFrameCount	- fOldAllHeaderCount) + 256) :
							(fRecentAllFrameHeader.fFrameCount - fOldAllHeaderCount));
			new ((*fOutputArrayAllHeader)[fOutputArrayAllHeader->GetEntriesFast()]) PndSdsDigiTopix4Header(fRecentAllFrameHeader.fFrameCount, fFE,
					fRecentAllFrameHeader.fChipAddress, fRecentAllFrameHeader.fECC, fTotalHeaderCount, deltaAllFrameCount, 0, 0);
			if (deltaAllFrameCount > 1)
				std::cout << "-W- deltaAllFrameCount > 1: "	<< deltaAllFrameCount << std::endl;
			fOldAllHeaderCount = fRecentAllFrameHeader.fFrameCount;

			if (fHeaderPresent == kTRUE) {
				// double header found, cannot check previous data without trailer, clear vector
				fDoubleHeader++;

				if (fVerbose > 1) {
					std::cout << "Double Header Found! count: " << fDoubleHeader
							<< "| FE: " << fFE << std::hex << " last ToPixFrame element: "
							<< fToPixFrame.back() << " new frame header " << rawData[i] << std::endl;
				}
				fPreFrameLossHitCount += fToPixFrame.size() - 1;
				fToPixFrame.clear();
				// load new header into vector
				fToPixFrame.push_back(rawData[i]);
			} else {
				// header found, start recording topix frame
				fTrailerPresent = kFALSE;
				fHeaderPresent = kTRUE;
				fToPixFrame.push_back(rawData[i]);
			}
		}

		else if (header == 2) // trailer word found
		{
			fTotalTrailerCount++;
			if (fTrailerPresent == kTRUE) {
				// double trailer found, cannot give the hits a valid timestamp without the header, clear vector

				fToPixFrame.clear();
				fDoubleTrailer++;
				if (fVerbose > 1) {
					std::cout << "Double Trailer Found! Double header counter: "
							<< fDoubleTrailer << std::endl;
				}
			} else {
				if (fHeaderPresent == kTRUE) {
					// one topix frame found! Go and analyze the vector...
					fHeaderPresent = kFALSE;
					fTrailerPresent = kTRUE;
					fToPixFrame.push_back(rawData[i]);
					//  if (fVerbose > 1)
					//  {
					//	std::cout << "ToPix Frame found! Go and analyze this amount of data: " << fToPixFrame.size() << std::endl;
					//}

					fTotalFrameCount++;
					AnalyzeToPixFrame(clockFrequency);
					fToPixFrame.clear();
				} else {
					if (fVerbose > 1) {
						std::cout
								<< "Trailer without header found! Double header counter: "
								<< fDoubleHeader << std::endl;
					}
					// trailer without header, can happen at the beginning of the file or the header was not detected correctly
					// this case is in principle impossible to enter
					fPreFrameLossHitCount += fToPixFrame.size() - 1;
					fDoubleTrailer++;
					fToPixFrame.clear();
					continue;
				}
			}
		} else if (header == 3) // data word found
				{
			fTotalHitCount++;
			if (fHeaderPresent == kTRUE) { // found data while a active header is present, go and save the data
				fToPixFrame.push_back(rawData[i]);
			} else { // found data without a valid header, may happen at the beginning of the file or the header was detected
				fPreFrameLossHitCount++;
				continue;
			}
		}
	}
}


void PndMvdReadInToPix4TBData::AnalyzeToPixFrame(Double_t clockFrequency) {
	if (fVerbose > 2) {
		std::cout << fFE << " PndMvdReadInToPix4TBData::AnalyzeToPixFrame: fToPixFrame size: "
				<< std::dec << fToPixFrame.size() << " header " << std::hex << fToPixFrame[0] << std::endl;
	}

	if (CheckDataIntegrity(fToPixFrame) != true){
		fToPixFrame.clear();
		return;
	}

	for (int i = 0; i < fToPixFrame.size(); i++) {
		ULong64_t header = fTopix.GetHeader(fToPixFrame[i]);

		switch (header) {
		case 1: {
			fRecentFrameHeader = fTopix.BitAnalyzeHeader(fToPixFrame[i]);

			if (fVerbose > 2)
				std::cout << fFE << " FrameHeader: rawData: " << std::hex
						<< fToPixFrame[i] << " chip " << std::dec
						<< fRecentFrameHeader.fChipAddress << " framecount "
						<< fRecentFrameHeader.fFrameCount << std::endl;

			if (fOldFrameCount + 1 != fRecentFrameHeader.fFrameCount) {
				if (!(fOldFrameCount == 255 & fRecentFrameHeader.fFrameCount == 0)) {
					if (fVerbose > 1)
						std::cout << fFE << "-E- non sequential FC: " << fOldFrameCount << " " << fRecentFrameHeader.fFrameCount << std::endl;
					fNonSequentialFC++;
				}
			}

			if (fOldFrameCount > fRecentFrameHeader.fFrameCount) {
				fSuperFrameCount++;
				if (fVerbose > 1)
					std::cout << fFE << " SuperFrameCount increased: " << std::dec << fSuperFrameCount << " oldFC "
							<< fOldFrameCount << " recent FC " << fRecentFrameHeader.fFrameCount << std::endl;
			}
			//fOldFrameCount = fRecentFrameHeader.fFrameCount;
			Int_t deltaFrameCount = ((int) (fRecentFrameHeader.fFrameCount - fOldFrameCount) < 0 ?
							((fRecentFrameHeader.fFrameCount - fOldFrameCount) + 256) :
							(fRecentFrameHeader.fFrameCount - fOldFrameCount));
			//  new ((*fOutputArrayHeader)[fOutputArrayHeader->GetEntriesFast()]) PndSdsDigiTopix4Header(fRecentFrameHeader.fFrameCount, fFE, fRecentFrameHeader.fChipAddress, fRecentFrameHeader.fECC, fTotalFrameCount,deltaFrameCount, 0, fToPixFrame.size()-2 );
			new ((*fOutputArrayHeader)[fOutputArrayHeader->GetEntriesFast()]) PndSdsDigiTopix4Header(fRecentFrameHeader.fFrameCount, 0, fFE,
					fRecentFrameHeader.fECC, fTotalFrameCount, deltaFrameCount, 0, fToPixFrame.size() - 2);
			if (deltaFrameCount > 1)
				std::cout << "-W- OutputArrayHeader deltaFrameCount > 1 " << deltaFrameCount << std::endl;
			fOldFrameCount = fRecentFrameHeader.fFrameCount;
		}
			break;

		case 2: {
			fRecentFrameTrailer = fTopix.BitAnalyzeTrailer(fToPixFrame[i]);
			if (fRecentFrameTrailer.fFrameCRC != 0 && (fToPixFrame.size() < 20)) {
				// std::cout << fFE << " Frame counter " << fRecentFrameHeader.fFrameCount << std::endl;
				if (fVerbose == -1) {
					for (int y = 0; y < fToPixFrame.size(); y++) {
						if (y == 0) {
							std::cout << fFE << " " << std::hex << fToPixFrame[y] << " - FCount " << fRecentFrameHeader.fFrameCount << std::endl;
						} else {
							std::cout << fFE << " " << std::hex << fToPixFrame[y] << std::endl;
						}
					}
					std::cout << std::endl;
				}
			}
			if (fVerbose > 2)
				std::cout << fFE << " FrameTrailer: nEvents " << fRecentFrameTrailer.fNEvents << " frame CRC: "
						<< fRecentFrameTrailer.fFrameCRC << std::endl;

			PndSdsDigiTopix4Header * header_trailer = (PndSdsDigiTopix4Header*) (fOutputArrayHeader->Last());
			header_trailer->SetNumberOfEvents(fRecentFrameTrailer.fNEvents);
		}
			break;

		case 3:
			PndSdsDigiTopix4 recentPixel = ProcessData(fToPixFrame[i], fRecentFrameHeader, clockFrequency);
			if (fVerbose > 2)
				std::cout << fFE << " Pixel: " << recentPixel << std::endl;
			new ((*fOutputArray)[fOutputArray->GetEntriesFast()]) PndSdsDigiTopix4(recentPixel);
			fCorrectHitCount++;
			break;
		}
	}
}

bool PndMvdReadInToPix4TBData::CheckDataIntegrity(std::vector<ULong64_t> topix4Frame)
{
	ULong_t hammingcheck = fHamming.CheckHammingCode(fTopix.ConvertToPix4HammingToStandardHamming(fToPixFrame[0]), 40); // check hamming of header
	if (hammingcheck != 0) {
		if (fVerbose > 1) {
			std::cout << "Wrong Hamming Code found! (Header) : " << std::hex<< fToPixFrame[0] << " Parity bits " << hammingcheck << std::endl;
		}
		fHammingLossFrameCount++;
		fHammingLossHitCount += topix4Frame.size() - 2;
		return false;
	}

	hammingcheck = fHamming.CheckHammingCode(fTopix.ConvertToPix4HammingToStandardHamming(topix4Frame[topix4Frame.size() - 1]), 40); // check hamming of trailer
	if (hammingcheck != 0) {
		if (fVerbose > 1) {
			std::cout << "Wrong Hamming Code found! (Trailer): " << std::hex << topix4Frame[0] << " Parity bits " << hammingcheck << std::endl;
		}
		fHammingLossFrameCount++;
		fHammingLossHitCount += topix4Frame.size() - 2;
		return false;
	}

	std::vector<char> topix_data = fHamming.ConvertData(topix4Frame);
	ULong64_t crc_calculated = fHamming.CalculateCRCTableFast(topix_data, topix_data.size());

	if (crc_calculated != ((topix4Frame.back() >> 6) & 0xffff)) {
		fCRCLossFrameCount++;
		fCRCLossHitCount += topix4Frame.size() - 2;
		if (fVerbose == -1) {
			std::cout << fFE << " CRC WRONG! Frame will be deleted. Calculated CRC: " << std::hex << crc_calculated <<
					" topix CRC: " << ((topix4Frame.back() >> 6) & 0xffff) << std::endl;

			for (int y = 0; y < topix4Frame.size(); y++) {
				if (y == 0) {
					std::cout << fFE << " " << std::hex << topix4Frame[y] << " - FCount " << fRecentFrameHeader.fFrameCount << std::endl;
				} else {
					std::cout << fFE << " " << std::hex << topix4Frame[y] << std::endl;
				}
			}
			std::cout << std::endl;
		}
		topix4Frame.clear(); // delete topix frame due to negative CRC check.
		return false;
	}
	return true;
}


PndSdsDigiTopix4 PndMvdReadInToPix4TBData::ProcessData(ULong64_t& data, ToPix4::frameHeader& header, Double_t& clockFrequency)
{
	if (fVerbose > 1) std::cout  << "PndMvdReadInToPix4TBData::ProcessData raw Data: " << data << std::endl;
	pixel pixelData = fTopix.BitAnalyzePixelData(data);
	std::pair<UInt_t, UInt_t> pixelAddress = fTopix.PixeladdressToMatrixAddress(pixelData.fPixelAddress);
	Double_t timestamp = ((Double_t)fSuperFrameCount * 256. * 4096. + (Double_t)header.fFrameCount * 4096. + (Double_t)pixelData.fLeadingEdge)/clockFrequency * 1000.;
	Double_t timestamp_independent = ((Double_t) fTotalHeaderCount * 4096. + (Double_t)pixelData.fLeadingEdge)/clockFrequency * 1000.;

	std::vector<Int_t> indices; // just for compatibility with PndSdsDigiPixel
//	return PndSdsDigiTopix4(indices, 0, 0, fFE, pixelAddress.first, pixelAddress.second, pixelData.fLeadingEdge, pixelData.fTrailingEdge, header.fFrameCount, timestamp, fCorrectHitCount,fTotalHitCount, timestamp_independent);
	return PndSdsDigiTopix4(indices, 0, fFE - 1, 0, pixelAddress.first, pixelAddress.second, pixelData.fLeadingEdge, pixelData.fTrailingEdge, header.fFrameCount, timestamp, fCorrectHitCount,fTotalHitCount, timestamp_independent);

}
