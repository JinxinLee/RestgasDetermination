
#include "PndTpcPoint.h"
#include "TFile.h"
#include "TError.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TH1D.h"
#include "TString.h"
#include "TF1.h"
#include "TCanvas.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>


void analize(std::string filename) {

  TFile* inFile = new TFile(filename.c_str());
  TTree* tree = (TTree*) inFile->Get("cbmsim");

  TClonesArray* points = new TClonesArray("PndTpcPoint");

  int nE=0;
  if(tree!=0) {
    tree->SetBranchAddress("PndTpcPoint", &points);
    nE = tree->GetEntries();
  }
  std::cout<<"found "<<nE<<" entries in tree"<<std::endl;

  //inFile->Close();

  double totalcharge=0.;
  
  std::cout<<"\nStarting loop over events ..."<<std::endl;

  for(int ev=0; ev<nE; ev++) {
    if(ev%500 == 0) {
      std::cout<<". ";
      std::cout.flush();
    }
    tree->GetEvent(ev);
    int nP = points->GetEntriesFast();
    
    for(int p=0; p<nP; p++) {  //loop over single points
      PndTpcPoint* the_point = (PndTpcPoint*) points->At(p);
      totalcharge+=the_point->GetEnergyLoss();
    }
      
  }
  std::cout<<"\n\nTotal accumulated charge before ionization was :"<<totalcharge<<" C"<<std::endl;

}
