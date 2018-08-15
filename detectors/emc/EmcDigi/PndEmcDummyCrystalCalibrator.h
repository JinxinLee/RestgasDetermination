#ifndef PNDEMCDUMMYCRYSTALCALIBRATOR_HH
#define PNDEMCDUMMYCRYSTALCALIBRATOR_HH

//---------------------------------------------------------
// Description:
// 	Dummy Calibrator for Emc Crystals.
//
// 	Christian Hammann
// 	31.7.2012

//---------------------------------------------------------

#include "PndEmcAbsCrystalCalibrator.h"

/**
 * @brief Dummy Calibrator for Emc Crystals
 * @author Ch. Hammann <chammann@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcDummyCrystalCalibrator: public PndEmcAbsCrystalCalibrator{
	public:
		PndEmcDummyCrystalCalibrator():PndEmcAbsCrystalCalibrator(){};
		virtual ~PndEmcDummyCrystalCalibrator(){};
		virtual CalibrationStatus_t Calibrate(Double_t &Energy, Long_t detId, Int_t SignalNr=1);
	private:
		ClassDef(PndEmcDummyCrystalCalibrator,1);
};
#endif
