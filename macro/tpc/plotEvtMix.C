{

TClonesArray* hitarray=new TClonesArray("PndTpcRiemannHit");
TClonesArray* trackarray=new TClonesArray("PndTpcRiemannTrack");
TClonesArray* resarray=new TClonesArray("PndTpcRiemannMCResidual");

cbmsim->SetBranchAddress("RiemannHit",&hitarray);
cbmsim->SetBranchAddress("RiemannTrack",&trackarray);
cbmsim->SetBranchAddress("RiemannRes",&resarray);

TH1D* hSZtrue=new TH1D("sztrue","sztrue",100,-20,20);
TH2D* hSZWeightTrue=new TH2D("szwt","szwt",100,-20,20,101,0.4,1.01);
TH1D* hRiemtrue=new TH1D("riemtrue","riemtrue",100,0.1,0.1);
TH1D* hSZ=new TH1D("sz","sztrue",100,-20,20);
TH1D* hRiem=new TH1D("riem","riemtrue",100,0.1,0.1);

int n=cbmsim->GetEntriesFast();
for(int i=0;i<n; ++i){ // event loop
  cbmsim->GetEntry(i);
  int nres=resarray->GetEntriesFast();
  for(int ires=0;ires<nres;++ires){ // loop over residuals
    PndTpcRiemannMCResidual* res=(PndTpcRiemannMCResidual*)resarray->At(ires);
    PndTpcRiemannHit* hit=res->hit();
    if(res.isTruth()){
      hSZtrue->Fill(res->szd());
      hSZWeightTrue->Fill(res->szd(),hit->cluster()->mcId().MaxRelWeight());
      hRiemtrue->Fill(res->riemd());
    }
     hSZ->Fill(res->szd());
     hRiem->Fill(res->riemd());
  } // end loop over residuals

}// end event loop


TCanvas* c=new TCanvas("c","c",10,10,800,800);
c->Divide(3,3);
c->cd(1);
hSZtrue->Draw();
c->cd(2);
hSZWeightTrue->Draw("col");
c->cd(3);
hRiemtrue->Draw();
c->cd(4);
hSZ->Draw();
c->cd(5);
hRiem->Draw();

}
