#ifndef PNDTRKADDTCA_H
#define PNDTRKADDTCA_H

// fairroot
#include "FairTask.h"
// pandaroot
#include "PndTrack.h"


class TClonesArray;
class PndTrkAddTCA : public FairTask {
 
 public:
  
  /** Default constructor **/
  PndTrkAddTCA();


  /** Destructor **/
  ~PndTrkAddTCA();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);




 private:

  Int_t fEventCounter;
  TClonesArray* fTrackArray;
  TClonesArray* fPrimaryTrackArray;
  TClonesArray* fSecondaryTrackArray;

  TClonesArray* fTrackCandArray;

  ClassDef(PndTrkAddTCA,1);
  
};

#endif
