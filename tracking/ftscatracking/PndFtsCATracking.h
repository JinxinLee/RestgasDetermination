#ifndef PNDFTSCATRACKING_HH
#define PNDFTSCATRACKING_HH


#include "PndPersistencyTask.h"
#include "FairTrackParP.h"

#include "TString.h"
#include "PndFTSCAGBHit.h"

#include <vector>
#include <fstream>
#include <map>


using std::map;

class TClonesArray;
class FairTrackParP;
class PndGeoHandling;
class PndFTSCATrackParam;
class TFile;
class PndFTSCAPerformance;
class PndFTSCAGBTracker;

class PndFtsCATracking : public PndPersistencyTask {
 public:

  // Constructors/Destructors ---------
  PndFtsCATracking(const char* name = "FtsCATracking", Int_t iVerbose = 0);
  ~PndFtsCATracking();
  void SetFtsHitsBranchName(const TString& name)        { fFtsHitsBranchName = name;  }

  virtual InitStatus Init();
  void SetParContainers();
  virtual void Exec(Option_t* opt);
  virtual void Finish();
  
  static void CATrackParToFairTrackParP( FairTrackParP *fairParam, const PndFTSCATrackParam* caParam );
  
  bool NonReconstructableEvent();
  
private:
  
  Int_t fVerbose;
  bool fDoPerformance;
  TString fFtsHitsBranchName;
  Int_t          fBranchID;
  TClonesArray*  fMCTracks;         //! Array of PndMCTrack
  TClonesArray*  fMCPoints;      //! Array of event's points
  TClonesArray*  fHits;          //! Array of event's hits
  
  TClonesArray  *fTracks;       //! Array of found tracks

  //PndGeoFtsPar *fFtsParameters;
  TClonesArray* fTubeArrayFts;  
  
  void WriteFTSHits(    /*PndFTSCAGBHit* vHits,*/ std::vector<PndFTSCAGBHit> &vHits,
           /*std::fstream &outH, std::fstream &outHL, std::fstream &outMCT, std::fstream &outMCP,*/ int &iHit, map<int, unsigned int> &nHitsInMCTrack);
  
  PndFTSCAGBTracker *fTracker;
  
  //for performance
  TFile* fPerfHistoFile;
  PndFTSCAPerformance* fPerformance;
  
  ClassDef(PndFtsCATracking,1);

};
#endif
