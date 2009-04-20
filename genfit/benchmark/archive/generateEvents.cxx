//g++ -o generateEvents generateEvents.cxx `root-config --glibs --cflags`

#include"TRandom3.h"

#include"TFile.h"
#include"TTree.h"
#include"TVector3.h"
#include"TMath.h"

#include<iostream>

struct myEvent_t{
  TVector3 *pos;
  TVector3 *posC;
  TVector3* mom;
  TVector3* momC;
  myEvent_t(){
    pos = new TVector3;
    posC = new TVector3;
    mom = new TVector3;
    momC = new TVector3;
  }

  void print(){
    pos->Print();
    posC->Print();
    mom->Print();
    momC->Print();
    std::cout << "=============" << std::endl;
  }
};

TRandom3 myrand;
void gen(myEvent_t& e){
  e.mom->SetXYZ(1.,1.,1.);
  e.mom->SetMag(1.);
  e.mom->SetTheta(28./180.*TMath::Pi());
  e.mom->SetPhi(myrand.Uniform(0.,2.*TMath::Pi()));
  *(e.momC)=*(e.mom);

  e.momC->SetMag( e.momC->Mag() + myrand.Uniform( -0.1*e.momC->Mag(), 0.1*e.momC->Mag() ) );
  e.momC->SetTheta(e.momC->Theta() + myrand.Uniform(-2./180.*TMath::Pi(),2./180.*TMath::Pi()));
  e.momC->SetPhi(e.momC->Phi() + myrand.Uniform(-2./180.*TMath::Pi(),2./180.*TMath::Pi()));
  
  e.pos->SetXYZ(0.01,20.,10.);
  *(e.posC)=*(e.pos);
  e.posC->SetX( myrand.Gaus(e.posC->X(),0.1));
  e.posC->SetY( myrand.Gaus(e.posC->Y(),0.1));
  e.posC->SetZ( myrand.Gaus(e.posC->Z(),0.1));
}

int main(int agrc,char** argv){
  myrand.SetSeed(0);
  TFile* file = TFile::Open("events.root","RECREATE");
  TTree *tree = new TTree("ev","echter dreggn");
    
  myEvent_t E;
  
  tree->Branch("pos","TVector3",&(E.pos),32000,99);
  tree->Branch("posC","TVector3",&(E.posC),32000,99);
  tree->Branch("mom","TVector3",&(E.mom),32000,99);
  tree->Branch("momC","TVector3",&(E.momC),32000,99);

  for(int i=0;i<100000;++i){
    gen(E);
    //E.print();
    tree->Fill();
  }
  tree->Write();
  file->Close();
  
}
