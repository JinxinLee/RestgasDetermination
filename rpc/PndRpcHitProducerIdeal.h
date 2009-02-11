// -------------------------------------------------------------------------
// -----                CbmTofHitProducerIdeal header file             -----
// -----                  Created 07/03/06  by V. Friese               -----
//----                   Modified 04/12/07 by A.Galoyan                ----
// -------------------------------------------------------------------------


/** CbmTofHitProducerIdeal.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** The ideal hit producer produces hits of type CbmTofHit by copying
 ** the MCPoint position. The position error is set to 1 mum, much 
 ** smaller than can be obtained by any detector. Using the hits from 
 ** this HitProducer is thus equivalent to using MC information
 ** directly, but with the correct data interface.
 **/


#ifndef PNDRPCHITPRODUCERIDEAL_H
#define PNDRPCHITPRODUCERIDEAL_H 1

#include "FairTask.h"

class TClonesArray;
class PndRpcHit;
class PndRpcPoint;
class TVector3;


class PndRpcHitProducerIdeal : public FairTask
{

 public:

  /** Default constructor **/  
  PndRpcHitProducerIdeal();


  /** Destructor **/
  ~PndRpcHitProducerIdeal();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);


 private:

  /** Input array of PndRpcPoints **/
  TClonesArray* fPointArray;

  /** Output array of PndRpcHits **/
  TClonesArray* fHitArray;  

  /** smearing of time of flight **/
//    Double_t  Smear(Double_t tof, Double_t sig_t);


  ClassDef(PndRpcHitProducerIdeal,1);

};

#endif
