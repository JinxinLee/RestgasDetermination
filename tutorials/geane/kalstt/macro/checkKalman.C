{

  TFile f("ex2r.root");

  // ptot
  TCanvas c8;
  TH1F *h8 = new TH1F("h8","Ptot distribution",100,1.,2.);
  h8->SetTitle("PREFIT");
  simtree->Draw("sqrt((STTTrack.fParamLast.fTx * 0.006 * STTTrack.fParamLast.fTy)**2 + (STTTrack.fParamLast.fTx * 0.006)**2)  >> h8", "fFlag >= 3");


  TFile f2("ex2g.root");
  TCanvas c9;
  TH1F *h9 = new TH1F("h9","Ptot distribution",100,1.,2.);
  h9->SetTitle("KALMAN");
  simtree->Draw("-1./GeaneTrackPar.fQp >> h9","(1./GeaneTrackPar.fQp) != 0");//,"-1./GeaneTrackParKal.fQp > 1.4");

}
