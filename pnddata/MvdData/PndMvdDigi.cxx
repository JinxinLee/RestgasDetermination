#include "PndMvdDigi.h"

PndMvdDigi::PndMvdDigi(){
       fCharge = -1;
}

PndMvdDigi::PndMvdDigi(std::vector<Int_t> index, Int_t detID, TString detName, Int_t fe, Double_t charge, Double_t tot){
	AddIndex(index);
	fDetID  = detID;
	fDetName= detName;
	fFE     = fe;
	fCharge	= charge;
	fTot = tot;
}

PndMvdDigi::PndMvdDigi(Int_t index, Int_t detID, TString detName, Int_t fe, Double_t charge, Double_t tot){
	AddIndex(index);
	fDetID  = detID;
	fDetName= detName;
	fFE     = fe;
	fCharge	= charge;
	fTot = tot;
}
ClassImp(PndMvdDigi);
