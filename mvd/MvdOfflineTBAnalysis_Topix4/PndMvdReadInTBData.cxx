/*
 * PndMvdReadInTBData.cxx
 *
 *  Created on: 23.10.2014
 *      Author: Stockmanns
 */

#include <MvdOfflineTBAnalysis_Topix4/PndMvdReadInTBData.h>

#include "mrfdata_8b.h"
#include "mrftools.h"
#include "boost/archive/binary_oarchive.hpp"
#include "boost/archive/binary_iarchive.hpp"
#include "boost/serialization/binary_object.hpp"
#include <boost/archive/archive_exception.hpp>

PndMvdReadInTBData::PndMvdReadInTBData() : fDigiArray(0), fClockFrequency(0), fSuperFrameCount(0), fOldFrameCount(0), fFirstHeader(kTRUE), fFE(-1), fVerbose(0) {
	// TODO Auto-generated constructor stub

}

PndMvdReadInTBData::~PndMvdReadInTBData() {
//	for (int i = 0; i < fFileHandle.size(); i++){
		  fFileHandle->close();
		  delete(fFileHandle);
//	}
}

void PndMvdReadInTBData::Init(){
	std::cout << "PndMvdReadInTBData::Init called" << std::endl;
//	for (int i = 0; i < fFileName.size(); i++){
		  std::ifstream* ifs = new std::ifstream(fFileName.Data(), std::ios::binary);
		  std::cout << "File: " << fFileName << " is good: " << ifs->good() << std::endl;
		  fFileHandle = ifs;
//	}
	fChipIdMap[0] = 0;
	fChipIdMap[1] = 1;
	fChipIdMap[2] = 2;
	fChipIdMap[3] = 3;

}

Bool_t PndMvdReadInTBData::ReadInData(TClonesArray* sdsDigiContainer){
	TMrfData_8b* tempdata;
	tempdata = new TMrfData_8b;
	ULong_t dataword=0;
	Bool_t endOfFile = kFALSE;

	fOutputArray = sdsDigiContainer;
//	for (int k = 0; k < fFileHandle.size(); k++){
		std::vector<ULong_t> rawArray;
		endOfFile |= ReadInRawData(fFileHandle, rawArray);
//		SetFE(k);
		AnalyzeData(rawArray, fClockFrequency);

//	}
	return endOfFile;
}

Bool_t PndMvdReadInTBData::ReadInRawData(std::ifstream* fileHandle, std::vector<ULong_t>& rawData){
	TMrfData_8b* tempdata;
	tempdata = new TMrfData_8b;
	ULong_t dataword=0;
	Bool_t endOfFile = kFALSE;

	if (fileHandle->good()){
		if (fVerbose > 1) std::cout << "PndMvdReadInTBData:ReadInData reading file " << std::endl;
		try{
			boost::archive::binary_iarchive iar(*fileHandle); 	//this line causes an "Invalid Signature Error" at the end of the file but the file is still good
			iar >> tempdata;
		}
		catch (boost::archive::archive_exception& exception){
			if (fVerbose > 1) std::cout << "PndMvdReadInTBData:Error found in reading file " << " : " << fileHandle->good() << " " << fileHandle->eof()
					<< " Exception: " << exception.code << std::endl;
			if (fVerbose > 1) std::cout << exception.what() << std::endl;
			if (exception.code == 3){
				endOfFile = kTRUE;
				return endOfFile;
			} else {
				return endOfFile;
			}
		}
		if (fVerbose > 1) std::cout << "PndMvdReadInTBData: NWords: " << tempdata->getNumWords() << std::endl;
		for (UInt_t i=0;i < tempdata->getNumWords();i+=5)
		{
			dataword=0;
			for(uint j=0; j< 5 ; j++)
			{
				dataword = dataword << 8;
				dataword += tempdata->getWord(i+j);
			}
			rawData.push_back(dataword);
			ULong_t frameCount = dataword & 0x3FC0000;
			frameCount = frameCount >> 18;
		//	if(i==0)
		//	{
			if (fVerbose > 1) std::cout << std::dec << "dataword No "<< i/5<< "/"<< tempdata->getNumWords()/5 << ": "<<std::hex << dataword << " " << std::dec << frameCount << std::endl;
		//	}
		}
		if (fVerbose > 1) std::cout << "---- End of message ----" << std::endl << std::endl;
	} else {
		endOfFile = kTRUE;
	}
	return endOfFile;
}

