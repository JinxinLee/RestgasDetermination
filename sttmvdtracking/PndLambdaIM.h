#ifndef PNDLAMBDAIM_H
#define PNDLAMBDAIM_H

#include "PndGeoSttPar.h"
#include "PndGemDigiPar.h"
#include "PndGemHit.h"
#include "PndSttHit.h"
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "FairTask.h"
#include "FairGeanePro.h"

#include "TCanvas.h"
#include "TH2F.h"

class TClonesArray; 

class PndLambdaIM : public FairTask {

 public:

  /** Default constructor **/  
  PndLambdaIM();

  PndLambdaIM(Int_t verbose);


  /** Destructor **/
  ~PndLambdaIM();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);


  void SetParContainers();


  Bool_t BuildInvariantMass(Int_t pitrkid, Int_t ptrkid, TVector3 lambdavertex, Int_t lamlambar);

  void WriteHistograms();

  void DrawHits();
  void DrawGeometry();

  void GetInitialParams(Int_t charge, TVector3 pos, TVector3 mom, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &fitm, Double_t &fitp);

  void SwitchOnDisplay() { fDisplayOn = kTRUE; }

 private: 

  /** Input array of STTHits **/
  TClonesArray* fSttHitArray;
  /** Input array of STTPoints **/
  TClonesArray* fSttPointArray;


  /** Input array of mctracks **/
  TClonesArray* fMCTrackArray;
  /** Input array of mvd + stt tracks **/
  TClonesArray* fSecTrackArray;
  /** Input array of mvd + stt trackID **/
  TClonesArray* fSecTrackIDArray;
  /** Input array of mvd + stt track cand **/
  TClonesArray* fSecTrackCandArray;

  TClonesArray* fTubeArray;


  PndGeoSttPar *fSttParameters; 
 
  /** GEANE propagator **/
  FairGeanePro *fPro;

  int evt;
  // ...................
  int mcpipid, mcpimid, mcppid, mcpmid;

  TH1F *hIM, *hIMpeak, *hPiVertexDistanceXY, *hPiVertexDistanceZ, *hPVertexDistanceXY, *hPVertexDistanceZ, *hPideltapoverp1, *hPideltaptoverpt1, *hPideltaploverpl1, *hPideltapoverp2, *hPideltaptoverpt2, *hPideltaploverpl2, *hPdeltapoverp1, *hPdeltaptoverpt1, *hPdeltaploverpl1, *hPdeltapoverp2, *hPdeltaptoverpt2, *hPdeltaploverpl2, *hprotmomentum, *hpionmomentum, *hMCpionmomentum, *hMCprotmomentum;


 TH2F *h2;
  TCanvas *display;
  Bool_t fDisplayOn; //!

  ClassDef(PndLambdaIM,1);

};

#endif
