{
  gStyle->SetOptTitle(0);
  gStyle->SetTitleOffset(.9,"X");gStyle->SetTitleOffset(1.01,"Y");
  
  TFile *inFile = TFile::Open("reco_tpccombi.root");

  TCanvas* c1 = new TCanvas("LHETRACK PID", "LHETRACK PID", 100, 100, 800, 800);

  c1.Divide(2,2);
  c1.cd(1);

  cbmsim->Draw("LhePidTrack.GetEmcELoss()/LhePidTrack.GetMomentum().Mag():LhePidTrack.GetMomentum().Mag()>>emc_pid(100,0,3,100,0,1.2)","LhePidTrack.GetEmcIndex()>-1","colz");
  emc_pid->GetXaxis()->SetTitle("Momentum [GeV/c]");
  emc_pid->GetYaxis()->SetTitle("EMC Energy / Momentum [c]");

  c1.cd(2);
  cbmsim->Draw("LhePidTrack.GetBeta():LhePidTrack.GetMomentum().Mag()>>tof_pid(100,0,2,100,0,1.2)","LhePidTrack.GetTofIndex()>-1","colz");
  tof_pid->GetXaxis()->SetTitle("Momentum [GeV/c]");
  tof_pid->GetYaxis()->SetTitle("#beta");

  c1.cd(3);
  cbmsim->Draw("LhePidTrack.GetDrcThetaC()*TMath::RadToDeg():LhePidTrack.GetMomentum().Mag()>>drc_pid(100,0,3,100,0,60)","LhePidTrack.GetDrcIndex()>-1","colz");
  drc_pid->GetXaxis()->SetTitle("Momentum [GeV/c]");
  drc_pid->GetYaxis()->SetTitle("#theta_{c} [^{0}]");

  c1.cd(4);
  cbmsim->Draw("1000*LhePidTrack.GetMvdELoss()/LhePidTrack.GetMvdHitCounts():LhePidTrack.GetMomentum().Mag()>>mvd_pid(100,0,3,100,0,2)","LhePidTrack.GetMvdHitCounts()>0","colz");
  mvd_pid->GetXaxis()->SetTitle("Momentum [GeV/c]");
  mvd_pid->GetYaxis()->SetTitle("MVD dE / # hit [MeV]");

  c1.cd();
}
