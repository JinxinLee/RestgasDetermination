{
  gStyle->SetOptTitle(0);
  gStyle->SetTitleOffset(.85,"X");gStyle->SetTitleOffset(.85,"Y");
  TFile *inFile = TFile::Open("lhepidmaker.root");

  TCanvas* c1 = new TCanvas("LHETRACK Correlation", "LHETRACK Correlation", 100, 100, 1200, 800);

  c1.Divide(4,2);
  c1.cd(1);
  emcCorr->Draw("track_z-emc_z>>emcz(100,-20,20)","");
  emcz->GetXaxis()->SetTitle("Z_{LHE} - Z_{EMC} [cm]");

  c1.cd(2);
  tofCorr->Draw("track_z-tof_z>>tofz(100,-20,20)","","");
  tofz->GetXaxis()->SetTitle("Z_{LHE} - Z_{TOF} [cm]");

  c1.cd(3);
  TH1F *mdtz = new TH1F("mdtz","mdtz",100,-100,100);
  mdtCorr->Draw("track_z-mdt_z>>mdtz","","");
  mdtz->GetXaxis()->SetTitle("Z_{LHE} - Z_{MDT} [cm]"); 

  c1.cd(5);
  emcCorr->Draw("dphi*TMath::RadToDeg()>>emcphi(100,-10,10)","");
  emcphi->GetXaxis()->SetTitle("#phi_{LHE} - #phi_{EMC} [°]");
  
  c1.cd(6);
  tofCorr->Draw("dphi*TMath::RadToDeg()>>tofphi(100,-10,10)","");
  tofphi->GetXaxis()->SetTitle("#phi_{LHE} - #phi_{TOF} [°]");

  c1.cd(7);
  TH1F *mdtphi = new TH1F("mdtphi","mdtphi",100,-50,50);
  mdtCorr->Draw("dphi*TMath::RadToDeg()>>mdtphi","");
  mdtphi->GetXaxis()->SetTitle("#phi_{LHE} - #phi_{MDT} [°]");

  c1.cd(8);
  drcCorr->Draw("dphi*TMath::RadToDeg()>>drcphi(100,-10,10)","");
  drcphi->GetXaxis()->SetTitle("#phi_{LHE} - #phi_{DRC} [°]");

  c1.cd();
}
