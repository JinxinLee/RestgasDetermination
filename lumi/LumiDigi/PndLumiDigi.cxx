#include "PndLumiDigi.h"

PndLumiDigi::PndLumiDigi()
//: FairHit()
{
	fDetId = -1;
	fSide = kTOP;
}

PndLumiDigi::PndLumiDigi (Int_t detID, Int_t index, TString detname, Int_t trkId,
		PndLumiStrip strip, SensorSide side)
{
	fDetId = detID;
	fStrip = strip;
	fDetName = detname;
	fIndex = index;
	fSide = side;
	fTrkId = trkId;
}


PndLumiDigi::~PndLumiDigi()
{
}


void PndLumiDigi::Print(const Option_t* opt) const
{
	TString side;
	if (fSide==0) side="FRONT";
	if (fSide==1) side="BACK";
    cout<<" * Detector hit	: "<< fDetName <<" | Side = "<<side<<endl;
    cout<< " * Strip fired	: " << fStrip << endl;
 }

ClassImp(PndLumiDigi)

