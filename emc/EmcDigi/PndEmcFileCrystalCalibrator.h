#ifndef PNDEMCFILECRYSTALCALIBRATOR_HH
#define PNDEMCFILECRYSTALCALIBRATOR_HH

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
 * @brief Class to use a textfile for Calibration
 *
 * @return 
 **/
class PndEmcFileCrystalCalibrator: public PndEmcAbsCrystalCalibrator{
	public:
		PndEmcFileCrystalCalibrator();
		PndEmcFileCrystalCalibrator(const char *CalFileName);

		/**
		 * @brief Set calibration file
		 *
		 * @param const char * filename of calibration file
		 *
		 * @return 
		 **/
		void SetCalibrationFile(const char *CalFileName);
		
		virtual Bool_t Init();

		virtual ~PndEmcFileCrystalCalibrator(){};
		virtual CalibrationStatus_t Calibrate(Double_t &Energy, Long_t detId, Int_t SignalNr=1);
	private:
		TString fCalFileName;
		std::map<Long_t, Double_t> fCalibrationMap;
		std::map<Long_t, Double_t> fGainMap;
		std::map<Long_t, Double_t> fOverflowMap;
		ClassDef(PndEmcFileCrystalCalibrator,1);
};
#endif
