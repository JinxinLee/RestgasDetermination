//-----------------------------------------------------------------------
// File and Version Information:
// $Id: $
// Software developed for the PANDA Detector at GSI.		
//---------------------------------------------------------------------

#ifndef PNDEMCMAKERECOHIT_HH
#define PNDEMCMAKERECOHIT_HH

#include "CbmTask.h"
#include <string>
#include <vector>		

class TClonesArray;
class TObjectArray;
class PndEmcBump;
class PndEmcRecoPar;

class PndEmcMakeRecoHit : public CbmTask
{


public:

  // Constructors

  PndEmcMakeRecoHit(Int_t verbose=0);

  // Destructor

  virtual ~PndEmcMakeRecoHit( );

  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);


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
	
	ClassDef(PndEmcMakeRecoHit,1)
};
#endif // PNDEMCMAKERECOHIT_HH
