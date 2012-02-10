#include "PndLumiStrip.h"
PndLumiStrip::PndLumiStrip()
{
	fNumber = -1;
	fCharge = -1.0;
	//fPath = -1.;
}

PndLumiStrip::PndLumiStrip(Int_t nr, Double_t charge)
{
	fNumber = nr;
	fCharge = charge;
	//fPath = path;
}
PndLumiStrip::~PndLumiStrip()
{
}
ClassImp(PndLumiStrip)
