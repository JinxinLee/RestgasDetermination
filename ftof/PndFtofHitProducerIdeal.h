// -------------------------------------------------------------------------
// -----                 PndFtofHitProducerIdeal  header file             -----
// -----                  Created by Alicia Sanchez              -----
// -------------------------------------------------------------------------


/** PndFtofHitProducerIdeal.h
 *@author a.sanchez <a.sanchez@gsi.de>
 **
 ** The ideal hit producer produces hits of type PndFtofHitProducerIdeal.h by copying
 ** the MCPoint position.  Using the hits from 
 ** this HitProducer is thus equivalent to using MC information
 ** directly, but with the correct data interface.
 **/


#ifndef PNDFTOFHITPRODUCERIDEAL_H
#define PNDFTOFHITPRODUCERIDEAL_H 

#include "FairTask.h"
#include "PndGeoFtofPar.h"
#include "PndFtofPoint.h"

#include "FairGeoVector.h"
#include "FairGeoTransform.h"

#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TRandom.h"
#include <string>

class TClonesArray;

class PndFtofHitProducerIdeal : public FairTask
{
 public:

  /** Default constructor **/  
  PndFtofHitProducerIdeal();
  PndFtofHitProducerIdeal(Double_t dt,Double_t dt2);

  /** Destructor **/
  ~PndFtofHitProducerIdeal();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  void smear(Double_t& time, Double_t& dt) {};

  void RunTimeBased(){fTimeOrderedDigi = kTRUE;}

  /** Set the Persistency of PndFtofHit **/
  void SetPersistency(Bool_t pers = kTRUE) { fPersistency = pers; };
  
 private:
  
  TString fBranchName;
  Bool_t fPersistency;                       ///< Persistency flag
  
  /** Input array of PndFtofPoints **/
  TClonesArray* fPointArray;
 
  /** Output array of PndFtofHits **/
  TClonesArray* fHitArray; 

  
  PndGeoFtofPar* fGeoPar;
  Double_t fdt,fdt2;
  Bool_t fTimeOrderedDigi;
  
  void Register();
  
  void Reset();
  
  void ProduceHits();
/* TGeoHMatrix GetTransformation (std::string detName) const; */
/* void GetLocalHitPoints(PndFtofPoint* myPoint, FairGeoVector& myHitIn,FairGeoVector& myInL); */
/* TVector3 GetSensorDimensions(std::string detName) const; */
  ClassDef(PndFtofHitProducerIdeal,3);

};

#endif
