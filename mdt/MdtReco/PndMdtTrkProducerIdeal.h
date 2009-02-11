#ifndef PNDMDTTRKPRODUCERIDEAL_H
#define PNDMDTTRKPRODUCERIDEAL_H 1


#include "FairTask.h"
#include "PndMdtTrk.h"

class TClonesArray;

class PndMdtTrkProducerIdeal : public FairTask
{

 public:

  /** Default constructor **/  
  PndMdtTrkProducerIdeal();


  /** Destructor **/
  ~PndMdtTrkProducerIdeal();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  PndMdtTrk* AddTrk(PndMdtTrk* track);

   private: 

  /** Input array of PndMdtPoint **/
  TClonesArray* fPointArray; 

  /** Input array of PndMdtHit **/
  TClonesArray* fHitArray; 

  /** Input array of PndMCTrack **/
  TClonesArray* fMCArray;

  /** Output array of PndMdtTrk **/
  TClonesArray* fTrkArray;  
   
  ClassDef(PndMdtTrkProducerIdeal,1);

};

#endif
