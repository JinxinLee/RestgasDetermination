#include "PndSdsDigi.h"

PndSdsDigi::PndSdsDigi(){
       fCharge = -1;
}

PndSdsDigi::PndSdsDigi(std::vector<Int_t> index, Int_t detID, TString detName, Int_t fe, Double_t charge)
{ 
        AddIndex(index);
	fDetID  = detID;
	fDetName= detName;
	fFE     = fe;
	fCharge	= charge;
}

PndSdsDigi::PndSdsDigi(Int_t index, Int_t detID, TString detName, Int_t fe, Double_t charge){ 
	        AddIndex(index); 
 	        fDetID  = detID; 
 	        fDetName= detName; 
 	        fFE     = fe; 
 	        fCharge = charge; 
 	} 

ClassImp(PndSdsDigi);
