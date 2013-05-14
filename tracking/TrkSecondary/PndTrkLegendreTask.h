#ifndef PNDTRKLEGENDRETASK_H
#define PNDTRKLEGENDRETASK_H

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

// ROOT
#include "TH2F.h"
#include "TCanvas.h"


#define MAXNOFHITS 1000  // CHECK

class TClonesArray;
class TObjectArray;
class TSpectrum2;
class PndTrkLegendreTransform;
class PndTrkConformalTransform;
class PndTrkLegendreTask : public FairTask {
 
 public:
  
  /** Default constructor **/
  PndTrkLegendreTask();


  /** Destructor **/
  ~PndTrkLegendreTask();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void SetParContainers();


  void Initialize();

  Int_t FillConformalHitList();
  void FillLegendreHisto(Int_t mode);

  void  ComputeTraAndRot(PndTrkHit *hit, Double_t &delta, Double_t trasl[2]);

  PndTrkCluster CreateSttCluster(PndTrkHit *firsthit);
  Bool_t IsSttAssociate(PndTrkHit *hit1, PndTrkHit *hit2);

  PndTrkCluster CreateClusterByConfDistance(double fitm, double fitq);
  PndTrkCluster CreateClusterByRealDistance(double xc0, double yc0, double R0);
  PndTrkCluster CreateClusterByMixedDistance(double fitm, double fitq);
  PndTrkCluster CreateClusterByDistance(Int_t mode, double fitm, double fitq);

  void FromConformalToRealTrack(double fitm, double fitp, double &x0, double &y0, double &R);



  Bool_t fDisplayOn;
  TH2F *hxy, *hxz, *hzphi;
  TCanvas *display;
  inline void SwitchOnDisplay() { fDisplayOn = kTRUE; }
  void DrawGeometry();
  void DrawHits(PndTrkHitList *hitlist);
  void Refresh();
  // conformal
  TH2F *huv;
  void RefreshConf();
  void DrawGeometryConf(double x1, double y1, double x2, double y2) ;
  void DrawConfHit(double x, double y, double r, int marker = 2);
  void LightCluster(PndTrkCluster *cluster);
  void DrawLegendreHisto();
  // z
  void RefreshZ();
  void DrawZGeometry(int whichone = 1, double phimin = 0, double phimax = 360, double zmin = -43, double zmax = 113);
  void DontUseMvdPix() { fUseMVDPix = kFALSE; }
  void DontUseMvdStr() { fUseMVDStr = kFALSE; }
  void DontUseStt()    { fUseSTT = kFALSE; }


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

  Int_t fEventCounter;


  PndTrkSttHitList *stthitlist;
  PndTrkSdsHitList *mvdpixhitlist;
  PndTrkSdsHitList *mvdstrhitlist;
  Double_t fSttParalDistance, fSttToMvdStripDistance;


  double  fDeltaThetaRad;
  //  TSpectrum2 *s;
  PndTrkLegendreTransform *legendre;
  Bool_t fPersistence, fUseMVDPix, fUseMVDStr, fUseSTT;



  PndTrkConformalTransform *conform;
  PndTrkConformalHitList *conformalhitlist;


  ClassDef(PndTrkLegendreTask,1);
  
};

#endif
