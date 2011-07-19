//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpc MCSkimming applies cuts based on MC Truth and skimms events
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

#ifndef TPCMCSKIMMINGTASK_HH
#define TPCMCSKIMMINGTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;
class PndTpcGas;
class PndTpcDigiPar;

class PndTpcMCSkimmingTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcMCSkimmingTask();
  ~PndTpcMCSkimmingTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetPointBranchName(const TString& name) {fpointBranchName=name;}
  void SetMCTrackBranchName(const TString& name) {ftrackBranchName=name;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();
  
  void ChargeConversion();


private:

  // Private Data Members ------------
  
  
  TString fpointBranchName;
  TString ftrackBranchName;
  TString fgemBranchName;
  TString fmvdBranchName;
  TClonesArray* fpointArray;
  TClonesArray* ftrackArray;
  TClonesArray* fgemArray;
  TClonesArray* fmvdArray;
  TClonesArray* fpointOutArray;
  TClonesArray* ftrackOutArray;
  TClonesArray* fgemOutArray;
  TClonesArray* fmvdOutArray;
  // Private Methods -----------------

public:
  ClassDef(PndTpcMCSkimmingTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
