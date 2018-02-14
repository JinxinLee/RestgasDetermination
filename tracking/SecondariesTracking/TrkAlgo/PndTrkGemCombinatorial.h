#ifndef PNDTRKGEMCOMBITASK_H
#define PNDTRKGEMCOMBITASK_H

// fairroot
#include "FairTask.h"

// ROOT
#include "TH2F.h"
#include "TCanvas.h"

class TClonesArray;

class PndTrkGemCombinatorial : public FairTask {
 
 public:
  
  /** Default constructor **/
  PndTrkGemCombinatorial();
  PndTrkGemCombinatorial(TClonesArray *gemhitarray, int verbose);


  /** Destructor **/
  ~PndTrkGemCombinatorial();
  std::map< int, bool > CombinatorialSuppression();

  void SwitchOnDisplay();
  void DrawGeometry();
  void DrawMCPoints();

  void SetMCPointTCA(TClonesArray *gempointarray) { fGemPointArray = gempointarray; }
  inline void Evaluate() { fMCEval = kTRUE; }

 private:
  TClonesArray* fGemHitArray;
  TClonesArray* fGemPointArray;



  // display
  Bool_t fDisplayOn, fMCEval;
  Int_t fVerbose;
  TH2F *hxy, *hxy1, *hxy2, *hxy3;
  TCanvas *display;

 
  ClassDef(PndTrkGemCombinatorial,1);
  
};

#endif
