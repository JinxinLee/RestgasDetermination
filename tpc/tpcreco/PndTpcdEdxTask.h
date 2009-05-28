//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      GENFIT-based dEdx
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer, Christian Hoeppner   TUM
//
//
//-----------------------------------------------------------

#ifndef DEDXTASK_HH
#define DEDXTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"

// Collaborating Class Declarations --
class TClonesArray;
class TH1D;

class PndTpcdEdxTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcdEdxTask();
  ~PndTpcdEdxTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {_trackBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);
  
  void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  TClonesArray* _trackArray;
  //TClonesArray* _trackOutArray;
  TString _trackBranchName;

  Bool_t _persistence;
  
  TH1D* _distHist;
  TH1D* _dirHist;


  

  // Private Methods -----------------

public:
  ClassDef(PndTpcdEdxTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
