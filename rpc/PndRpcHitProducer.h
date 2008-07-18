// -------------------------------------------------------------------------
// -----                CbmTofHitProducer  header file             -----
//----                  Created  12/06/08 by A.Galoyan                ----
// -------------------------------------------------------------------------


/** CbmTofHitProducer.h
 **
 **/


#ifndef PNDRPCHITPRODUCER_H
#define PNDRPCHITPRODUCER_H 1

#include "CbmTask.h"

class TClonesArray;
class PndRpcHit;
class PndRpcPoint;
class TVector3;


class PndRpcHitProducer : public CbmTask
{

 public:

  /** Default constructor **/  
  PndRpcHitProducer();


  /** Destructor **/
  ~PndRpcHitProducer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);



 private:

  /** Input array of PndRpcPoints **/
  TClonesArray* fPointArray;

  /** Output array of PndRpcHits **/
  TClonesArray* fHitArray;  


  ClassDef(PndRpcHitProducer,1);

};

#endif
