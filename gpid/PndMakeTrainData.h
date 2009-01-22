#pragma once
#ifndef PND_MAKE_TRAIN_DATA_H
#define PND_MAKE_TRAIN_DATA_H

//C++ headers
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <memory>

// ROOT & PandaRootheaders
#include "TNtuple.h"
#include "TClonesArray.h"
#include "PndTofPoint.h"
#include "PndMvdMCPoint.h"
#include "PndTpcPoint.h"
#include "PndTofHit.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"
#include "CbmMCTrack.h"
#include "CbmMCPoint.h"
#include "CbmTrackParH.h"
#include "TNtuple.h"
#include "TFile.h"
#include "PndPidCand.h"
#include "PndLhePidTrack.h"
#include "TDirectory.h"
#include "TChain.h"

using namespace std;

class PndMakeTrainData  {
 public:
  // constructor
  PndMakeTrainData();
  // destructor
  ~PndMakeTrainData();
  // function generating the tree
  void GenerateTree();
  // function call adds the input file
  void AddInFile(string className,string simFileName,string recoFileName);
  // data modifiers 
  void SetOutFileName(string fName) {outFileName = fName; }

 private:
  int fNCLASS;                // number of class
  // file name of the tree file containing all the class signals
  string outFileName;
  //  map from class name to the corresponding input files
  map <string, vector<pair<string,string> > > fInFileNameArray; 
  vector <string> fClassNameArray;  // array of class names
  // function which fills the  class ntuples from corresponding files
  void FillNTuple(TChain  &simChain,TChain  &recoChain, TNtuple &ntuple);
};
#endif

