#ifndef PNDTRKGEMCOMBINATORIAL_H
#define PNDTRKGEMCOMBINATORIAL_H

// fairroot
#include "FairTask.h"

// ROOT
#include "TH2F.h"
#include "TCanvas.h"
#include "PndTrkGemCombinatorial.h"

class TClonesArray;

class PndTrkGemCombiTask : public FairTask {
 
 public:
  
  /** Default constructor **/
  PndTrkGemCombiTask();
  PndTrkGemCombiTask(int verbose);


  /** Destructor **/
  ~PndTrkGemCombiTask();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);


  void Reset();

  inline void SwitchOnDisplay() { fDisplayOn = kTRUE; }
  inline void Evaluate() { fMCEval = kTRUE; }

 private:

  PndTrkGemCombinatorial *fCombiFinder;
  TClonesArray* fGemHitArray;
  TClonesArray* fGemPointArray;


  Bool_t fDisplayOn, fMCEval;
 

 
  ClassDef(PndTrkGemCombiTask,1);
  
};

#endif
