//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      example task for the usage of the genfit tracking farmework
//      inside FAIRRoot
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef LAMBDASTATTASK_HH
#define LAMBDASTATTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include "GFTrack.h"

#include "TApplication.h"

// Collaborating Class Declarations --
class TClonesArray;

class LambdaStatTask : public FairTask {
public:

  // Constructors/Destructors ---------
  LambdaStatTask();
  virtual ~LambdaStatTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void SetLambdaBranchName(const TString& name) {_lambdaBranchName=name;}
  void SetVertexBranchName(const TString& name) {_vertexBranchName=name;}
  void SetMCBranchName(const TString& name){_mcBranchName=name;}
 
private:

  // Private Data Members ------------
  //TApplication app;
  TString _lambdaBranchName;
  TString _vertexBranchName;
  TString _mcBranchName;
  TString _mcAnnexBranchName;

  TClonesArray* _mcTrackArray;
  TClonesArray* _lambdaArray;
  TClonesArray* _vertexArray;
  TClonesArray* _mcvertexArray;
  TClonesArray* _mcAnnexArray;

  Bool_t _persistence;

    // Private Methods -----------------

public:
  ClassDef(LambdaStatTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
