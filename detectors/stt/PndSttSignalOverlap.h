#ifndef PNDSTTSIGNALOVERLAP_H
#define PNDSTTSIGNALOVERLAP_H 1

#include "PndSttHit.h"

#include <vector>

class TClonesArray;

class PndSttSignalOverlap : public TObject{

 public:

  /** Default constructor **/
  PndSttSignalOverlap();

  PndSttSignalOverlap(TClonesArray * OriginalHitArray);
  PndSttSignalOverlap(Int_t verbose, TClonesArray * OriginalHitArray);


  /** Destructor **/
  ~PndSttSignalOverlap();

  Bool_t OverlapSimultaneousSignals(TClonesArray * OverlapHitArray);
  Bool_t WriteToOutputHit(Int_t hitid);

 private:

  TClonesArray *fOriginalHitArray;
  TClonesArray *fOverlapHitArray;
  Int_t fVerbose;
  ClassDef(PndSttSignalOverlap,1);

};

#endif
