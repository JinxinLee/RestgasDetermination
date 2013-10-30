void plotsTrkPerformance(TString storePath="/panda/myResults/TrkSearchHIMsterSep2013/results_mom_1_5GeV/"){
  // //style ----------------------------------
  gROOT->Macro("/home/karavdina/Disser/materials/Style_Thesis.C");
  gROOT->SetStyle("Thesis");
  // //(end) style---------------------------

  // /// Missed vs hits ------------------------------------------------------------------------------
  TString inputCA1=storePath+"/Lumi_missed_ghost_CA_1trks.root";
  TFile *finCA1 = new TFile(inputCA1,"READ");
  TH1 *hmiss1 = (TH1 *)finCA1->Get("hnhitsmissed");
  double int1 = hmiss1->Integral();
  double scl1 = 100./int1;
  hmiss1->Scale(scl1);
  hmiss1->SetLineWidth(2);
  //  hmiss1->SetLineColor(kAzure+9);
  //kOrange = CA; kAzure = Follow
  hmiss1->SetLineColor(kOrange+10);
  hmiss1->GetYaxis()->SetTitle("(#missed trks)/(tot.missed trks), %");
  hmiss1->Draw();

  TString inputCA2=storePath+"/Lumi_missed_ghost_CA_2trks.root";
  TFile *finCA2 = new TFile(inputCA2,"READ");
  TH1 *hmiss2 = (TH1 *)finCA2->Get("hnhitsmissed");
  double int2 = hmiss2->Integral();
  double scl2 = 100./int2;
  hmiss2->Scale(scl2);
  hmiss2->SetLineWidth(2);
  hmiss2->SetLineColor(kOrange+8);
  hmiss2->Draw("same");

  TString inputCA3=storePath+"/Lumi_missed_ghost_CA_3trks.root";
  TFile *finCA3 = new TFile(inputCA3,"READ");
  TH1 *hmiss3 = (TH1 *)finCA3->Get("hnhitsmissed");
  double int3 = hmiss3->Integral();
  double scl3 = 100./int3;
  hmiss3->Scale(scl3);
  hmiss3->SetLineWidth(2);
  hmiss3->SetLineColor(kOrange+7);
  hmiss3->Draw("same");

  TString inputCA4=storePath+"/Lumi_missed_ghost_CA_4trks.root";
  TFile *finCA4 = new TFile(inputCA4,"READ");
  TH1 *hmiss4 = (TH1 *)finCA4->Get("hnhitsmissed");
  double int4 = hmiss4->Integral();
  double scl4 = 100./int4;
  hmiss4->Scale(scl4);
  hmiss4->SetLineWidth(2);
  hmiss4->SetLineColor(kOrange+6);
  hmiss4->Draw("same");

  TString inputCA5=storePath+"/Lumi_missed_ghost_CA_5trks.root";
  TFile *finCA5 = new TFile(inputCA5,"READ");
  TH1 *hmiss5 = (TH1 *)finCA5->Get("hnhitsmissed");
  double int5 = hmiss5->Integral();
  double scl5 = 100./int5;
  hmiss5->Scale(scl5);
  hmiss5->SetLineWidth(2);
  hmiss5->SetLineColor(kOrange+5);
  hmiss5->Draw("same");

  TString inputCA10=storePath+"/Lumi_missed_ghost_CA_10trks.root";
  TFile *finCA10 = new TFile(inputCA10,"READ");
  TH1 *hmiss10 = (TH1 *)finCA10->Get("hnhitsmissed");
  double int10 = hmiss10->Integral();
  double scl10 = 100./int10;
  hmiss10->Scale(scl10);
  hmiss10->SetLineWidth(2);
  hmiss10->SetLineColor(kOrange+9);
  hmiss10->Draw("same");

  TString inputCA15=storePath+"/Lumi_missed_ghost_CA_15trks.root";
  TFile *finCA15 = new TFile(inputCA15,"READ");
  TH1 *hmiss15 = (TH1 *)finCA15->Get("hnhitsmissed");
  double int15 = hmiss15->Integral();
  double scl15 = 100./int15;
  hmiss15->Scale(scl15);
  hmiss15->SetLineWidth(2);
  hmiss15->SetLineColor(kOrange+4);
  hmiss15->Draw("same");

  TString inputCA20=storePath+"/Lumi_missed_ghost_CA_20trks.root";
  TFile *finCA20 = new TFile(inputCA20,"READ");
  TH1 *hmiss20 = (TH1 *)finCA20->Get("hnhitsmissed");
  double int20 = hmiss20->Integral();
  double scl20 = 100./int20;
  hmiss20->Scale(scl20);
  hmiss20->SetLineWidth(2);
  hmiss20->SetLineColor(kOrange+3);
  hmiss20->Draw("same");

  TLegend *leg = new TLegend(0.55,0.45,0.88,0.85);
  leg->SetFillColor(0);
  leg->AddEntry(hmiss1,"1 trk/event","lep");
  leg->AddEntry(hmiss2,"2 trks/event","lep");
  leg->AddEntry(hmiss3,"3 trks/event","lep");
  leg->AddEntry(hmiss4,"4 trks/event","lep");
  leg->AddEntry(hmiss5,"5 trks/event","lep");
  leg->AddEntry(hmiss10,"10 trks/event","lep");
  leg->AddEntry(hmiss15,"15 trks/event","lep");
  leg->AddEntry(hmiss20,"20 trks/event","lep");

  leg->Draw();
  // /// END Missed vs hits ------------------------------------------------------------------------------

    // ///Missed & fake vs phi&theta
    // TString inputCA5=storePath+"/Lumi_missed_ghost_Follow_5trks.root";
    // TFile *finCA5 = new TFile(inputCA5,"READ");
    // TH2 *hthphimc_missed_I = (TH2D *)finCA5->Get("hthphimc_missed_I");
    // TH2 *hthphimc_missed_II = (TH2D *)finCA5->Get("hthphimc_missed_II");
  
    // double int1 = hthphimc_missed_I->Integral();
    // double int2 = hthphimc_missed_II->Integral();
    // double scale1 = 100./int1;
    // double scale2 = 100./int2;
    // hthphimc_missed_I->Scale(scale1);
    // hthphimc_missed_II->Scale(scale2);
   
    // //    hthphimc_missed_I->Draw("colz");
    // hthphimc_missed_II->Draw("colz");

    // // //    TH2 *hthphirec_split = (TH2D *)finCA5->Get("hthphirec_ghost");
    // // TH2 *hthphirec_split = (TH2D *)finCA5->Get("hthphirec_split");
    // // double int3 = hthphirec_split->Integral();
    // // double scale3 = 100./int3;
    // // hthphirec_split->Scale(scale3);
    // // hthphirec_split->Draw();
}
