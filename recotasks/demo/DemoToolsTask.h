//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Tools Task 
//      shows how to use different gools from GENFIT:
//      - getting the length of a piece of track
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

#ifndef DEMOTOOLSTASK_HH
#define DEMOTOOLSTASK_HH

// Base Class Headers ----------------
#include "CbmTask.h"

// Collaborating Class Headers -------
#include "TString.h"

// Collaborating Class Declarations --
class TClonesArray;
class TH1D;

class DemoToolsTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  DemoToolsTask();
  ~DemoToolsTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {_trackBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);
  
  void WriteHistograms();

private:

  // Private Data Members ------------
  TString _trackBranchName;
  TClonesArray* _trackArray;
  
  Bool_t _persistence;
  
  TH1D* _lengthH; // track length histo;
  
  int _trackcount;
  int _evt; // event counter

  // Private Methods -----------------

public:
  ClassDef(DemoToolsTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
