void XYcutSet(){
  const int nEv = 1e8;

  // //Let's immulate X&Y cuts --------------------------------------------------------
  // //double Xref = -19.1+1.12*1e3*MomRecLMD.Theta();
  // //double diffX = abs(fFittedTrkP.GetX() - Xref);
  // // if(diffX>1.5) dirOK=false;
  // //double Yref = -0.00651+0.045*1e3*MomRecLMD.Phi() + fdY;
  // //double diffY = abs(fFittedTrkP.GetY() - Yref);
  // //if(diffY>2.1) dirOK=false;

  // TString condX = "abs(LMDTrackQ.fXrecLMD-(1.121*1e3*LMDTrackQ.fThetarecLMD-19.14))<1.5";
  // TString condY = "abs(LMDTrackQ.fYrecLMD-(0.04468*1e3*LMDTrackQ.fPhirecLMD+0.04149))<2.1";
  // TString condXY = condX;
  // condXY += " && ";
  // condXY += condY;
  // //---------------------------------------------------------------------------------------

  // //Let's immulate M cut ------------------------------------------------------------
  // TString condM = condXY+" && abs(LMDTrackQ.fMomrec-1.5)<1e-2 ";
  // //---------------------------------------------------------------------------------------

  TString condition = "LMDTrackQ.fSecondary<0 && LMDTrackQ.fTrkRecStatus==0";
  //TString condition = condXY;
  // TString condition = condM;
  //   condition += " && LMDTrackQ.fSecondary<0 && LMDTrackQ.fTrkRecStatus==0";

  //create reference from BOX -----------------------------------------------------
  // TString pathBOX = "/panda/myResults/BOXoutpitMarch2014_pointBeam/mom_1_5/Lumi_TrksQA_pointBeam_NOcut.root";
  // TString pathBOX = "/panda/myResults/BOXoutpitMarch2014_BeamProf/mom_1_5/Lumi_TrksQA_BeamShape_NOcut.root";
 TString pathBOX = "/panda/myResults/BOXoutpitMarch2014_pointBeam/mom_15/Lumi_TrksQA_pointBeam_noCuts_BOX.root";
  TFile *finBOX = new TFile(pathBOX.Data(),"READ");
  TTree *treeBOX = (TTree*)finBOX->Get("cbmsim");
  //  TH2 *hxthlmd = new TH2D("hxthlmd",";#hat{#theta_{REC}}, mrad;X_{REC}, cm",1e2,30,50,2e2,14,36);
   int numAllEv = treeBOX->Project("hxthlmd","LMDTrackQ.fXrecLMD:1e3*LMDTrackQ.fThetarecLMD",condition,"",nEv);
  // hxthlmd->FitSlicesY();
  // hxthlmd_1->SetMinimum(14);
  // hxthlmd_1->SetMaximum(36);
  // hxthlmd_1->GetYaxis()->SetTitle("X_{REC}, cm");
  // hxthlmd_1->SetMarkerColor(2);
  // hxthlmd_1->SetMarkerStyle(20);
  // hxthlmd_2->SetMinimum(0);
  // hxthlmd_2->SetMaximum(1);
  // hxthlmd_2->GetYaxis()->SetTitle("#sigma(X_{REC}), cm");
  // hxthlmd_2->SetMarkerColor(2);
  // hxthlmd_2->SetMarkerStyle(20);

  // TH1 *hxthlmdref1 = (TH1F*)hxthlmd_1->Clone("hxthlmdref");
  // TH1 *hxthlmdref2 = (TH1F*)hxthlmd_2->Clone("hxthlmdref");
   //   TString condition3sig  = condition+" &&  1e3*abs(LMDTrackQ.fThetamc-LMDTrackQ.fThetarec)<2.1";//1.5 GeV
   TString condition3sig  = condition+" &&  1e3*abs(LMDTrackQ.fThetamc-LMDTrackQ.fThetarec)<0.3";//15 GeV
  // TH2 *hxthlmdneu = new TH2D("hxthlmdneu",";#hat{#theta_{REC}}, mrad;X_{REC}, cm",1e2,30,50,2e2,14,36);
  int numGoodEv = treeBOX->Project("hxthlmdneu","LMDTrackQ.fXrecLMD:1e3*LMDTrackQ.fThetarecLMD",condition3sig,"",nEv);
  // hxthlmdneu->FitSlicesY();
  // hxthlmdneu_1->SetMinimum(14);
  // hxthlmdneu_1->SetMaximum(36);
  // hxthlmdneu_1->GetYaxis()->SetTitle("X_{REC}, cm");
  // hxthlmdneu_1->SetMarkerColor(4);
  // hxthlmdneu_1->SetMarkerStyle(21);
  // hxthlmdneu_2->SetMinimum(0);
  // hxthlmdneu_2->SetMaximum(1);
  // hxthlmdneu_2->GetYaxis()->SetTitle("#sigma(X_{REC}), cm");
  // hxthlmdneu_2->SetMarkerColor(4);
  // hxthlmdneu_2->SetMarkerStyle(21);
  // TH1 *hxthlmdneuref1 = (TH1F*)hxthlmdneu_1->Clone("hxthlmdneuref");
  // TH1 *hxthlmdneuref2 = (TH1F*)hxthlmdneu_2->Clone("hxthlmdneuref");
  // hxthlmdneuref1->Add(hxthlmdref1,-1);
  // hxthlmdneuref1->GetYaxis()->SetTitle("(X^{3#sigma}_{REC}-X^{all}_{REC}), cm");
  // TCanvas c2;
  // hxthlmdneuref1->Draw();
  // c2.SaveAs("Xth_new_old_BOX_1_5GeV_beamShape.root");

 //  ///////////////////////
 // TH2 *hyphlmd = new TH2D("hyphlmd",";#hat{#phi_{REC}}, mrad;Y_{REC}, cm",1e1,-11,11,2e2,-210,210);
 //  treeBOX->Project("hyphlmd","LMDTrackQ.fYrecLMD:1e3*LMDTrackQ.fPhirecLMD",condition,"",nEv);
 //  hyphlmd->FitSlicesY();
 //  hyphlmd_1->SetMinimum(-11);
 //  hyphlmd_1->SetMaximum(11);
 //  hyphlmd_1->GetYaxis()->SetTitle("X_{REC}, cm");
 //  hyphlmd_1->SetMarkerColor(2);
 //  hyphlmd_1->SetMarkerStyle(20);
 //  hyphlmd_2->SetMinimum(0);
 //  hyphlmd_2->SetMaximum(1);
 //  hyphlmd_2->GetYaxis()->SetTitle("#sigma(X_{REC}), cm");
 //  hyphlmd_2->SetMarkerColor(2);
 //  hyphlmd_2->SetMarkerStyle(20);

 //  TH1 *hyphlmdref1 = (TH1F*)hyphlmd_1->Clone("hyphlmdref");
 //  TH1 *hyphlmdref2 = (TH1F*)hyphlmd_2->Clone("hyphlmdref");
 //  TString condition3sig  = condition+" &&  1e3*abs(LMDTrackQ.fThetamc-LMDTrackQ.fThetarec)<2.1";
 // TH2 *hyphlmdneu = new TH2D("hyphlmdneu",";#hat{#theta_{REC}}, mrad;X_{REC}, cm",1e1,-11,11,2e2,-210,210);
 //  treeBOX->Project("hyphlmdneu","LMDTrackQ.fYrecLMD:1e3*LMDTrackQ.fPhirecLMD",condition3sig,"",nEv);
 //  hyphlmdneu->FitSlicesY();
 //  hyphlmdneu_1->SetMinimum(-11);
 //  hyphlmdneu_1->SetMaximum(11);
 //  hyphlmdneu_1->GetYaxis()->SetTitle("X_{REC}, cm");
 //  hyphlmdneu_1->SetMarkerColor(4);
 //  hyphlmdneu_1->SetMarkerStyle(21);
 //  hyphlmdneu_2->SetMinimum(0);
 //  hyphlmdneu_2->SetMaximum(1);
 //  hyphlmdneu_2->GetYaxis()->SetTitle("#sigma(X_{REC}), cm");
 //  hyphlmdneu_2->SetMarkerColor(4);
 //  hyphlmdneu_2->SetMarkerStyle(21);
 //  TH1 *hyphlmdneuref1 = (TH1F*)hyphlmdneu_1->Clone("hyphlmdneuref");
 //  TH1 *hyphlmdneuref2 = (TH1F*)hyphlmdneu_2->Clone("hyphlmdneuref");
 //  hyphlmdneuref1->Add(hyphlmdref1,-1);
 //  hyphlmdneuref1->GetYaxis()->SetTitle("(Y^{3#sigma}_{REC}-Y^{all}_{REC}), cm");
 //  TCanvas c1;
 //  hyphlmdneuref1->Draw();
 //  c1.SaveAs("Yph_new_old_BOX_1_5GeV_beamShape.root");

  //////////////////////////
  // Let's play with cuts
  double xwd = 0.5;
  double ywd = 0.7;
  double eff[10][10];
  double noise[10][10];
  TGraph2D *effgr = new TGraph2D();
  TGraph2D *noisegr = new TGraph2D();
  for(int ix=0;ix<10;ix++){
    for(int iy=0;iy<10;iy++){
      TString condX = " && abs(LMDTrackQ.fXrecLMD-(1.121*1e3*LMDTrackQ.fThetarecLMD-19.14))<";
      double xcur = xwd*(ix+1);
      condX +=xcur;
      TString condY = " && abs(LMDTrackQ.fYrecLMD-(0.04468*1e3*LMDTrackQ.fPhirecLMD+0.04149))<";
      double ycur = ywd*(iy+1);
      condY +=ycur;
      TString condXY = condX+condY;
      TString conditiontest  = condition+condXY;
      TString conditiontest3sig  = condition3sig+condXY;
      TH2 *hthrecmc = new TH2D("hthrecmc",";#theta_{MC},mrad;#theta_{REC},mrad",1e2,0,20,1e2,0,20);
      int numTestEv3sig = treeBOX->Project("hxthlmdneu","1e3*LMDTrackQ.fThetarec:1e3*LMDTrackQ.fThetamc",conditiontest3sig,"",nEv);
      int numTestEv = treeBOX->Project("hxthlmdneu","1e3*LMDTrackQ.fThetarec:1e3*LMDTrackQ.fThetamc",conditiontest,"",nEv);
      cout<<"numTestEv = "<<numTestEv<<" numGoodEv = "<<numGoodEv<<endl;
      eff[ix][iy] = 100.*double(numTestEv3sig)/double(numGoodEv);
      noise[ix][iy] = 100.*double(numTestEv-numTestEv3sig)/double(numGoodEv);
      int in = ix*10+iy;
      effgr->SetPoint(in,xcur,ycur,eff[ix][iy]);
      noisegr->SetPoint(in,xcur,ycur,noise[ix][iy]);
      //      cout<<"eff = "<<eff<<" noise = "<<noise<<endl;
    }
  }
  TCanvas c3;
  effgr->GetXaxis()->SetTitle("X width, cm");
  effgr->GetYaxis()->SetTitle("Y width, cm");
  effgr->GetZaxis()->SetTitle("Eff for 4#sigma trks, %");
  effgr->Draw("surf1");
  c3.SaveAs("eff3D_old_BOX_15GeV_PointBeam.root");
  TCanvas c4;
  noisegr->Draw("surf1");
  c4.SaveAs("noise3D_old_BOX_15GeV_PointBeam.root");
  ///////////////////////////


}
