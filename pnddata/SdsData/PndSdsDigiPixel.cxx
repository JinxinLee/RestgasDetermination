#include "PndSdsDigiPixel.h"

PndSdsDigiPixel::PndSdsDigiPixel() : PndSdsDigi()
{
  fCol = fRow = -1;
}

PndSdsDigiPixel::PndSdsDigiPixel(std::vector<Int_t> index, Int_t detID, Int_t sensorID, Int_t fe, Int_t col, Int_t row, Double_t charge, fDetectorType mcPointType):
PndSdsDigi(index, detID, sensorID, fe, charge, mcPointType)
{
	fCol	= col;
	fRow	= row;
}

ClassImp(PndSdsDigiPixel);
