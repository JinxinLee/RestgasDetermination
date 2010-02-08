{
  gStyle->SetOptTitle(0);
  gStyle->SetTitleOffset(.9,"X");gStyle->SetTitleOffset(.9,"Y");
  
  TFile *inFile = TFile::Open("reco_tpccombi.root");

  TCanvas* c1 = new TCanvas("LHETRACK Momentum", "LHETRACK Momentum", 100, 100, 800, 800);

  c1.Divide(2,2);
  c1.cd(1);

  cbmsim->Draw("LhePidTrack.GetMomentum().Mag()>>mom(100,0.5,1.5)","");
  mom->GetXaxis()->SetTitle("Momentum [GeV/c]");

  c1.cd(2);
  cbmsim->Draw("LhePidTrack.GetMomentum().Theta()*TMath::RadToDeg()>>theta(100,0,180)","","");
  theta->GetXaxis()->SetTitle("#theta [^{0}]");

  c1.cd(3);
  cbmsim->Draw("LhePidTrack.GetMomentum().Phi()*TMath::RadToDeg()>>phi(100,-180,180)","","");
  phi->GetXaxis()->SetTitle("#phi [^{0}]");

  c1.cd(4);
}
