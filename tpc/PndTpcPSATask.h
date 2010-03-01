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
#include "FairTask.h"
#include "PndTpcDigiPar.h"
// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;
class PndTpcFrontend;
class PndTpcAbsPSAStrategy;
class PndTpcDigiPar;

class PndTpcPSATask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcPSATask();
  ~PndTpcPSATask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetSampleBranchName(const TString& name) {fsampleBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {fpersistence=opt;}
  

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();

private:

  // Private Data Members ------------
  TString fsampleBranchName;
  TClonesArray* fsampleArray;
  TClonesArray* fdigiArray;
  
  Bool_t fpersistence;
 
  PndTpcFrontend* ffrontend;
  PndTpcAbsPSAStrategy* fpsa;

  PndTpcDigiPar* fpar;

  // Private Methods -----------------

public:
  ClassDef(PndTpcPSATask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
