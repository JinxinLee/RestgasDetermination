//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Task that mixes (copies) events from an other input file to the 
//      current data stream. Only PndTpcSignals are added for the moment
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCEVTMIXTASK_HH
#define TPCEVTMIXTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include <set>

// Collaborating Class Declarations --
class TClonesArray;
class TFile;
class TTree;
class TBranch;
class PndTpcPadPlane;
class PndTpcDigiPar;

class PndTpcEvtMixTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcEvtMixTask();
  ~PndTpcEvtMixTask();

  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------
  void SetInBranchName(const TString& name) {finBranchName=name;}
  void SetBkgBranchName(const TString& name) {fbkgBranchName=name;}
  void SetBkgFileName(const TString& name) {fbkgFileName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {fpersistence=opt;}
  void SetNBkgEvts(Int_t n) {fnbkgEvts=n;}
  void AddSector(UInt_t id){fsectors.insert(id);}

  // Operations ----------------------
  virtual InitStatus Init();
  virtual void SetParContainers();
  virtual void Exec(Option_t* opt);

private:

  // Private Data Members ------------
  TString finBranchName;
  TString fbkgBranchName;
  TString fbkgFileName;
 

  TClonesArray* fsignalArray;
  TClonesArray* fbkgArray;
  TClonesArray* ftimeArray;

  TFile* finFile;
  TTree* fbkgTree;
  TBranch* fbkgBranch;

  Bool_t fpersistence;
  Int_t fnbkgEvts;

  std::set<unsigned int> fsectors;
  PndTpcDigiPar* fpar;

  const PndTpcPadPlane* fpadPlane;
 

  // Private Methods -----------------

public:
  ClassDef(PndTpcEvtMixTask,1);

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
