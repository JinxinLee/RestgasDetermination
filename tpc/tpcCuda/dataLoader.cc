#include "dataLoader.h"
#include "TVector3.h"

dataLoader::dataLoader(TString filename){
  

  TFile* file =  TFile::Open(filename);
  _tree = (TTree*) file->Get("cbmsim");
  unsigned int entries = _tree->GetEntries();
  
  _tree->Print();
  //tr->Scan("PndTpcDigi._amp");

  std::cout<<"\n\nTree has "<<entries<<" entries ..."<<std::endl;
  
  _digis = new TClonesArray("PndTpcDigi");
  _clusters = new TClonesArray("PndTpcCluster");
  _tree->SetBranchAddress("PndTpcDigi", &_digis);
  _tree->SetBranchAddress("PndTpcCluster", &_clusters);
  _tree->GetEntry(0);

}

void 
dataLoader::setEvent(unsigned int i) {
  unsigned int events = _tree->GetEntries();
  if(i > events) {
    std::cout<<"\ndataLoader::setEvent: ERROR: invalid event number!"<<std::endl;
    _tree->GetEvent(events);
  }
  else
    _tree->GetEvent(i);
}

double
dataLoader::getDigiAmp(int i){
  return ((PndTpcDigi*)_digis->At(i))->amp();
}

void
dataLoader::getClusterPos(int i, double& x, double& y, double& z) {
  TVector3 pos = ((PndTpcCluster*)_clusters->At(i))->pos();
  x = pos.X();
  y = pos.Y();
  z = pos.Z();
}
  
void
dataLoader::getClusterPosPolar(int i, double& r, double& phi, double& z) {
  TVector3 pos = ((PndTpcCluster*)_clusters->At(i))->pos();
  z = pos.Z();
  pos.SetZ(0.);
  r = pos.Mag();
  phi = pos.Phi();
  
}
