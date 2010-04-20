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
class PndTpcAbsPulseshape;

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
  void SetPeakingtime(int pik) {fpeak=pik;}
  void SetTail(bool b){fTail=b;}

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
  bool fTail;
 
  PndTpcFrontend* ffrontend;
  PndTpcAbsPSAStrategy* fpsa;
  PndTpcAbsPulseshape* fpulseshape;
  
  PndTpcDigiPar* fpar;

  int fpeak;

  // Private Methods -----------------

public:
  ClassDef(PndTpcPSATask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
