// -------------------------------------------------------------------------
// -----                 PndSciTHitProducerIdeal  header file             -----
// -----                  Created by Alicia Sanchez              -----
// -------------------------------------------------------------------------


/** PndSciTHitProducerIdeal.h
 *@author a.sanchez <a.sanchez@gsi.de>
 **
 ** The ideal hit producer produces hits of type PndSciTHitProducerIdeal.h by copying
 ** the MCPoint position.  Using the hits from 
 ** this HitProducer is thus equivalent to using MC information
 ** directly, but with the correct data interface.
 **/


#ifndef PNDSCITHITPRODUCERIDEAL_H
#define PNDSCITHITPRODUCERIDEAL_H 

#include "FairTask.h"
#include "PndGeoSciTPar.h"
#include "PndSciTPoint.h"

#include "FairGeoVector.h"
#include "FairGeoTransform.h"

#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TRandom.h"
#include <string>

class TClonesArray;

class PndSciTHitProducerIdeal : public FairTask
{
 public:

  /** Default constructor **/  
  PndSciTHitProducerIdeal();
  PndSciTHitProducerIdeal(Double_t dt);

  /** Destructor **/
  ~PndSciTHitProducerIdeal();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  void smear(Double_t& time, Double_t& dt);

 private:
  
  TString fBranchName;

  /** Input array of PndSciTPoints **/
  TClonesArray* fPointArray;
 
  /** Output array of PndSciTHits **/
  TClonesArray* fHitArray; 

  
  PndGeoSciTPar* fGeoPar;
  Double_t fdt,fdt2;
  
  void Register();
  
  void Reset();
  
  void ProduceHits();
TGeoHMatrix GetTransformation (std::string detName) const;
void GetLocalHitPoints(PndSciTPoint* myPoint, FairGeoVector& myHitIn);
TVector3 GetSensorDimensions(std::string detName) const;
  ClassDef(PndSciTHitProducerIdeal,1);

};

#endif
