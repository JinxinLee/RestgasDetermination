//---------------------------------------------------------
// Description:
// 	Dummy CrystalCalibrator for Emc Crystals.
//
// 	Christian Hammann
// 	31.7.2012

//---------------------------------------------------------

#include "PndEmcDummyCrystalCalibrator.h"

PndEmcAbsCrystalCalibrator::CalibrationStatus_t PndEmcDummyCrystalCalibrator::Calibrate(Double_t &Energy, Long_t detId, Int_t SignalNr){
	return kCALOK;
}
ClassImp(PndEmcDummyCrystalCalibrator);
