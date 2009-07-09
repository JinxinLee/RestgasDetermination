#include <iostream>
#include "TFile.h"
#include "TTree.h"


int main() {

  std::cout<<"\nHello World!"<<std::endl;

  TFile* file =  new TFile("iron55_GF.mc.root");
  TTree* tr = (TTree*) file->Get("cbmsim");
  unsigned int entries = tr->GetEntries();
  
  std::cout<<"\n\nTree has "<<entries<<" entries ..."<<std::endl;

}
