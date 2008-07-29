// -------------------------------------------------------------------------
// -----                   PndDrcHitProducer header file               -----
// -----               Created 11/10/06  by Annalisa Cecchi            -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------

/**  PndDrcHitProducer.h

 **
 ** Class for producing DRC hits directly from MCPoints

 **/


#ifndef PNDDRCHITPRODUCER_H
#define PNDDRCHITPRODUCER_H

#include "CbmTask.h"
#include "TClonesArray.h"
#include "CbmMCTrack.h"
#include "PndDrcHit.h"
#include "CbmStack.h"
#include "CbmBaseParSet.h"
#include "PndGeoDrcPar.h"
#include "TString.h"
#include <string>

#ifndef ROOT_TParticlePDG
#include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
#include "TDatabasePDG.h"
#endif


class PndDrcHitProducer : public CbmTask {

public:

  /** Default constructor **/
  PndDrcHitProducer();

  /** Constructor with verbosity **/
  PndDrcHitProducer(Int_t verbose);

   /** Destructor **/
  virtual ~PndDrcHitProducer();

  /** Initialization of the task **/  
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Executed task **/ 
  virtual void Exec(Option_t* option);

  /** Finish task **/ 
  virtual void Finish();

  /** method AddHit
   **
   ** Adds a DrcHit to the HitCollection
   **/
  PndDrcHit* AddHit(Int_t detID, 
		    TVector3 posHit, 
		    TVector3 dPosHit, 
		    Double_t thetaC,
		    Double_t errThetaC,
		    Int_t index);


 protected:
  
 private:
  
  Int_t fDetectorID;
  TVector3 fPosHit;
  TVector3 fDPosHit;
  Double_t fThetaC, fErrThetaC;
  Int_t fRefIndex;

  TClonesArray* fBarPointArray; // DRC MC points in the bars
  TClonesArray* fHitArray; // DRC hits
  TObjArray *fVolumeArray;
  TClonesArray* fListStack;     // Tracks
  
  PndGeoDrcPar *fPar;           //!

  /** Set the parameters to the default values. **/
  void SetDefaultParameters();

  /** Verbosity level **/
  Int_t fVerbose;
  
  Int_t nevents;

  ClassDef(PndDrcHitProducer,1)

};

#endif
