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

  /** ordering **/
  std::vector<int> OrderHits(TClonesArray *hitarray, Int_t detId, Bool_t skewed);
  std::vector< TMatrixT<double> > OrderClusterInPhiBIS(std::vector< TMatrixT<double> > cluster, double xc, double yc, double radius, int charge);
  std::vector<int> OrderClusterInZ(std::vector<int> cluster, std::vector<TVector3> positions, std::map<int, int> hitidtointersection, double xc, double yc, double radius);
  void OrderConformal(Double_t oX, Double_t oY, Int_t nHits, Double_t XY[][2], Int_t  Charge, UShort_t *ListHits);
  std::vector< TMatrixT<double> > OrderByDistanceFromRefPointWithoutCharge(Int_t refihit, std::vector< TMatrixT<double> > cluster, TMatrixT<double> par);
  std::vector< TMatrixT<double> > OrderByDistanceFromRefPoint(Int_t refihit, std::vector< TMatrixT<double> > cluster);
  std::vector< TMatrixT<double> > OrderInPhiFromRefPointWithCorrectCharge(Int_t refihit, std::vector< TMatrixT<double> > cluster,  TMatrixT<double> par);
  std::vector< TMatrixT<double> > OrderInPhiFromRefPointWithReverseCharge(Int_t refihit, std::vector< TMatrixT<double> > cluster,  TMatrixT<double> par);
  std::vector< TMatrixT<double> > OrderInPhiFromRefPoint(Int_t refihit, std::vector< TMatrixT<double> > cluster,  TMatrixT<double> par, Bool_t ischcorr);
  std::vector< TMatrixT<double> > ReverseOrdering(std::vector< TMatrixT<double> > track);

  void Merge_Sort(UShort_t n_ele, Double_t *array, UShort_t *ind);
  void Merge(UShort_t nl, Double_t *left, UShort_t *ind_left, UShort_t nr, Double_t *right, UShort_t *ind_right,  Double_t *result, UShort_t *ind);

  /** deleting functions **/
  void DeleteHit(Int_t ihit, std::vector<int> *hits);
  void DeleteHits(TString detectors, std::vector<int> *hits);
  void DeleteHitBIS(TMatrixT<double> hit, std::vector< TMatrixT<double> > *hits);


  void GetInitialParams(PndTrack * track, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &fitm, Double_t &fitp);
  void GetInitialParamsMC(PndMCTrack * mctrack, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &fitm, Double_t &fitp);
  Double_t CalculatePhi(TVector2 v, TVector2 p, double alpha, double Phi0, int charge);
  Double_t CompareToPreviousPhi(Double_t Fi, Double_t Fi_pre, int charge);
  Double_t CalculatePhi(TVector2 v0, TVector2 p0);
  Double_t BringPhiInto2Pi(Double_t Fi, Int_t charge);

  
  std::vector<std::vector< TMatrixT<double> > > ClusterFinder3b(std::vector<int> hits, Int_t detId);

  /** display **/
 void SwitchOnDisplay() { fDisplayOn = kTRUE; }
  void Brief(std::vector< std::vector< TMatrixT<double> > > clusterlist, std::vector< TMatrixT<double> > xyparameters);
  void DrawFoundTracks();
  void DrawFoundTracks(std::vector< TMatrixT<double> > xyparameters);
  void DrawMCTracks();
  void DrawGeometry();
  void DrawScosZGeometry();
  void DrawGeometryConformal(Double_t umin, Double_t vmin, Double_t umax, Double_t vmax);
  void DrawHits(std::vector<int> hits, Int_t detId);
  void DrawUsableHits(std::vector<int> hits, Int_t detId);
  void DrawHitsColor(std::vector<int> hits, Int_t detId, Int_t color);
  void DrawAllHits();
  void DrawAllUsableHits();
  void PrintClustersBIS(std::vector< std::vector< TMatrixT<double> > > clusterlist);
  void PrintFoundTracks(std::vector< TMatrixT<double> > xyparameters);
  void DrawClustersBIS(std::vector< std::vector< TMatrixT<double> > > clusterlist);
  Color_t GetColor(int iclus);
  void  DrawMultipleAssignedHits(std::vector< std::vector< TMatrixT<double> > > clusterlist);


  void Refresh();
  void DrawLinks(std::vector<int> cluster, Int_t detId, Int_t iclus);

  Bool_t ConformalFit(std::vector<std::vector<double> > conformalhits, Int_t iclus, Double_t delta, Double_t trasl[2], Double_t &xc, Double_t &yc, Double_t &radius);
  Bool_t AddRemainingPoints(std::vector<int> hits, Int_t detId,   Double_t xc, Double_t yc, Double_t radius, std::vector<int> *cluster, Int_t iclus);
  Short_t FitHelixCylinder( UShort_t nHitsinTrack, Double_t auxinfoparalConformal[][3], Double_t rotationangle, Double_t trajectory_vertex[2], Double_t &slope, Double_t &intercept, Double_t &alpha, Double_t &beta, Double_t &gamma, Bool_t &TypeConf);
  Bool_t Fit(TMatrixT<double> points, Double_t &outxc, Double_t &outyc, Double_t &outradius);
  Bool_t IntersectionFinder(Double_t xc, Double_t yc, Double_t radius, PndSttHit* stthit, TVector3 &xyz, TVector3 &dxyz);
  std::vector< TMatrixT<double> > AddPointsBIS(std::vector<int> hits, Int_t detId,  Double_t xc, Double_t yc, Double_t radius, int iclus);
 // std::vector< std::vector< TMatrixT<double> > > MergeClustersBIS(std::vector< std::vector< TMatrixT<double> > > clusterlist);
  std::vector< std::vector< TMatrixT<double> > > MergeClustersBIS(std::vector< std::vector< TMatrixT<double> > > clusterlist, std::vector< std::vector<int> > *combinations);
