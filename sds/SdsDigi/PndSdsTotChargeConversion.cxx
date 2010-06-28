#include "PndSdsTotChargeConversion.h"

///Default constructor
PndSdsTotChargeConversion::PndSdsTotChargeConversion(Int_t VerboseLevel) : PndSdsChargeConversion(kToT){
		SetParameter("ftr", 100.);
		SetParameter("fa", 60.);
		SetParameter("fth", 3000.);
		SetParameter("fclk", 50.);
		ftimestep = 1. / GetParameter("fclk") * 1000.;
		fVerboseLevel = VerboseLevel;
		StartExecute();
		Warning("constructor","default constructor used");
		if (fVerboseLevel>0){
			std::cout<<"Tot parameter "<<std::endl;
			std::cout<<"  charge time: "<<GetParameter("ftr")<<" ns"<<std::endl;
			std::cout<<"  const. current: "<<GetParameter("fa")<<" e/ns"<<std::endl;
			std::cout<<"  threshold: "<<GetParameter("fth")<<" e"<<std::endl;
			std::cout<<"  clock frequency: "<<GetParameter("fclk")<<" MHz"<<std::endl;
		}
};

///Main constructor
PndSdsTotChargeConversion::PndSdsTotChargeConversion(Double_t tr, Double_t a, Double_t threshold, Double_t clockfrequency, Int_t VerboseLevel) : PndSdsChargeConversion(kToT) {
	    SetParameter("ftr", tr);				//charge time [ns]
	    SetParameter("fa", a);					//const. curren [e/ns]
	    SetParameter("fth", threshold);
	    SetParameter("fclk", clockfrequency); 	//the frequency the readout chip works with
	    ftimestep = 1. / clockfrequency * 1000.;
	    fVerboseLevel = VerboseLevel;
	    if (fVerboseLevel>0){
			std::cout<<"Tot parameter "<<std::endl;
			std::cout<<"  charge time: "<<GetParameter("ftr")<<" ns"<<std::endl;
			std::cout<<"  const. current: "<<GetParameter("fa")<<" e/ns"<<std::endl;
			std::cout<<"  threshold: "<<GetParameter("fth")<<" e"<<std::endl;
			std::cout<<"  clock frequency: "<<GetParameter("fclk")<<" MHz"<<std::endl;
	    }
		StartExecute();
};

Double_t PndSdsTotChargeConversion::ChargeToDigiValue(Double_t charge){ //returns the TOT in ns
	if (GetParameter("fa") <= 0){
		Error("ConvertChargeToDigiValue(Double_t charge)","const. current is less or equal zero -> now set to 60 e/ns");
		SetParameter("fa",60.);
	}

	Q = charge;
	Qt = GetParameter("fth");

//Error handling: if there is a parameter leading to a division by zero Q=Qt=1 is set to prevent this
		
	if (Qt < 0){
		Error("ConvertChargeToDigiValue(Double_t charge)","threshold is less than zero -> now set to 0 eV");
		Qt = 0.;
	}

	if ((Q <= Qt) or (Q <= 0)){
		Warning("ConvertChargeToDigiValue(Double_t charge)","charge is equal or less than threshold -> zero TOT");
		Q = 1.;
		Qt = 1.;
	}
	
	t1e = (GetParameter("ftr")*Qt/Q+ftimeoffset);							//exact time when signal is over threshold
	t2e = (Q-Qt)/GetParameter("fa")+ftimeoffset+GetParameter("ftr");		//exact time when signal is again below threshold

	//return (t2e-t1e);						//turn off clock
	return GetTotWC();
}

Double_t PndSdsTotChargeConversion::GetRelativeError(Double_t Charge)
{
  // formula from D.Pohl/FZJ
  // dQ_rec     a             /               (Q_t - a*t_c)/2  +  (Q - Q_t)(t_c/Q - 1/a)*a/2              \
  // ------  =  - * dt_max * | 1 + ----------------------------------------------------------------------  |
  //  Q_rec     2             \    sqrt{ [ (Q_t - a*t_c)/2 + (Q - Q_t)(t_c/Q - 1/a)*a/2 ]^2 + a*Q_t*t_c } /
  
  return 0.;
}


Double_t PndSdsTotChargeConversion::GetTotWC(){ //calculates start time, stop time with a clock
	fstarttime = (Int_t) ( t1e / ftimestep ) * ftimestep + ftimestep; //quantization of the start signal
	fstoptime = (Int_t) ( t2e / ftimestep ) * ftimestep + ftimestep; //quantization of the stop signal
/*
	std::cout<<"  start point exact: "<<t1e<<std::endl;
	std::cout<<"  stop point exact: "<<t2e<<std::endl;
	std::cout<<"  time step is: "<<ftimestep<<" ns"<<std::endl;
	std::cout<<"  fclockfrequency is: "<<GetParameter("fclk")<<" MHz"<<std::endl;
	std::cout<<"  time offset is: "<<ftimeoffset<<" ns"<<std::endl;
	std::cout<<"  start point is: "<<fstarttime<<" ns"<<std::endl;
	std::cout<<"  stop point is: "<<fstoptime<<" ns"<<std::endl;
*/
	return (fstoptime-fstarttime);
}

void PndSdsTotChargeConversion::StartExecute(){	//function called for different events to set different time offsets
	ftimeoffset = fRand.Uniform(ftimestep);
}

Double_t PndSdsTotChargeConversion::GetPileUpTime(Double_t fcharge){	//returns the time the detector is not sensitive for other events
	return (GetParameter("ftr") + fcharge / GetParameter("fa"));
}

Double_t PndSdsTotChargeConversion::DigiValueToCharge(Double_t digivalue){ //returns the charge for the given tot value
	if (digivalue<0){
		Error("ConvertDigiValueToELoss(Double_t digi)","charge digitization value not calculated properly");
		return -1;
	}
	return (-GetParameter("fa")*GetParameter("ftr")+GetParameter("fth")+digivalue*GetParameter("fa"))/2.+sqrt( pow( (GetParameter("fa")*GetParameter("ftr")-GetParameter("fth")-digivalue*GetParameter("fa")),2) / 4. + GetParameter("fa")* GetParameter("fth") * GetParameter("ftr"));
}
