/*
 * PndMvdReadInTBData.cxx
 *
 *  Created on: 23.10.2014
 *      Author: Stockmanns
 */

#include <MvdOfflineTBAnalysis_Topix4/PndMvdReadInTBData.h>

#include "mrfdata_8b.h"
#include "boost/archive/binary_oarchive.hpp"
#include "boost/archive/binary_iarchive.hpp"
#include "boost/serialization/binary_object.hpp"
#include <boost/archive/archive_exception.hpp>

PndMvdReadInTBData::PndMvdReadInTBData() : fDigiArray(0), fClockFrequency(0) {
	// TODO Auto-generated constructor stub

}

PndMvdReadInTBData::~PndMvdReadInTBData() {
	for (int i = 0; i < fFileHandles.size(); i++){
		  fFileHandles[i]->close();
		  delete(fFileHandles[i]);
	}
}

void PndMvdReadInTBData::Init(){
	std::cout << "PndMvdReadInTBData::Init called" << std::endl;
	for (int i = 0; i < fFileNames.size(); i++){
		  std::ifstream* ifs = new std::ifstream(fFileNames[i].Data(), std::ios::binary);
		  std::cout << "File: " << fFileNames[i] << " is good: " << ifs->good() << std::endl;
		  fFileHandles.push_back(ifs);
	}
	fChipIdMap[0] = 0;
	fChipIdMap[1] = 1;
	fChipIdMap[2] = 2;
	fChipIdMap[3] = 3;

	fClockFrequency = 160.0;

}

Bool_t PndMvdReadInTBData::ReadInData(TClonesArray* sdsDigiContainer){
	TMrfData_8b* tempdata;
	tempdata = new TMrfData_8b;
	ULong_t dataword=0;
	Bool_t endOfFile = kFALSE;

	fOutputArray = sdsDigiContainer;
	for (int k = 0; k < fFileHandles.size(); k++){
		std::vector<ULong_t> rawArray;
		endOfFile |= ReadInRawData(fFileHandles[k], rawArray);
		std::cout << "PndMvdReadInTBData::ReadInData " << rawArray.size() << std::endl;
		AnalyzeData(rawArray, fClockFrequency);

	}
	return endOfFile;
}

Bool_t PndMvdReadInTBData::ReadInRawData(std::ifstream* fileHandle, std::vector<ULong_t>& rawData){
	TMrfData_8b* tempdata;
	tempdata = new TMrfData_8b;
	ULong_t dataword=0;
	Bool_t endOfFile = kFALSE;

	if (fileHandle->good()){
		std::cout << "PndMvdReadInTBData:ReadInData reading file " << std::endl;
		try{
			boost::archive::binary_iarchive iar(*fileHandle); 	//this line causes an "Invalid Signature Error" at the end of the file but the file is still good
			iar >> tempdata;
		}
		catch (boost::archive::archive_exception& exception){
			std::cout << "PndMvdReadInTBData:Error found in reading file " << " : " << fileHandle->good() << " " << fileHandle->eof()
					<< " Exception: " << exception.code << std::endl;
			std::cout << exception.what() << std::endl;
			if (exception.code == 3){
				endOfFile = kTRUE;
				return endOfFile;
			} else {
				return endOfFile;
			}
		}
		std::cout << "PndMvdReadInTBData: NWords: " << tempdata->getNumWords() << std::endl;
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
				std::cout << std::dec << "dataword No "<< i/5<< "/"<< tempdata->getNumWords()/5 << ": "<<std::hex << dataword << " " << std::dec << frameCount << std::endl;
		//	}
		}
		std::cout << "Finish with message " << std::endl;
	} else {
		endOfFile = kTRUE;
	}
	return endOfFile;
}

void PndMvdReadInTBData::AnalyzeData(std::vector<ULong_t>& rawData, Double_t clockFrequency)
{
	frameHeader recentFrameHeader;
	frameTrailer recentFrameTrailer;
	PndSdsDigiTopix4 recentPixel;
	Bool_t firstHeader = kTRUE;

	for (int i = 0; i < rawData.size(); i++){
		ULong_t header = rawData[i] & 0xC000000000;
		header = header >> 38;
		std::cout << "HEADER: " << header << std::endl;
		if (firstHeader) {
			if (header == 1){
				firstHeader = kFALSE;
			} else {
				continue;
			}
		}
		switch (header) {
		case 1 : recentFrameHeader = BitAnalyzeHeader(rawData[i]);
			std::cout << "FrameHeader: chip " << recentFrameHeader.fChipAddress << " frame " << recentFrameHeader.fFrameCount << std::endl;
					break;
		case 2 : recentFrameTrailer = BitAnalyzeTrailer(rawData[i]);
			std::cout << "FrameTrailer: nEvents " << recentFrameTrailer.fNEvents << " frame CRC: " << recentFrameTrailer.fFrameCRC << std::endl;
					break;
		case 3 : recentPixel = ProcessData(rawData[i], recentFrameHeader, clockFrequency);
			std::cout << "Pixel: " << recentPixel << std::endl;
			new ((*fOutputArray)[fOutputArray->GetEntriesFast()]) PndSdsDigiTopix4(recentPixel);
					break;
		}
	}
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
	pixel tempPixel;
	ULong_t temp = data;
	tempPixel.fTrailingEdge = temp & 0XFFF;

	temp = temp >> 12;
	tempPixel.fLeadingEdge = temp & 0XFFF;

	temp = temp >> 12;
	tempPixel.fPixelAddress = temp & 0X3FFF;

	return tempPixel;

}

