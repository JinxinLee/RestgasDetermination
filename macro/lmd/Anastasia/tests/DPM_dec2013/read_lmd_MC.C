void read_lmd_MC(TString path="testDPM/mom_1_5/"){
  TString name = path+"/Lumi_MC_0.root";
  TFile *fin = new TFile(name,"READ");
  TTree *pndtree =(TTree*)fin->Get("cbmsim");
  TCanvas c1;
  cbmsim->Draw("MCTrack.GetMomentum().Theta()");
  TString namef = path+"/MCtheta.pdf";
  c1.SaveAs(namef);
}
