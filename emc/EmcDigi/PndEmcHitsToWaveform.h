//----------------------------------------------------------------------
// File and Version Information:
//      $Id: Exp $
//
// Description:
//      Class PndEmcHitsToWaveform. Module to take the hit list for the 
//      calorimeter and make ADC waveforms from them.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//
// Author List:
//      Phil Strother                  Original author
// 		 Dima Melnichuk - adaption for PANDA
// Copyright Information:
//      Copyright (C) 1996             Imperial College
//----------------------------------------------------------------------
#pragma once
#ifndef PndEmcHitsToWaveform_H
#define PndEmcHitsToWaveform_H

#include "PndEmcWaveform.h"
#include "PndEmcTwoCoordIndex.h"

#include "FairTask.h"
//#include <string>
//#include <map>

//class PndEmcTwoCoordIndex;
//class PndEmcWaveform;
class PndEmcMapper;

class TClonesArray;
class PndEmcDigiPar;

class PndEmcHitsToWaveform : public FairTask
{

public:

  // Constructors

  PndEmcHitsToWaveform(Int_t verbose=0, Bool_t storewaves=kTRUE);

  // Destructor

  virtual ~PndEmcHitsToWaveform();

  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  
  PndEmcWaveform * AddWaveform(Int_t detId,Int_t hitIndex);

  void SetStorageOfData(Bool_t val); // Method to specify whether waveforms are stored or not.

private:

  /** Input array of PndEmcHits **/
  TClonesArray* fHitArray;

  /** Output array of PndEmcWaveforms **/
  TClonesArray* fWaveformArray;  

	Double_t fOneBitResolution;
	Double_t fOneBitResolutionBW;

	Int_t fNBits;
	Double_t fDetectedPhotonsPerMeV;
	Double_t fEnergyRange; //GeV
	Double_t fEnergyRangeBW; //GeV
	Double_t fExcessNoiseFactor;
	Double_t fFirstSamplePhase;
	Int_t fNumber_of_samples_in_waveform;
	Double_t fShaping_diff_time;     //s
	Double_t fShaping_int_time;      //s
	Double_t fCrystal_time_constant;  //s
	Double_t fIncoherent_elec_noise_width_GeV; //GeV
	Double_t fSampleRate;
	Int_t fUse_shaped_noise;
	Int_t fUse_photon_statistic;
	Int_t fMapVersion;

	Double_t fFirstADCBinTime;

	Double_t fGevPeakAnalogue;

	PndEmcDigiPar*    fDigiPar;      /** Digitisation parameter container **/
	/** Get parameter containers **/
	virtual void SetParContainers();

	std::set<Int_t> fWaveformInd;
	
	/** Verbosity level **/
	Int_t fVerbose;

	Bool_t fStoreWaves;

  ClassDef(PndEmcHitsToWaveform,1);
};

#endif





