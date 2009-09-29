//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpc RadialDrifter reads in PndTpcPrimaryClusters and produces drifted electrons
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

#ifndef TPCRADIALDRIFTTASK_HH
#define TPCRADIALDRIFTTASK_HH

// Base Class Headers ----------------
#include "CbmTask.h"
#include "TH1.h"
#include "TH2.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;
class PndTpcGas;
class PndTpcDevmapCyl;
class PndTpcDigiPar;
class QAPlotCollection;

class PndTpcRadialDriftTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  PndTpcRadialDriftTask();
  ~PndTpcRadialDriftTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetPrimBranchName(const TString& name) {fprimBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {fpersistence=opt;}
  void SetDistort(Bool_t opt=kTRUE) {fdistort=opt;}
  void SetDeviationFile(const char* file) {fdevFile = file;}
  void SetPhiCut(double phimin, double phimax){fphicut=true;fphimin=phimin;fphimax=phimax;}
  void SetQAPlotCol(QAPlotCollection* col){fqa=col;}
  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();

  void FillHistograms(double, double, double);   //to keep track of the drifts
  void WriteHistograms();                        //has to be called in Digi macro
  void SetR(double r){frGem=r;}
  void SetHV(double U){fHV=U;}
  void SetLorentz(double l){fLorentzAngle=l;}
  
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
  Double_t frGem; // get from Geom!
  Double_t frInner;
  Double_t ffrac; //= ln rGem/rInner
  Double_t fLorentzAngle;
  Double_t fHV;
  Double_t fmobility;
  

  Bool_t fphicut;
  double fphimin, fphimax;
  
  Bool_t fpersistence;
  Bool_t fattach;
  Bool_t fdiffuseL;
  Bool_t fdiffuseT;
  Bool_t fdistort;

  Bool_t finitialized;

  const char* fdevFile;

  // Private Methods -----------------

public:
  ClassDef(PndTpcRadialDriftTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
