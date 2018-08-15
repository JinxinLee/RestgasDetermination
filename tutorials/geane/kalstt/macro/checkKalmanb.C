{

  TFile f("ex2r.root");

  // ptot
  TCanvas c8;
  TH1F *h8 = new TH1F("h8","Pt distribution",100,1.,2.);
  h8->SetTitle("PREFIT");
  simtree->Draw("(STTTrack.fParamLast.fTx * 0.006)  >> h8", "fFlag >= 3");


  TFile f2("ex2g.root");
  TCanvas c9;
  TH1F *h9 = new TH1F("h9","Pt distribution",100,1.,2.);
  h9->SetTitle("KALMAN");
  simtree->Draw("sqrt(GeaneTrackPar.fPx * GeaneTrackPar.fPx + GeaneTrackPar.fPy * GeaneTrackPar.fPy) >> h9");

}
