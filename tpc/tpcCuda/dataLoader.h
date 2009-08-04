#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "PndTpcDigi.h"
#include "PndTpcCluster.h"
#include "TClonesArray.h"



class dataLoader {
 public:
  dataLoader(TString filename);
  
  void setEvent(unsigned int i);
  
  double getDigiAmp(int i);
  void getClusterPos(int i, double& x, double& y, double& z);
  void getClusterPosF(int i, float& x, float& y, float& z);
  void getClusterPosPolar(int i, double& r, double& phi, double& z);
  int nDigis(){return _digis->GetEntriesFast();}
  int nClusters() {return _clusters->GetEntriesFast();}

 private:

  TTree* _tree;
  TClonesArray* _digis;
  TClonesArray* _clusters;
  
};
