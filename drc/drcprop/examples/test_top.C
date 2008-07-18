{
  //TFile* f = TFile::Open("test_top_1_30_nod.root");
  TFile* f = TFile::Open("test_top.root");
  TH2F* h1 = new TH2F("h1", "TOP",1000, -100.0, 100.0,1000,10,12);
  TH2F* h2 = new TH2F("h2", "TOP",1000, -100.0, 100.0,1000,10,12);
  ntuple->Draw("t:x>>h1","m>400");
  ntuple->Draw("t:x>>h2","m<400");

  h1->SetXTitle("x position (mm)");  
  h1->SetYTitle("time (ns)");

  h1->SetMarkerColor(4);
  h1->SetMarkerStyle(7);
  h2->SetMarkerColor(2);
  h2->SetMarkerStyle(7);

  h1->SetStats(0);
  h2->SetStats(0);
  
  h1->Draw();
  h2->Draw("same");
  
}