PndSdsDigiTopix4 PndMvdReadInTBData::ProcessData(ULong_t& data, frameHeader& header, Double_t& clockFrequency)
{
	pixel pixelData = BitAnalyzePixelData(data);
	std::pair<UInt_t, UInt_t> pixelAddress = PixeladdressToMatrixAddress(pixelData.fPixelAddress);
	Double_t timestamp = (header.fFrameCount * 4096 + pixelData.fLeadingEdge)/clockFrequency;

	std::vector<Int_t> indices; // just for compatibility with PndSdsDigiPixel
	return PndSdsDigiTopix4(indices, 0, 0, header.fChipAddress, pixelAddress.first, pixelAddress.second, pixelData.fLeadingEdge, pixelData.fTrailingEdge, header.fFrameCount, timestamp);

}

std::pair<UInt_t, UInt_t> PndMvdReadInTBData::PixeladdressToMatrixAddress(UInt_t pixelglobaladdress)
{
    // Matrix: 32 columns x 20 rows

    UInt_t double_column_address =0;
    UInt_t double_column_side=0;
    UInt_t pixel_address=0;

    UInt_t matrix_column, matrix_row;

    double_column_address= ((pixelglobaladdress & 0x00003f00)>>8);
    double_column_side= ((pixelglobaladdress & 0x00000080)>>7);
    pixel_address= (pixelglobaladdress & 0x0000007F);

    UInt_t sel = (double_column_address<<1) | (double_column_side);

    if(sel == 0)
    {
        matrix_column = 31-pixel_address;
        matrix_row = 1;
    }
    else if(sel ==1)
    {
        matrix_column = 31-pixel_address;
        matrix_row = 0;
    }
    else if(sel == 6)
    {
        matrix_column = 31-pixel_address;
        matrix_row = 19;
    }
    else if(sel == 7)
    {
        matrix_column = 31-pixel_address;
        matrix_row = 18;
    }
    else if (sel==2)
    {
        if (pixel_address <32)
        {
            matrix_column = 31-pixel_address;
            matrix_row = 9;
        }
        else if (pixel_address < 64)
        {
            matrix_column = (pixel_address-32);
            matrix_row = 6;
        }
        else if (pixel_address <96)
        {
            matrix_column = 31-(pixel_address-64);
            matrix_row = 5;
        }
        else
        {
            matrix_column = (pixel_address-96);
            matrix_row = 2;
        }
    }
    else if (sel==3)
    {
        if (pixel_address <32)
        {
            matrix_column = 31-pixel_address;
            matrix_row = 8;
        }
        else if (pixel_address < 64)
        {
            matrix_column = (pixel_address-32);
            matrix_row = 7;
        }
        else if (pixel_address <96)
        {
            matrix_column = 31-(pixel_address-64);
            matrix_row = 4;
        }
        else
        {
            matrix_column = (pixel_address-96);
            matrix_row = 3;
        }
    }
    else if (sel==4)
    {
        if (pixel_address <32)
        {
            matrix_column = 31-pixel_address;
            matrix_row = 17;
        }
        else if (pixel_address < 64)
        {
            matrix_column = (pixel_address-32);
            matrix_row = 14;
        }
        else if (pixel_address <96)
        {
            matrix_column = 31-(pixel_address-64);
            matrix_row = 13;
        }
        else
        {
            matrix_column = (pixel_address-96);
            matrix_row = 10;
        }
    }
    else if (sel==5)
    {
        if (pixel_address <32)
        {
            matrix_column = 31-pixel_address;
            matrix_row = 16;
        }
        else if (pixel_address < 64)
        {
            matrix_column = (pixel_address-32);
            matrix_row = 15;
        }
        else if (pixel_address <96)
        {
            matrix_column = 31-(pixel_address-64);
            matrix_row = 12;
        }
        else
        {
            matrix_column = (pixel_address-96);
            matrix_row = 11;
        }
    }
    return std::pair<UInt_t, UInt_t>(matrix_column, matrix_row);
}

