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


#ifndef PNDTOFHITPRODUCERIDEAL_H
#define PNDTOFHITPRODUCERIDEAL_H 

#include "FairTask.h"
#include "PndGeoTofPar.h"
#include "PndTofPoint.h"

#include "FairGeoVector.h"
#include "FairGeoTransform.h"

#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TRandom.h"
#include <string>

class TClonesArray;

class PndTofHitProducerIdeal : public FairTask
{
 public:

  /** Default constructor **/  
  PndTofHitProducerIdeal();
  PndTofHitProducerIdeal(Double_t dt,Double_t dt2);

  /** Destructor **/
  ~PndTofHitProducerIdeal();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  void smear(Double_t& time, Double_t& dt);

 private:
  
  TString fBranchName;
  TString fBranchName2;
  /** Input array of PndTofPoints **/
  TClonesArray* fPointArray;
   TClonesArray* fsciFPointArray;

  /** Output array of PndTofHits **/
  TClonesArray* fHitArray; 
  TClonesArray* fsciFHitArray;
  
  PndGeoTofPar* fGeoPar;
  Double_t fdt,fdt2;
  
  void Register();
  
  void Reset();
  
  void ProduceHits();
TGeoHMatrix GetTransformation (std::string detName) const;
void GetLocalHitPoints(PndTofPoint* myPoint, FairGeoVector& myHitIn,FairGeoVector& myInL);
TVector3 GetSensorDimensions(std::string detName) const;
  ClassDef(PndTofHitProducerIdeal,3);

};

#endif
