// -------------------------------------------------------------------------
// -----                 CbmStsHitProducerIdel header file             -----
// -----                  Created 10/01/06  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmStsHitProducerIdeal.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** The ideal hit producer produces hits of type CbmStsMapsHit by copying
 ** the MCPoint position. The position error is set to 1 mum, much 
 ** smaller than can be obtained by any detector. Using the hits from 
 ** this HitProducer is thus equivalent to using MC information
 ** directly, but with the correct data interface.
 **/


#ifndef PNDSDSHITPRODUCERIDEAL_H
#define PNDSDSHITPRODUCERIDEAL_H 

#include "FairTask.h"
//#include "PndSdsGeoPar.h"

#include "TVector3.h"
#include "TRandom.h"
 
class TClonesArray;

class PndSdsHitProducerIdeal : public FairTask
{
 public:

  /** Default constructor **/  
  PndSdsHitProducerIdeal();


  /** Destructor **/
  virtual ~PndSdsHitProducerIdeal();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** pure virtual method SetBranchNames
   **
   ** called by Init()
   ** function to set individual branch names
   **/
  virtual void SetBranchNames()=0;


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void SetPersistance(Bool_t p = kTRUE) {fPersistance=p;};
  Bool_t GetPersistance() {return fPersistance;};
  
protected:
  
  Bool_t fPersistance; // switch to turn on/off storing the arrays to a file
  
  TString fBranchName;
  /** Input array of PndSdsMCPoints **/
  TClonesArray* fPointArray;

  /** Output array of PndSdsHits **/
  TClonesArray* fHitArray;  

  TString fOutBranchName;
  TString fFolderName;
  
  //PndSdsGeoPar* fGeoPar;
  
  void Register();
  
  void Reset();
  
  void ProduceHits();

  ClassDef(PndSdsHitProducerIdeal,3);

};

#endif
