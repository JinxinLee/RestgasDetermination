#ifndef PULLTASKFWD_HH
#define PULLTASKFWD_HH

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

class PullTaskFwd : public FairTask {
public:

  // Constructors/Destructors ---------
  PullTaskFwd();
  ~PullTaskFwd();

  // Operators
  

  // Accessors -----------------------
  
  // Modifiers -----------------------

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);
  
  void WriteHistograms();
private:

  // Private Data Members ------------
  TClonesArray* fTrackArray; 
  TClonesArray* fMCTrackArray; 
  TClonesArray * fGemTrackArray;
 
  TH1F *hhrecop, *hgrecop, * hp, *htv, *htw,* hv, *hw;
  TH1F *hrpx, *hrpy, *hrpz,* hrp, *hrtv, *hrtw,* hrv, *hrw;
  int fPdgCode;
  int evtcounter, cutmom, cutexc;
  ClassDef(PullTaskFwd,1);

};

#endif
