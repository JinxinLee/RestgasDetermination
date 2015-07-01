//----------------------------------------------------------------------
// File and Version Information:
//      $Id: Exp $
//
// Description:
//      Class PndEmcMakeDigi. This class takes array of EmcHit's and produce
// an array of EmcDigis.
// It is convenient to study reconstruction algoritms without
// disturbance from digitization
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//----------------------------------------------------------------------
//#pragma once
#ifndef PndEmcMakeDigi_H
#define PndEmcMakeDigi_H

#include "FairTask.h"
#include <string>		

class PndEmcTwoCoordIndex;
class PndEmcHit;
class PndEmcDigi;

class TClonesArray;
class PndEmcGeoPar;
class PndEmcDigiPar;
class PndEmcRecoPar;

class PndEmcMakeDigi : public FairTask
{

public:

  // Constructors

  PndEmcMakeDigi(Bool_t storedigis=kTRUE);

  // Destructor

  virtual ~PndEmcMakeDigi();

  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  
  PndEmcDigi* AddDigi(Int_t trackID,Int_t detID, Float_t energy, Float_t time, Int_t hitIndex);

  void SetStorageOfData(Bool_t val); // Method to specify whether digis are stored or not.
  
private:

	/** Input array of EmcWaveforms **/
	TClonesArray* fHitArray;  
	
	/** output array of EmcDigis **/
	TClonesArray* fDigiArray;  
	
	Double_t fThreshold;
	
  	std::string fDigiPosMethod;// "surface" or "depth"
	Double_t fEmcDigiRescaleFactor;
	Double_t fEmcDigiPositionDepthPWO;
	Double_t fEmcDigiPositionDepthShashlyk;
	
	Int_t fUseDigiEffectiveSmearing;
	Double_t fDetectedPhotonsPerMeV;
	Double_t fNPhotoElectronsPerMeVAPDBarrel;
	Double_t fNPhotoElectronsPerMeVAPDBWD;
	Double_t fNPhotoElectronsPerMeVVPT;
	Double_t fSensitiveAreaAPD; //mm^2
	Double_t fSensitiveAreaVPT; //mm^2
	Double_t fQuantumEfficiencyAPD;
	Double_t fQuantumEfficiencyVPT;
	Double_t fExcessNoiseFactorAPD;
	Double_t fExcessNoiseFactorVPT;
	Double_t fIncoherent_elec_noise_width_GeV_APD; //GeV
	Double_t fIncoherent_elec_noise_width_GeV_VPT; //GeV

	
	Int_t fMapVersion;

	PndEmcGeoPar*     fGeoPar;       /** Geometry parameter container **/
	PndEmcDigiPar*    fDigiPar;      /** Digitisation parameter container **/
	PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/
	/** Get parameter containers **/
	virtual void SetParContainers();
	
	Bool_t fStoreDigis;

        PndEmcMakeDigi(const  PndEmcMakeDigi& L);
        PndEmcMakeDigi& operator= (const  PndEmcMakeDigi&) {return *this;};

	ClassDef(PndEmcMakeDigi,1);
};

#endif





