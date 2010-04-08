#include "PndMvdDigiPixel.h"

PndMvdDigiPixel::PndMvdDigiPixel() : PndMvdDigi()
{
       fCol = fRow = -1;
}

PndMvdDigiPixel::PndMvdDigiPixel(std::vector<Int_t> index, Int_t detID, TString detName, Int_t fe, Int_t col, Int_t row, Double_t charge, Double_t tot) : PndMvdDigi(index, detID, detName, fe, charge, tot)
{
	fCol	= col;
	fRow	= row;
}

ClassImp(PndMvdDigiPixel);