void PndMvdReadInTBData::AnalyzeData(std::vector<ULong_t>& rawData, Double_t clockFrequency)
{
	PndSdsDigiTopix4 recentPixel;

	if (fVerbose > 1) std::cout << "PndMvdReadInTBData::AnalyzeData " << rawData.size() << std::endl;

	for (int i = 0; i < rawData.size(); i++){
		ULong_t header = rawData[i] & 0xC000000000;
		header = header >> 38;
		if (fVerbose > 1) std::cout << "HEADER: " << header << std::endl;
		if (fFirstHeader) {
			if (header == 1){
				fFirstHeader = kFALSE;
			} else {
				continue;
			}
		}

		switch (header) {
		case 1 : fRecentFrameHeader = BitAnalyzeHeader(rawData[i]);
			if (fVerbose > 1) std::cout << "FrameHeader: chip " << fRecentFrameHeader.fChipAddress << " frame " << fRecentFrameHeader.fFrameCount << std::endl;
			if (fOldFrameCount != fRecentFrameHeader.fFrameCount && fOldFrameCount + 1 != fRecentFrameHeader.fFrameCount){
				if (fVerbose > 1) std::cout << "-E- PndMvdReadInTBData::AnalyzeData frameCount not consecutive: "
						<< fOldFrameCount << " " << fRecentFrameHeader.fFrameCount << std::endl;
			}
			if (fOldFrameCount > fRecentFrameHeader.fFrameCount){
				fSuperFrameCount++;
				if (fVerbose > 1) std::cout << "SuperFrameCount increased: " << fSuperFrameCount << " oldFC " << fOldFrameCount << " recent FC " << fRecentFrameHeader.fFrameCount << std::endl;
			}
			fOldFrameCount = fRecentFrameHeader.fFrameCount;
					break;
		case 2 : fRecentFrameTrailer = BitAnalyzeTrailer(rawData[i]);
			if (fVerbose > 1) std::cout << "FrameTrailer: nEvents " << fRecentFrameTrailer.fNEvents << " frame CRC: " << fRecentFrameTrailer.fFrameCRC << std::endl;
					break;
		case 3 : recentPixel = ProcessData(rawData[i], fRecentFrameHeader, clockFrequency);
			if (fVerbose > 1) std::cout << "Pixel: " << recentPixel << std::endl;
			new ((*fOutputArray)[fOutputArray->GetEntriesFast()]) PndSdsDigiTopix4(recentPixel);
					break;
		}
	}
	if (fVerbose > 1) std::cout << "End of Analyze Data" << std::endl << std::endl;
}

frameHeader PndMvdReadInTBData::BitAnalyzeHeader(ULong_t& header)
{
	frameHeader tempHeader;
	ULong_t temp = header;
	tempHeader.fECC = temp & 0x1F;

	temp = temp >> 18;
	tempHeader.fFrameCount = temp & 0xFF;

	temp = temp >> 8;
	tempHeader.fChipAddress = temp & 0xFFF;

	return tempHeader;
}

frameTrailer PndMvdReadInTBData::BitAnalyzeTrailer(ULong_t& trailer)
{
	frameTrailer tempTrailer;
	ULong_t temp = trailer;
	tempTrailer.fECC = temp & 0x1F;

	temp = temp >> 6;
	tempTrailer.fFrameCRC = temp & 0xFFFF;

	temp = temp >> 16;
	tempTrailer.fNEvents = temp & 0xFFFF;

	return tempTrailer;

}

pixel PndMvdReadInTBData::BitAnalyzePixelData(ULong_t& data)
{
	//le_dataword = ((dataword & 0x0000000000fff000)>>12);¬
	//te_dataword = (dataword & 0x0000000000000fff);¬
	//pixeladdress = ((dataword & 0x0000003fff000000)>>24);¬

	pixel tempPixel;
	ULong_t temp = data;
	tempPixel.fTrailingEdge = mrftools::grayToBin(temp & 0X0000000000000FFF);

	temp = temp >> 12;
	tempPixel.fLeadingEdge = mrftools::grayToBin(temp & 0X0000000000000FFF);

	temp = temp >> 12;
	tempPixel.fPixelAddress = temp & 0X00000000000003FFF;

	if (fVerbose > 1) std::cout << "BitAnalyzePixelData: " << data << " pixel " << tempPixel.fPixelAddress << " " << tempPixel.fLeadingEdge << " " << tempPixel.fTrailingEdge << std::endl;

	return tempPixel;

}

