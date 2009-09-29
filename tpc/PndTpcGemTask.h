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
#include "FairTask.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;
class PndTpcDigiPar;


class PndTpcGemTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcGemTask();
  ~PndTpcGemTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetDriftedBranchName(const TString& name) {fdriftedBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {fpersistence=opt;}
  void SetGainFluctuations(Bool_t opt=kTRUE) {fgainFluctuations=opt;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();

private:

  // Private Data Members ------------
  TString fdriftedBranchName;
  TClonesArray* fdriftedArray;
  TClonesArray* favalancheArray;
  
  Bool_t fpersistence;
  Bool_t fgainFluctuations;
  Double_t fgain;

  PndTpcDigiPar* fpar;

  Bool_t finitialized;

  // Private Methods -----------------

public:
  ClassDef(PndTpcGemTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
