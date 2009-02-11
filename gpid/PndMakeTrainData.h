/*
 *  
 *  S.Vanniarajan  V.Suyam.Jothi@kvi.nl
 *
 *  This is the Mutivariate Event classification tool
 *  designed for PANDAROOT Analysis package for 
 *  PANDA Experiments.
 *       TMVA(Toolkit for MultiVariate Analysis) is a 
 *  two class classifier(signal and background). 
 *  available with ROOT package. mainly used for 
 *  Event Classification in High Energy Physics Experiments.
 *         
 *       This tool here is Designed from TMVA, for Multiclass 
 * Classification purpose. 
 *
 *                  S.Vanniarajan  01-08-08
 * Modified:
 * M. Babai
 */

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
#include "PndMCTrack.h"
#include "FairMCPoint.h"
#include "FairTrackParH.h"
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
  void AddInFile(const string className, const string simFileName, 
		 const string recoFileName);
  
  // data modifiers 
  void SetOutFileName(const string fName) {outFileName = fName; }

  //protected:
  
 private:
  int fNCLASS; // number of class
  // file name of the tree file containing all the class signals
  string outFileName;
  //  map from class name to the corresponding input files
  map <string, vector<pair<string,string> > > fInFileNameArray; 
  
  vector <string> fClassNameArray;  // array of class names
  
  // function which fills the  class ntuples from corresponding files
  void FillNTuple(TChain  &simChain,TChain  &recoChain, TNtuple &ntuple);
};
#endif

