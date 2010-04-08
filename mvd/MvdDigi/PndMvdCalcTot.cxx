#include "PndMvdCalcTot.h"

Double_t PndMvdCalcTot::GetTot(Double_t fcharge){ //returns the TOT in ns
	if (fa <= 0){
		std::cout<<"-W- <PndMvdCalcTot::GetTot(Double_t fcharge)>: falling ration is less or equal zero -> now set to 60 e/ns"<<std::endl;
		fa = 60.;
	}

	Q = fcharge;
	Qt = fthreshold;

//Error handling: if there is a parameter leading to a division by zero Q=Qt=1 is set to prevent this
		
	if (Qt < 0){
		std::cout<<"-W- <PndMvdCalcTot::GetTot(Double_t fcharge)>: threshold is less than zero -> now set to 0 eV"<<std::endl;
		Qt = 0.;
	}

	if ((Q <= Qt) or (Q <= 0)){
		std::cout<<"-W- <PndMvdCalcTot::GetTot(Double_t fcharge)>: charge is equal or less than threshold -> zero TOT"<<std::endl;
		Q = 1.;
		Qt = 1.;
	}
	
	t1e = (ftr*Qt/Q+ftimeoffset);			//exact time when signal is over threshold
	t2e = (Q-Qt)/fa+ftimeoffset+ftr;		//exact time when signal is again below threshold

	//return (t2e-t1e);						//turn off clock
	return GetTotWC();
}

Double_t PndMvdCalcTot::GetTotWC(){ //calculates start time, stop time with a clock
	fstarttime = (Int_t) ( t1e / ftimestep ) * ftimestep + ftimestep; //quantization of the start signal
	fstoptime = (Int_t) ( t2e / ftimestep ) * ftimestep + ftimestep; //quantization of the stop signal
/*
	std::cout<<"  start point exact: "<<t1e<<std::endl;
	std::cout<<"  stop point exact: "<<t2e<<std::endl;
	std::cout<<"  time step is: "<<ftimestep<<" ns"<<std::endl;
	std::cout<<"  fclockfrequency is: "<<fclockfrequency<<" MHz"<<std::endl;
	std::cout<<"  time offset is: "<<ftimeoffset<<" ns"<<std::endl;
	std::cout<<"  start point is: "<<fstarttime<<" ns"<<std::endl;
	std::cout<<"  stop point is: "<<fstoptime<<" ns"<<std::endl;
*/
	return (fstoptime-fstarttime);
}

void PndMvdCalcTot::SetStartOffset(){	//function called for different events to set different time offsets
	ftimeoffset = fRand.Uniform(ftimestep);
}

void PndMvdCalcTot::SetParameter(Double_t tr, Double_t a, Double_t threshold){
	ftr = tr;
	fa = a;
	fthreshold = threshold;
}


Double_t PndMvdCalcTot::GetPileUpTime(Double_t fcharge){	//returns the time the detector is not sensitive for other events
	return (ftr + fcharge / fa);
}
