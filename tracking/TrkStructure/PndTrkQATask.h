#ifndef PNDTRKQATASK_H
#define PNDTRKQATASK_H

// fairroot
#include "FairTask.h"
// pandaroot
#include "PndTrack.h"
#include "PndMCTrack.h"
// stt
#include "PndGeoSttPar.h"
#include "PndSttHit.h"

// ROOT
#include "TH2F.h"



class PndTrkQATask : public FairTask {
 
 public:
  
  /** Default constructor **/
  PndTrkQATask();


  /** Destructor **/
  ~PndTrkQATask();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void SetParContainers();

  void SetInputTrackBranchName(char* track) {sprintf(fInputTrackBranch,"%s", track);}
  void SetInputTrackIDBranchName(char* trackid) {sprintf(fInputTrackIDBranch,"%s", trackid);}
 
 void Initialize();
  Int_t CheckIfPresent(Int_t trackid);
  Bool_t IdealTrackFinding();

  void WriteHistograms();
  void DontUseMvdPix() { fUseMVDPix = kFALSE; }
  void DontUseMvdStr() { fUseMVDStr = kFALSE; }
  void DontUseStt()    { fUseSTT = kFALSE; }
  void DontUseSttSkew()    { fUseSTTSkew = kFALSE; }

 private:

#define MAXNOFTRACKS 1000
#define MAXNOFPOINTS 1000

  Int_t fMCPoints[MAXNOFTRACKS][3], fMCTracks[MAXNOFTRACKS][MAXNOFPOINTS];

  Int_t fEventCounter,  fNofMCTrack, fNofMvdPixHits, fNofMvdStrHits, fNofSttHits, fNofTriplets, fNofHits;
 

  /** Input array of PndSttPoints **/
  TClonesArray* fSttPointArray;
  /** Input array of PndSttHit **/
  TClonesArray* fSttHitArray;

  TClonesArray* fMvdPointArray;
  /** Input array of MvdPixelHitArray **/
  TClonesArray* fMvdPixelHitArray;
  /** Input array of MvdStripHitArray **/
  TClonesArray* fMvdStripHitArray;

  TClonesArray* fMCTrackArray, * fTrackArray, * fTrackCandArray, *fIdealTrackCandArray;

  TClonesArray* fTubeArray;
  TClonesArray* fTrackIDArray;
  PndGeoSttPar *fSttParameters;  //  CHECK added
  char	fSttBranch[200],
    fMvdPixelBranch[200],
    fMvdStripBranch[200],
    fInputTrackBranch[200],
    fInputTrackIDBranch[200]    ;

  Bool_t fPersistence;

  TH2F *hEfficiency, *hEfficiencyStt, *hEfficiencySttSkew, *hEfficiencySttParal, *hEfficiencyMvdPixel, *hEfficiencyMvdStrip, *hInefficiency, *hInefficiencyStt, *hInefficiencySttSkew, *hInefficiencySttParal, *hInefficiencyMvdPixel, *hInefficiencyMvdStrip, *hContamination, *hPurity;

  Int_t fGoodTrack, fBadTrack, fMCReconstructableTrack, fNotReconstructed;
  ClassDef(PndTrkQATask,1);
  Bool_t  fUseMVDPix, fUseMVDStr, fUseSTT, fUseSTTSkew;

};

#endif
