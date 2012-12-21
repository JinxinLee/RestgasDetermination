//---------------------------------------------------------
// Description:
// 	Calibrator for Emc Crystals for use in Simulations
//
// 	Christian Hammann
// 	31.7.2012

//---------------------------------------------------------

#include "PndEmcSimCrystalCalibrator.h"

using std::map;
using std::pair;

PndEmcSimCrystalCalibrator::PndEmcSimCrystalCalibrator() :fRandomCal(kFALSE),fRandomSigma(0),PndEmcAbsCrystalCalibrator(){
};

void PndEmcSimCrystalCalibrator::SetCalibration(Int_t ModId, Double_t cal){
	fModuleCalibrationMap.insert(pair<Int_t,Double_t>(ModId,1.0/cal));
}

void PndEmcSimCrystalCalibrator::CreateRandomCalibration(Double_t Sigma){
	//not implemented yet
};

Bool_t PndEmcSimCrystalCalibrator::Init(){
	return kTRUE;
};

PndEmcAbsCrystalCalibrator::CalibrationStatus_t PndEmcSimCrystalCalibrator::Calibrate(Double_t &Energy, Long_t DetId, Int_t SignalNr){
	Int_t ModId = (Int_t)(DetId / 1e8);
	map<Int_t,Double_t>::iterator it;
	it=fModuleCalibrationMap.find(ModId);
	if(it!=fModuleCalibrationMap.end()){
		Energy*=it->second;
	}else{
		return kCALMISSING;
	}
	if(fRandomCal){
		map<Long_t,Double_t>::iterator it2;
		it2 = fCrystalCalibrationMap.find(DetId);
		if(it2!=fCrystalCalibrationMap.end()){
			Energy*=it2->second;
		}else{
			return kCALMISSING;
		}
	}
	return kCALOK;
};

ClassImp(PndEmcSimCrystalCalibrator);
