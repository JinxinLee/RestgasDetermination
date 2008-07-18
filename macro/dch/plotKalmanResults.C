{
// ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libgeant321");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libEmc");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libGen");
  gSystem->Load("libPGen");

  gSystem->Load("libTrkBase");
  gSystem->Load("libGeane");
  gSystem->Load("libgenfit");
  gSystem->Load("libtrackrep");
  gSystem->Load("libtpc");
  gSystem->Load("libtpcreco");
  gSystem->Load("librecotasks");

gSystem->ListLibraries();

 gStyle->SetOptStat(111111);

TFile* f=TFile::Open("tst_Tracks.root");
TTree* t=(TTree*)f->Get("cbmsim");

//TCanvas* c2=new TCanvas("c2","Covariances",20,20,800,800);
//c2->Divide(5,5);
// for(int i=0; i<5;++i){
//   for(int j=0;j<5;++j){
//     c2->cd(1+i*5+j);
//     TString com="Track.getCardinalRep().getCovElem(";
//     com+=i;
//     com+=",";
//     com+=j;
//     com+=")";
//     t->Draw(com.Data());
//   }
// }



TCanvas* c=new TCanvas("c","Demo Plots",10,10,800,800);
c->Divide(3,3);

c->cd(1);
//t->Draw("Track.getMom().Mag()>>h(100,1.5,2.5)");
t->Draw("Track.getMom().Mag()");
c->cd(2);
t->Draw("Track.getMom().Px()");
c->cd(3);
t->Draw("Track.getMom().Py()");
c->cd(4);
 t->Draw("Track.getMom().Theta()*TMath::RadToDeg()","Track.getCardinalRep()->getStatusFlag()!=0");
c->cd(5);
t->Draw("Track.getMom().Phi()*TMath::RadToDeg()");
c->cd(7);
t->Draw("Track.getChiSqu()");
c->cd(8);
t->Draw("Track.getNumHits()");
c->cd(9);
t->Draw("Track.");



}
