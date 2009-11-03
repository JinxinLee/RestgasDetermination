{
  gStyle->SetOptTitle(0);
  gStyle->SetTitleOffset(.9,"X");gStyle->SetTitleOffset(.9,"Y");
  
  TFile *inFile1 = TFile::Open("reco_tpccombi.root");
  TFile *inFile2 = TFile::Open("fit_tpccombi.root");

  TNtuple *lhe = (TTree*)inFile1->Get("cbmsim");
  TNtuple *fit = (TTree*)inFile2->Get("cbmsim");
  //TCanvas* c1 = new TCanvas("LHETRACK Momentum", "LHETRACK Momentum", 100, 100, 800, 800);

  //c1.Divide(2,2);
  //c1.cd(1);

  lhe->Draw("LheTrack.GetParamFirst().GetMomentum().Mag()>>momlhe(100,0.5,1.5)","","goff");
  momlhe->GetXaxis()->SetTitle("Momentum [GeV/c]");

  fit->Draw("LheGenTrack.GetParamFirst().GetMomentum().Mag()>>momfit(100,0.5,1.5)","","goff");
  momfit->GetXaxis()->SetTitle("Momentum [GeV/c]");

  momlhe.SetLineColor(2);
  momfit.SetLineColor(4);
  momlhe.SetLineWidth(2);
  momfit.SetLineWidth(2);

  Float_t max;
  if (momfit.GetMaximum()>momlhe.GetMaximum()) 
    {
      max = momfit.GetMaximum();
      momfit.Draw();
      momlhe.Draw("same");
    }
  else
    {
      max = momlhe.GetMaximum();
      momlhe.Draw();
      momfit.Draw("same");
    }
  TLatex llhe(1.2,0.9*max,"lhetrack");
  llhe->SetTextColor(2);
  
  TLatex lfit(1.2,0.8*max,"genfit");
  lfit->SetTextColor(4);
  
  llhe.Draw();
  lfit.Draw();

  c1.cd();
}
