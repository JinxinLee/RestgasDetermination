#include "PndMvdRecoCharge.h"

///Default constructor
	PndMvdRecoCharge::PndMvdRecoCharge(){
		ftr = 100.;
    	fa = 60.;
    	fthreshold = 3000;
    	std::cout << "-W- <PndMvdRecoCharge::PndMvdRecoCharge()>: constructor with default parameters used"<<std::endl;
	};

///Main constructor
	PndMvdRecoCharge::PndMvdRecoCharge(Double_t tr, Double_t a, Double_t threshold){
		ftr = tr;
    	fa = a;
    	fthreshold = threshold;
    };

Double_t PndMvdRecoCharge::GetCharge(PndSdsDigi &digi)
{
	if (digi.GetTot()<0){
		std::cout<<"-E- <PndMvdRecoCharge::GetCharge(PndSdsDigi &digi)>: time over threshold not calculated in digitization step"<<std::endl;
		return -1;
	}
	return (-fa*ftr+fthreshold+digi.GetTot()*fa)/2.+sqrt( pow( (fa*ftr-fthreshold-digi.GetTot()*fa),2) / 4. + fa* fthreshold * ftr );
}

Double_t PndMvdRecoCharge::GetCharge(Double_t tot)
{
	if (tot<0){
		std::cout<<"-E- <PndMvdRecoCharge::GetCharge(Double_t tot)>: time over threshold not calculated in digitization step"<<std::endl;
		return -1;
	}
	return (-fa*ftr+fthreshold+tot*fa)/2.+sqrt( pow( (fa*ftr-fthreshold-tot*fa),2) / 4. + fa* fthreshold * ftr );
}





