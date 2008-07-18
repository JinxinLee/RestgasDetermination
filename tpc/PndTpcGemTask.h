//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpc Gem reads in MCPoints and produces primary electrons
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

#ifndef TPCGEMTASK_HH
#define TPCGEMTASK_HH

// Base Class Headers ----------------
#include "CbmTask.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;
class PndTpcDigiPar;


class PndTpcGemTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  PndTpcGemTask();
  ~PndTpcGemTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetDriftedBranchName(const TString& name) {_driftedBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetGainFluctuations(Bool_t opt=kTRUE) {_gainFluctuations=opt;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();

private:

  // Private Data Members ------------
  TString _driftedBranchName;
  TClonesArray* _driftedArray;
  TClonesArray* _avalancheArray;
  
  Bool_t _persistence;
  Bool_t _gainFluctuations;
  Double_t _gain;

  PndTpcDigiPar* _par;

  Bool_t _initialized;

  // Private Methods -----------------

public:
  ClassDef(PndTpcGemTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
