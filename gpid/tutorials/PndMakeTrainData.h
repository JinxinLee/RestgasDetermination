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
#include "FairMCPoint.h"
#include "FairTrackParH.h"
#include "TNtuple.h"
#include "TFile.h"
#include "PndPidCand.h"
#include "TDirectory.h"

using namespace std;

class PndMakeTrainData  {
  private:
    int fNCLASS;
    string outFileName;
    map <string, vector<pair<string,string> > > fInFileNameArray;
    vector <string> fClassNameArray;
    void FillNTuple(string const inf1,string const inf1, TNtuple &ntuple);
  public:
    PndMakeTrainData();
    ~PndMakeTrainData();
    void GenerateTree();
    void AddInFile(string className,string simFileName,string recoFileName);
    void SetOutFileName(string fName) {outFileName = fName; }
};
#endif

