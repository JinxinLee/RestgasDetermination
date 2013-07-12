// *************************************************************************
// Author: Lia Lavezzi
// FTS added: Martin Galuska
//
// This task measures the performance of tracking algorithms for the following detectors (so far)
//
// MVD
// STT
// FTS
//
// Created: 01.06.2013
// Last Modification: 06.06.2013 13:45
//
//
// *************************************************************************




#ifndef PNDTRKQATASK_H
#define PNDTRKQATASK_H

// fairroot
#include "FairTask.h"
// pandaroot
#include "PndTrack.h"
#include "PndMCTrack.h"
// fts
#include "PndGeoFtsPar.h"
#include "PndFtsHit.h"
// stt
#include "PndGeoSttPar.h"
#include "PndSttHit.h"

// ROOT
#include "TH2F.h"




class PndTrkQATask : public FairTask {

 public:

  /** Default constructor **/
  PndTrkQATask();
  PndTrkQATask(TString detectorsToStudy, Int_t verbose);


  /** Destructor **/
  ~PndTrkQATask();


  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void FinishTask();
  virtual void ReInitTask();

  void SetParContainers();

  void SetInputTrackBranchName(char* track) {sprintf(fInputTrackBranch,"%s", track);}
  void SetInputTrackIDBranchName(char* trackid) {sprintf(fInputTrackIDBranch,"%s", trackid);}

  void SetDetectorsToStudy(TString detectorsToStudy);


  void Initialize();
  //	Int_t CheckIfPresent(Int_t trackid);
  Bool_t IdealTrackFinding();

  void WriteHistograms();
  void DontUseMvdPix() { fUseMVDPixHits = kFALSE; }
  void DontUseMvdStr() { fUseMVDStrHits = kFALSE; }
  void DontUseFts()    { fUseFTSHits = kFALSE; }
  void DontUseFtsSkew()    { fUseFTSSkewHits = kFALSE; }
  void DontUseStt()    { fUseSTTHits = kFALSE; }
  void DontUseSttSkew()    { fUseSTTSkewHits = kFALSE; }

  void MapMCToReco();

  void SetMinimumNofSttParalHits(Int_t nofhits) { fMinimumNofSttParalHits = nofhits; }
  void SetMinimumNofSttSkewHits(Int_t nofhits) { fMinimumNofSttSkewHits = nofhits; }
  void SetMinimumNofFtsHits(Int_t nofhits) { fMinimumNofFtsHits = nofhits; }
  void SetMaximumNofSttHits(Int_t nofhits) { fMaximumNofSttHits = nofhits; }

  // multiple hits
  Int_t FillTubeIDMap();
  Int_t CleanTubeIDMap();
  Bool_t CheckRefIndexAgainstMultiHit(int tubeID, int refindex);
  Bool_t CheckIfTrackHitTube(int thistrack, int tubeID);
  Bool_t IsTubeMultiHit(int tubeID);
  TArrayI GetTubeMultiHits(int tubeID);
  

 private:


#define NOFDETECTORS 4
  // So far the following detectors are included in this task: MVD, STT, FTS

#define MAXNOFTRACKS 1000
#define MAXNOFPOINTS 1000

  Int_t fMCPoints[MAXNOFTRACKS][NOFDETECTORS], fMCTracks[MAXNOFTRACKS][MAXNOFPOINTS];

  Int_t fEventCounter, fNFtsPoints,  fNSttPoints,  fNMvdPoints,  fNMCPoints,  fNFtsHits,  fNSttHits,  fNMvdPixHits,  fNMvdStrHits,  fNHits,  fNMCTracks;

  Bool_t IsMcTrackAcceptable(Int_t iMCTrack, Int_t hitMap[][NOFDETECTORS]);


  /** Input array of PndFtsPoints **/
  TClonesArray* fFtsPointArray;
  /** Input array of PndFtsHit **/
  TClonesArray* fFtsHitArray;

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

  TClonesArray* fTubeArrayStt;
  TClonesArray* fTubeArrayFts;
  TClonesArray* fTrackIDArray;
  PndGeoSttPar *fSttParameters;  //  CHECK added
  PndGeoFtsPar *fFtsParameters;
  char	fSttBranch[200],
    fFtsBranch[200],
    fMvdPixelBranch[200],
    fMvdStripBranch[200],
    fInputTrackBranch[200],
    fInputTrackIDBranch[200]    ;

  Bool_t fPersistence;

  TH2F *hEfficiency;
  TH2F *hEfficiencyStt, *hEfficiencySttSkew, *hEfficiencySttParal;
  TH2F *hEfficiencyFts, *hEfficiencyFtsSkew, *hEfficiencyFtsParal;
  TH2F *hEfficiencyMvdPixel, *hEfficiencyMvdStrip;
  TH2F *hInefficiency;
  TH2F *hInefficiencyStt, *hInefficiencySttSkew, *hInefficiencySttParal;
  TH2F *hInefficiencyFts, *hInefficiencyFtsSkew, *hInefficiencyFtsParal;
  TH2F *hInefficiencyMvdPixel, *hInefficiencyMvdStrip;
  TH2F *hContamination, *hPurity;

  Int_t fGoodTrack, fBadTrack, fMCReconstructableTrack, fNotReconstructed, fRecoTrack, fGhostTrack, fIgnoredTrack, fThisGoodTrack, fThisBadTrack, fThisMCReconstructableTrack, fThisNotReconstructed, fThisGhostTrack, fThisRecoTrack, fThisIgnoredTrack;

  ClassDef(PndTrkQATask,1);
  TString fDetectorsToStudy;  // This flag determines which detector's tracking algorithm should be investigated
  Bool_t  fUseMVDPixHits, fUseMVDStrHits, fUseSTTHits, fUseSTTSkewHits, fUseFTSHits, fUseFTSSkewHits; // These determine which detectors' hits should be looked at
  std::map< Int_t, std::vector<Int_t> > fMC2RecoMap;
  Int_t fMinimumNofFtsHits,  fMinimumNofSttParalHits, fMinimumNofSttSkewHits, fMaximumNofSttHits;
  std::multimap< int, int > fSttTubeIDToMCPointID;
  Bool_t fAccountForMultiHits;
};

#endif
