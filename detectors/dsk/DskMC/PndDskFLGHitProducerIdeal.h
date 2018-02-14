// -------------------------------------------------------------------------
// -----                   PndDskFLGHitProducer header file               -----
// -----                 Created 23/04/10  by Yutie Liang              -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------

/**  PndDskFLGHitProducer.h

 **
 ** Class for producing DSK hits directly from MCPoints

 **/


#ifndef PNDDSKHITPRODUCERIDEAL_H
#define PNDDSKHITPRODUCERIDEAL_H

#include "FairTask.h"
#include "TClonesArray.h"
#include "PndMCTrack.h"
#include "PndDskFLGHit.h" 
#include "PndGeoDskFLG.h" 
#include "PndStack.h"
#include "FairBaseParSet.h"
#include "TString.h"
#include <string>

#include "TRandom.h"

#ifndef ROOT_TParticlePDG
#include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
#include "TDatabasePDG.h"
#endif


class PndDskFLGHitProducerIdeal : public FairTask {

public:

  /** Default constructor **/
  PndDskFLGHitProducerIdeal();

  /** Constructor with verbosity **/
  PndDskFLGHitProducerIdeal(Int_t verbose);

   /** Destructor **/
  virtual ~PndDskFLGHitProducerIdeal();

  /** Initialization of the task **/  
  //  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Executed task **/ 
  virtual void Exec(Option_t* option);

  /** Finish task **/ 
  virtual void Finish();

  /** method AddHit
   **
   ** Adds a DskHit to the HitCollection
   **/
  PndDskFLGHit* AddHit(Int_t trackID, Int_t detectorID,
                TVector3 position_store, TVector3 momentum_store, Double_t time,
                Double_t angIn, Double_t thetaC_store,
                TVector3 Cherenkov_photon, Int_t light_guide, Int_t pixel);

 protected:
  
 private:
  
  Int_t fDetectorID;
  TVector3 fPosHit;
  TVector3 fDPosHit;
  Double_t fThetaC, fErrThetaC;
  Int_t fRefIndex;

  Int_t f_light_guide;
  Int_t f_pixel;

  TClonesArray* fDskParticleArray; // DSK MC points in the bars
  TClonesArray* fHitArray; // DSK hits
  // TObjArray *fVolumeArray;
  // TClonesArray* fListStack;     // Tracks
  
  //PndGeoDskPar *fPar;           

  PndGeoDskFLG *fGeo;

  /** Set the parameters to the default values. **/
  void SetDefaultParameters();

  /** Verbosity level **/
  Int_t fVerbose;
  
  Int_t nevents;

  TRandom *rand;

  ClassDef(PndDskFLGHitProducerIdeal,1)

};

#endif
