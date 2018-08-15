// -------------------------------------------------------------------------
// -----                 PndSttFullTCAProducer header file             -----
// -------------------------------------------------------------------------

#ifndef PNDSTTFULLTCAPRODUCER_H
#define PNDSTTFULLTCAPRODUCER_H 1
#include "FairTask.h"

 
class TClonesArray;

class PndSttFullTCAProducer : public FairTask
{
 public:

  /** Default constructor **/  
  PndSttFullTCAProducer();


  /** Destructor **/
  ~PndSttFullTCAProducer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

 private:

  TClonesArray* fSttHitArray1;  
  TClonesArray* fSttHitArray2;  
  TString fBranchName1, fBranchName2;

  /** Output array of PndSttHits **/
  TClonesArray* fSttHitArray0;  
  
  ClassDef(PndSttFullTCAProducer,1);

};

#endif
