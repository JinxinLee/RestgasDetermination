// -----------------------------------------
// PndDrcLutReco.h
//
// Created on: 13.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------
// Class for reconstruction in DIRC using look-up table method
 
#ifndef PNDDRCLUTRECO_H
#define PNDDRCLUTRECO_H
 
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
#include "TF1.h"
#include "TSpectrum.h"

class PndDrcLutReco : public FairTask {

public:

  // Default constructor
  PndDrcLutReco();

  // Standard constructors
  PndDrcLutReco(Int_t verbose);
  PndDrcLutReco(Int_t verbose, TString infilename);

  // Destructor
  virtual ~PndDrcLutReco();

  virtual InitStatus Init();

  // Executed task 
  virtual void Exec(Option_t* option);

  // Finish task 
  virtual void Finish();
  
  void SetOutputFile(TString infilename = "luttab.root"){fInputFile = infilename;}
 
private:

  void ProcessPhotonHit();
  Double_t FindPeak();
  Int_t FindPdg(Double_t mom, Double_t cangle);
  PndGeoDrc* fGeo;
  Int_t fDetectorID;  
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
  TTree *fTree;

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
  TString fInputFile;
  TH1F *fHist;
  TF1 *fFit;
  TSpectrum *fSpect;

  ClassDef(PndDrcLutReco,1)

};

#endif
