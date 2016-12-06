#include "PndHypDigi.h"

PndHypDigi::PndHypDigi() :
		fIndex(),
    fDetID(-1),
		fDetName(""),
		fCharge(-1),
		fFE(0)
{
}

PndHypDigi::PndHypDigi(Int_t index, Int_t detID, TString detName, Int_t fe, Double_t charge) :
		fIndex(),
    fDetID(detID),
		fDetName(detName),
		fCharge(charge),
		fFE(fe)
{
	fIndex.push_back(index);
}

ClassImp(PndHypDigi)
