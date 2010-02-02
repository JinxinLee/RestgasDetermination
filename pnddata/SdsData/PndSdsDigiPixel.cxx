#include "PndSdsDigiPixel.h"

PndSdsDigiPixel::PndSdsDigiPixel() : PndSdsDigi()
{
       fCol = fRow = -1;
}

PndSdsDigiPixel::PndSdsDigiPixel(std::vector<Int_t> index, Int_t detID, TString detName, Int_t fe, Int_t col, Int_t row, Double_t charge) : PndSdsDigi(index, detID, detName, fe, charge)
{
	fCol	= col;
	fRow	= row;
}

ClassImp(PndSdsDigiPixel);
