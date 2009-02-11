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

#ifndef V0SELECTOR_HH
#define V0SELECTOR_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
 

// Collaborating Class Declarations --
class TClonesArray;


class V0Selector : public FairTask {
public:

  // Constructors/Destructors ---------
  V0Selector();
  virtual ~V0Selector();

  
  // Accessors -----------------------


  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {_trackBranchName=name;}
  void SetV0BranchName(const TString& name) {_V0BranchName=name;}
  void SetPersistence(Bool_t flag=kTRUE) {_persistence=flag;}

  void SetPositivePartMass(double m){_mass_pos=m;}
  void SetNegativePartMass(double m){_mass_neg=m;}


  // Operations ----------------------
  virtual InitStatus Init();
  
  virtual void Exec(Option_t* opt);

private:

  // Private Data Members ------------
  TString _trackBranchName;
  TString _V0BranchName;

  Bool_t _persistence;

  //masses of particles default: electrons
  double _mass_pos;
  double _mass_neg;


  TClonesArray* _trackArray;
  TClonesArray* _vertexArray;
  TClonesArray* _V0Array;
  // Private Methods -----------------

public:
  ClassDef(V0Selector,1)
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
