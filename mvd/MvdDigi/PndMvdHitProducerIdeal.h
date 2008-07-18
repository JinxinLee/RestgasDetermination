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


#ifndef PNDMVDHITPRODUCERIDEAL_H
#define PNDMVDHITPRODUCERIDEAL_H 

#include "CbmTask.h"
#include "PndMvdGeoPar.h"

#include "TVector3.h"
#include "TRandom.h"
 
class TClonesArray;

class PndMvdHitProducerIdeal : public CbmTask
{
 public:

  /** Default constructor **/  
  PndMvdHitProducerIdeal();


  /** Destructor **/
  virtual ~PndMvdHitProducerIdeal();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);


 private:
  
  TString fBranchName;
  /** Input array of PndMvdMCPoints **/
  TClonesArray* fPointArray;

  /** Output array of PndMvdHits **/
  TClonesArray* fHitArray;  
  
  PndMvdGeoPar* fGeoPar;
  
  void Register();
  
  void Reset();
  
  void ProduceHits();

  ClassDef(PndMvdHitProducerIdeal,3);

};

#endif
