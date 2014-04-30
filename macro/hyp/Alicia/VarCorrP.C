

#include "TParticle.h" 
void VarCorrP() {

  TFile *f = new TFile("carbon_onlyXi_2_5.root");
  Double_t y=2.;
  TTree *t;
  t = (TTree*)f->Get("data");
  
  TClonesArray* fParticles = new TClonesArray("TParticle",100);
  t->SetBranchAddress("Particles", &fParticles);
  TH2F *source = new TH2F("source","source hist",500,0,180,500,0,3.);
  
  for(int i=0;i<t->GetEntries();i++){
    
    t->GetEntry(i);
    
    for(int j=0;j<fParticles->GetEntriesFast();j++){
      
      TParticle* part = (TParticle*)fParticles->At(j);
      // if(part->GetPdgCode()==3312)
      //{

      source->Fill((part->Theta()*180)/3.1416,part->P());
	  //}
    }
  }

  Double_t a;
  Double_t b;
  
  TFile *out = TFile::Open("XiGengiBUUSamp.root","RECREATE");

  TH2F *final = new TH2F("final","final hist",500,0,180,500,0,2.);
// continued…

   for (Int_t i=0;i<10000;i++) {
     source->GetRandom2(b,a);
     Double_t px=0.,py=0., pz=0.,p=0.,Th=0.,phi=0;

    //  phi = gRandom->Uniform(0,360);// * TMath::DegToRad();

    // Double_t pz = p*TMath::Cos(theta* TMath::DegToRad());
    // Double_t py = p*TMath::Sin(theta* TMath::DegToRad())*TMath::Sin(phi);
    // Double_t px = p*TMath::Sin(theta* TMath::DegToRad())*TMath::Cos(phi);
     final->Fill(b,a);
   }
    TCanvas *c1 = new TCanvas("c1","c1",800,800);
    c1->Divide(2,1);
    c1->cd(1);
    source->Draw("colz");
    c1->cd(2);
    final->Draw("colz");
    c1->cd();
    //creating a file container for TH2F source
    out->cd();
    source->Write();
    out->Save();

}
