#ifndef PNDFTSSIGNALOVERLAP_H
#define PNDFTSSIGNALOVERLAP_H 1

#include "PndFtsHit.h"

#include <vector>

class TClonesArray;

class PndFtsSignalOverlap : public TObject{

 public:

  /** Default constructor **/
  PndFtsSignalOverlap();

  PndFtsSignalOverlap(TClonesArray * OriginalHitArray);
  PndFtsSignalOverlap(Int_t verbose, TClonesArray * OriginalHitArray);


  /** Destructor **/
  ~PndFtsSignalOverlap();

  Bool_t OverlapSimultaneousSignals(TClonesArray * OverlapHitArray);
  Bool_t WriteToOutputHit(Int_t hitid);

 private:

  TClonesArray *fOriginalHitArray;
  TClonesArray *fOverlapHitArray;
  Int_t fVerbose;
  ClassDef(PndFtsSignalOverlap,1);

};

#endif
