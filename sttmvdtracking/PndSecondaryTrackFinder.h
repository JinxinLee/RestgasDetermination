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
#define MAXNOFHITS 1000  // CHECK

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
  
  void FillHitFullList();
  Int_t FromFullListToSttList(int id);
  Int_t Find8Circles(int hitID1, int hitID2, int hitID3);
  void SequentialHistogramming();

  void CalculateHr();
  void CalculateHa(Double_t r1);
  void CalculateHb(Double_t r1, Double_t x1);
  Bool_t FindNextPeak(double &max, TH1F *h);
  Bool_t FindNextRPeak(double &maxr);
  Bool_t FindNextAPeak(double &maxa);
  Bool_t FindNextBPeak(double &maxb);
  Bool_t Test(Double_t a, Double_t b, Double_t c, std::vector<int> *foundhits);
  
  void DeleteSttHits(std::vector<int> *foundhits);

  void DrawFoundTracks(std::vector< TMatrixT<double> > xyparameters);
  void DrawGeometry();
  void DrawGeometryConformal(Double_t umin, Double_t vmin, Double_t umax, Double_t vmax);
  void DrawScosZGeometry();
  void DrawHits();
  void Refresh();
  /** display **/
  void SwitchOnDisplay() { fDisplayOn = kTRUE; }

  void SetInSkewOut( bool inSkewOut) { fInSkewOut = inSkewOut; }
  void SetInSkew( bool inSkew) { fInSkew = inSkew; }
  void SetSkewOut( bool skewOut) { fSkewOut = skewOut; }




  std::vector< std::vector<int> > SttClusterFinder(int iregion);

  Bool_t IsAssociate(Int_t hitID1, Int_t hitID2);
  void SequentialHistogramming(std::vector<int> thiscluster);
  void CalculateHr(std::vector<int> thiscluster);

  void LightSttCluster(std::vector<int> sttcluster);


  std::vector<std::vector<double> > ConformalPlaneHits(std::vector<int> cluster, int firsthitid, Double_t delta, Double_t trasl[2]);

  Int_t ComputeTraAndRot(std::vector<int> cluster, Double_t &delta, Double_t trasl[2]);


  Bool_t ConformalFit(std::vector<std::vector<double> > conformalhits, Double_t delta, Double_t trasl[2], Double_t &xc, Double_t &yc, Double_t &radius, Double_t &m, Double_t &p);
  Short_t FitHelixCylinder( UShort_t nHitsinTrack, Double_t auxinfoparalConformal[][3], Double_t rotationangle, Double_t trajectory_vertex[2], Double_t &slope, Double_t &intercept, Double_t &alpha, Double_t &beta, Double_t &gamma, Bool_t &TypeConf);
  Bool_t FullFit(std::vector<int> cluster, Double_t &xc, Double_t &yc, Double_t &radius, std::vector<int> *goodhits, std::vector< std::pair<int, int> > *lefthits);

  Bool_t SelectHits(std::vector<std::vector<double> > conformalhits, Double_t m, Double_t p, Double_t &chi2, Double_t &fullchi2, std::vector<int> *goodhits, std::vector< std::pair<int, int> > *lefthits);


  std::vector<int> OrderByDistanceFromRefPointWithoutCharge(std::vector<int> cluster, TVector3 refposition);
  std::vector< std::pair< int, int > > OrderByDistanceFromRefPointWithoutCharge(std::vector< std::pair<int, int> > trackr, TVector3 refposition);

  Int_t FindCharge(Double_t oX, Double_t oY, std::vector<int> cluster);


  void FillTrack(Int_t itrk, Int_t nclusters);
  Bool_t ComputeFirstParameters(std::vector< std::pair<int, int> > cluster, std::vector<double> par, TVector3 &position, TVector3 &momentum);
  Bool_t ComputeLastParameters(std::vector< std::pair<int, int> > cluster, std::vector<double> par, TVector3 &position, TVector3 &momentum);
  Bool_t ComputeParametersAtHit(int ihit, std::vector< std::pair<int, int> > cluster, std::vector<double> par, TVector3 &position, TVector3 &momentum);

  Bool_t FindXYpca(Double_t xc, Double_t yc, Double_t radius, Double_t x, Double_t y, TVector3 &xyz);
  TVector3 ComputeMomentumAtPos(Double_t xc, Double_t yc, Double_t radius, Double_t tanl, Int_t charge, TVector3 position);

  typedef Double_t Mat2x3[2][3];
  void FindLimits(std::vector< std::pair<int, int> > track, Mat2x3 &m);
  std::vector<int> ZFinderBIS(int iregion, Double_t xc, Double_t yc, Double_t radius, Double_t limits[2][3], std::vector< TVector3 > &intersections, std::vector< TVector3 > &intersections1, std::vector< TVector3 > &intersections2, Double_t &z0, Double_t &tanl, Bool_t &success);

  Bool_t DoesHitBelong(Int_t hitId, Double_t xc, Double_t yc, Double_t radius, Double_t limits[2][3], TVector3 &intersection, Bool_t draw);
  Bool_t DoesHitBelong(Int_t hitId, Double_t xc, Double_t yc, Double_t radius, TVector3 &intersection, Bool_t draw);
  Bool_t IsInsideLimits(TVector3 intersection, Double_t limits[2][3]);
 void CalculateZ2(Int_t hitId, Double_t x, Double_t y, Double_t xc, Double_t yc, Double_t radius, TVector3 &int1, TVector3 &int2, Double_t &errz);
  TVector3 FindTangentInPoint(Double_t  xc, Double_t yc, Double_t radius, Double_t x, Double_t y, Double_t &m, Double_t &p) ;
  Double_t CalculatePhi(TVector2 v, TVector2 p, double alpha, double Phi0, int charge);


  Bool_t ProcessCluster(std::vector<int> thiscluster, int izregion, std::vector< std::pair<int, int> > *lefthits,  Int_t nclusters);
  void RunListOfSingleClusters(std::vector< std::vector<int> > *clusterN, int iregion);
  void RunListOfDoubleClusters(std::vector< std::vector<int> > *clusterA, std::vector< std::vector<int> > *clusterB, int iregionA, int  iregionB);
   
 private:

  Int_t fNofMvdPixHits, fNofMvdStrHits, fNofSttHits, fNofTriplets, fNofHits;
  Int_t fFullList[MAXNOFHITS][4];


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
  TH1F *fHr, *fHa, *fHb;
  TH2F *hhough;
  TCanvas *display;
  TString fDisName;
  Int_t fEventCounter;
  
  Double_t fLimit, fChi2Limit, fCountElemLimit;
  
  std::vector<std::vector<int> > fDetList;
  std::map<int, int> fDetMap;
  
  // std::vector<TMatrixT<double> > xyzpositions; // skewflag x y z dx dy dz
  TMatrixT<double> fSttUsableHits;
  Bool_t fUsePrimary;

  std::vector< std::vector<double> > *fgoodtriplet;

  double fRLimit, fXLimit, fYLimit;

  std::vector< std::vector< std::pair<int, int> > > fTrackList;
  std::vector< std::vector<double> > fParList;

  std::map< int, TVector3 > fskewintmap;


  Bool_t fInSkewOut, fInSkew, fSkewOut;

  ClassDef(PndSecondaryTrackFinder,1);
  
};

#endif