/*  Bool_t CompleteSttFitBIS(std::vector< TMatrixT<double> > cluster, Int_t iclus, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &chosenchi2, Int_t &chosencountelem); */
  Bool_t CompleteSttFitBIS(std::vector< TMatrixT<double> > *cluster, Int_t iclus, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &chosenchi2, Int_t &chosencountelem);
  Bool_t ConformalPlaneStt4BIS(std::vector< TMatrixT<double> > cluster, Int_t iclus, std::vector<std::vector<double> > &conformalhits, Double_t &firstdrift, Double_t &delta, Double_t trasl[2]);
  Double_t CalculateRedChi2BIS(std::vector< TMatrixT<double> > cluster, Double_t xc, Double_t yc, Double_t radius, Int_t &countelements);
  //  Bool_t RefitConformalBIS(std::vector< TMatrixT<double> > cluster, Double_t xc, Double_t yc, Double_t radius, Double_t &outxc, Double_t &outyc, Double_t &outradius);
  Bool_t RefitConformalBIS(std::vector< TMatrixT<double> > * cluster, Double_t xc, Double_t yc, Double_t radius, Double_t &outxc, Double_t &outyc, Double_t &outradius);
  Int_t FindChargeBIS(Double_t oX, Double_t oY, std::vector< TMatrixT<double> > cluster);
    
  void DeleteClusterBIS(std::vector< std::vector< TMatrixT<double> > > *clusterlist, std::vector<int> deletecluster);
  void DeleteParametersBIS(std::vector< TMatrixT<double> > *xyparameters, std::vector<int> deletecluster);
  void DeleteTrackBIS(std::vector< std::vector< TMatrixT<double> > > *clusterlist, std::vector< TMatrixT<double> > *xyparameters, std::vector<int> deletecluster);

  Bool_t TestChi2BIS(std::vector< TMatrixT<double> > cluster, Double_t xc, Double_t yc, Double_t radius,  Int_t iclus, Double_t chi2, Int_t countelem, Double_t &newxc, Double_t &newyc, Double_t &newradius, std::vector< TMatrixT<double> > *newcluster, Double_t &newchi2);

   /** Z FINDING **/
  Bool_t DoesHitBelong(Int_t hitId, Double_t xc, Double_t yc, Double_t radius, TVector3 &intersection, Bool_t draw);
  Bool_t DoesHitBelong(Int_t hitId, Double_t xc, Double_t yc, Double_t radius, Double_t limits[2][3], TVector3 &intersection, Bool_t draw);
  std::vector<int> ZFinderBIS(std::vector<int> hits, Double_t xc, Double_t yc, Double_t radius, Double_t limits[2][3], std::vector< TVector3 > &intersections);
  void CalculateZ2(Int_t hitId, Double_t x, Double_t y, Double_t xc, Double_t yc, Double_t radius, TVector3 &int1, TVector3 &int2, Double_t &errz);
  TVector3 FindTangentInPoint(Double_t  xc, Double_t yc, Double_t radius, Double_t x, Double_t y, Double_t &m, Double_t &p) ;
  void ReCalculateXY(Int_t hitId, Double_t z, Double_t &x, Double_t &y);
  Bool_t ZFit3BIS(std::vector< TMatrixT<double> > *cluster, Int_t charge, Double_t xc, Double_t yc, Double_t radius, Double_t &fitm, Double_t &fitp);
  std::vector<TVector3> FindRealIntersectionsBIS(std::vector< TMatrixT<double> > *cluster, std::vector<TVector3> intersectionpoints, std::vector<TVector3>  intersectionpoints1, std::vector<TVector3> intersectionpoints2,  std::map<std::pair<int, int> , int> hitidtointersections, Int_t charge, Double_t xc, Double_t yc, Double_t radius, Double_t &fitm, Double_t &fitp, std::map<std::pair<int, int>, int> &hitidtolistmap);
  Int_t FindInMap(std::map< std::pair<int, int>, int> m, std::pair<int, int> p);

  typedef Double_t Mat2x3[2][3];
  void FindLimits(std::vector< TMatrixT<double> > cluster, Mat2x3 &m);
  Bool_t IsInsideLimits(TVector3 intersection, Double_t limits[2][3]) ;
  void ForbidCrossingTracks(std::vector< std::vector< TMatrixT<double> > > * clusterlist, std::vector< TMatrixT<double> > parlist);
  void SetUpUsableHitMap();

  std::vector<int> FindAssignedClusters(TMatrixT<double> singlehit, std::vector< std::vector< TMatrixT<double> > > clusterlist);
  Bool_t IsMultiplyAssigned(TMatrixT<double> singlehit, std::vector< std::vector< TMatrixT<double> > > clusterlist, std::vector<int> &assignlist);
  std::vector< TMatrixT<double> > FindConnections(TMatrixT<double> singlehit, std::vector<TMatrixT<double> > cluster);
   Bool_t IsAggregation(TMatrixT<double> singlehit, std::vector<TMatrixT<double> > cluster,  std::vector< TMatrixT<double> > &connectionslist);
  std::vector< TMatrixT<double> > CleanAggregations(std::vector<TMatrixT<double> > cluster, std::vector< std::vector<TMatrixT<double> > > clusterlist);

  /** final parameters **/

  TVector3 ComputePositionAtParallelHit(Int_t hitid, Double_t xc, Double_t yc, Double_t radius, Double_t fitm, Double_t fitp, Int_t charge, TVector3 &dxyz);
  Bool_t ComputeZ(Double_t xc, Double_t yc, Double_t radius, Double_t fitm, Double_t fitp, Int_t charge, TVector3 &position);

  TVector3 ComputeMomentumAtPos(Double_t xc, Double_t yc, Double_t radius, Double_t tanl, Int_t charge, TVector3 position);
  Bool_t ComputeParametersAtHit(Int_t ihit, std::vector< TMatrixT<double> > cluster, TMatrixT<double> par, TVector3 &position, TVector3 &dposition, TVector3 &momentum);
  Bool_t ComputeFirstParameters(std::vector< TMatrixT<double> > cluster, TMatrixT<double> par, TVector3 &position, TVector3 &dposition, TVector3 &momentum);
  Bool_t ComputeLastParameters(std::vector< TMatrixT<double> > cluster, TMatrixT<double> par, TVector3 &position, TVector3 &dposition, TVector3 &momentum);

  /** geometrical stuff **/
  Int_t TrackType(TMatrixT<double> par);
  void FindIntersectingPoints(Double_t xc, Double_t yc, Double_t radius, TVector3 &int1, TVector3 &int2);
  void FindIntersectingPoints(Double_t xc, Double_t yc, Double_t radius, Double_t circ, TVector3 &int1, TVector3 &int2);
  void FindOuterIntersectingPoints(Double_t xc, Double_t yc, Double_t radius, TVector3 &int1, TVector3 &int2);
  void FindInnerIntersectingPoints(Double_t xc, Double_t yc, Double_t radius, TVector3 &int1, TVector3 &int2);
  Int_t FindRefPoint(std::vector< TMatrixT<double> > cluster, TMatrixT<double> par);
  Bool_t BreakTooLongTracks(std::vector< TMatrixT<double> > cluster, TMatrixT<double> par, std::vector< TMatrixT<double> > &track1, TMatrixT<double> &param1, std::vector< TMatrixT<double> > &track2, TMatrixT<double> &param2);

  void Break(std::vector< TMatrixT<double> > newtrack, int from1, int  to1,  int from2,  int to2, std::vector< TMatrixT<double> > &track1, std::vector< TMatrixT<double> > &track2);

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



  /** Output array of secondary track cand **/
  TClonesArray* fSecondaryTrackCandArray;
  /** Output array of secondary track **/
  TClonesArray* fSecondaryTrackArray;

  /** SttMvdGemTrackCandArray **/ 
 TClonesArray* fSttMvdGemTrackCandArray;
  /** SttMvdGemTrackArray **/ 
 TClonesArray* fSttMvdGemTrackArray;

 
  TClonesArray* fTubeArray;


  /** object persistence **/
  Bool_t  fPersistence; //!
  Bool_t fDisplayOn; //!
  Color_t fColors[84];
  PndGeoSttPar *fSttParameters;  //  CHECK added


  /**  Branch names to be used to fetch the hits of the backgound mixed events  **/

  char	fSttBranch[200],
		fMvdPixelBranch[200],
		fMvdStripBranch[200];

  TH2F *h2;
  TCanvas *display;
  TString fDisName;
  Int_t fEventCounter;

  Double_t fLimit, fChi2Limit, fCountElemLimit;

  std::vector<std::vector<int> > fDetList;
  std::map<int, int> fDetMap;

  // std::vector<TMatrixT<double> > xyzpositions; // skewflag x y z dx dy dz
  TMatrixT<double> fSttUsableHits;

  ClassDef(PndSecondaryTrackFinder,1);

};

#endif
