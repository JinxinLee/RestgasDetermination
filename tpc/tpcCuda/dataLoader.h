#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "PndTpcDigi.h"
#include "TClonesArray.h"



class dataLoader {
 public:
  dataLoader(TString filename);
  
  double getDigiAmp(int i);
  int nDigis(){return digis->GetEntriesFast();}

 private:

  TTree* tree;
  TClonesArray* digis;
  
};
