{
  gStyle->SetOptTitle(0);
  gStyle->SetFillColor(0);
  gStyle->SetTitleOffset(.85,"X");gStyle->SetTitleOffset(.85,"Y");
  gStyle->SetTitleSize(.05,"X");gStyle->SetTitleSize(.05,"Y");
  gStyle->SetLabelSize(.05,"X");gStyle->SetLabelSize(.05,"Y");
  gStyle->SetHistFillColor(0);
  gStyle->SetHistLineWidth(2);
  gStyle->SetOptFit(1);
  TFile *inFile = TFile::Open("pid_sttcombi.root");
  cbmsim->AddFriend("cbmsim","points_sttcombi.root");
  
  TCanvas* c1 = new TCanvas("Charged Candidate", "Charged Candidate", 100, 100, 1200, 800);
  TCanvas* c2 = new TCanvas("Neutral Candidate", "Neutral Candidate", 100, 100, 1200, 800);
  
  c1.Divide(2,2);
  c1->cd(1);
  TH1F *charg_mom = new TH1F("charg_mom","charg_mom",100,-0.2,0.2);
  charg_mom->GetXaxis()->SetTitle("(P_{MC} - P_{track}) / P_{MC}");
  cbmsim.Draw("(MCTrack.GetMomentum().Mag()[PidChargedCand.GetMcIndex()]-PidChargedCand.GetMomentum().Mag())/MCTrack.GetMomentum().Mag()[PidChargedCand.GetMcIndex()]>>charg_mom","PidChargedCand.GetMcIndex()!=-1","");
  charg_mom->Fit("gaus");

  c1.cd(2);
  TH1F *charg_theta = new TH1F("charg_theta","charg_theta",100,-1,1);
  charg_theta->GetXaxis()->SetTitle("#theta_{MC} - #theta_{track} [degrees]");
  cbmsim.Draw("5.72957795130823229e+01*(MCTrack.GetMomentum().Theta()[PidChargedCand.GetMcIndex()]-PidChargedCand.GetMomentum().Theta())>>charg_theta","PidChargedCand.GetMcIndex()!=-1","");
  charg_theta->Fit("gaus");
  
  c1.cd(3);
  TH1F *charg_phi = new TH1F("charg_phi","charg_phi",100,-1,1);
  charg_phi->GetXaxis()->SetTitle("#phi_{MC} - #phi_{track} [degrees]");
  cbmsim.Draw("5.72957795130823229e+01*(MCTrack.GetMomentum().Phi()[PidChargedCand.GetMcIndex()]-PidChargedCand.GetMomentum().Phi())>>charg_phi","PidChargedCand.GetMcIndex()!=-1","");
  charg_phi->Fit("gaus");
  c1.cd(4);
  cbmsim.Draw("MCTrack.GetPdgCode()[PidChargedCand.GetMcIndex()]","PidChargedCand.GetMcIndex()!=-1");
   
  c2.Divide(2,2);
  c2->cd(1);
  TH1F *neu_mom = new TH1F("neu_mom","neu_mom",100,-2,2);
  neu_mom->GetXaxis()->SetTitle("(P_{MC} - P_{track}) / P_{MC}");
  cbmsim.Draw("(MCTrack.GetMomentum().Mag()[PidNeutralCand.GetMcIndex()]-PidNeutralCand.GetMomentum().Mag())/MCTrack.GetMomentum().Mag()[PidNeutralCand.GetMcIndex()]>>neu_mom","PidNeutralCand.GetMcIndex()!=-1","");
  neu_mom->Fit("gaus");

  c2.cd(2);
  TH1F *neu_theta = new TH1F("neu_theta","neu_theta",100,-20,20);
  neu_theta->GetXaxis()->SetTitle("#theta_{MC} - #theta_{track} [degrees]");
  cbmsim.Draw("5.72957795130823229e+01*(MCTrack.GetMomentum().Theta()[PidNeutralCand.GetMcIndex()]-PidNeutralCand.GetMomentum().Theta())>>neu_theta","PidNeutralCand.GetMcIndex()!=-1","");
  neu_theta->Fit("gaus");
  
  c2.cd(3);
  TH1F *neu_phi = new TH1F("neu_phi","neu_phi",100,-20,20);
  neu_phi->GetXaxis()->SetTitle("#phi_{MC} - #phi_{track} [degrees]");
  cbmsim.Draw("5.72957795130823229e+01*(MCTrack.GetMomentum().Phi()[PidNeutralCand.GetMcIndex()]-PidNeutralCand.GetMomentum().Phi())>>neu_phi","PidNeutralCand.GetMcIndex()!=-1","");
  neu_phi->Fit("gaus");
  c2.cd(4);
  cbmsim.Draw("MCTrack.GetPdgCode()[PidNeutralCand.GetMcIndex()]","PidNeutralCand.GetMcIndex()!=-1");
   
  

}
