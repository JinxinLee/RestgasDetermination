//-----------------------------------------------------------
//
// Description:
//      Task to generate and store some useful histograms
//
// Author List:
//	Anastasia Karavdina
//      Mathias Michel
//
//-----------------------------------------------------------

#ifndef PNDLMDQATASK_H
#define PNDLMDQATASK_H

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"
#include "TH2.h"
#include "TH1.h"

// Collaborating Class Declarations --
class TClonesArray;
class TGeoManager;

class PndLmdQATask :public FairTask{

public:

  // Constructors/Destructors ---------
  PndLmdQATask(TString mcHitBranch="LMDPoint", TString mcTrkBranch="MCTrack", TString clusterBranch="LMDPixelClusterCand", TString digiBrunch="LMDPixelDigis", TString hitBranch="LmdHits", TString TrkCandBranch="LMDTrackCand", TString trackBranch="LMDTrack", TString geaneBranch="GeaneTrackFinal", TString outFile="tmpOutput/QA.root", double Plab = 0);
  PndLmdQATask(const PndLmdQATask& o){};
  PndLmdQATask& operator=(const PndLmdQATask& o) { return *this;};
  virtual ~PndLmdQATask();
  void SetVerboseLevel(int verbose){verboseLevel = verbose;};
  void WriteHists();
  virtual InitStatus Init();

  virtual void FinishTask();

  virtual void Exec(Option_t* opt);

protected:
  int verboseLevel;
  double fPlab;
  // Input Data------------
  TClonesArray* fmcHitArray;
  TClonesArray* fmcTrkArray;
  TClonesArray* fHitArray;
  TClonesArray* fClusterArray;
  TClonesArray* fDigiArray;
  TClonesArray* fTrkCandArray;
  TClonesArray* fTrkArray;
  TClonesArray* fGeaneArray;

  TString fmcHitName;
  TString fmcTrkName;
  TString fHitName;
  TString fDigiName;
  TString fClusterName;
 TString fTrkCandName;
  TString fTrkName;
  TString fGeaneName;

  //storage / output
  TString foutFile;
  Double_t trkpar[4];
  int tot;
  int all, uneff, mistrk, ghosttrk;
  Int_t fEvent;

  // Analysis Functions
  void ResoAndPulls();
  void HitReco();
 // void Acceptance(); TODO

  //histos ResoAndPulls
 //Near IP
  TH1 *hResMom;
  TH1 *hErrMom;
  TH1 *hPullMom;
  TH1 *hResTheta;
  TH2 *hResTheta_th; //resolution vs. theta
  TH2 *hResTheta_ph; //resolution vs. phi
  TH1 *hErrTheta;
  TH1 *hPullTheta;
  TH1 *hResPhi;
  TH1 *hErrPhi;
  TH1 *hPullPhi;
  TH1 *hResPointPx;
  TH1 *hErrPointPx;
  TH1 *hPullPointPx;
  
  TH1 *hResPointPy;
  TH1 *hErrPointPy;
  TH1 *hPullPointPy;
  
  TH1 *hResPointPz;
  TH1 *hErrPointPz;
  TH1 *hPullPointPz;

  TH1 *hResPointX;
  TH1 *hPullPointX;
  TH1 *hResPointY;
  TH1 *hPullPointY;
  TH1 *hResPointZ;
  TH1 *hPullPointZ;

  //QA candidates
 TH1 *hPullLikePointX;
 TH1 *hPullLikePointY;
 TH1 *hPullLikePointZ;
 TH2 *hPullLikeX_Zmc;
 TH2 *hPullLikeY_Zmc;
 TH2 *hPullLikeZ_Zmc;
 TH2 *hPullLikeTh_Zmc;
 TH2 *hPullLikePh_Zmc;
 TH2 *hPullLikeP_Zmc;
 TH2 *hXrecZmc;
 TH2 *hYrecZmc;
 TH2 *hZrecZmc;
 TH2 *hThrecZmc;
 TH2 *hPhrecZmc;
 TH2 *hPrecZmc;
TH2 *hResXrecZmc;
 TH2 *hResYrecZmc;
 TH2 *hResZrecZmc;
 TH2 *hResThrecZmc;
 TH2 *hResPhrecZmc;
 TH2 *hResPrecZmc;
 TH2 *herrXrecZmc;
 TH2 *herrYrecZmc;
 TH2 *herrZrecZmc;
 TH2 *herrThrecZmc;
 TH2 *herrPhrecZmc;
 TH2 *herrPrecZmc;

  //Near 1st LMD plane
  TH1 *hhits;
  TH1 *hchi2;
  TH1 *hResLumiTrkMom;
  TH1 *hResLumiTrkTheta;
  TH1 *hResLumiTrkPhi;
  TH1 *hResLumiTrkPointX;
  TH1 *hResLumiTrkPointY;
  TH1 *hResLumiTrkPointZ;

  TH1 *hResLumiTrkPointPx;
  TH1 *hResLumiTrkPointPy;
  TH1 *hResLumiTrkPointPz;

  TH1 *hResLumiTrkPointXErr;
  TH1 *hResLumiTrkPointYErr;
  TH1 *hResLumiTrkPointZErr;
  TH1 *hResLumiTrkPointPxErr;
  TH1 *hResLumiTrkPointPyErr;
  TH1 *hResLumiTrkPointPzErr;

  TH1 *hResLumiTrkPointXPull;
  TH1 *hResLumiTrkPointYPull;
  TH1 *hResLumiTrkPointZPull;

  TH1 *hResLumiTrkPointPxPull;
  TH1 *hResLumiTrkPointPyPull;
  TH1 *hResLumiTrkPointPzPull;
  TH1 *hResLumiTrkThetaPull;
  TH1 *hResLumiTrkPhiPull;

  
  //TFile *fouthists;
  ClassDef(PndLmdQATask,1);

};

#endif

