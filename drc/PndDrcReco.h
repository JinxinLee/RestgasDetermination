// -----------------------------------------
// PndDrcReco.h
//
// Created on: 04.03.2016
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------
// look-up-table and time imaging reconstruction
 
#ifndef PNDDRCRECO_H
#define PNDDRCRECO_H
 
#include "FairTask.h"
#include "TClonesArray.h"
#include "PndMCTrack.h"
#include "PndDrcBarPoint.h"
#include "PndDrcEVPoint.h"
#include "PndDrcPDPoint.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "PndDrcDigi.h"
#include "PndGeoDrc.h"

#include "TString.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TSpectrum.h"

#include "PndDrcTrackInfo.h"
#include "PndDrcPhotonInfo.h"
#include "PndDrcAmbiguityInfo.h"


class PndDrcReco : public FairTask {

public:

  PndDrcReco();
  PndDrcReco(TString outFile, TString lutFile, TString pdfFile, Int_t verbose=0);

  virtual ~PndDrcReco(){};

  virtual InitStatus Init();

  // Executed task 
  virtual void Exec(Option_t* option);

  // Finish task 
  virtual void Finish();
  
private:

  void DetermineCherenkov(Int_t boxId, Int_t barId);
  void DetermineBarId(Double_t &boxPhi, Int_t &barId);
  void LookUpTable(Int_t barId, Int_t sensorId);
  void TimeImaging(Int_t sensorId);
  
  Double_t FindPeak();
  Int_t FindPdg(Double_t mom, Double_t cangle);
  PndGeoDrc* fGeo;
  Double_t fBboxNum,fPipehAngle,fDphi,fBarPhi;

  TClonesArray* fMCArray;      // DRC MCPoints in the photon detector
  TClonesArray *fBarPointArray;
  TClonesArray *fEVPointArray;
  TClonesArray* fPDPointArray; // DRC points in the photon detector
  TClonesArray* fDigiArray;
  TClonesArray* fPDHitArray;   // DRC Hits in the photon detector
  TClonesArray *fLut[5];
  TClonesArray *fDrcTrackInfoArray;

  TFile *fFile;
  TFile *fFileOut; 
  TTree *fTree;
  TTree *fTreeOut;

  PndMCTrack* fMCTrack;
  PndDrcBarPoint *fBarPoint;
  PndDrcEVPoint *fEVPoint;
  PndDrcPDPoint* fPDPoint;
  PndDrcDigi* fDigi;
  PndDrcPDHit* fPDHit;
  
  // Set the parameters to the default values.
  void SetDefaultParameters();
  
  // Verbosity level
  Int_t fVerbose;
  Int_t nevents;
  TString fOutFile;
  TString fLutFile;
  TString fPdfFile;
  Int_t fEvType,fRadType,fLensType;
  TH1F *fHist,*fHist1,*fHist2;
  TF1 *fFit;
  TSpectrum *fSpect;

  Int_t fMethod;
  TVector3 fMomInBar;
  TVector3 fPosInBar;
  Double_t fTimeInBar;
  Double_t fTimeHit;
  Double_t fLenz;
  Double_t fCriticalAngle;
  Bool_t fReflected;
  Double_t fPdg[5];
  Double_t fMass[5];
  Double_t fAngle[5];
  TF1 * fFunc[5];
  Double_t fLk1[5];
  Double_t fLk2[5];
  Double_t fDiffLn1;
  Double_t fDiffLn2;
  TH1F *fHlk1[5], *fHlk2[5],*fHtang[5];
  
  Double_t fMom, fTheta, fPhi, fSpr[5], fNph[5], fCangle[5], fLikelihood[2], fSeparation[2];
  Int_t fMcTrackId, fPidTrue, fPidDist, fPidLike[2];
  
  TVector3 fNx;
  TVector3 fNy;

  ClassDef(PndDrcReco,1)

};

#endif
