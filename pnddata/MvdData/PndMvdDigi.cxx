#include "PndMvdDigi.h"

PndMvdDigi::PndMvdDigi(){
       fCharge = -1;
}

PndMvdDigi::PndMvdDigi(Int_t index, Int_t detID, TString detName, Int_t fe, Double_t charge){
	fIndex.push_back(index);
	fDetID  = detID;
	fDetName= detName;
	fFE     = fe;
	fCharge	= charge;
}

ClassImp(PndMvdDigi);
