void checkAcceptance(TString filepath) {
  TFile *f = new TFile(filepath + "/lmd_acc_data.root", "READ");
  PndLmdAcceptance *acc;
  f->GetObject("lmd_acceptance", acc);

  TCanvas *c = new TCanvas("c", "c", 1000, 700);
  c->Divide(2,1);
  c->cd(1);
  TEfficiency *eff = acc->getAcceptance1D();
  //eff->SetStatisticOption(1); 
  eff->Draw();
  gPad->Update();
  TGraphAsymmErrors *graph = eff->GetPaintedGraph();
  TGraph *relerrhigh = new TGraph(graph->GetN());
  TGraph *relerrlow = new TGraph(graph->GetN());
  for(int i =0; i < graph->GetN(); i++) {
    double val_low = 0.0;
    double val_high = 0.0;
    if(graph->GetY()[i] > 0.0) {
      val_low = graph->GetErrorYlow(i)/graph->GetY()[i];
      val_high = graph->GetErrorYhigh(i)/graph->GetY()[i];
    }
    double N = eff->GetTotalHistogram()->GetBinContent(i+1);
    double k = eff->GetPassedHistogram()->GetBinContent(i+1);
    double p = k/N;
    double myval = 0.842*sqrt(p*(1.0-p)/N);

    cout<<"N: "<<N<<" | k: "<<k<<" | p_root: "<<graph->GetY()[i]<<" | p_mine: "<<p<<" | np & n(1-p) > 5??: "<<N*p<<" & "<<N*(1-p)<<" | errors (low, high, mine): "<<val_low<<" , "<<val_high<<" , "<< myval<<endl;
    relerrlow->SetPoint(i, graph->GetX()[i], val_low);
    relerrhigh->SetPoint(i, graph->GetX()[i], val_high);
  }
  relerrlow->Draw("A*");
  relerrlow->SetTitle("");
  relerrlow->GetXaxis()->SetTitle("#Theta_{MC} [rad]");
  relerrlow->GetYaxis()->SetTitle("Error/Value");
  relerrlow->GetYaxis()->SetRangeUser(0.0, 0.04);
  relerrlow->GetYaxis()->SetTitleOffset(2.2);
  relerrhigh->Draw("*SAME");
  relerrhigh->SetMarkerColor(2);

  c->cd(2);
  graph->Draw("AP");
  graph->GetXaxis()->SetTitle("#Theta_{MC} [rad]");
  graph->GetYaxis()->SetTitle("Efficiency");
}
