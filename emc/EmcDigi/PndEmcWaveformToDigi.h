//----------------------------------------------------------------------
// File and Version Information:
//      $Id: Exp $
//
// Description:
//      Class PndEmcWaveformToDigi. Module to take the hit list for the 
//      calorimeter and make ADC waveforms from them.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//		
// Author List:
//      Phil Strother                  Original Author
// Dima Melnichuk - adaption for PANDA				
//
// Copyright Information:
//      Copyright (C) 1996             Imperial College
//
//----------------------------------------------------------------------

#ifndef PndEmcWaveformToDigi_HH
#define PndEmcWaveformToDigi_HH

#include "CbmTask.h"
#include <string>		
#include <vector>

class PndEmcHit;
class PndEmcTwoCoordIndex;
class PndEmcWaveform;

class TClonesArray;
class PndEmcDigiPar;
class PndEmcRecoPar;

class PndEmcWaveformToDigi : public CbmTask
{

public:

  // Constructors

  PndEmcWaveformToDigi(Int_t verbose=0, Bool_t storedigis=kTRUE);

  // Destructor

  virtual ~PndEmcWaveformToDigi();

  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void SetStorageOfDigis(Bool_t val); // Method to specify whether digis are stored or not.

private:

	/** Input array of PndEmcWaveforms **/
	TClonesArray* fWaveformArray;  
	
	/** output array of EmcDigis **/
	TClonesArray* fDigiArray;  
	
	Double_t fSampleRate;
	Double_t fEnergyDigiThreshold;
	
  	std::string fDigiPosMethod;// "surface" or "depth"
	Double_t fEmcDigiRescaleFactor;
	Double_t fEmcDigiPositionDepth;

	PndEmcDigiPar*    fDigiPar;      /** Digitisation parameter container **/
	PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/
	/** Get parameter containers **/
	virtual void SetParContainers();
	
	/** Verbosity level **/
	Int_t fVerbose;

	Bool_t fStoreDigis;

	ClassDef(PndEmcWaveformToDigi,1);
	
	
};

#endif





