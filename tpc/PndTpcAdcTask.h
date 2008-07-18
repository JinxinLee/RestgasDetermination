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
#include "CbmTask.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;
class PndTpcFrontend;
class PndTpcAbsPulseshape;

class PndTpcAdcTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  PndTpcAdcTask();
  ~PndTpcAdcTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetSignalBranchName(const TString& name) {_signalBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);


private:

  // Private Data Members ------------
  TString _signalBranchName;
  TClonesArray* _sampleArray;
  TClonesArray* _signalArray;
  
  Bool_t _persistence;
 
  PndTpcFrontend* _frontend;
  PndTpcAbsPulseshape* _pulseshape;

  

  // Private Methods -----------------

public:
  ClassDef(PndTpcAdcTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
