{
   gStyle->SetOptTitle(0);
   gStyle->SetTitleOffset(.85,"X");gStyle->SetTitleOffset(.85,"Y");
  gStyle->SetTitleSize(.05,"X");gStyle->SetTitleSize(.05,"Y");
  gStyle->SetLabelSize(.05,"X");gStyle->SetLabelSize(.05,"Y");
  gStyle->SetHistLineWidth(2);
  gStyle->SetOptFit(1);
  TFile *inFile = TFile::Open("pid_sttcombi.root");

  TCanvas* c1 = new TCanvas("PID", "PID", 100, 100, 800, 800);

  c1.Divide(3,2);
  c1.cd(1);

  simtree->Draw("PidChargedCand.GetEmcRawEnergy()/PidChargedCand.GetMomentum().Mag():PidChargedCand.GetMomentum().Mag()>>emc_pid(100,0,3,100,0,1.2)","PidChargedCand.GetEmcIndex()>-1","colz");
  emc_pid->GetXaxis()->SetTitle("Momentum [GeV/c]");
  emc_pid->GetYaxis()->SetTitle("EMC Energy / Momentum [c]");

  c1.cd(2);
  simtree->Draw("PidChargedCand.GetTofBeta():PidChargedCand.GetMomentum().Mag()>>tof_pid(100,0,3,100,0,1.2)","PidChargedCand.GetTofIndex()>-1","colz");
  tof_pid->GetXaxis()->SetTitle("Momentum [GeV/c]");
  tof_pid->GetYaxis()->SetTitle("#beta");

  c1.cd(3);
  simtree->Draw("PidChargedCand.GetDrcThetaC()*TMath::RadToDeg():PidChargedCand.GetMomentum().Mag()>>drc_pid(100,0,3,100,0,60)","PidChargedCand.GetDrcIndex()>-1","colz");
  drc_pid->GetXaxis()->SetTitle("Momentum [GeV/c]");
  drc_pid->GetYaxis()->SetTitle("#theta_{c} [^{0}]");

  c1.cd(4);
  simtree->Draw("PidChargedCand.GetMvdDEDX():PidChargedCand.GetMomentum().Mag()>>mvd_pid(100,0,3,100,0,0.1)","PidChargedCand.GetMvdHits()>0","colz");
  mvd_pid->GetXaxis()->SetTitle("Momentum [GeV/c]");
  mvd_pid->GetYaxis()->SetTitle("MVD dE / # hit [MeV]");

  c1.cd(5);
  simtree->Draw("PidChargedCand.GetMuoNumberOfLayers():PidChargedCand.GetMomentum().Mag()>>muo_pid(100,0,3,17,-0.5,16.5)","","colz");
  muo_pid->GetXaxis()->SetTitle("Momentum [GeV/c]");
  muo_pid->GetYaxis()->SetTitle("MDT Fired Layers");

  c1.cd(6);
  simtree->Draw("PidChargedCand.GetMuoNumberOfLayers():PidChargedCand.GetMomentum().Theta()*TMath::RadToDeg()>>muo_pidt(100,0,100,17,-0.5,16.5)","","colz");
  muo_pidt->GetXaxis()->SetTitle("#theta [degrees]");
  muo_pidt->GetYaxis()->SetTitle("MDT Fired Layers");

  c1.cd();
}
