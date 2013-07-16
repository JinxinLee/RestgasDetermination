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
#include "PndDrcPDPoint.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "PndDrcDigi.h"

#include "TString.h"
#include "TFile.h"

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
 
  Int_t fDetectorID;  

  TClonesArray* fMCArray;      // DRC MCPoints in the photon detector
  TClonesArray *fBarPointArray;
  TClonesArray* fPDPointArray; // DRC points in the photon detector
  TClonesArray* fDigiArray;
  TClonesArray* fPDHitArray;   // DRC Hits in the photon detector
  TClonesArray *fLut;
  TClonesArray *fDrcLutInfoArray;

  TFile *fFile; 
  TTree *fTree;

  PndMCTrack* fMCTrack;
  PndDrcBarPoint *fBarPoint;
  PndDrcPDPoint* fPDPoint;
  PndDrcDigi* fDigi;
  PndDrcPDHit* fPDHit;
  
  // Set the parameters to the default values.
  void SetDefaultParameters();
  
  // Verbosity level
  Int_t fVerbose;
  Int_t nevents;
  TString fInputFile;

  ClassDef(PndDrcLutReco,1)

};

#endif
