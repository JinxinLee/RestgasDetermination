#include "PndLumiDigi.h"

PndLumiDigi::PndLumiDigi()
: FairHit()
{

}

PndLumiDigi::PndLumiDigi (Int_t detID, TVector3 pos, TVector3 dpos,
		Int_t index, PndLumiStrip strip, TString detname,
		SensorSide side, Int_t multi, Bool_t IsActive)
: FairHit(detID, pos, dpos, index)
{
	fStrip = strip;
	fDetName = detname;
	fSide = side;
	fMulti = multi;
	fIsActive = IsActive;
}


PndLumiDigi::~PndLumiDigi()
{
}


void PndLumiDigi::Print(const Option_t* opt) const
{
    cout<<" * Detector hit	: "<< fDetName <<endl;
    if(fIsActive){
    	cout<<" * Note : ( Side : "<<fSide << "| Active ) "<<endl;
    	cout  << " * Strip Involved	: " << fStrip << endl;
    }else
    	cout<<" * Note : ( Side : "<<fSide << "| Passive ) "<<endl;


}

ClassImp(PndLumiDigi)

