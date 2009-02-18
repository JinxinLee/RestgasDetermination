{
// ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libgeant321");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
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


TFile* f=TFile::Open("demo.mcreco.root");
TTree* t=(TTree*)f->Get("cbmsim");

TCanvas* c2=new TCanvas("c2","Covariances",20,20,800,800);
c2->Divide(5,5);
for(int i=0; i<5;++i){
  for(int j=0;j<5;++j){
    c2->cd(1+i*5+j);
    TString com="Track.getCardinalRep().getCovElem(";
    com+=i;
    com+=",";
    com+=j;
    com+=")";
    t->Draw(com.Data());
  }
}



TCanvas* c=new TCanvas("c","Demo Plots",10,10,1000,500);
c->Divide(2,1);

c->cd(1);
t->Draw("Track.getMom().Mag()>>h(100,0.2,0.7)");
h->Fit("gaus");
c->cd(2);
t->Draw("Track.getMom().Theta()");



}
