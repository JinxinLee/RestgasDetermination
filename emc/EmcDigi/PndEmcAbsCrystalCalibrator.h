#ifndef EMCABSCRYSTALCALIBRATOR_HH
#define EMCABSCRYSTALCALIBRATOR_HH
//---------------------------------------------------------
// Description:
// 	Abstract Interface for a CrystalCalibrator for Emc Crystals.
//
// 	Christian Hammann
// 	31.7.2012

//---------------------------------------------------------

#include "TObject.h"

/**
 * @brief Abstract interface for a emc calibrator
 *
 * @return 
 **/
class PndEmcAbsCrystalCalibrator { 
	public:
		PndEmcAbsCrystalCalibrator():fVerbose(0){};
		virtual ~PndEmcAbsCrystalCalibrator(){};

		/**
		 * @brief Init
		 *
		 * @return kTRUE on success
		 **/
		virtual Bool_t Init(){return kTRUE;};

		/**
		 * @brief ReInit
		 *
		 * @return kTRUE on success
		 **/
		
		virtual Bool_t ReInit(){return kTRUE;};
		
		virtual void SetVerbose(Int_t verbose){fVerbose=verbose;};
		
		enum CalibrationStatus_t {kCALOK , kCALMISSING, kCALERROR};

		/**
		 * @brief Apply CrystalCalibration to Energy of Crystal derId 
		 *
		 * @param Double_t  Energy in Crystal
		 * @param Long_t  detId of Crystal
		 * @param Int_t  Number of Signal (in MultiWaveform)
		 *
		 * @return PndEmcAbsCrystalCalibrator::kCALOK if successfull
		 **/
		
		virtual CalibrationStatus_t Calibrate(Double_t &Energy, Long_t detId, Int_t SignalNr=1)=0;
	protected:
	Int_t fVerbose;
	ClassDef(PndEmcAbsCrystalCalibrator,1);	
};
#endif
