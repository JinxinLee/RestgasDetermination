#include "PndMvdDigi.h"

PndMvdDigi::PndMvdDigi(){
       fCharge = -1;
}

PndMvdDigi::PndMvdDigi(std::vector<Int_t> index, Int_t detID, TString detName, Int_t fe, Double_t charge){
	AddIndex(index);
	fDetID  = detID;
	fDetName= detName;
	fFE     = fe;
	fCharge	= charge;
}

PndMvdDigi::PndMvdDigi(Int_t index, Int_t detID, TString detName, Int_t fe, Double_t charge){
	AddIndex(index);
	fDetID  = detID;
	fDetName= detName;
	fFE     = fe;
	fCharge	= charge;
}
ClassImp(PndMvdDigi);
