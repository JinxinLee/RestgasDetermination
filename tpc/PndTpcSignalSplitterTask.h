//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Splitting Signals into one file per region
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

#ifndef TPCSIGNALSPLITTERTASK_HH
#define TPCSIGNALSPLITTERTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <vector>
#include "TString.h"

// Collaborating Class Declarations --
class TClonesArray;
class PndTpcDigiMapper;
class TTree;
class TBranch;
class FairEventHeader;

class PndTpcSignalSplitterTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcSignalSplitterTask();
  ~PndTpcSignalSplitterTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetSignalBranchName(const TString& name) {fsignalBranchName=name;}
  void SetOutfileBase(const TString& name){fbase=name;}
  void SelectSector(int id){fsecid=id; fsingle=true;}
 
  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void FillEmptyEvents();

private:

  // Private Data Members ------------
  TString fsignalBranchName;
  TString fbase;
  TClonesArray* fsignalArray;
  PndTpcDigiMapper* fmapper;
  FairEventHeader* fevtHeader;

  int fsecid;
  int fsingle;

  std::vector<TString> ffiles;
  std::vector<unsigned int> femptycounter;

  // methods
  void TranicateBranchNames(TTree *fTree, const char *folderName);
  void TranicateBranchNames(TBranch *b, TString ffn);




public:
  ClassDef(PndTpcSignalSplitterTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
