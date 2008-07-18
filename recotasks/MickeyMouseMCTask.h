#ifndef MICKEYMOUSEMCTASK_HH
#define MICKEYMOUSEMCTASK_HH

// Base Class Headers ----------------
#include "CbmTask.h"

// Collaborating Class Headers -------
#include "Track.h"
#include "MCHitGenerator.h"

// Collaborating Class Declarations --
class TClonesArray;
class TObjArray;
class RecoHitFactory;

class MickeyMouseMCTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  MickeyMouseMCTask();
  virtual ~MickeyMouseMCTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);


private:
  MCHitGenerator gen;
  Track* createTrack();
  Track* createLSLTrack();

  // Private Data Members ------------
  TClonesArray* _trackOutputArray;
  int counter;
  Bool_t _persistence;

  // Private Methods -----------------

public:
  ClassDef(MickeyMouseMCTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
