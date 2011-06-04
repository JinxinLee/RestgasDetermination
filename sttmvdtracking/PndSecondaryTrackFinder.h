#ifndef PNDSECONDARYTRACKFINDER_H
#define PNDSECONDARYTRACKFINDER_H 1


#include "PndGeoSttPar.h"
#include "PndTrack.h"
#include "PndMCTrack.h"
#include "PndSttHit.h"

#include "FairTask.h"

#include "TH2F.h"
#include "TCanvas.h"
#include "TString.h"


#include <vector>

class TClonesArray;
class TObjectArray;

class PndSecondaryTrackFinder : public FairTask {

 public:

  /** Default constructor **/
  PndSecondaryTrackFinder();

  /** First constructor **/
  PndSecondaryTrackFinder(Int_t verbose);

  /** Destructor **/
  ~PndSecondaryTrackFinder();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void WriteHistograms();

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }
 

  void SetParContainers();


  void SetInputBranchName(	char* string1,
			char* string2,
			char* string3
		    )
  {
	sprintf(fSttBranch,"%s", string1);
	sprintf(fMvdPixelBranch,"%s", string2);
	sprintf(fMvdStripBranch,"%s", string3);
	return;
  };


  std::vector<int> OrderHits(TClonesArray *hitarray, Int_t detId, Bool_t skewed);
  std::vector<int> OrderCluster(std::vector<int> cluster, Int_t detId, TVector3 point);
  std::vector<int> OrderCluster2(std::vector<int> cluster, Int_t detId, double xc, double yc, double radius) ;

  void DeleteHit(Int_t ihit, std::vector<int> *hits);
  void DeleteHits(TString detectors, std::vector<int> *hits);

  void SwitchOnDisplay() { fDisplayOn = kTRUE; }
  void GetInitialParams(PndTrack * track, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &fitm, Double_t &fitp);
  void GetInitialParamsMC(PndMCTrack * mctrack, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &fitm, Double_t &fitp);
  Double_t CalculatePhi(TVector2 v, TVector2 p, double alpha, double Phi0, int charge);
  Double_t CompareToPreviousPhi(Double_t Fi, Double_t Fi_pre, int charge);
  
  std::vector<std::vector<int> > ClusterFinder(std::vector<int> hits, Int_t detId);
  std::vector<std::vector<int> > ClusterFinder2(std::vector<int> hits, Int_t detId);
  
  void DrawFoundTracks();
  void DrawMCTracks();
  void DrawGeometry();
  void DrawGeometryConformal(Double_t umin, Double_t vmin, Double_t umax, Double_t vmax);
  void DrawHits(std::vector<int> hits, Int_t detId);
  void DrawUsableHits(std::vector<int> hits, Int_t detId);
  void DrawHitsColor(std::vector<int> hits, Int_t detId, Int_t color);
  void DrawAllHits();
  void DrawAllUsableHits();
 
  void Refresh();
  void DrawLinks(std::vector<int> cluster, Int_t detId, Int_t iclus);
  void FindBoundary(Int_t iclus, std::vector<int> cluster, Int_t detId, TMatrixT<double> &boundaries, Bool_t draw);

  Bool_t ConformalPlane(std::vector<int> cluster, TMatrixT<double> boundaries,  std::vector<int> hits, Int_t detId, Int_t iclus);
  Bool_t ConformalPlaneStt(std::vector<int> cluster, TMatrixT<double> boundaries, std::vector<int> hits, Int_t iclus);
  Bool_t ConformalPlaneStt2(std::vector<int> cluster, TMatrixT<double> boundaries, std::vector<int> hits, Int_t iclus);
  Bool_t ConformalPlaneStt3(std::vector<int> cluster, TMatrixT<double> boundaries, std::vector<int> hits, Int_t iclus, Double_t &xc, Double_t &yc, Double_t &radius);
  Bool_t ConformalPlaneStt4(std::vector<int> cluster, Int_t iclus, std::vector<std::vector<double> > &conformalhits, Double_t &firstdrift, Double_t &delta, Double_t trasl[2]);
  Bool_t ConformalFit(std::vector<std::vector<double> > conformalhits, Int_t iclus, Double_t delta, Double_t trasl[2], Double_t &xc, Double_t &yc, Double_t &radius);
  Double_t CalculateRedChi2(std::vector<int> cluster, Int_t detId, Double_t xc, Double_t yc, Double_t radius);
  Bool_t RefitConformal(std::vector<int> cluster, Int_t detId, Double_t xc, Double_t yc, Double_t radius, Double_t &outxc, Double_t &outyc, Double_t &outradius);
  Bool_t AddRemainingPoints(std::vector<int> hits, Int_t detId,   Double_t xc, Double_t yc, Double_t radius, std::vector<int> *cluster, Int_t iclus);
  Short_t FitHelixCylinder( UShort_t nHitsinTrack, Double_t auxinfoparalConformal[][3], Double_t rotationangle, Double_t trajectory_vertex[2], Double_t &slope, Double_t &intercept, Double_t &alpha, Double_t &beta, Double_t &gamma, Bool_t &TypeConf);
  Bool_t Fit(TMatrixT<double> points, Double_t &outxc, Double_t &outyc, Double_t &outradius);
  Bool_t IntersectionFinder(Double_t xc, Double_t yc, Double_t radius, PndSttHit* stthit, TVector3 &xyz, TVector3 &dxyz);
  std::vector<int> AddPoints(std::vector<int> hits, Int_t detId, Double_t xc, Double_t yc, Double_t radius, Int_t iclus);
  void MergeClusters(std::vector< std::vector<int> > clusterlist);
    

 private:

  /** Input array of PndSttTube (map of STT tubes) **/
  TClonesArray* fMCTrackArray;

   /** Input array of PndSttPoints **/
  TClonesArray* fSttPointArray;
  /** Input array of PndSttHit **/
  TClonesArray* fSttHitArray;
  /** Input array of PndSttTracks **/
  TClonesArray* fSttTrackArray;
 /** Input array of PndTracksCand of Stt **/
  TClonesArray* fSttTrackCandArray;


 /** Input array of MvdPixelHitArray **/
  TClonesArray* fMvdPixelHitArray;
 /** Input array of MvdStripHitArray **/
  TClonesArray* fMvdStripHitArray;
 /** Input array of PndTracksCand of Mvd**/
  TClonesArray* fMvdTrackCandArray;

 /** Input array of MC points  of Mvd**/
  TClonesArray* fMvdMCPointArray;



  /** Output array of PndSttMvd  PndTrackCand **/
  TClonesArray* fSttMvdPndTrackCandArray;
  /** Output array of PndSttMvd   PndTrack **/
  TClonesArray* fSttMvdPndTrackArray;

  /** SttMvdGemTrackCandArray **/ 
 TClonesArray* fSttMvdGemTrackCandArray;
  /** SttMvdGemTrackArray **/ 
 TClonesArray* fSttMvdGemTrackArray;

 
  TClonesArray* fTubeArray;


  /** object persistence **/
  Bool_t  fPersistence; //!
  Bool_t fDisplayOn; //!
  int fColors[10];
  PndGeoSttPar *fSttParameters;  //  CHECK added


  /**  Branch names to be used to fetch the hits of the backgound mixed events  **/

  char	fSttBranch[200],
		fMvdPixelBranch[200],
		fMvdStripBranch[200];

  TH2F *h2;
  TCanvas *display;
  TString fDisName;
  Int_t fEventCounter;

  Double_t fLimit;

  std::vector<std::vector<int> > fDetList;
  std::map<int, int> fDetMap;

  ClassDef(PndSecondaryTrackFinder,1);

};

#endif
