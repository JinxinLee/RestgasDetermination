#ifndef PNDMDTHITPRODUCERIDEAL_H
#define PNDMDTHITPRODUCERIDEAL_H 1


#include "FairTask.h"
#include "PndMdtHit.h"
#include "TVector3.h"

class TClonesArray;

class PndMdtHitProducerIdeal : public FairTask
{

 public:

  /** Default constructor **/  
  PndMdtHitProducerIdeal();


  /** Destructor **/
  ~PndMdtHitProducerIdeal();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  PndMdtHit* AddHit(Int_t detID, TVector3& pos, TVector3& dpos, Int_t index);

  /** 
   * @brief Set the smearing of the PndMdtHit position
   * @param res Resolution in [cm]
   */
  void SetPositionSmearing(Float_t res) { fPosResolution = res; };
  
  /** Set the Persistency of PndMdtHit **/
  void SetPersistency(Bool_t pers = kTRUE) { fPersistency = pers; };

 private: 
 
  Float_t fPosResolution;                    ///< Position smearing [cm]
  Bool_t fPersistency;                       ///< Persistency flag
  
  /** Input array of PndMdtPoint **/
  TClonesArray* fPointArray;

  /** Output array of PndMdtHit **/
  TClonesArray* fHitArray;  

   
  ClassDef(PndMdtHitProducerIdeal,1);

};

#endif
