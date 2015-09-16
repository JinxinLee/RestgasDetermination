/*
 * PndTopix4.cxx
 *
 *  Created on: 16.09.2015
 *      Author: Stockmanns
 */

#include <MvdOfflineTBAnalysis_Topix4/PndTopix4.h>
#include "mrfdata_8b.h"
#include "mrftools.h"

using namespace ToPix4;

PndTopix4::PndTopix4() {
	// TODO Auto-generated constructor stub

}

PndTopix4::~PndTopix4() {
	// TODO Auto-generated destructor stub
}

ToPix4::frameHeader PndTopix4::BitAnalyzeHeader(ULong64_t& header)
{
	ToPix4::frameHeader tempHeader;
	ULong_t temp = header;
	tempHeader.fECC = temp & 0x3F;
	temp = temp >> 18;
	tempHeader.fFrameCount = temp & 0xFF;

	temp = temp >> 8;
	tempHeader.fChipAddress = temp & 0xFFF;

	return tempHeader;
}

ToPix4::frameTrailer PndTopix4::BitAnalyzeTrailer(ULong64_t& trailer)
{
	ToPix4::frameTrailer tempTrailer;
	ULong_t temp = trailer;
	tempTrailer.fECC = temp & 0x3F;

	temp = temp >> 6;
	tempTrailer.fFrameCRC = temp & 0xFFFF;

	temp = temp >> 16;
	tempTrailer.fNEvents = temp & 0xFFFF;

	return tempTrailer;

}

ToPix4::pixel PndTopix4::BitAnalyzePixelData(ULong64_t& data)
{
	//le_dataword = ((dataword & 0x0000000000fff000)>>12);¬
	//te_dataword = (dataword & 0x0000000000000fff);¬
	//pixeladdress = ((dataword & 0x0000003fff000000)>>24);¬

	ToPix4::pixel tempPixel;
	ULong_t temp = data;
	tempPixel.fTrailingEdge = mrftools::grayToBin(temp & 0X0000000000000FFF);

	temp = temp >> 12;
	tempPixel.fLeadingEdge = mrftools::grayToBin(temp & 0X0000000000000FFF);

	temp = temp >> 12;
	tempPixel.fPixelAddress = temp & 0X00000000000003FFF;

//	if (fVerbose > 1) std::cout << "BitAnalyzePixelData: " << std::hex << data << " pixel " << std::dec << tempPixel.fPixelAddress << " " << tempPixel.fLeadingEdge << " " << tempPixel.fTrailingEdge << std::endl;

	return tempPixel;

}

std::pair<UInt_t, UInt_t> PndTopix4::PixeladdressToMatrixAddress(UInt_t pixelglobaladdress)
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


