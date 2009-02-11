#ifndef fMVDSTRIP_HH
#define fMVDSTRIP_HH

#include "FairGeoVector.h"
#include <iostream>
#include <vector>


//! Class representing strips on wafer-scale
/** 
 * This is just a thumb representation of strips on a wafer
 * with index and assigned charge. Do not confuse with PndMvdDigiStrip, which represents
 * a digitised frontend-channel.
 **/
class PndMvdStrip
{
public : 
          PndMvdStrip(){
			fNumber = -1;
			fCharge = -1.0;
		  }
		  
		  PndMvdStrip(Int_t nr, Double_t charge){
			fNumber = nr;
			fCharge = charge;
		  }
		  
		  void SetIndex(Int_t nr) {fNumber = nr;}
		  void SetCharge(Double_t charge) {fCharge = charge;}
		  
		  Int_t GetIndex() const {return fNumber;}
		  Double_t GetCharge() const {return fCharge;}
		  
		  friend std::ostream& operator<< (std::ostream& out, const PndMvdStrip& strip)
		  {
			out << " Strip Nr.: " << strip.GetIndex() << " "
			    << " Charge: " << strip.GetCharge();
			return out;
		  }	
	
	private : 
		  Int_t fNumber;
		  Double_t fCharge;
};

#endif //fMVDSTRIP_HH
