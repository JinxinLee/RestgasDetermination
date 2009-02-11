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

#ifndef TPCSPRTASK_HH
#define TPCSPRTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include "PndTpcCluster.h"


// Collaborating Class Declarations --
class TClonesArray;
class PndTpcFrontend;
class RecoHitFactory;

class PndTpcSimplePatternRecoTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcSimplePatternRecoTask();
  ~PndTpcSimplePatternRecoTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetClusterBranchName(const TString& name) {_clusterBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);


private:

  // Private Data Members ------------
  TString _clusterBranchName;
  TClonesArray* _clusterArray;
  TClonesArray* _trackArray;
  
  Bool_t _persistence;

  RecoHitFactory* _theRecoHitFactory;

 
  // Private Methods -----------------

public:
  ClassDef(PndTpcSimplePatternRecoTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
