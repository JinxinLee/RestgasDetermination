#include "PndSdsDigiTopix4.h"

PndSdsDigiTopix4::PndSdsDigiTopix4() : PndSdsDigiPixel(), fLeadingEdge(0), fTrailingEdge(0), fFrameCount(0)
{
}

PndSdsDigiTopix4::PndSdsDigiTopix4(std::vector<Int_t> index, Int_t detID, Int_t sensorID, Int_t fe, Int_t col, Int_t row, UInt_t leadingEdge, UInt_t trailingEdge, UInt_t frameCount, Double_t timeStamp):
PndSdsDigiPixel(index, detID, sensorID, fe, col, row, 0, timeStamp), fLeadingEdge(leadingEdge), fTrailingEdge(trailingEdge), fFrameCount(frameCount)
{
	Double_t charge = 0;
	if(leadingEdge < trailingEdge){
		charge = trailingEdge - leadingEdge;
	} else {
		charge = (trailingEdge + 4096) - leadingEdge;
	}
	SetCharge(charge);
}

ClassImp(PndSdsDigiTopix4);
