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

#ifndef PndEmcMakeDigi_HH
#define PndEmcMakeDigi_HH

#include "CbmTask.h"
#include <string>		

class PndEmcTwoCoordIndex;
class PndEmcHit;
class PndEmcDigi;

class TClonesArray;
class PndEmcDigiPar;
class PndEmcRecoPar;

class PndEmcMakeDigi : public CbmTask
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

  void SetStorageOfDigis(Bool_t val); // Method to specify whether digis are stored or not.
  
private:

	/** Input array of EmcWaveforms **/
	TClonesArray* fHitArray;  
	
	/** output array of EmcDigis **/
	TClonesArray* fDigiArray;  
	
	Double_t fThreshold;
	
  	std::string fDigiPosMethod;// "surface" or "depth"
	Double_t fEmcDigiRescaleFactor;
	Double_t fEmcDigiPositionDepth;
	
	Int_t fUseDigiEffectiveSmearing;
	Double_t fSigmaEa;
	Double_t fSigmaEb;
	
	Int_t fMapVersion;

	PndEmcDigiPar*    fDigiPar;      /** Digitisation parameter container **/
	PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/
	/** Get parameter containers **/
	virtual void SetParContainers();
	
	Bool_t fStoreDigis;

	ClassDef(PndEmcMakeDigi,1);
};

#endif





