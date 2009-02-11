#ifndef FITSTATTASK_HH
#define FITSTATTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include "statTree.h"

// Collaborating Class Declarations --
class TClonesArray;


class FitStatTask : public FairTask {
public:

  // Constructors/Destructors ---------
  FitStatTask();
  virtual ~FitStatTask();

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

  statTreeMaker* stat;

  // Private Methods -----------------

public:
  ClassDef(FitStatTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
