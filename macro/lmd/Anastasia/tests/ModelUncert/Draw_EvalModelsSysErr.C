//systematic deviation between the exp.data and the models for the data in the LMD range
void Draw_EvalModelsSysErr(){
  const int Npoints = 10;
  double plab[Npoints]={2.607, 3.7, 4.07, 4.2, 5.6, 5.72, 5.94, 6.0, 6.23, 10};
  double dpm_lmd[Npoints]={19.757, 4.17208, 1.6741, 8.13539, 7.0112, 10.6863, 10.5607, 3.15489, 8.29421, 1.60728};
  double err_dpm_lmd[Npoints]={9.91507, 2.79634, 4.25298, 7.10238, 3.47239, 2.65089, 2.58092, 7.35912, 3.48531, 3.4545};
  double e760_lmd[Npoints]={11.5314, 4.06736, 0.255933, 10.1748, 0.348997, 1.89268, 3.04421, 13.2598, 1.91386, 5.98838};
  double err_e760_lmd[Npoints]={10.6307, 0.932878, 1.15201, 5.24287, 0.704797, 0.544919, 0.551098, 4.96827, 0.620728, 2.47324};
  

TGraphErrors *gr_dpm_lmd = new TGraphErrors(Npoints, plab, dpm_lmd, 0, err_dpm_lmd);
gr_dpm_lmd->SetMarkerColor(kBlue+1);
gr_dpm_lmd->SetLineColor(kBlue+1);
gr_dpm_lmd->SetMarkerStyle(21);


TGraphErrors *gr_e760_lmd = new TGraphErrors(Npoints, plab, e760_lmd, 0, err_e760_lmd);
gr_e760_lmd->SetMarkerColor(kAzure+1);
gr_e760_lmd->SetLineColor(kAzure+1);
gr_e760_lmd->SetMarkerStyle(20);

 TLegend *leg_sigt = new TLegend(0.89,0.77,0.99,0.95);
  leg_sigt->SetFillColor(0);
  leg_sigt->SetTextFont(42);
  leg_sigt->SetTextSize(0.04);

TCanvas c1("c1","canvas",600,400);
 // c1.Divide(1,2);
// c1.cd(1);

// c1.cd(2);
//gPad->SetLogy();
TMultiGraph *mg_lmd = new TMultiGraph();
mg_lmd->Add(gr_dpm_lmd);
mg_lmd->Add(gr_e760_lmd);
 leg_sigt->AddEntry(gr_dpm_lmd,"DPM","pel");
 leg_sigt->AddEntry(gr_e760_lmd,"E760","pel");
mg_lmd->Draw("AP");
mg_lmd->GetYaxis()->SetTitle("model system.err, %");
mg_lmd->GetXaxis()->SetTitle("P_{lab}, GeV/c");
 leg_sigt->Draw();
 c1.SaveAs("ModelsSysErrEval.pdf");
 c1.SaveAs("ModelsSysErrEval.root");
}
