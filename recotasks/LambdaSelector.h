//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Task to select lambdas
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

#ifndef LAMBDASELECTOR_HH
#define LAMBDASELECTOR_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
 

// Collaborating Class Declarations --
class TClonesArray;


class LambdaSelector : public FairTask {
public:

  // Constructors/Destructors ---------
  LambdaSelector();
  virtual ~LambdaSelector();

  
  // Accessors -----------------------


  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {_trackBranchName=name;}
  void SetPersistence(Bool_t flag=kTRUE) {_persistence=flag;}

  // Operations ----------------------
  virtual InitStatus Init();
  
  virtual void Exec(Option_t* opt);

private:

  // Private Data Members ------------
  TString _trackBranchName;
  Bool_t _persistence;

  TClonesArray* _trackArray;
  TClonesArray* _vertexArray;
  TClonesArray* _lambdaArray;
  // Private Methods -----------------

public:
  ClassDef(LambdaSelector,1)
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
