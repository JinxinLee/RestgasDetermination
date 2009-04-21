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
  void SetDigiBranchName(const TString& name) {_digiBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  virtual void SetParContainers();
  
  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void WriteHistos();

private:

  // Private Data Members ------------
  TString _digiBranchName;
  TClonesArray* _digiArray;
  
  Bool_t _persistence;
 
  PndTpcDigiPar* _par;

  const PndTpcFrontend* _frontend;
  PndTpcDigiMapper* _mapper;

  TH1D* _hTimesOverThres;
  TH2D* _hTimesXY;
  TH2D* _hTimesR;
  TH2D* _hDigiLengthR;
  TH2D* _hDigiLengthXY;
  TH1D* _hContOT;
  TH2D* _hContOTR;

  // Private Methods -----------------

public:
  ClassDef(PndTpcDigiAnalysisTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
