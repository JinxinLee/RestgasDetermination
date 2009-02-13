#include "PndHypDigi.h"

PndHypDigi::PndHypDigi(){
  //fIndex = -1;
       fCharge = -1;
}

PndHypDigi::PndHypDigi(Int_t index, Int_t detID, TString detName, Int_t fe, Double_t charge){
  //fIndex 	= index;
	fIndex.push_back(index);
	fDetID  = detID;
	fDetName= detName;
	fFE     = fe;
	fCharge	= charge;
}

ClassImp(PndHypDigi)