PndSdsDigiTopix4 PndMvdReadInTBData::ProcessData(ULong_t& data, frameHeader& header, Double_t& clockFrequency)
{
	if (fVerbose > 1) std::cout  << "PndMvdReadInTBData::ProcessData raw Data: " << data << std::endl;
	pixel pixelData = BitAnalyzePixelData(data);
	std::pair<UInt_t, UInt_t> pixelAddress = PixeladdressToMatrixAddress(pixelData.fPixelAddress);
	if (fVerbose > 1) std::cout << "PndMvdReadInTBData::ProcessData timestamp: FE " << fFE << " SFC " << fSuperFrameCount << " FC " << header.fFrameCount << " LE " << pixelData.fLeadingEdge << " TE " << pixelData.fTrailingEdge << std::endl;
	Double_t timestamp = (fSuperFrameCount * 256 * 4096 + header.fFrameCount * 4096 + pixelData.fLeadingEdge)/clockFrequency * 1000;
	if (fVerbose > 1) std::cout << "RawAddress: " << pixelData.fPixelAddress << " " << pixelAddress.first << "/" << pixelAddress.second << " LE " << pixelData.fLeadingEdge << " TE " << pixelData.fTrailingEdge  << std::endl;
	std::vector<Int_t> indices; // just for compatibility with PndSdsDigiPixel
	return PndSdsDigiTopix4(indices, 0, 0, fFE, pixelAddress.first, pixelAddress.second, pixelData.fLeadingEdge, pixelData.fTrailingEdge, header.fFrameCount, timestamp);

}

std::pair<UInt_t, UInt_t> PndMvdReadInTBData::PixeladdressToMatrixAddress(UInt_t pixelglobaladdress)
{
    // Matrix: 32 columns x 20 rows

    UInt_t double_column_address =0;
    UInt_t double_column_side=0;
    UInt_t pixel_address=0;

    UInt_t matrix_column, matrix_row;

    UInt_t temp = pixelglobaladdress;

    pixel_address= temp & 0x7f; //todo check if this conversion is correct!
    temp = temp >> 7;
    double_column_side= temp & 0x1;
    temp = temp >> 1;
    double_column_address= temp & 0x3f;
     temp = temp >> 6;

     if (pixel_address > 127) {
     	std::cout << "-E- PndMvdReadInTBData::PixeladdressToMatrixAddress PixelAddress > 128 " << pixel_address << std::endl;
     }



 //   temp = temp >> 6;



    if (fVerbose > 1) std::cout << "PixeladdressToMatrix rawData " << pixelglobaladdress << " dc " << double_column_address << " dcs " << double_column_side << " pixel " << pixel_address << std::endl;

    UInt_t sel = (double_column_address<<1) | (double_column_side);

    if(sel == 0)
    {
        matrix_row = pixel_address;
        matrix_column = 1;
    }
    else if(sel ==1)
    {
        matrix_row = pixel_address;
        matrix_column = 0;
    }
    else if(sel == 6)
    {
        matrix_row = pixel_address;
        matrix_column = 19;
    }
    else if(sel == 7)
    {
        matrix_row = pixel_address;
        matrix_column = 18;
    }
    else if (sel==2)
    {
        if (pixel_address <32)
        {
            matrix_row = pixel_address;
            matrix_column = 3;
        }
        else if (pixel_address < 64)
        {
            matrix_row = 31 - (pixel_address-32);
            matrix_column = 4;
        }
        else if (pixel_address < 96)
        {
            matrix_row = (pixel_address-64);
            matrix_column = 7;
        }
        else if (pixel_address < 128)
        {
            matrix_row = 31 - (pixel_address-96);
            matrix_column = 8;
        }
    }
    else if (sel==3)
    {
        if (pixel_address <32)
        {
            matrix_row = pixel_address;
            matrix_column = 2;
        }
        else if (pixel_address < 64)
        {
            matrix_row = 31 - (pixel_address-32);
            matrix_column = 5;
        }
        else if (pixel_address < 96)
        {
            matrix_row = (pixel_address-64);
            matrix_column = 6;
        }
        else if (pixel_address < 128)
        {
            matrix_row = 31 - (pixel_address-96);
            matrix_column = 9;
        }
    }
    else if (sel==4)
    {
        if (pixel_address < 32)
        {
            matrix_row = pixel_address;
            matrix_column = 10;
        }
        else if (pixel_address < 64)
        {
            matrix_row = 31 - (pixel_address-32);
            matrix_column = 13;
        }
        else if (pixel_address < 96)
        {
            matrix_row = (pixel_address-64);
            matrix_column = 14;
        }
        else if (pixel_address < 128)
        {
            matrix_row = 31 - (pixel_address-96);
            matrix_column = 17;
        }
    }
    else if (sel==5)
    {
        if (pixel_address < 32)
        {
            matrix_row = pixel_address;
            matrix_column = 11;
        }
        else if (pixel_address < 64)
        {
            matrix_row = 31 - (pixel_address-32);
            matrix_column = 12;
        }
        else if (pixel_address < 96)
        {
            matrix_row = (pixel_address-64);
            matrix_column = 15;
        }
        else if (pixel_address < 128)
        {
            matrix_row = 31 - (pixel_address-96);
            matrix_column = 16;
        }
    }

    return std::pair<UInt_t, UInt_t>(matrix_column, matrix_row);
}

