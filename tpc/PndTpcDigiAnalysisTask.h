//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpc analysis of the digitization
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

#ifndef TPCDIGIANALYSISTASK_HH
#define TPCDIGIANALYSISTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;
class PndTpcFrontend;
class PndTpcDigiMapper;
class PndTpcDigiPar;
class TH1D;
class TH2D;

class PndTpcDigiAnalysisTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcDigiAnalysisTask();
  ~PndTpcDigiAnalysisTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetDigiBranchName(const TString& name) {fdigiBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {fpersistence=opt;}
  virtual void SetParContainers();
  
  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void WriteHistos();

private:

  // Private Data Members ------------
  TString fdigiBranchName;
  TClonesArray* fdigiArray;
  
  Bool_t fpersistence;
 
  PndTpcDigiPar* fpar;

  const PndTpcFrontend* ffrontend;
  PndTpcDigiMapper* fmapper;

  TH1D* fhTimesOverThres;
  TH2D* fhTimesXY;
  TH2D* fhTimesR;
  TH2D* fhDigiLengthR;
  TH2D* fhDigiLengthXY;
  TH1D* fhContOT;
  TH2D* fhContOTR;

  // Private Methods -----------------

public:
  ClassDef(PndTpcDigiAnalysisTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
