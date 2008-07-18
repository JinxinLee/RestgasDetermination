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
//      Christian Hoeppner    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef IDEALPATTERNRECTASK_HH
#define IDEALPATTERNRECTASK_HH

// Base Class Headers ----------------
#include "CbmTask.h"

// Collaborating Class Headers -------
#include "Track.h"

// Collaborating Class Declarations --
class TClonesArray;
class TObjArray;
class RecoHitFactory;

class IdealPatternRecTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  IdealPatternRecTask();
  virtual ~IdealPatternRecTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetHitBranchName(const TString& name) {_hitBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);


private:

  // Private Data Members ------------
  TString _hitBranchName;
  TClonesArray* _mctruthArray;
  TClonesArray* _trackOutputArray;
  TClonesArray* _hitArray;
  TClonesArray* _digiArray;
  TClonesArray* _TSThitArray;
  
  Bool_t _persistence;

  //  RecoHitFactory* theRecoHitFactory;

  // Private Methods -----------------

public:
  ClassDef(IdealPatternRecTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
