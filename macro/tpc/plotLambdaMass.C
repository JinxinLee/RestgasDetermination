{
gStyle->SetOptFit();


TCanvas* c=new TCanvas("m","#Lambda",10,10,600,900);
c->Divide(1,4);

TString fname[4]={"/afs/e18/data/panda/MC/FAIRRoot/lambdas.nodiff.reco.root", 
		  "/afs/e18/data/panda/MC/FAIRRoot/lambdas.noDl.reco.root",
		  "/afs/e18/data/panda/MC/FAIRRoot/lambdas.noDt.reco.root",
		  "/afs/e18/data/panda/MC/FAIRRoot/lambdas.reco.root"};

for(int i=0; i<4; ++i){
  c->cd(i+1);

  TFile* infile=new TFile(fname[i]);

  cbmsim->Draw("LambdaMom.M()>>h(20,1.1,1.13)","abs(LambdaMom.M()-1.1157)<0.006","");
  h->Fit("gaus");
}




}
