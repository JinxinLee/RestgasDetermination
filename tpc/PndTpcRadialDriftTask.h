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
  void SetPrimBranchName(const TString& name) {_primBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetDistort(Bool_t opt=kTRUE) {_distort=opt;}
  void SetDeviationFile(const char* file) {_devFile = file;}
  void SetPhiCut(double phimin, double phimax){_phicut=true;_phimin=phimin;_phimax=phimax;}
  void SetQAPlotCol(QAPlotCollection* col){_qa=col;}
  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();

  void FillHistograms(double, double, double);   //to keep track of the drifts
  void WriteHistograms();                        //has to be called in Digi macro
  void SetR(double r){_rGem=r;}
  void SetHV(double U){_HV=U;}
  void SetLorentz(double l){_LorentzAngle=l;}
  
private:

  // Private Data Members ------------
  TString _primBranchName;
  TClonesArray* _primArray;
  TClonesArray* _driftedArray;

  
  QAPlotCollection* _qa;
  TH1D* _xVariation;
  TH1D* _yVariation;
  TH2D* _xVarAndDriftL;
  TH2D* _yVarAndDriftL;

  const PndTpcGas* _gas;
  PndTpcDigiPar* _par;
  PndTpcDevmapCyl* _devmap;
  Double_t _rGem; // get from Geom!
  Double_t _rInner;
  Double_t _frac; //= ln rGem/rInner
  Double_t _LorentzAngle;
  Double_t _HV;
  Double_t _mobility;
  

  Bool_t _phicut;
  double _phimin, _phimax;
  
  Bool_t _persistence;
  Bool_t _attach;
  Bool_t _diffuseL;
  Bool_t _diffuseT;
  Bool_t _distort;

  Bool_t _initialized;

  const char* _devFile;

  // Private Methods -----------------

public:
  ClassDef(PndTpcRadialDriftTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
