#ifndef PNDRICHDBTASK_H
#define PNDRICHDBTASK_H 1

#include "FairTask.h"
#include "PndRichReco.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TF1.h"
#include <vector>

class TClonesArray;

class PndRichDbTask : public FairTask {

 public:

  /** Default constructor **/  
  PndRichDbTask();

  /** Destructor **/
  ~PndRichDbTask();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }

  void FinishEvent();
  void FinishTask();

 private: 

  /** object persistence **/
  Bool_t  fPersistence; //!

  ClassDef(PndRichDbTask,1);

};

#endif
