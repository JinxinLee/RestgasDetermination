void DrawSum(){
  double pbeam[6]={2.33,2.85,3.55,5.7,10.4,15.95};
  double err[6]={8.,5.,30.,5.,1.2,2.1};
  TGraph *gr = new TGraph(6,pbeam,err);
  gr->SetMarkerStyle(20);
  gr->SetMarkerSize(2.0);
  gr->Draw("AP");
}
