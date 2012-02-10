#ifndef PNDLUMISTRIP_H
#define PNDLUMISTRIP_H

#include "FairGeoVector.h"
#include <iostream>
#include <vector>

/**
 * This looks after PndMvdStrip. This is an way to represent strip fired
 * assigned with index, intern track path and deposited charge.
 **/

class PndLumiStrip
{
public :
	PndLumiStrip();

	PndLumiStrip(Int_t nr,  Double_t charge);

	~PndLumiStrip();

	void SetIndex(Int_t nr) {fNumber = nr;}
	void SetCharge(Double_t charge) {fCharge = charge;}

	Int_t GetIndex() const {return fNumber;}
	Double_t GetCharge() const {return fCharge;}
	//Double_t GetPath() const {return fPath;}

	friend std::ostream& operator<< (std::ostream& out, const PndLumiStrip& strip)
	{
		out <<"( STRIP Nr. ,  CHARGE ) : ( " << strip.GetIndex() << " , "<< strip.GetCharge() <<" ) ";
		return out;
	}

	private :
		Int_t fNumber;
		Double_t fCharge;
		//Double_t fPath;

		ClassDef(PndLumiStrip,1);
};

#endif /*PNDLUMISTRIP_H_*/
