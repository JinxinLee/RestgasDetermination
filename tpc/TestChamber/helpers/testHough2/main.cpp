#include "../../src/TChough2.h"
#include "../../src/TChough1.h"
#include "../../src/TCcluster.h"
#include "../../src/TCevent.h"
#include "../../src/TCalign.h"
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TROOT.h>
#include <iostream>

int main(){
  using namespace std;
  TCalign* al = TCalign::getInstance("alignment/simRealAlign1.txt");
  al->clear();
  al->read("../../alignment/simRealAlign1.txt");
  TVector3 X(1.,0.,0.);
  TVector3 Y(0.,1.,0.);
  TVector3 Z(0.,0.,1.);
  TChough2* a = new TChough2(Y,Z);
  TChough1* a1 = new TChough1(Y,Z,0.3,0.35);

  TFile* file= TFile::Open("event.root");
  TTree *t =(TTree*)gROOT->FindObject("at2");
  int nEvt = t->GetEntries();
  TCevent *ev = 0;
  
  t->SetBranchAddress("event",&ev);
  int counter=0;
  t->GetEntry(1);
  vector<TCcluster> cl;
  for(unsigned int i=0;i<ev->nClusters();++i){
    if(ev->getCluster(i).getId()%2==0){
      cl.push_back(ev->getCluster(i));
      cout<<i<<endl;
    }

  }
  a->make(cl,2);
  a1->make(cl);
  vector<TCcluster> ret1;
  vector<TCcluster> ret2;
  

  for (unsigned int i=0 ; i < cl.size(); ++i ){
    if(a->hot(i,0)){
      cl.at(i).setFit();
      ret1.push_back(cl.at(i));
      //  cout<<"bla "<<i<<endl;
    }
    /*if(a->hot(i,1)){
      cl.at(i).setFit();
      ret2.push_back(cl.at(i));
      //      cout<<"bla2 "<<i<<endl;
    }
    */

  }
  a1->draw(true,700,50,600,600);
  a->draw(true,700,50,600,600);
  delete a;

}
