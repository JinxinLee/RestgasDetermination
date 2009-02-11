#ifndef PNDMDTTRKPRODUCER_H
#define PNDMDTTRKPRODUCER_H 1


#include "FairTask.h"
#include "PndMdtTrk.h"
#include "PndMdtRecoPar.h"

class TClonesArray;

class PndMdtTrkProducer : public FairTask
{

 public:

  /** Default constructor **/  
  PndMdtTrkProducer();


  /** Destructor **/
  ~PndMdtTrkProducer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  PndMdtTrk* AddTrk(PndMdtTrk* track);

  Bool_t UseSimulation()        const { return fUseSimulation; };

  void SetSimulation(Bool_t sim)      { fUseSimulation = sim; };
   
  virtual void SetParContainers();
  
   private: 

  /** Input array of PndMdtPoint **/
  TClonesArray* fPointArray; 

  /** Input array of PndMdtHit **/
  TClonesArray* fHitArray; 

    /** Output array of PndMdtTrk **/
  TClonesArray* fTrkArray; 

  PndMdtRecoPar *fRecoPar;
  Bool_t fUseSimulation;
   
  ClassDef(PndMdtTrkProducer,1);

};

#endif
