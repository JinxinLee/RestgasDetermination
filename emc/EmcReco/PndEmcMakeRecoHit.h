//-----------------------------------------------------------------------
// File and Version Information:
// $Id: $
// Software developed for the PANDA Detector at GSI.		
//---------------------------------------------------------------------
#pragma once
#ifndef PNDEMCMAKERECOHIT_H
#define PNDEMCMAKERECOHIT_H

#include "FairTask.h"
//#include <string>
//#include <vector>		

class TClonesArray;
class TObjectArray;
class PndEmcBump;
class PndEmcRecoPar;

class PndEmcMakeRecoHit : public FairTask
{


public:

  // Constructors

  PndEmcMakeRecoHit(Int_t verbose=0, Bool_t storerecohits=kTRUE);

  // Destructor

  virtual ~PndEmcMakeRecoHit( );

  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void SetStorageOfData(Bool_t val); // Method to specify whether recohits are stored or not.

protected:

  
private:
	/** Input array of PndEmcBumps **/
	TClonesArray* fBumpArray;
	
	/** Output array of PndEmcRecoHits **/
	TClonesArray* fRecoHitArray;
	  
	PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/
	/** Get parameter containers **/
	virtual void SetParContainers();
	
	/** Verbosity level **/
	Int_t fVerbose;

	Bool_t fStoreRecoHits;

        PndEmcMakeRecoHit(const  PndEmcMakeRecoHit& L);
        PndEmcMakeRecoHit& operator= (const  PndEmcMakeRecoHit&) {return *this;};
	
	ClassDef(PndEmcMakeRecoHit,1)
};
#endif // PNDEMCMAKERECOHIT_HH
