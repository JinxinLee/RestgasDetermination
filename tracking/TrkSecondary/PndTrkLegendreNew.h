#ifndef PNDTRKLEGENDRENEW_H
#define PNDTRKLEGENDRENEW_H

#include "PndTrkLegendreTransform.h"
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
#include "PndTrkCluster.h"
#include "PndTrkClusterList.h"
#include "PndTrkTrack.h"
#include "PndTrkConformalHitList.h"
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
class PndTrkLegendreNew : public FairTask {
 
 public:
  
  /** Default constructor **/
  PndTrkLegendreNew();
  PndTrkLegendreNew(int verbose);


  /** Destructor **/
  ~PndTrkLegendreNew();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void SetParContainers();


  void Initialize();
  void Reset();

  inline void SwitchOnDisplay() { fDisplayOn = kTRUE; }
  void DrawGeometry();
  void DrawHits(PndTrkHitList *hitlist);
  void DrawLists(PndTrkNeighboringMap hitmap); 
  void DrawNeighborings(PndTrkNeighboringMap hitmap);
  void Refresh();

  void LightCluster(PndTrkCluster *cluster);
 private:


  Int_t fNofMvdPixHits, fNofMvdStrHits, fNofSttHits, fNofTriplets, fNofHits;
 

   /** Input array of PndSttPoints **/
  TClonesArray* fSttPointArray;
  /** Input array of PndSttHit **/
  TClonesArray* fSttHitArray;


 /** Input array of MvdPixelHitArray **/
  TClonesArray* fMvdPixelHitArray;
 /** Input array of MvdStripHitArray **/
  TClonesArray* fMvdStripHitArray;

  TClonesArray* fTrackArray, * fTrackCandArray;

  TClonesArray* fTubeArray;


  PndGeoSttPar *fSttParameters;  //  CHECK added
  char	fSttBranch[200],
    fMvdPixelBranch[200],
    fMvdStripBranch[200];

  PndSttMapCreator *fMapper;

  Int_t fEventCounter; // , fVerbose;


  PndTrkSttHitList *stthitlist;
  PndTrkSdsHitList *mvdpixhitlist;
  PndTrkSdsHitList *mvdstrhitlist;
  Double_t fSttParalDistance, fSttToMvdStripDistance;


  double  fDeltaThetaRad;
  //  TSpectrum2 *s;
  PndTrkLegendreTransform *legendre;
  Bool_t fPersistence, fUseMVDPix, fUseMVDStr, fUseSTT, fSecondary, fInitDone;



  PndTrkConformalTransform *conform;
  PndTrkConformalHitList *conformalhitlist;
  PndTrkTools *tools;

  PndTrkHit *fRefHit;

  Double_t fMvdPix_RealDistLimit, fMvdStr_RealDistLimit, fStt_RealDistLimit,  fMvdPix_ConfDistLimit, fMvdStr_ConfDistLimit, fStt_ConfDistLimit; 

  std::vector< std::pair<double, double> > fFoundPeaks;
  double fTime;
  TStopwatch *fTimer;
  PndTrkFitter *fFitter;

  // display
  Bool_t fDisplayOn;
  TH2F *hxy, *hxz, *hzphi;
  TCanvas *display;
  TH2F *huv;



  ClassDef(PndTrkLegendreNew,1);
  
};

#endif
