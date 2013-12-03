void read_lmd_MC(){
  TFile *fin = new TFile("testDPM/mom_1_5/Lumi_MC_0.root","READ");
  TTree *pndtree =(TTree*)fin->Get("cbmsim");
  TCanvas c1;
  cbmsim->Draw("MCTrack.GetMomentum().Theta()");
  c1.SaveAs("testDPM/mom_1_5/MCtheta.pdf");
}
