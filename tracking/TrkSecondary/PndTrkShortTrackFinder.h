#ifndef PNDTRKSHORTTRACKFINDER_H
#define PNDTRKSHORTTRACKFINDER_H

// fairroot
#include "FairTask.h"
// pandaroot
#include "PndTrack.h"
#include "PndMCTrack.h"
// stt
#include "PndGeoSttPar.h"
#include "PndSttHit.h"
#include "PndSttMapCreator.h"
// tracking
#include "PndTrkSttHitList.h"
#include "PndTrkSdsHitList.h"
#include "PndTrkSciTHitList.h"
#include "PndTrkGemHitList.h"
#include "PndTrkCluster.h"
#include "PndTrkClusterList.h"
#include "PndTrkTrack.h"
#include "PndTrkTrackList.h"
#include "PndTrkConformalHitList.h"
#include "PndTrkLegendreTransform.h"
#include "PndTrkSkewHitList.h"
#include "PndTrkFitter.h"
#include "PndTrkNeighboringMap.h"

// ROOT
#include "TH2F.h"
#include "TCanvas.h"


#define MAXNOFHITS 1000  // CHECK

class TClonesArray;
class TObjectArray;
class PndTrkLegendreTransform;
class PndTrkConformalTransform;
class PndTrkShortTrackFinder : public FairTask {
 
 public:
  
  /** Default constructor **/
  PndTrkShortTrackFinder();
  PndTrkShortTrackFinder(int verbose);


  /** Destructor **/
  ~PndTrkShortTrackFinder();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void SetParContainers();

   void FillHitMap();

  void Initialize();
  void Reset();

  inline void SwitchOnDisplay() { fDisplayOn2 = kTRUE; }
  void DrawGeometry();
  void DrawHits(PndTrkHitList *hitlist);
  void DrawLists();
  void DrawNeighborings();
  void DrawNeighboringsToHit(PndTrkHit *hit);
  void Refresh();
  void RefreshConf(); 
  void DrawGeometryConf(double x1, double x2, double y1, double y2);
  void DrawConfHit(double x, double y, double r, int marker = 2);

  void LightCluster(PndTrkCluster *cluster);
  void DrawLegendreHisto();




 private:


  Int_t fNofMvdPixHits, fNofMvdStrHits, fNofSttHits, fNofTriplets, fNofHits, fNofSciTHits, fNofGemHits;
 

   /** Input array of PndSttPoints **/
  TClonesArray* fSttPointArray;
  /** Input array of PndSttHit **/
  TClonesArray* fSttHitArray;


 /** Input array of MvdPixelHitArray **/
  TClonesArray* fMvdPixelHitArray;
 /** Input array of MvdStripHitArray **/
  TClonesArray* fMvdStripHitArray;
 
  /** Input array of SciTHitArray **/
  TClonesArray* fSciTHitArray;
  /** Input array of GemHitArray **/
  TClonesArray* fGemHitArray;

  TClonesArray *fLongTrackArray, *fFwdTrackArray;

  TClonesArray* fTrackArray, * fTrackCandArray;

  TClonesArray* fTubeArray;


  PndGeoSttPar *fSttParameters;  //  CHECK added
  char	fSttBranch[200],
    fMvdPixelBranch[200],
    fMvdStripBranch[200],
    fSciTBranch[200],
    fGemBranch[200];

  PndSttMapCreator *fMapper;

  Int_t fEventCounter; // , fVerbose;


  PndTrkSttHitList *stthitlist;
  PndTrkSdsHitList *mvdpixhitlist;
  PndTrkSdsHitList *mvdstrhitlist;
  PndTrkSciTHitList *scithitlist;
  PndTrkGemHitList *gemhitlist;
  Double_t fSttParalDistance, fSttToMvdStripDistance;


  double  fDeltaThetaRad;
  //  TSpectrum2 *s;
  PndTrkLegendreTransform *legendre;
  Bool_t fPersistence, fUseMVDPix, fUseMVDStr, fUseSTT, fUseSCIT, fUseGEM, fSecondary, fInitDone;



  PndTrkConformalTransform *conform;
  PndTrkConformalHitList *fConformalHitList;
  PndTrkTools *tools;

  PndTrkHit *fRefHit;

  Double_t fMvdPix_RealDistLimit, fMvdStr_RealDistLimit, fStt_RealDistLimit,  fMvdPix_ConfDistLimit, fMvdStr_ConfDistLimit, fStt_ConfDistLimit; 

  std::vector< std::pair<double, double> > fFoundPeaks;
  double fTime;
  TStopwatch *fTimer;
  PndTrkFitter *fFitter;

  PndTrkNeighboringMap *fHitMap;
  double fUmin, fUmax, fVmin, fVmax, fRmin, fRmax, fThetamin, fThetamax;

  // display
  Bool_t fDisplayOn2;
  TH2F *hxy, *hxz, *hzphi;
  TCanvas *display;
  TH2F *huv;

  PndTrkCluster *fCluster;
  PndTrkCluster *fFinalCluster;
  PndTrkCluster *fIndivCluster;
  PndTrkCluster *fSkewCluster;
  PndTrkCluster *fFinalSkewCluster;
  PndTrkCluster *fIndivisibleHitList;

  PndTrkTrackList *fTrackList;

  ClassDef(PndTrkShortTrackFinder,1);
  
};

#endif
