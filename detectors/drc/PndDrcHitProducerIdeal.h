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

#ifndef PNDDRCHITPRODUCERIDEAL_H
#define PNDDRCHITPRODUCERIDEAL_H

#include "FairTask.h"
#include "TClonesArray.h"
#include "PndMCTrack.h"
#include "PndDrcHit.h"
#include "PndStack.h"
#include "FairBaseParSet.h"
#include "PndGeoDrcPar.h"
#include "TString.h"
#include <string>

#ifndef ROOT_TParticlePDG
#include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
#include "TDatabasePDG.h"
#endif

class PndDrcHitProducerIdeal : public FairTask {

public:

  /** Default constructor **/
  PndDrcHitProducerIdeal();

  /** Constructor with verbosity **/
  PndDrcHitProducerIdeal(Int_t verbose);

   /** Destructor **/
  virtual ~PndDrcHitProducerIdeal();

  /** Initialization of the task **/  
  //  virtual void SetParContainers();
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
		    Int_t sensorID, 
		    TVector3 posHit, 
		    TVector3 dPosHit, 
		    Double_t thetaC,
		    Double_t errThetaC,
		    Int_t index);

  void SetPersistency(Bool_t v = kTRUE) { fPersistency = v; }

 private:
  
  Int_t fDetectorID;
  TVector3 fPosHit;
  TVector3 fDPosHit;
  Double_t fThetaC, fErrThetaC;
  Int_t fRefIndex;
  Bool_t fPersistency;

  TClonesArray* fBarPointArray; // DRC MC points in the bars
  TClonesArray* fHitArray;      // DRC hits
  
  PndGeoDrcPar *fPar;
  PndGeoDrc* fGeo;              // Basic geometry data of barrel DRC.

  void SetDefaultParameters();

  Int_t nevents;

  ClassDef(PndDrcHitProducerIdeal,1)

};

#endif
