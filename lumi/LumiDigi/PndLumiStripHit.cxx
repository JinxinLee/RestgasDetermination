#include "PndLumiStripHit.h"

PndLumiStripHit::PndLumiStripHit()
{
}

PndLumiStripHit::PndLumiStripHit( Int_t detID, TVector3& entrypos, TVector3& dpos,
		Int_t index, TVector3& exitpos, Int_t multi, PndLumiStrip strip,
		Double_t eLoss)
{
	fMultiplicity = multi;
	fStrip = strip;
	fELoss = eLoss;
}

PndLumiStripHit::~PndLumiStripHit()
{
}
ClassImp(PndLumiStripHit)
