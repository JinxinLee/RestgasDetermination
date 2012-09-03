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

class PndLmdQATask : public FairTask {

public:

  // Constructors/Destructors ---------
  PndLmdQATask(TString mcHitBranch="LMDPoint", TString mcTrkBranch="MCTrack", TString hitBranch="LmdHits", TString trackBranch="LMDTrack", TString geaneBranch="GeaneTrackFinal", TString outFile="tmpOutput/QA.root");
  PndLmdQATask(const PndLmdQATask& o){};
  PndLmdQATask& operator=(const PndLmdQATask& o) { return *this;};
  virtual ~PndLmdQATask();

  virtual InitStatus Init();

  virtual void FinishTask();

  virtual void Exec(Option_t* opt);

protected:

  // Input Data------------
  TClonesArray* fmcHitArray;
  TClonesArray* fmcTrkArray;
  TClonesArray* fHitArray;
  TClonesArray* fTrkArray;
  TClonesArray* fGeaneArray;

  TString fmcHitName;
  TString fmcTrkName;
  TString fHitName;
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
  TH1 * hchi2;

  TH2 * hchi2ang;
  TH2 * hMom2D;
  TH2 * hMomchi2;
  TH2 * hMomR;
  TH2 * hMomXY;
  TH2 * hMomZ;
  TH2 * hchi2R;
  TH2 * hangR;
  TH1 * hangMCRec;
  TH1 * hResMom;
  TH1 * hResMomIO;
  TH1 * hResMomIMC;
  TH1 * hErrMom;
  TH1 * hPullMom;
  TH1 * hResTheta;
  TH1 * hErrTheta;
  TH1 * hPullTheta;
  TH1 * hResPhi;
  TH1 * hErrPhi;
  TH1 * hPullPhi;

  TH1 * hResMomX;
  TH1 * hResMomY;
  TH1 * hResMomZ;
  TH1 * hErrMomX;
  TH1 * hErrMomY;
  TH1 * hErrMomZ;
  TH1 * hPullMomX;
  TH1 * hPullMomY;
  TH1 * hPullMomZ;
  
  TH1 * hResPointX;
  TH1 * hResPointY;
  TH1 * hResPointZ;
  TH1 * hErrPointX;
  TH1 * hErrPointY;
  TH1 * hErrPointZ;
  TH1 * hPullPointX;
  TH1 * hPullPointY;
  TH1 * hPullPointZ;

  TH1 * hPointIN;
  TH1 * hPointOUT;

  TH1 * hThetaUneff;
  TH2 * hThetaeffR;
  TH1 * hPhiUneff;
  TH2 * hnRecnMC;
  TH2 * hPullThetaAng;
  TH2 * hPullThetaR;

  //histos HitReco
  TH2 *h2dPnts;
  
  TH1 *hResHitX;
  TH1 *hResHitY;
  TH1 *hResHitZ;
  TH1 *hErrHitX;
  TH1 *hErrHitY;
  TH1 *hErrHitZ;
  TH1 *hPullHitX;
  TH1 *hPullHitY;
  TH1 *hPullHitZ;

  ClassDef(PndLmdQATask,1);

};

#endif

