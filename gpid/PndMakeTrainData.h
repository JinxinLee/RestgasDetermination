#ifndef PND_MAKE_TRAIN_DATA_HH
#define PND_MAKE_TRAIN_DATA_HH 1

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "TNtuple.h"
#include "TClonesArray.h"
#include "PndTofPoint.h"
#include "PndMvdMCPoint.h"
#include "PndTpcPoint.h"
#include "PndTofHit.h"
#include "TROOT.h"
#include "TStyle.h"
#include <memory>
#include "TSystem.h"
#include "CbmMCTrack.h"
#include "CbmMCPoint.h"
#include "CbmTrackParH.h"
#include "TNtuple.h"
#include "TFile.h"
#include "PndPidCand.h"
#include "TDirectory.h"
#include "TChain.h"

using namespace std;

class PndMakeTrainData  {
  private:
    int fNCLASS;                // number of class
    string outFileName;         // file name of the tree file containing all the class signals
    map <string, vector<pair<string,string> > > fInFileNameArray; //  map from class name to the corresponding input files
    vector <string> fClassNameArray;  // array of class names
    void FillNTuple(TChain  &simChain,TChain  &recoChain, TNtuple &ntuple);  // function which fills the  class ntuples from corresponding files
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
};
#endif

