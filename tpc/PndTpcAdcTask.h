//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpc ADC: produces samples from signals
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

#ifndef TPCADCTASK_HH
#define TPCADCTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;
class PndTpcFrontend;
class PndTpcAbsPulseshape;

class PndTpcAdcTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcAdcTask();
  ~PndTpcAdcTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetSignalBranchName(const TString& name) {fsignalBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {fpersistence=opt;}
  

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);


private:

  // Private Data Members ------------
  TString fsignalBranchName;
  TClonesArray* fsampleArray;
  TClonesArray* fsignalArray;
  
  Bool_t fpersistence;
 
  PndTpcFrontend* ffrontend;
  PndTpcAbsPulseshape* fpulseshape;

  

  // Private Methods -----------------

public:
  ClassDef(PndTpcAdcTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
