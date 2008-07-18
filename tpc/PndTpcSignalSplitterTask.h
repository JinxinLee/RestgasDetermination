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
#include "CbmTask.h"

// Collaborating Class Headers -------
#include <vector>
#include "TString.h"

// Collaborating Class Declarations --
class TClonesArray;
class PndTpcDigiMapper;
class TTree;
class TBranch;
class CbmEventHeader;

class PndTpcSignalSplitterTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  PndTpcSignalSplitterTask();
  ~PndTpcSignalSplitterTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetSignalBranchName(const TString& name) {_signalBranchName=name;}
  void SetOutfileBase(const TString& name){_base=name;}
  void SelectSector(int id){_secid=id; _single=true;}
 
  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void FillEmptyEvents();

private:

  // Private Data Members ------------
  TString _signalBranchName;
  TString _base;
  TClonesArray* _signalArray;
  PndTpcDigiMapper* _mapper;
  CbmEventHeader* _evtHeader;

  int _secid;
  int _single;

  std::vector<TString> _files;
  std::vector<unsigned int> _emptycounter;

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
