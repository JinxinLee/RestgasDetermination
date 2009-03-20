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
#include"TApplication.h"
int main(){
  
  TApplication theApp("theApp",NULL,NULL);
  using namespace std;
  TCalign* al = TCalign::getInstance("alignment/simRealAlign1.txt");
  al->clear();
  al->read("../../alignment/simRealAlign1.txt");
  TVector3 X(1.,0.,0.);
  TVector3 Y(0.,1.,0.);
  TVector3 Z(0.,0.,1.);
  TChough2* a = new TChough2(Y,Z);


  TFile* file= TFile::Open("simExample.root");
  TTree *t =(TTree*)gROOT->FindObject("at2");
  int nEvt = t->GetEntries();
  TCevent *ev = 0;
  
  t->SetBranchAddress("event",&ev);
  int counter=0;
  for(int nEv = 0;nEv<nEvt;++nEv){
    t->GetEntry(nEv);
    vector<TCcluster> cl;
    for(unsigned int i=0;i<ev->nClusters();++i){
      if(ev->getCluster(i).getId()%2==0){
        if(ev->getCluster(i).getId()==0){
          cout<<"not working!"<<i<<endl;
        }
	cl.push_back(ev->getCluster(i));
	//	cout<<i<<endl;
      }
      
    }


    a->make(cl,2);

    vector<TCcluster> ret1;
    vector<TCcluster> ret2;
    
    
    for (unsigned int i=0 ; i < cl.size(); ++i ){
      if(a->hot(i,0)){
	cl.at(i).setFit();
	ret1.push_back(cl.at(i));
	//  cout<<"bla "<<i<<endl;
      }
      if(a->hot(i,1)){
	cl.at(i).setFit();
	ret2.push_back(cl.at(i));
	//      cout<<"bla2 "<<i<<endl;
      }
      
      
    }
    
    a->draw(true,10,10,600,600,ev);

 
    if(nEv%100==0){
      cout<<nEv<<endl;
    } 
  }
  delete a;

}
