void DPMvsBOXbehaviour(){
  const int nEv = 1e8;

  //Let's immulate X&Y cuts --------------------------------------------------------
  //double Xref = -19.1+1.12*1e3*MomRecLMD.Theta();
  //double diffX = abs(fFittedTrkP.GetX() - Xref);
  // if(diffX>1.5) dirOK=false;
  //double Yref = -0.00651+0.045*1e3*MomRecLMD.Phi() + fdY;
  //double diffY = abs(fFittedTrkP.GetY() - Yref);
  //if(diffY>2.1) dirOK=false;

  TString condX = "abs(LMDTrackQ.fXrecLMD-(1.121*1e3*LMDTrackQ.fThetarecLMD-19.14))<1.5";
  TString condY = "abs(LMDTrackQ.fYrecLMD-(0.04468*1e3*LMDTrackQ.fPhirecLMD+0.04149))<2.1";
  TString condXY = condX;
  condXY += " && ";
  condXY += condY;
  //---------------------------------------------------------------------------------------

  //Let's immulate M cut ------------------------------------------------------------
  TString condM = condXY+" && abs(LMDTrackQ.fMomrec-1.5)<1e-2 ";
  //---------------------------------------------------------------------------------------

  // TString condition = "LMDTrackQ.fSecondary<0 && LMDTrackQ.fTrkRecStatus==0";
  //TString condition = condXY;
  TString condition = condM;
  condition += " && LMDTrackQ.fSecondary<0 && LMDTrackQ.fTrkRecStatus==0";

  //create reference from BOX -----------------------------------------------------
  TString pathBOX = "/panda/myResults/BOXoutpitMarch2014_pointBeam/mom_1_5/Lumi_TrksQA_pointBeam_NOcut.root";
  TFile *finBOX = new TFile(pathBOX.Data(),"READ");
  TTree *treeBOX = (TTree*)finBOX->Get("cbmsim");
  TH2 *hthbox = new TH2D("hthbox",";#theta_{MC}, mrad; #theta_{REC}, mrad",2e2,0,20,4e2,0,20);
  treeBOX->Project("hthbox","1e3*LMDTrackQ.fThetarec:1e3*LMDTrackQ.fThetamc",condition,"",nEv);
  TCanvas c2;
  hthbox->Draw("colz");
  c2.SaveAs("thREC_vs_thMC_BOX.root");
  TH2 *hdthbox = new TH2D("hdthbox",";#theta_{REC}, mrad; (#theta_{MC}-#theta_{REC}), mrad",2e2,0,20,4e2,-20,20);
  treeBOX->Project("hdthbox","1e3*(LMDTrackQ.fThetamc-LMDTrackQ.fThetarec):1e3*LMDTrackQ.fThetarec",condition,"",nEv);
  //  hdthbox->Draw("colz");
  hdthbox->FitSlicesY();
  hdthbox_1->SetMinimum(-10);
  hdthbox_1->SetMaximum(10);
  hdthbox_1->GetYaxis()->SetTitle("Mean (#theta_{MC}-#theta_{REC}), mrad");
  hdthbox_1->SetMarkerColor(2);
  hdthbox_1->SetMarkerStyle(20);
  TH1 *hrefbox = (TH1F*)hdthbox_1->Clone("hrefbox");
  // hdthbox_1->Draw();
  //---------------------------------------------------------------------------------------

 //and now let's work with DPM data. a lot! ------------------------------------
  TString pathDPM = "/panda/myResults/DPMoutputMarch2014_pointBeam/mom_1_5/Lumi_TrksQA_pointBeam.root";
  TFile *finDPM = new TFile(pathDPM.Data(),"READ");
  TTree *treeDPM = (TTree*)finDPM->Get("cbmsim");
  TH2 *hrefdthdpm = new TH2D("hrefdthdpm",";#theta_{REC}, mrad; (#theta_{MC}-#theta_{REC}), mrad",2e2,0,20,4e2,-20,20);
  treeDPM->Project("hrefdthdpm","1e3*(LMDTrackQ.fThetamc-LMDTrackQ.fThetarec):1e3*LMDTrackQ.fThetarec",condition,"",nEv);
  TCanvas c3;
  hrefdthdpm->Draw("colz");
  c3.SaveAs("deltaTh_dpm.root");

  //  hdthdpm->Draw("colz");
  hrefdthdpm->FitSlicesY();
  hrefdthdpm_1->Add(hrefbox,-1);
  hrefdthdpm_1->SetMinimum(-2);
  hrefdthdpm_1->SetMaximum(2);
  hrefdthdpm_1->GetYaxis()->SetTitle("#delta[Mean (#theta_{MC}-#theta_{REC})], mrad");
  hrefdthdpm_1->SetMarkerColor(4);
  hrefdthdpm_1->SetMarkerStyle(20);
  TCanvas c1;
  hrefdthdpm_1->Draw();

  const int nstep = 0;
  TH2 *hdthdpm[nstep];   
  TH1 *hdthdpm_1[nstep];
  TLegend *leg = new TLegend(0.75,0.7,0.95,0.95);
  leg->SetFillColor(0);
  leg->SetHeader("Condition:");
  leg->AddEntry(hrefdthdpm_1,"all","lep");
 
  for(int i=1;i<nstep;i++){
    double dth_width = 0.7*i;
    TString conditioncur = condition;
    conditioncur +=  " && 1e3*abs(LMDTrackQ.fThetamc-LMDTrackQ.fThetarec)<";
    conditioncur += dth_width;
    TString outcond = "|#theta_{MC}-#theta_{REC}|<";
    outcond +=i;
    outcond +="*#sigma_{#theta}";
    TString namehist = "hdthdpm";
    namehist +=i;
    hdthdpm[i] = new TH2D(namehist,";#theta_{REC}, mrad; (#theta_{MC}-#theta_{REC}), mrad",2e2,0,20,4e2,-20,20);
    treeDPM->Project(namehist,"1e3*(LMDTrackQ.fThetamc-LMDTrackQ.fThetarec):1e3*LMDTrackQ.fThetarec",conditioncur,"",nEv);
    hdthdpm[i]->FitSlicesY();
    TString namehist_1 = namehist;
    namehist_1 += "_1";
    hdthdpm_1[i] = (TH1D*)gDirectory->Get(namehist_1);
    hdthdpm_1[i]->Add(hrefbox,-1);
    hdthdpm_1[i]->SetMinimum(-2);
    hdthdpm_1[i]->SetMaximum(2);
    hdthdpm_1[i]->GetYaxis()->SetTitle("#delta[Mean (#theta_{MC}-#theta_{REC})], mrad");
    hdthdpm_1[i]->SetMarkerColor(4+i);
    hdthdpm_1[i]->SetMarkerStyle(20);
    hdthdpm_1[i]->Draw("same");
    leg->AddEntry(hdthdpm_1[i],outcond,"lep");
  }
  leg->Draw();
  //---------------------------------------------------------------------------------------
  c1.SaveAs("deltaTh_dpm_box.root");
}
