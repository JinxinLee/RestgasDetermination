#ifndef PNDEMCSIMCRYSTALCALIBRATOR_HH
#define PNDEMCSIMCRYSTALCALIBRATOR_HH

//---------------------------------------------------------
// Description:
// 	File Calibrator for Emc Crystals.
//
// 	Christian Hammann
// 	31.7.2012

//---------------------------------------------------------

#include "PndEmcAbsCrystalCalibrator.h"
#include "TString.h"
#include <map>

/**
 * @brief Class to simulate a Calibration
 *
 * @return 
 **/
class PndEmcSimCrystalCalibrator: public PndEmcAbsCrystalCalibrator{
	public:
		PndEmcSimCrystalCalibrator();

		/**
		 * @brief Set Calibration for a Module
		 *
		 * @param Int_t  Module
		 * @param Double_t  Height of 1GeV Pulse
		 *
		 * @return 
		 **/
		virtual void SetCalibration(Int_t ModId, Double_t cal);	

		/**
		 * @brief Creates a random gaussion distributed calibration for all crystals
		 * 	Not implemented yet
		 *
		 * @param Double_t  relative spread of calibration values
		 *
		 * @return 
		 **/
		virtual void CreateRandomCalibration(Double_t Sigma);
		virtual Bool_t Init();

		virtual ~PndEmcSimCrystalCalibrator(){};
		virtual CalibrationStatus_t Calibrate(Double_t &Energy, Long_t DetId, Int_t SignalNr=1);
	private:
		Double_t fRandomCal;
		Double_t fRandomSigma;
		std::map<Int_t, Double_t> fModuleCalibrationMap;
		std::map<Long_t, Double_t> fCrystalCalibrationMap;
		ClassDef(PndEmcSimCrystalCalibrator,1);
};
#endif
