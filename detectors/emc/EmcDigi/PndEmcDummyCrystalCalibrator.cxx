//---------------------------------------------------------
// Description:
// 	Dummy CrystalCalibrator for Emc Crystals.
//
// 	Christian Hammann
// 	31.7.2012

//---------------------------------------------------------

#include "PndEmcDummyCrystalCalibrator.h"

PndEmcAbsCrystalCalibrator::CalibrationStatus_t PndEmcDummyCrystalCalibrator::Calibrate(Double_t &, Long_t , Int_t ){// Energy detId SignalNr //[R.K.03/2017] unused variable(s)
	return kCALOK;
}
ClassImp(PndEmcDummyCrystalCalibrator);
