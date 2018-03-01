//#pragma once

#ifndef PNDEMCFWENDCAPDIGI_H_
#define PNDEMCFWENDCAPDIGI_H_

#include "PndEmcPSAFPGA/PndEmcHighLowPSA.h"
#include "PndEmcDigiWriteoutBuffer.h"
#include "PndEmcSimCrystalCalibrator.h"
#include "PndEmcPSAFPGA/PndEmcPSAFPGASampleAnalyser.h"

#include "PndBranchTask.h"

#include <string>
//#include <vector>

class PndEmcHit;
class PndEmcTwoCoordIndex;
class PndEmcWaveform;

class TClonesArray;
class PndEmcFWEndcapDigiPar;
class PndEmcRecoPar;
class PndEmcGeoPar;
class PndEmcAbsPSA;
class PndEmcAbsPulseshape;

/**
 * @brief Task to create digis from waveforms.
 * 
 * This is an alternative to PndEmcWaveformToDigi, which despite its name could be 
 * used for all the EMC, not just FwEndcap.
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcFWEndcapDigi : public PndBranchTask
{
public:
  // Constructors
  PndEmcFWEndcapDigi(Int_t verbose=0, Bool_t storedigis=kTRUE);
  // Destructor
  virtual ~PndEmcFWEndcapDigi();

  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);

  void SetStorageOfData(Bool_t val); // Method to specify whether digis are stored or not.
  void RunTimeBased(){fTimeOrderedDigi = kTRUE;};

  void SetPSAAlgorithms(PndEmcAbsPSA* highgain, PndEmcAbsPSA* lowgain) {
	  fHighgainPSA = dynamic_cast<PndEmcPSAFPGASampleAnalyser*>(highgain);
	  fLowgainPSA = dynamic_cast<PndEmcPSAFPGASampleAnalyser*>(lowgain);

	  if(fHighgainPSA==NULL || fLowgainPSA==NULL) {
	  	std::cerr << "-E- in PndEmcFWEndcapDigi::SetPSAAlgorithms: PSA must be a PndEmcPSAFPGASampleAnalyser. External PSA setting has no effect" << std::endl;
	  }
  } 

  virtual void SetCrystalCalibrator(PndEmcAbsCrystalCalibrator *Cal){
	  fCalibrator = dynamic_cast<PndEmcSimCrystalCalibrator*>(Cal);
	  if(fCalibrator==NULL) {
	  	std::cerr << "-E- in PndEmcFWEndcapDigi::SetCrystalCalibrator: Calibrator must be a PndEmcSimCrystalCalibrator. External Calibrator setting has no effect" << std::endl;
	  }
  };
protected:
	/** Get parameter containers **/
	virtual void SetParContainers();
private:
	// don't allow copying (-Weffc++)
	PndEmcFWEndcapDigi(const PndEmcFWEndcapDigi&);	// no implementation
	PndEmcFWEndcapDigi& operator= (const PndEmcFWEndcapDigi&);	// no implementation

private:

	/** Input array of PndEmcWaveforms **/
	TClonesArray* fWaveformArray;  
	
	/** output array of EmcDigis **/
	TClonesArray* fDigiArray;  
	
	Double_t fEnergyDigiThreshold;
	TString fDigiPosMethod;	//"surface" or "depth"
	Double_t fEmcDigiRescaleFactor;
	Double_t fEmcDigiPositionDepthPWO;
	Double_t fEmcDigiPositionDepthShashlyk;

	PndEmcPSAFPGASampleAnalyser* fHighgainPSA;
	PndEmcPSAFPGASampleAnalyser* fLowgainPSA;
	PndEmcHighLowPSA fHighLowPSA;

	PndEmcSimCrystalCalibrator *fCalibrator;

	PndEmcFWEndcapDigiPar*    fDigiPar;      //!< Digitisation parameter container
	PndEmcRecoPar*    fRecoPar;      //!< Reconstruction parameter container
	PndEmcGeoPar*    fGeoPar;      //!< Digitisation parameter container

	/** Verbosity level **/
	Int_t fVerbose;

	Bool_t fTimeOrderedDigi; ///<set to kTRUE to use the time ordering of the output data.

	ClassDef(PndEmcFWEndcapDigi,1);
};

#endif
