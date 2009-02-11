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

#ifndef GENFITTASK_HH
#define GENFITTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include "Track.h"

// Collaborating Class Declarations --
class TClonesArray;
class TObjArray;
class RecoHitFactory;

class GenfitTask : public FairTask {
public:

  // Constructors/Destructors ---------
  GenfitTask();
  virtual ~GenfitTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);


private:

  // Private Data Members ------------
  TClonesArray* _trackInputArray;
  TClonesArray* _trackOutputArray;
  TClonesArray* _hitArray;
  TClonesArray* _digiArray;
  TClonesArray* _TSThitArray;
  
  Bool_t _persistence;

  RecoHitFactory* theRecoHitFactory;

  // Private Methods -----------------

public:
  ClassDef(GenfitTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
