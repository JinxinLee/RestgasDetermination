//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpc Drifter reads in PndTpcPrimaryClusters and produces drifted electrons
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCDRIFTTASK_HH
#define TPCDRIFTTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"
#include "TH1.h"
#include "TH2.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;
class PndTpcGas;
class PndTpcDevmapCyl;
class PndTpcDigiPar;
class QAPlotCollection;

class PndTpcDriftTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcDriftTask();
  ~PndTpcDriftTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetPrimBranchName(const TString& name) {fprimBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {fpersistence=opt;}
  void SetDistort(Bool_t opt=kTRUE) {fdistort=opt;}
  void SetDeviationFile(const char* file) {fdevFile = file;}
  void SetPhiCut(double phimin, double phimax){fphicut=true;fphimin=phimin;fphimax=phimax;}
  void SetQAPlotCol(QAPlotCollection* col){fqa=col;}
  void SetShortTPC(Bool_t opt=kTRUE) {fshortTpc=opt;}
  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();

  void FillHistograms(double, double, double);   //to keep track of the drifts
  void WriteHistograms();                        //has to be called in Digi macro

private:

  // Private Data Members ------------
  TString fprimBranchName;
  TClonesArray* fprimArray;
  TClonesArray* fdriftedArray;

  
  QAPlotCollection* fqa;
  TH1D* fxVariation;
  TH1D* fyVariation;
  TH2D* fxVarAndDriftL;
  TH2D* fyVarAndDriftL;

  const PndTpcGas* fgas;
  PndTpcDigiPar* fpar;
  PndTpcDevmapCyl* fdevmap;
  Double_t fzGem; // get from Geom!

  Bool_t fphicut;
  double fphimin, fphimax;
  
  Bool_t fpersistence;
  Bool_t fattach;
  Bool_t fdiffuseL;
  Bool_t fdiffuseT;
  Bool_t fdistort;

  Bool_t finitialized;

  Bool_t fshortTpc;   //shorten TPC to 120cm length 

  const char* fdevFile;

  // Private Methods -----------------

public:
  ClassDef(PndTpcDriftTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
