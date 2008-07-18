{

gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
basiclibs();
gSystem->Load("libGeoBase");
gSystem->Load("libParBase");
gSystem->Load("libBase");
gSystem->Load("libMCStack");
gSystem->Load("libField");
gSystem->Load("libPassive");
gSystem->Load("libGen");  
gSystem->Load("libEmc"); 
gSystem->Load("libgenfit");
gSystem->Load("libtpc"); 
gSystem->Load("libtpcreco");
gSystem->Load("libtrackrep");
gSystem->Load("librecotasks");
gSystem->Load("libMvd");
gSystem->Load("libMvdReco");
gSystem->Load("libLHETrack");
   

TFile* f = new TFile("tracks_tpcmvdemc.root");
TTree *t=f->Get("cbmsim") ;
TClonesArray *fTr=new TClonesArray("PndTpcLheTrack");
t->SetBranchAddress("PndTpcLheTrack",&fTr) ;
PndTpcLheTrack  *tr1;
TFile* fsim = new TFile("points_tpcmvdemc.root");
PndEmcMapper *emcMap=PndEmcMapper::Instance(1);

TFile *out = TFile::Open("pT_histo.root","RECREATE");

TH1D* pT = new TH1D("pT","pT",100,0.5,1.5);
pT->SetTitle("pT / GeV");

for (Int_t j=0; j< t->GetEntriesFast(); j++){
  t->GetEntry(j);
  for (Int_t i1=0; i1<fTr->GetEntriesFast(); i1++){
    tr1=  (PndTpcLheTrack *)fTr->At(i1);
    // cout << "p " << tr1->GetPx() << " " << tr1->GetPy() << " " << tr1->GetPz() << "\n";
    cout << "pT " << sqrt(tr1->GetPx()*tr1->GetPx()+tr1->GetPy()*tr1->GetPy()) << "\n";
    pT->Fill(sqrt(tr1->GetPx()*tr1->GetPx()+tr1->GetPy()*tr1->GetPy()));
  }
}

pT->Draw();

out->cd();
pT->Write();
out->Save();

}

