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

#ifndef TRACKREADTASK_HH
#define TRACKREADTASK_HH

// Base Class Headers ----------------
#include "CbmTask.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;


class TrackReadTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  TrackReadTask();
  virtual ~TrackReadTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {_trackBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);


private:

  // Private Data Members ------------
  TString _trackBranchName;
  TClonesArray* _trackArray;
  
  Bool_t _persistence;

  // Private Methods -----------------

public:
  ClassDef(TrackReadTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
