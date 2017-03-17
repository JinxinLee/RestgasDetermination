//-----------------------------------------------------------
//-----------------------------------------------------------

#ifndef PndCATracking_HH
#define PndCATracking_HH

#include "FairTask.h"

#include "TString.h"
#include "PndCAGBHit.h"

#include <vector>
#include <fstream>
#include <map>

using std::map;

class TClonesArray;
class PndCATrackParam;
class FairTrackParP;
class PndGeoHandling;

class PndCATracking : public FairTask {
 public:

  // Constructors/Destructors ---------
  PndCATracking(const char* name = "TrackingCA", Int_t iVerbose = 0);
  ~PndCATracking();

  void SetMvdPixelHitsBranchName(const TString& name)   { fMvdPixelHitsBranchName = name;  }
  void SetMvdStripHitsBranchName(const TString& name)   { fMvdStripHitsBranchName = name;  }
  void SetSttHitsBranchName(const TString& name)        { fSttHitsBranchName = name;  }

  virtual InitStatus Init();
  void SetParContainers();
  virtual void Exec(Option_t* opt);
  
 private:
   
  TString fMvdPixelHitsBranchName;
  TString fMvdStripHitsBranchName;
  TString fMvdPixelClusterBranchName;
  TString fMvdStripClusterBranchName;
  TString fMvdPixelDigiBranchName;
  TString fMvdStripDigiBranchName;
  TString fSttHitsBranchName;

  TString fMvdPointsBranchName;
  TString fSttPointsBranchName;
  TString fMCTracksBranchName;

  //hits
  TClonesArray* fMvdPixelHitsArray;
  TClonesArray* fMvdStripHitsArray;
  TClonesArray* fMvdPixelClusterArray;
  TClonesArray* fMvdStripClusterArray;
  TClonesArray* fMvdPixelDigiArray;
  TClonesArray* fMvdStripDigiArray;
  TClonesArray* fSttHitsArray;

  TClonesArray* fMvdPointsArray; //MVD MC points
  TClonesArray* fSttPointsArray; //STT MC points
  TClonesArray* fMCTrackArray;

  //STT Map 
  TClonesArray* fTubeArray;
  //for MVD geometry extraction
  PndGeoHandling* fGeoH;

  /** Output array of PndSttMvd   PndTrack **/
  TClonesArray *fSttMvdPndTrackArray;

  bool fDoPerformance;
  void WriteMVDHits(   std::vector<PndCAGBHit> &vHits,
		       std::fstream &outH, std::fstream &outHL, std::fstream &outMCT, std::fstream &outMCP, int &iHit, map<int, unsigned int> &nHitsInMCTrack, bool isPixel);

  static void CATrackParToFairTrackParP( FairTrackParP *fairParam, const PndCATrackParam* caParam );


  ClassDef(PndCATracking,1);
};

#endif
