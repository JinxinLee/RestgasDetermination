#ifndef PNDMVDRECOCHARGE_H
#define PNDMVDRECOCHARGE_H

#include <iostream>
#include <cmath>
#include "PndSdsDigi.h"

class PndMvdRecoCharge				//calculates the charge of a digipixel for the given tot
{

public:
	PndMvdRecoCharge();
	PndMvdRecoCharge(Double_t tr, Double_t a, Double_t threshold);
	//~PndMvdPixelCharge();
	Double_t GetCharge(PndSdsDigi &digi);
	Double_t GetCharge(Double_t tot);

private:
	Double_t fthreshold;			//threshold [e]
	Double_t ftr;					//raising time [ns]
	Double_t fa;					//falling ration [e/ns]

};
#endif /*PNDMVDRECOCHARGE_H*/
