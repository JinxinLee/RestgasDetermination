//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Task that mixes (copies) events from an other input file to the 
//      current data stream. Only PndTpcSignals are added for the moment
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCEVTMIXTASK_HH
#define TPCEVTMIXTASK_HH

// Base Class Headers ----------------
#include "CbmTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class TClonesArray;
class TFile;
class TTree;
class TBranch;

class PndTpcEvtMixTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  PndTpcEvtMixTask();
  ~PndTpcEvtMixTask();

  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------
  void SetInBranchName(const TString& name) {_inBranchName=name;}
  void SetBkgBranchName(const TString& name) {_bkgBranchName=name;}
  void SetBkgFileName(const TString& name) {_bkgFileName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetMeanEvtSpacing(Double_t deltaT) {_meanEvtSpacing=deltaT;} // [ns]
  void SetEvtRate(Double_t evt_per_sec) {_meanEvtSpacing=1/evt_per_sec*1.E9;}
  void SetNBkgEvts(Int_t n) {_nbkgEvts=n;}

  // Operations ----------------------
  virtual InitStatus Init();
  
  virtual void Exec(Option_t* opt);

private:

  // Private Data Members ------------
  TString _inBranchName;
  TString _bkgBranchName;
  TString _bkgFileName;
 

  TClonesArray* _signalArray;
  TClonesArray* _bkgArray;
  TClonesArray* _timeArray;

  TFile* _inFile;
  TTree* _bkgTree;
  TBranch* _bkgBranch;

  Bool_t _persistence;

  Double_t _meanEvtSpacing;
  Int_t _nbkgEvts;

  // Private Methods -----------------

public:
  ClassDef(PndTpcEvtMixTask,1);

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
