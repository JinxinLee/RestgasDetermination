//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpc PSA: Pulse shape analysis
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

#ifndef TPCPSATASK_HH
#define TPCPSATASK_HH

// Base Class Headers ----------------
#include "CbmTask.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;
class PndTpcFrontend;
class PndTpcAbsPSAStrategy;

class PndTpcPSATask : public CbmTask {
public:

  // Constructors/Destructors ---------
  PndTpcPSATask();
  ~PndTpcPSATask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetSampleBranchName(const TString& name) {_sampleBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);


private:

  // Private Data Members ------------
  TString _sampleBranchName;
  TClonesArray* _sampleArray;
  TClonesArray* _digiArray;
  
  Bool_t _persistence;
 
  PndTpcFrontend* _frontend;
  PndTpcAbsPSAStrategy* _psa;

  

  // Private Methods -----------------

public:
  ClassDef(PndTpcPSATask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
