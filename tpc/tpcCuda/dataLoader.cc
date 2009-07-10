#include "dataLoader.h"

dataLoader::dataLoader(TString filename){
  

  TFile* file =  TFile::Open(filename);
  TTree* tr = (TTree*) file->Get("cbmsim");
  unsigned int entries = tr->GetEntries();
  
  tr->Print();
  //tr->Scan("PndTpcDigi._amp");

  std::cout<<"\n\nTree has "<<entries<<" entries ..."<<std::endl;
  
  digis = new TClonesArray("PndTpcDigi");
  tr->SetBranchAddress("PndTpcDigi", &digis);
  tr->GetEntry(0);

}

double
dataLoader::getDigiAmp(int i){
  return ((PndTpcDigi*)digis->At(i))->amp();
}
