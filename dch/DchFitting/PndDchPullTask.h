#ifndef PNDDCHPULLTASK_HH
#define PNDDCHPULLTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include "TH1F.h"

// Collaborating Class Declarations --
class TClonesArray;
class PndTpcLheTrack;
class RecoHitFactory;
class TrackCand;
class Track;
class TGeoManager; 
class TCanvas;
class TF1;


class PndDchPullTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndDchPullTask();
  ~PndDchPullTask();

  // Operators
  

  // Accessors -----------------------
  
  // Modifiers -----------------------

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void PlotHistograms();////
  
  void WriteHistograms();
private:

  // Private Data Members ------------
  TClonesArray* fTrackArray; 
  TClonesArray* fMCTrackArray; 
  TClonesArray * fGemTrackArray;

  TF1* gaussRes, *gaussPull; 
  TH1F *hhrecop, *hgrecop, * hp, *htv, *htw,* hv, *hw;
  TH1F *hrpx, *hrpy, *hrpz,* hrp, *hrtv, *hrtw,* hrv, *hrw, *hpreco;
  TCanvas* fCanvas;
  TCanvas* fCanvasRes;
  int fPdgCode;
  int evtcounter, cutmom, cutexc, cut2;
  ClassDef(PndDchPullTask,1);

};

#endif
