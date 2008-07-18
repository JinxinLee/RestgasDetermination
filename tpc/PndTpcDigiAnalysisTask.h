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
#include "CbmTask.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;
class PndTpcFrontend;
class PndTpcDigiMapper;
class TH1D;
class TH2D;

class PndTpcDigiAnalysisTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  PndTpcDigiAnalysisTask();
  ~PndTpcDigiAnalysisTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetDigiBranchName(const TString& name) {_digiBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  
  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void WriteHistos();

private:

  // Private Data Members ------------
  TString _digiBranchName;
  TClonesArray* _digiArray;
  
  Bool_t _persistence;
 
  PndTpcFrontend* _frontend;
  PndTpcDigiMapper* _mapper;

  TH1D* _hTimesOverThres;
  TH2D* _hTimesXY;
  TH2D* _hTimesR;

  // Private Methods -----------------

public:
  ClassDef(PndTpcDigiAnalysisTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
