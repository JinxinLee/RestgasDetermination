//---------------------------------------------------------------------- // File and Version Information:
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
#pragma once
#ifndef PndEmcWaveformToDigi_H
#define PndEmcWaveformToDigi_H

#include "FairTask.h"
#include "PndEmcDigiWriteoutBuffer.h"
#include <string>		
#include "FairTSBufferFunctional.h"
#include <map>
#include <list>
#include <set>
#include "TTree.h"
#include "TFile.h"
#include "TVectorD.h"

class PndEmcHit;
class PndEmcTwoCoordIndex;
class PndEmcWaveform;

class TClonesArray;
class PndEmcDigiPar;
class PndEmcRecoPar;
class PndEmcFpgaPar;
class PndEmcAbsPSA;
class PndEmcAbsPulseshape;
class PndEmcAbsCrystalCalibrator;


class PndEmcWaveformToDigi : public FairTask
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

  void SetStorageOfData(Bool_t val); // Method to specify whether digis are stored or not.
  void RunTimeBased(){fTimeOrderedDigi = kTRUE;}

	void UseDigitizationVersion2() { fDigitizationVersion2 = kTRUE;}

	/**
	 * @brief Set PSA Algorithm to be used for Barrel and Endcaps
	 *
	 * @param psa  The psa to be used
	 *
	 **/
	virtual void SetPSAAlgorithm(PndEmcAbsPSA *psa){fpsaAlgorithm=psa;}
	/**
	 * @brief Set PSA Algorithm to be used for Forward Shashlik
	 *
	 * @param psa  The psa to be used
	 *
	 * @return 
	 **/
	virtual void SetPSAAlgorithmPMT(PndEmcAbsPSA *psa){fpsaAlgorithm_pmt=psa;}
	/**
	 * @brief Set The Crystal Calibrator to use
	 *
	 * @param Cal The Calibrator to be used  
	 *
	 * @return 
	 **/
	virtual void SetCrystalCalibrator(PndEmcAbsCrystalCalibrator *Cal){fCalibrator=Cal;}

	virtual void FinishTask();

	bool isUseCalibartion ;
	bool isUsePileupAna;
private:
	//Double_t GetEventTimebyDigiTime(Double_t digiT, Double_t digiE, Int_t detID, bool PrintOut=false) const;
	//Int_t GetIdxByEnergy(Double_t energy) const;

	/** Input array of PndEmcWaveforms **/
	TClonesArray* fWaveformArray;  
	//TClonesArray* fMcTrackArray;  
	TClonesArray* fEvtHeaderArray;  

	/** output array of EmcDigis **/
	TClonesArray* fDigiArray;  

	Double_t fSampleRate;
	Double_t fSampleRate_PMT;
	Double_t fSampleRate_FWD;
	Double_t fEnergyDigiThreshold;
	Double_t fASIC_Shaping_int_time;      //s
	Double_t fPMT_Shaping_int_time;    //s
	Double_t fPMT_Shaping_diff_time;    //s
	Double_t fCrystal_time_constant;  //s
	Double_t fShashlyk_time_constant;  //s
	Int_t fNumber_of_samples_in_waveform;
	Int_t fNumber_of_samples_in_waveform_pmt;
	Int_t fNumber_of_samples_in_waveform_fwd;
	Double_t  fFWD_Shaping_int_time;
	Double_t	fFWD_time_constant;

	//std::string fDigiPosMethod;// "surface" or "depth"
	TString fDigiPosMethod;
	Double_t fEmcDigiRescaleFactor;
	Double_t fEmcDigiPositionDepthPWO;
	Double_t fEmcDigiPositionDepthShashlyk;

	PndEmcAbsPulseshape* fPulseshape;
	PndEmcAbsPulseshape* fPulseshape_pmt;
	PndEmcAbsPulseshape* fPulseshape_fwd;
	PndEmcAbsPSA *fpsaAlgorithm;
	PndEmcAbsPSA *fpsaAlgorithm_pmt;
	PndEmcAbsPSA *fpsaAlgorithm_fwd;

	PndEmcAbsCrystalCalibrator *fCalibrator;

	PndEmcDigiPar*    fDigiPar;      /** Digitisation parameter container **/
	PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/
	PndEmcFpgaPar*    fFpgaPar;      /** FPGA parameter container **/
	/** Get parameter containers **/
	virtual void SetParContainers();

	//PndEmcDigiWriteoutBuffer* fDataBuffer;

	/** Verbosity level **/
	Int_t fVerbose;
	Bool_t fStoreDigis;
	Bool_t fTimeOrderedDigi; ///<set to kTRUE to use the time ordering of the output data.

	Double_t fWfNormalisation; // Waveform normalisation constant
	Double_t fWfNormalisation_fwd; // Waveform normalisation constant
	Double_t fWfNormalisation_pmt;

	PndEmcWaveformToDigi(const  PndEmcWaveformToDigi& L);
	PndEmcWaveformToDigi& operator= (const  PndEmcWaveformToDigi&) {return *this;};

	ClassDef(PndEmcWaveformToDigi,1);

	BinaryFunctor* fFunctor;

	//coefficients
	TVectorD CoeffMod3;
	TVectorD CoeffMod5;
	TVectorD CoeffModo;

	Int_t fEventNo;
	Double_t fTimeWindow;
	Double_t fTimeShift;
	//buffer for undetermined event

	Int_t totDigisAboveThreshold;
	Int_t totHits ;
	Int_t totExpHits ;
	Int_t totNumOfWave ;

	Bool_t fDigitizationVersion2;
	//

};

#endif





