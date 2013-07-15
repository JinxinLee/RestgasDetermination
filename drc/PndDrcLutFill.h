// -----------------------------------------
// PndDrcLutFill.h
//
// Created on: 08.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------
// Class allows to fill look-up tables for reconstruction in DIRC
 
#ifndef PNDDRCLUTFILL_H
#define PNDDRCLUTFILL_H
 
#include "FairTask.h"
#include "TClonesArray.h"
#include "PndMCTrack.h"
#include "PndDrcPDPoint.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "PndDrcDigi.h"

#include "TString.h"
#include "TFile.h"

class PndDrcLutFill : public FairTask {

public:

  // Default constructor
  PndDrcLutFill();

  // Standard constructors
  PndDrcLutFill(Int_t verbose);
  PndDrcLutFill(Int_t verbose, TString outfilename);

  // Destructor
  virtual ~PndDrcLutFill();

  virtual InitStatus Init();

  // Executed task 
  virtual void Exec(Option_t* option);

  // Finish task 
  virtual void Finish();
  
  // Look-up table initialization
  void InitLut();
 
  void SetOutputFile(TString outfilename = "luttab.root"){fOutputFile = outfilename;}
 
private:

  void ProcessPhotonHit();
 
  Int_t fDetectorID;  

  TClonesArray* fMCArray;      // DRC MCPoints in the photon detector
  TClonesArray* fPDPointArray; // DRC points in the photon detector
  TClonesArray* fDigiArray;
  TClonesArray* fPDHitArray;   // DRC Hits in the photon detector
  TClonesArray *fLut;
  
  TFile *fFile; 
  TTree *fTree;

  PndMCTrack* fMCTrack;
  PndDrcPDPoint* fPDPoint;
  PndDrcDigi* fDigi;
  PndDrcPDHit* fPDHit;

  // Set the parameters to the default values.
  void SetDefaultParameters();
  
  // Verbosity level
  Int_t fVerbose;
  Int_t nevents;
  TString fOutputFile;

  ClassDef(PndDrcLutFill,1)

};

#endif
