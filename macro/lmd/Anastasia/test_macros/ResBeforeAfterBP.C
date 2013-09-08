void ResBeforeAfterBP(TString storePath="/panda/pandaroot/macro/lmd/testPixel_newMinuit_BPgeane_FullLMD_160steps_NOpipe_large/mom_1_5/"){
  //style ----------------------------------
  gROOT->Macro("/home/karavdina/Disser/materials/Style_ThesisMini.C");
  gROOT->SetStyle("miniPlots");
  //(end) style---------------------------

  TString storePathOUT=storePath+"/RESULTS/";
  // ---- Input file ----------------------------------------------------------------
  TString in=storePath+"/Lumi_Geane_0.root";
  TFile *fin = new TFile(in,"READ");
  // ---------------------------------------------------------------------------------

 // ---- Output file ----------------------------------------------------------------
  TString out=storePath+"/plotsShiftsRESBP.root";
  TFile *fout = new TFile(out,"RECREATE");
  TString resname_pdf =  storePath+"/plotsShiftsRESBP.pdf";
  TString resname_pdf_o = resname_pdf+"(";
  TString resname_pdf_c = resname_pdf+")";
  // ---------------------------------------------------------------------------------
  TBox dip1(560.,0.,660.,1.);
  dip1.SetFillStyle(3017);
  dip1.SetFillColor(kBlue-6);
  TBox dip2(342.,0.,602.,1.);
  dip2.SetFillStyle(3018);
  dip2.SetFillColor(kBlue-3);
  TBox trans(283,0.,343,1.);
  trans.SetFillStyle(3003);
  trans.SetFillColor(kGreen-5);
  TTree *tbp = (TTree *)fin->Get("tprop"); 
  TString condition = "abs(prec-pmc)<pmc*1e2 && abs(pmclmd-pmc)<pmc*1e2"; //1.5 GeV

  TString conditionLMD = "zmc>1115.";
  TString conditionIP = "zmc<0.01";
  // // 1.5 GeV/c--------------------------
  // double thlim = 0.25;
  // double thmax = 0.9;
  // double phlim_lmd = 10;
  // double phmax_lmd = 30;
  // double phlim_ip = 40;
  // double phmax_ip = 250;
  // // (END) 1.5 GeV/c--------------------------

 // 15 GeV/c--------------------------
  double thlim = 0.1*0.25;
  double thmax = 0.1*0.9;
  double phlim_lmd = 0.1*10;
  double phmax_lmd = 0.1*30;
  double phlim_ip = 0.1*40;
  double phmax_ip = 0.1*250;
  // (END) 15 GeV/c--------------------------

   //theta resolution vs. theta and phi
   TH2D *hresthmcrec_phi_LMD = new TH2D("hresthmcrec_phi_LMD",";#phi, rad; (#theta_{MC} - #theta_{REC}), mrad",1e2,-3.15,3.15,1e2,-6*thmax,6*thmax);
   TH2D *hresthmcrec_phi_IP = new TH2D("hresthmcrec_phi_IP",";#phi, rad; (#theta_{MC} - #theta_{REC}), mrad",1e2,-3.15,3.15,1e2,-6*thmax,6*thmax);
   tbp->Project("hresthmcrec_phi_LMD","(1e3*(thetamc-thetarec)):phiMCip",conditionLMD);
   tbp->Project("hresthmcrec_phi_IP","(1e3*(thetamc-thetarec)):phiMCip",conditionIP);
   TH2D *hresthmclmdrec_phi_LMD = new TH2D("hresthmclmdrec_phi_LMD",";#phi, rad; (#theta_{MCLMD} - #theta_{REC}), mrad",1e2,-3.15,3.15,1e2,-6*thmax,6*thmax);
   TH2D *hresthmclmdrec_phi_IP = new TH2D("hresthmclmdrec_phi_IP",";#phi, rad; (#theta_{MCLMD} - #theta_{REC}), mrad",1e2,-3.15,3.15,1e2,-6*thmax,6*thmax);
   tbp->Project("hresthmclmdrec_phi_LMD","1e3*(thetamclmd-thetarec):phiMCip",conditionLMD);
   tbp->Project("hresthmclmdrec_phi_IP","1e3*(thetamclmd-thetarec):phiMCip",conditionIP);
   TH2D *hresthmcmclmd_phi_LMD = new TH2D("hresthmcmclmd_phi_LMD",";#phi, rad; (#theta_{MCLMD} - #theta_{MC}), mrad",1e2,-3.15,3.15,1e2,-6*thmax,6*thmax);
   TH2D *hresthmcmclmd_phi_IP = new TH2D("hresthmcmclmd_phi_IP",";#phi, rad; (#theta_{MCLMD} - #theta_{MC}), mrad",1e2,-3.15,3.15,1e2,-6*thmax,6*thmax);
   tbp->Project("hresthmcmclmd_phi_LMD","1e3*(thetamclmd-thetamc):phiMCip",conditionLMD);
   tbp->Project("hresthmcmclmd_phi_IP","1e3*(thetamclmd-thetamc):phiMCip",conditionIP);

   TH2D *hresthmcrec_theta_LMD = new TH2D("hresthmcrec_theta_LMD",";#theta, mrad;#theta_{MC} - #theta_{REC}, mrad",5e1,3,9,1e2,-6*thmax,6*thmax);
   TH2D *hresthmcrec_theta_IP = new TH2D("hresthmcrec_theta_IP",";#theta, mrad;#theta_{MC} - #theta_{REC}, mrad",5e1,3,9,1e2,-6*thmax,6*thmax);
   tbp->Project("hresthmcrec_theta_LMD","1e3*(thetamc-thetarec):1e3*thetaMCip",conditionLMD);
   tbp->Project("hresthmcrec_theta_IP","1e3*(thetamc-thetarec):1e3*thetaMCip",conditionIP);
   TH2D *hresthmclmdrec_theta_LMD = new TH2D("hresthmclmdrec_theta_LMD",";#theta, mrad;#theta_{MCLMD} - #theta_{REC}, mrad",5e1,3,9,1e2,-6*thmax,6*thmax);
   TH2D *hresthmclmdrec_theta_IP = new TH2D("hresthmclmdrec_theta_IP",";#theta, mrad;#theta_{MCLMD} - #theta_{REC}, mrad",5e1,3,9,1e2,-6*thmax,6*thmax);
   tbp->Project("hresthmclmdrec_theta_LMD","1e3*(thetamclmd-thetarec):1e3*thetaMCip",conditionLMD);
   tbp->Project("hresthmclmdrec_theta_IP","1e3*(thetamclmd-thetarec):1e3*thetaMCip",conditionIP);
   TH2D *hresthmcmclmd_theta_LMD = new TH2D("hresthmcmclmd_theta_LMD",";#theta, mrad;#theta_{MCLMD} - #theta_{MC}, mrad",5e1,3,9,1e2,-6*thmax,6*thmax);
   TH2D *hresthmcmclmd_theta_IP = new TH2D("hresthmcmclmd_theta_IP",";#theta, mrad;#theta_{MCLMD} - #theta_{MC}, mrad",5e1,3,9,1e2,-6*thmax,6*thmax);
   tbp->Project("hresthmcmclmd_theta_LMD","1e3*(thetamclmd-thetamc):1e3*thetaMCip",conditionLMD);
   tbp->Project("hresthmcmclmd_theta_IP","1e3*(thetamclmd-thetamc):1e3*thetaMCip",conditionIP);
   


   TF1 *f1phiMCREC = new TF1("f1phiMCREC","[0]+[1]*x",-4.,4.);
   f1phiMCREC->SetLineColor(kBlack);
   f1phiMCREC->SetLineWidth(3.0);
   TF1 *f1phiMCLMDREC = new TF1("f1phiMCLMDREC","[0]+[1]*x",-4.,4.);
   f1phiMCLMDREC->SetLineColor(kGreen+2);
   f1phiMCLMDREC->SetLineWidth(3.0);
   TF1 *f1phiMCMCLMD = new TF1("f1phiMCMCLMD","[0]+[1]*x",-4.,4.);
   f1phiMCMCLMD->SetLineColor(kRed+1);
   f1phiMCMCLMD->SetLineWidth(3.0);

   TF1 *f1thetaMCREC = new TF1("f1thetaMCREC","[0]+[1]*x",3.,9.);
   f1thetaMCREC->SetLineColor(kBlack);
   f1thetaMCREC->SetLineWidth(3.0);
   TF1 *f1thetaMCLMDREC = new TF1("f1thetaMCLMDREC","[0]+[1]*x",3.,9.);
   f1thetaMCLMDREC->SetLineColor(kGreen+2);
   f1thetaMCLMDREC->SetLineWidth(3.0);
   TF1 *f1thetaMCMCLMD = new TF1("f1thetaMCMCLMD","[0]+[1]*x",3.,9.);
   f1thetaMCMCLMD->SetLineColor(kRed+1);
   f1thetaMCMCLMD->SetLineWidth(3.0);

   TString nameTHphMeanLMDPDF = storePathOUT+"/Theta_phi_mean_LMD.pdf";
   TString nameTHphMeanLMDROOT = storePathOUT+"/Theta_phi_mean_LMD.root";
   
   hresthmcrec_phi_LMD->SetTitle("#theta_{MC}-#theta_{REC} (near LMD)");
   hresthmcrec_phi_LMD->FitSlicesY();
   hresthmcrec_phi_LMD_1->GetYaxis()->SetTitle("#Delta#theta, mean [mrad]");
   hresthmcrec_phi_LMD_2->GetYaxis()->SetTitle("#Delta#theta, sigma [mrad]");
   hresthmcrec_phi_LMD_1->SetMinimum(-thlim);
   hresthmcrec_phi_LMD_1->SetMaximum(thlim);
   hresthmcrec_phi_LMD_2->SetMinimum(0);
   hresthmcrec_phi_LMD_2->SetMaximum(thmax);
  
   hresthmcrec_phi_LMD_1->Fit(f1phiMCREC,"qr");
   hresthmcrec_phi_LMD_1->SetMarkerSize(3.0);
   hresthmcrec_phi_LMD_1->SetMarkerColor(kBlack);
   hresthmcrec_phi_LMD_1->SetMarkerStyle(24);
   hresthmcrec_phi_LMD_2->SetMarkerSize(3.0);
   hresthmcrec_phi_LMD_2->SetMarkerColor(kBlack);
   hresthmcrec_phi_LMD_2->SetMarkerStyle(24);

   hresthmclmdrec_phi_LMD->SetTitle("#theta_{MCLMD}-#theta_{REC} (near LMD)");
   hresthmclmdrec_phi_LMD->FitSlicesY();
   hresthmclmdrec_phi_LMD_1->SetMinimum(-thlim);
   hresthmclmdrec_phi_LMD_1->SetMaximum(thlim);
   hresthmclmdrec_phi_LMD_2->SetMinimum(0);
   hresthmclmdrec_phi_LMD_2->SetMaximum(thmax);
  
   hresthmclmdrec_phi_LMD_1->Fit(f1phiMCLMDREC,"qr");
   hresthmclmdrec_phi_LMD_1->SetMarkerSize(3.0);
   hresthmclmdrec_phi_LMD_1->SetMarkerColor(kGreen+2);
   hresthmclmdrec_phi_LMD_1->SetMarkerStyle(24);
   hresthmclmdrec_phi_LMD_2->SetMarkerSize(3.0);
   hresthmclmdrec_phi_LMD_2->SetMarkerColor(kGreen+2);
   hresthmclmdrec_phi_LMD_2->SetMarkerStyle(24);
   
   hresthmcmclmd_phi_LMD->SetTitle("#theta_{MCLMD}-#theta_{MC} (near LMD)");
   hresthmcmclmd_phi_LMD->FitSlicesY();
   hresthmcmclmd_phi_LMD_1->SetMinimum(-thlim);
   hresthmcmclmd_phi_LMD_1->SetMaximum(thlim);
   hresthmcmclmd_phi_LMD_2->SetMinimum(0);
   hresthmcmclmd_phi_LMD_2->SetMaximum(thmax);
  
   hresthmcmclmd_phi_LMD_1->Fit(f1phiMCMCLMD,"qr");
   hresthmcmclmd_phi_LMD_1->SetMarkerSize(3.0);
   hresthmcmclmd_phi_LMD_1->SetMarkerColor(kRed+1);
   hresthmcmclmd_phi_LMD_1->SetMarkerStyle(24);
   hresthmcmclmd_phi_LMD_2->SetMarkerSize(3.0);
   hresthmcmclmd_phi_LMD_2->SetMarkerColor(kRed+1);
   hresthmcmclmd_phi_LMD_2->SetMarkerStyle(24);

   TCanvas cTHphmeanLMD;
   hresthmcrec_phi_LMD_1->SetTitle("");
   hresthmcrec_phi_LMD_1->Draw();
   hresthmclmdrec_phi_LMD_1->Draw("same");
   hresthmcmclmd_phi_LMD_1->Draw("same");
   hresthmcrec_phi_LMD_1->Draw("same");
   cTHphmeanLMD.SaveAs(nameTHphMeanLMDPDF);
   cTHphmeanLMD.SaveAs(nameTHphMeanLMDROOT);

   TString nameTHphMeanIPPDF = storePathOUT+"/Theta_phi_mean_IP.pdf";
   TString nameTHphMeanIPROOT = storePathOUT+"/Theta_phi_mean_IP.root";
   
   hresthmcrec_phi_IP->SetTitle("#theta_{MC}-#theta_{REC} (near IP)");
   hresthmcrec_phi_IP->FitSlicesY();
   hresthmcrec_phi_IP_1->GetYaxis()->SetTitle("#Delta#theta, mean [mrad]");
   hresthmcrec_phi_IP_2->GetYaxis()->SetTitle("#Delta#theta, sigma [mrad]");
   hresthmcrec_phi_IP_1->SetMinimum(-thlim);
   hresthmcrec_phi_IP_1->SetMaximum(thlim);
   hresthmcrec_phi_IP_2->SetMinimum(0);
   hresthmcrec_phi_IP_2->SetMaximum(thmax);
  
   hresthmcrec_phi_IP_1->Fit(f1phiMCREC,"qr");
   hresthmcrec_phi_IP_1->SetMarkerSize(3.0);
   hresthmcrec_phi_IP_1->SetMarkerColor(kBlack);
   hresthmcrec_phi_IP_1->SetMarkerStyle(20);
   hresthmcrec_phi_IP_2->SetMarkerSize(3.0);
   hresthmcrec_phi_IP_2->SetMarkerColor(kBlack);
   hresthmcrec_phi_IP_2->SetMarkerStyle(20);

   hresthmclmdrec_phi_IP->SetTitle("#theta_{MCLMD}-#theta_{REC} (near IP)");
   hresthmclmdrec_phi_IP->FitSlicesY();
   hresthmclmdrec_phi_IP_1->SetMinimum(-thlim);
   hresthmclmdrec_phi_IP_1->SetMaximum(thlim);
   hresthmclmdrec_phi_IP_2->SetMinimum(0);
   hresthmclmdrec_phi_IP_2->SetMaximum(thmax);
  
   hresthmclmdrec_phi_IP_1->Fit(f1phiMCLMDREC,"qr");
   hresthmclmdrec_phi_IP_1->SetMarkerSize(3.0);
   hresthmclmdrec_phi_IP_1->SetMarkerColor(kGreen+2);
   hresthmclmdrec_phi_IP_1->SetMarkerStyle(20);
   hresthmclmdrec_phi_IP_2->SetMarkerSize(3.0);
   hresthmclmdrec_phi_IP_2->SetMarkerColor(kGreen+2);
   hresthmclmdrec_phi_IP_2->SetMarkerStyle(20);

   hresthmcmclmd_phi_IP->SetTitle("#theta_{MCLMD}-#theta_{MC} (near IP)");
   hresthmcmclmd_phi_IP->FitSlicesY();
   hresthmcmclmd_phi_IP_1->SetMinimum(-thlim);
   hresthmcmclmd_phi_IP_1->SetMaximum(thlim);
   hresthmcmclmd_phi_IP_2->SetMinimum(0);
   hresthmcmclmd_phi_IP_2->SetMaximum(thmax);
  
   hresthmcmclmd_phi_IP_1->Fit(f1phiMCMCLMD,"qr");
   hresthmcmclmd_phi_IP_1->SetMarkerSize(3.0);
   hresthmcmclmd_phi_IP_1->SetMarkerColor(kRed+1);
   hresthmcmclmd_phi_IP_1->SetMarkerStyle(20);
   hresthmcmclmd_phi_IP_2->SetMarkerSize(3.0);
   hresthmcmclmd_phi_IP_2->SetMarkerColor(kRed+1);
   hresthmcmclmd_phi_IP_2->SetMarkerStyle(20);

   TCanvas cTHphmeanIP;
   hresthmcrec_phi_IP_1->SetTitle("");
   hresthmcrec_phi_IP_1->Draw();
   hresthmclmdrec_phi_IP_1->Draw("same");
   hresthmcmclmd_phi_IP_1->Draw("same");
   hresthmcrec_phi_IP_1->Draw("same");
   cTHphmeanIP.SaveAs(nameTHphMeanIPPDF);
   cTHphmeanIP.SaveAs(nameTHphMeanIPROOT);

   ///-----------------------------------
   TString nameTHthmeanLMDPDF = storePathOUT+"/Theta_theta_mean_LMD.pdf";
   TString nameTHthmeanLMDROOT = storePathOUT+"/Theta_theta_mean_LMD.root";
   
   hresthmcrec_theta_LMD->SetTitle("#theta_{MC}-#theta_{REC} (near LMD)");
   hresthmcrec_theta_LMD->FitSlicesY();
   hresthmcrec_theta_LMD_1->GetYaxis()->SetTitle("#Delta#theta, mean [mrad]");
   hresthmcrec_theta_LMD_2->GetYaxis()->SetTitle("#Delta#theta, sigma [mrad]");
   hresthmcrec_theta_LMD_1->SetMinimum(-thlim);
   hresthmcrec_theta_LMD_1->SetMaximum(thlim);
   hresthmcrec_theta_LMD_2->SetMinimum(0);
   hresthmcrec_theta_LMD_2->SetMaximum(thmax);
  
   hresthmcrec_theta_LMD_1->Fit(f1thetaMCREC,"qr");
   hresthmcrec_theta_LMD_1->SetMarkerSize(3.0);
   hresthmcrec_theta_LMD_1->SetMarkerColor(kBlack);
   hresthmcrec_theta_LMD_1->SetMarkerStyle(24);
  hresthmcrec_theta_LMD_2->SetMarkerSize(3.0);
   hresthmcrec_theta_LMD_2->SetMarkerColor(kBlack);
   hresthmcrec_theta_LMD_2->SetMarkerStyle(24);

   hresthmclmdrec_theta_LMD->SetTitle("#theta_{MCLMD}-#theta_{REC} (near LMD)");
   hresthmclmdrec_theta_LMD->FitSlicesY();
   hresthmclmdrec_theta_LMD_1->SetMinimum(-thlim);
   hresthmclmdrec_theta_LMD_1->SetMaximum(thlim);
   hresthmclmdrec_theta_LMD_2->SetMinimum(0);
   hresthmclmdrec_theta_LMD_2->SetMaximum(thmax);
  
   hresthmclmdrec_theta_LMD_1->Fit(f1thetaMCLMDREC,"qr");
   hresthmclmdrec_theta_LMD_1->SetMarkerSize(3.0);
   hresthmclmdrec_theta_LMD_1->SetMarkerColor(kGreen+2);
   hresthmclmdrec_theta_LMD_1->SetMarkerStyle(24);
   hresthmclmdrec_theta_LMD_2->SetMarkerSize(3.0);
   hresthmclmdrec_theta_LMD_2->SetMarkerColor(kGreen+2);
   hresthmclmdrec_theta_LMD_2->SetMarkerStyle(24);

   hresthmcmclmd_theta_LMD->SetTitle("#theta_{MCLMD}-#theta_{MC} (near LMD)");
   hresthmcmclmd_theta_LMD->FitSlicesY();
   hresthmcmclmd_theta_LMD_1->SetMinimum(-thlim);
   hresthmcmclmd_theta_LMD_1->SetMaximum(thlim);
   hresthmcmclmd_theta_LMD_2->SetMinimum(0);
   hresthmcmclmd_theta_LMD_2->SetMaximum(thmax);
  
   hresthmcmclmd_theta_LMD_1->Fit(f1thetaMCMCLMD,"qr");
   hresthmcmclmd_theta_LMD_1->SetMarkerSize(3.0);
   hresthmcmclmd_theta_LMD_1->SetMarkerColor(kRed+1);
   hresthmcmclmd_theta_LMD_1->SetMarkerStyle(24);
   hresthmcmclmd_theta_LMD_2->SetMarkerSize(3.0);
   hresthmcmclmd_theta_LMD_2->SetMarkerColor(kRed+1);
   hresthmcmclmd_theta_LMD_2->SetMarkerStyle(24);

   TCanvas cTHthmeanLMD;
   hresthmcrec_theta_LMD_1->SetTitle("");
   hresthmcrec_theta_LMD_1->Draw();
   hresthmclmdrec_theta_LMD_1->Draw("same");
   hresthmcmclmd_theta_LMD_1->Draw("same");
   hresthmcrec_theta_LMD_1->Draw("same");
   cTHthmeanLMD.SaveAs(nameTHthmeanLMDPDF);
   cTHthmeanLMD.SaveAs(nameTHthmeanLMDROOT);

   TString nameTHthmeanIPPDF = storePathOUT+"/Theta_theta_mean_IP.pdf";
   TString nameTHthmeanIPROOT = storePathOUT+"/Theta_theta_mean_IP.root";
   
   hresthmcrec_theta_IP->SetTitle("#theta_{MC}-#theta_{REC} (near IP)");
   hresthmcrec_theta_IP->FitSlicesY();
   hresthmcrec_theta_IP_1->GetYaxis()->SetTitle("#Delta#theta, mean [mrad]");
   hresthmcrec_theta_IP_2->GetYaxis()->SetTitle("#Delta#theta, sigma [mrad]");
   hresthmcrec_theta_IP_1->SetMinimum(-thlim);
   hresthmcrec_theta_IP_1->SetMaximum(thlim);
   hresthmcrec_theta_IP_2->SetMinimum(0);
   hresthmcrec_theta_IP_2->SetMaximum(thmax);
  
   hresthmcrec_theta_IP_1->Fit(f1thetaMCREC,"qr");
   hresthmcrec_theta_IP_1->SetMarkerSize(3.0);
   hresthmcrec_theta_IP_1->SetMarkerColor(kBlack);
   hresthmcrec_theta_IP_1->SetMarkerStyle(20); 
   hresthmcrec_theta_IP_2->SetMarkerSize(3.0);
   hresthmcrec_theta_IP_2->SetMarkerColor(kBlack);
   hresthmcrec_theta_IP_2->SetMarkerStyle(20);


   hresthmclmdrec_theta_IP->SetTitle("#theta_{MCLMD}-#theta_{REC} (near IP)");
   hresthmclmdrec_theta_IP->FitSlicesY();
   hresthmclmdrec_theta_IP_1->SetMinimum(-thlim);
   hresthmclmdrec_theta_IP_1->SetMaximum(thlim);
   hresthmclmdrec_theta_IP_2->SetMinimum(0);
   hresthmclmdrec_theta_IP_2->SetMaximum(thmax);
  
   hresthmclmdrec_theta_IP_1->Fit(f1thetaMCLMDREC,"qr");
   hresthmclmdrec_theta_IP_1->SetMarkerSize(3.0);
   hresthmclmdrec_theta_IP_1->SetMarkerColor(kGreen+2);
   hresthmclmdrec_theta_IP_1->SetMarkerStyle(20);
   hresthmclmdrec_theta_IP_2->SetMarkerSize(3.0);
   hresthmclmdrec_theta_IP_2->SetMarkerColor(kGreen+2);
   hresthmclmdrec_theta_IP_2->SetMarkerStyle(20);
   
   hresthmcmclmd_theta_IP->SetTitle("#theta_{MCLMD}-#theta_{MC} (near IP)");
   hresthmcmclmd_theta_IP->FitSlicesY();
   hresthmcmclmd_theta_IP_1->SetMinimum(-thlim);
   hresthmcmclmd_theta_IP_1->SetMaximum(thlim);
   hresthmcmclmd_theta_IP_2->SetMinimum(0);
   hresthmcmclmd_theta_IP_2->SetMaximum(thmax);
  
   hresthmcmclmd_theta_IP_1->Fit(f1thetaMCMCLMD,"qr");
   hresthmcmclmd_theta_IP_1->SetMarkerSize(3.0);
   hresthmcmclmd_theta_IP_1->SetMarkerColor(kRed+1);
   hresthmcmclmd_theta_IP_1->SetMarkerStyle(20);
   hresthmcmclmd_theta_IP_2->SetMarkerSize(3.0);
   hresthmcmclmd_theta_IP_2->SetMarkerColor(kRed+1);
   hresthmcmclmd_theta_IP_2->SetMarkerStyle(20);

   TCanvas cTHthmeanIP;
   hresthmcrec_theta_IP_1->SetTitle("");
   hresthmcrec_theta_IP_1->Draw();
   hresthmclmdrec_theta_IP_1->Draw("same");
   hresthmcmclmd_theta_IP_1->Draw("same");
   hresthmcrec_theta_IP_1->Draw("same");
   cTHthmeanIP.SaveAs(nameTHthmeanIPPDF);
   cTHthmeanIP.SaveAs(nameTHthmeanIPROOT);

 TString nameTHphSigmaLMDPDF = storePathOUT+"/Theta_phi_sigma_LMD.pdf";
   TString nameTHphSigmaLMDROOT = storePathOUT+"/Theta_phi_sigma_LMD.root";
   TCanvas cTHphsigmaLMD;
   hresthmcrec_phi_LMD_2->SetTitle("");
   hresthmcrec_phi_LMD_2->Draw();
   hresthmclmdrec_phi_LMD_2->Draw("same");
   hresthmcmclmd_phi_LMD_2->Draw("same");
   hresthmcrec_phi_LMD_2->Draw("same");
   cTHphsigmaLMD.SaveAs(nameTHphSigmaLMDPDF);
   cTHphsigmaLMD.SaveAs(nameTHphSigmaLMDROOT);

   TString nameTHphSigmaIPPDF = storePathOUT+"/Theta_phi_sigma_IP.pdf";
   TString nameTHphSigmaIPROOT = storePathOUT+"/Theta_phi_sigma_IP.root";
   TCanvas cTHphsigmaIP;
   hresthmcrec_phi_IP_2->SetTitle("");
   hresthmcrec_phi_IP_2->Draw();
   hresthmclmdrec_phi_IP_2->Draw("same");
   hresthmcmclmd_phi_IP_2->Draw("same");
   hresthmcrec_phi_IP_2->Draw("same");
   cTHphsigmaIP.SaveAs(nameTHphSigmaIPPDF);
   cTHphsigmaIP.SaveAs(nameTHphSigmaIPROOT);
   ///-----------------------------------
   TString nameTHthsigmaLMDPDF = storePathOUT+"/Theta_theta_sigma_LMD.pdf";
   TString nameTHthsigmaLMDROOT = storePathOUT+"/Theta_theta_sigma_LMD.root";
   TCanvas cTHthsigmaLMD;
   hresthmcrec_theta_LMD_2->SetTitle("");
   hresthmcrec_theta_LMD_2->Draw();
   hresthmclmdrec_theta_LMD_2->Draw("same");
   hresthmcmclmd_theta_LMD_2->Draw("same");
   hresthmcrec_theta_LMD_2->Draw("same");
   cTHthsigmaLMD.SaveAs(nameTHthsigmaLMDPDF);
   cTHthsigmaLMD.SaveAs(nameTHthsigmaLMDROOT);

   TString nameTHthsigmaIPPDF = storePathOUT+"/Theta_theta_sigma_IP.pdf";
   TString nameTHthsigmaIPROOT = storePathOUT+"/Theta_theta_sigma_IP.root";
   TCanvas cTHthsigmaIP;
   hresthmcrec_theta_IP_2->SetTitle("");
   hresthmcrec_theta_IP_2->Draw();
   hresthmclmdrec_theta_IP_2->Draw("same");
   hresthmcmclmd_theta_IP_2->Draw("same");
   hresthmcrec_theta_IP_2->Draw("same");
   cTHthsigmaIP.SaveAs(nameTHthsigmaIPPDF);
   cTHthsigmaIP.SaveAs(nameTHthsigmaIPROOT);
   ///=======================================
   ///=======================================
  //phi resolution vs. theta and phi
   TH2D *hresphimcrec_phi_LMD = new TH2D("hresphimcrec_phi_LMD",";#phi, rad; (#phi_{MC} - #phi_{REC}), mrad",1e2,-3.15,3.15,1e2,-6*phmax_lmd,6*phmax_lmd);
   TH2D *hresphimcrec_phi_IP = new TH2D("hresphimcrec_phi_IP",";#phi, rad; (#phi_{MC} - #phi_{REC}), mrad",1e2,-3.15,3.15,1e2,-6*phmax_ip,6*phmax_ip);
   tbp->Project("hresphimcrec_phi_LMD","1e3*(phimc-phirec):phiMCip",conditionLMD);
   tbp->Project("hresphimcrec_phi_IP","1e3*(phimc-phirec):phiMCip",conditionIP);
   TH2D *hresphimclmdrec_phi_LMD = new TH2D("hresphimclmdrec_phi_LMD",";#phi, rad; (#phi_{MCLMD} - #phi_{REC}), mrad",1e2,-3.15,3.15,1e2,-6*phmax_lmd,6*phmax_lmd);
   TH2D *hresphimclmdrec_phi_IP = new TH2D("hresphimclmdrec_phi_IP",";#phi, rad; (#phi_{MCLMD} - #phi_{REC}), mrad",1e2,-3.15,3.15,1e2,-6*phmax_ip,6*phmax_ip);
   tbp->Project("hresphimclmdrec_phi_LMD","1e3*(phimclmd-phirec):phiMCip",conditionLMD);
   tbp->Project("hresphimclmdrec_phi_IP","1e3*(phimclmd-phirec):phiMCip",conditionIP);
   TH2D *hresphimcmclmd_phi_LMD = new TH2D("hresphimcmclmd_phi_LMD",";#phi, rad; (#phi_{MCLMD} - #phi_{MC}), mrad",1e2,-3.15,3.15,1e2,-6*phmax_lmd,6*phmax_lmd);
   TH2D *hresphimcmclmd_phi_IP = new TH2D("hresphimcmclmd_phi_IP",";#phi, rad; (#phi_{MCLMD} - #phi_{MC}), mrad",1e2,-3.15,3.15,1e2,-6*phmax_ip,6*phmax_ip);
   tbp->Project("hresphimcmclmd_phi_LMD","1e3*(phimclmd-phimc):phiMCip",conditionLMD);
   tbp->Project("hresphimcmclmd_phi_IP","1e3*(phimclmd-phimc):phiMCip",conditionIP);

   TH2D *hresphimcrec_theta_LMD = new TH2D("hresphimcrec_theta_LMD",";#theta, mrad;#phi_{MC} - #phi_{REC}, mrad",5e1,3,9,1e2,-6*phmax_lmd,6*phmax_lmd);
   TH2D *hresphimcrec_theta_IP = new TH2D("hresphimcrec_theta_IP",";#theta, mrad;#phi_{MC} - #phi_{REC}, mrad",5e1,3,9,1e2,-6*phmax_ip,6*phmax_ip);
   tbp->Project("hresphimcrec_theta_LMD","1e3*(phimc-phirec):1e3*thetaMCip",conditionLMD);
   tbp->Project("hresphimcrec_theta_IP","1e3*(phimc-phirec):1e3*thetaMCip",conditionIP);
   TH2D *hresphimclmdrec_theta_LMD = new TH2D("hresphimclmdrec_theta_LMD",";#theta, mrad;#phi_{MCLMD} - #phi_{REC}, mrad",5e1,3,9,1e2,-6*phmax_lmd,6*phmax_lmd);
   TH2D *hresphimclmdrec_theta_IP = new TH2D("hresphimclmdrec_theta_IP",";#theta, mrad;#phi_{MCLMD} - #phi_{REC}, mrad",5e1,3,9,1e2,-6*phmax_ip,6*phmax_ip);
   tbp->Project("hresphimclmdrec_theta_LMD","1e3*(phimclmd-phirec):1e3*thetaMCip",conditionLMD);
   tbp->Project("hresphimclmdrec_theta_IP","1e3*(phimclmd-phirec):1e3*thetaMCip",conditionIP);
   TH2D *hresphimcmclmd_theta_LMD = new TH2D("hresphimcmclmd_theta_LMD",";#theta, mrad;#phi_{MCLMD} - #phi_{MC}, mrad",5e1,3,9,1e2,-6*phmax_lmd,6*phmax_lmd);
   TH2D *hresphimcmclmd_theta_IP = new TH2D("hresphimcmclmd_theta_IP",";#theta, mrad;#phi_{MCLMD} - #phi_{MC}, mrad",5e1,3,9,1e2,-6*phmax_ip,6*phmax_ip);
   tbp->Project("hresphimcmclmd_theta_LMD","1e3*(phimclmd-phimc):1e3*thetaMCip",conditionLMD);
   tbp->Project("hresphimcmclmd_theta_IP","1e3*(phimclmd-phimc):1e3*thetaMCip",conditionIP);


   TString namePHphMeanLMDPDF = storePathOUT+"/Phi_phi_mean_LMD.pdf";
   TString namePHphMeanLMDROOT = storePathOUT+"/Phi_phi_mean_LMD.root";
   
   hresphimcrec_phi_LMD->SetTitle("#phi_{MC}-#phi_{REC} (near LMD)");
   hresphimcrec_phi_LMD->FitSlicesY();
   hresphimcrec_phi_LMD_1->GetYaxis()->SetTitle("#Delta#phi, mean [mrad]");
   hresphimcrec_phi_LMD_2->GetYaxis()->SetTitle("#Delta#phi, sigma [mrad]");
   hresphimcrec_phi_LMD_1->SetMinimum(-phlim_lmd);
   hresphimcrec_phi_LMD_1->SetMaximum(phlim_lmd);
   hresphimcrec_phi_LMD_2->SetMinimum(0);
   hresphimcrec_phi_LMD_2->SetMaximum(phmax_lmd);
  
   hresphimcrec_phi_LMD_1->Fit(f1phiMCREC,"qr");
   hresphimcrec_phi_LMD_1->SetMarkerSize(3.0);
   hresphimcrec_phi_LMD_1->SetMarkerColor(kBlack);
   hresphimcrec_phi_LMD_1->SetMarkerStyle(24);
   hresphimcrec_phi_LMD_2->SetMarkerSize(3.0);
   hresphimcrec_phi_LMD_2->SetMarkerColor(kBlack);
   hresphimcrec_phi_LMD_2->SetMarkerStyle(24);

   hresphimclmdrec_phi_LMD->SetTitle("#phi_{MCLMD}-#phi_{REC} (near LMD)");
   hresphimclmdrec_phi_LMD->FitSlicesY();
   hresphimclmdrec_phi_LMD_1->SetMinimum(-phlim_lmd);
   hresphimclmdrec_phi_LMD_1->SetMaximum(phlim_lmd);
   hresphimclmdrec_phi_LMD_2->SetMinimum(0);
   hresphimclmdrec_phi_LMD_2->SetMaximum(phmax_lmd);
  
   hresphimclmdrec_phi_LMD_1->Fit(f1phiMCLMDREC,"qr");
   hresphimclmdrec_phi_LMD_1->SetMarkerSize(3.0);
   hresphimclmdrec_phi_LMD_1->SetMarkerColor(kGreen+2);
   hresphimclmdrec_phi_LMD_1->SetMarkerStyle(24);
   hresphimclmdrec_phi_LMD_2->SetMarkerSize(3.0);
   hresphimclmdrec_phi_LMD_2->SetMarkerColor(kGreen+2);
   hresphimclmdrec_phi_LMD_2->SetMarkerStyle(24);

   hresphimcmclmd_phi_LMD->SetTitle("#phi_{MCLMD}-#phi_{MC} (near LMD)");
   hresphimcmclmd_phi_LMD->FitSlicesY();
   hresphimcmclmd_phi_LMD_1->SetMinimum(-phlim_lmd);
   hresphimcmclmd_phi_LMD_1->SetMaximum(phlim_lmd);
   hresphimcmclmd_phi_LMD_2->SetMinimum(0);
   hresphimcmclmd_phi_LMD_2->SetMaximum(phmax_lmd);
  
   hresphimcmclmd_phi_LMD_1->Fit(f1phiMCMCLMD,"qr");
   hresphimcmclmd_phi_LMD_1->SetMarkerSize(3.0);
   hresphimcmclmd_phi_LMD_1->SetMarkerColor(kRed+1);
   hresphimcmclmd_phi_LMD_1->SetMarkerStyle(24);
   hresphimcmclmd_phi_LMD_2->SetMarkerSize(3.0);
   hresphimcmclmd_phi_LMD_2->SetMarkerColor(kRed+1);
   hresphimcmclmd_phi_LMD_2->SetMarkerStyle(24);

   TCanvas cPHphmeanLMD;
   hresphimcrec_phi_LMD_1->SetTitle("");
   hresphimcrec_phi_LMD_1->Draw();
   hresphimclmdrec_phi_LMD_1->Draw("same");
   hresphimcmclmd_phi_LMD_1->Draw("same");
   hresphimcrec_phi_LMD_1->Draw("same");
   cPHphmeanLMD.SaveAs(namePHphMeanLMDPDF);
   cPHphmeanLMD.SaveAs(namePHphMeanLMDROOT);

   TString namePHphMeanIPPDF = storePathOUT+"/Phi_phi_mean_IP.pdf";
   TString namePHphMeanIPROOT = storePathOUT+"/Phi_phi_mean_IP.root";
   
   hresphimcrec_phi_IP->SetTitle("#phi_{MC}-#phi_{REC} (near IP)");
   hresphimcrec_phi_IP->FitSlicesY();
   hresphimcrec_phi_IP_1->GetYaxis()->SetTitle("#Delta#phi, mean [mrad]");
   hresphimcrec_phi_IP_2->GetYaxis()->SetTitle("#Delta#phi, sigma [mrad]");
   hresphimcrec_phi_IP_1->SetMinimum(-phlim_ip);
   hresphimcrec_phi_IP_1->SetMaximum(phlim_ip);
   hresphimcrec_phi_IP_2->SetMinimum(0);
   hresphimcrec_phi_IP_2->SetMaximum(phmax_ip);
  
   hresphimcrec_phi_IP_1->Fit(f1phiMCREC,"qr");
   hresphimcrec_phi_IP_1->SetMarkerSize(3.0);
   hresphimcrec_phi_IP_1->SetMarkerColor(kBlack);
   hresphimcrec_phi_IP_1->SetMarkerStyle(20);
   hresphimcrec_phi_IP_2->SetMarkerSize(3.0);
   hresphimcrec_phi_IP_2->SetMarkerColor(kBlack);
   hresphimcrec_phi_IP_2->SetMarkerStyle(20);

   hresphimclmdrec_phi_IP->SetTitle("#phi_{MCLMD}-#phi_{REC} (near IP)");
   hresphimclmdrec_phi_IP->FitSlicesY();
   hresphimclmdrec_phi_IP_1->SetMinimum(-phlim_ip);
   hresphimclmdrec_phi_IP_1->SetMaximum(phlim_ip);
   hresphimclmdrec_phi_IP_2->SetMinimum(0);
   hresphimclmdrec_phi_IP_2->SetMaximum(phmax_ip);
  
   hresphimclmdrec_phi_IP_1->Fit(f1phiMCLMDREC,"qr");
   hresphimclmdrec_phi_IP_1->SetMarkerSize(3.0);
   hresphimclmdrec_phi_IP_1->SetMarkerColor(kGreen+2);
   hresphimclmdrec_phi_IP_1->SetMarkerStyle(20);
   hresphimclmdrec_phi_IP_2->SetMarkerSize(3.0);
   hresphimclmdrec_phi_IP_2->SetMarkerColor(kGreen+2);
   hresphimclmdrec_phi_IP_2->SetMarkerStyle(20);

   hresphimcmclmd_phi_IP->SetTitle("#phi_{MCLMD}-#phi_{MC} (near IP)");
   hresphimcmclmd_phi_IP->FitSlicesY();
   hresphimcmclmd_phi_IP_1->SetMinimum(-phlim_ip);
   hresphimcmclmd_phi_IP_1->SetMaximum(phlim_ip);
   hresphimcmclmd_phi_IP_2->SetMinimum(0);
   hresphimcmclmd_phi_IP_2->SetMaximum(phmax_ip);
  
   hresphimcmclmd_phi_IP_1->Fit(f1phiMCMCLMD,"qr");
   hresphimcmclmd_phi_IP_1->SetMarkerSize(3.0);
   hresphimcmclmd_phi_IP_1->SetMarkerColor(kRed+1);
   hresphimcmclmd_phi_IP_1->SetMarkerStyle(20);
   hresphimcmclmd_phi_IP_2->SetMarkerSize(3.0);
   hresphimcmclmd_phi_IP_2->SetMarkerColor(kRed+1);
   hresphimcmclmd_phi_IP_2->SetMarkerStyle(20);

   TCanvas cPHphmeanIP;
   hresphimcrec_phi_IP_1->SetTitle("");
   hresphimcrec_phi_IP_1->Draw();
   hresphimclmdrec_phi_IP_1->Draw("same");
   hresphimcmclmd_phi_IP_1->Draw("same");
   hresphimcrec_phi_IP_1->Draw("same");
   cPHphmeanIP.SaveAs(namePHphMeanIPPDF);
   cPHphmeanIP.SaveAs(namePHphMeanIPROOT);

   ///-----------------------------------
   TString namePHthmeanLMDPDF = storePathOUT+"/Phi_theta_mean_LMD.pdf";
   TString namePHthmeanLMDROOT = storePathOUT+"/Phi_theta_mean_LMD.root";
   
   hresphimcrec_theta_LMD->SetTitle("#phi_{MC}-#phi_{REC} (near LMD)");
   hresphimcrec_theta_LMD->FitSlicesY();
   hresphimcrec_theta_LMD_1->GetYaxis()->SetTitle("#Delta#phi, mean [mrad]");
   hresphimcrec_theta_LMD_2->GetYaxis()->SetTitle("#Delta#phi, sigma [mrad]");
   hresphimcrec_theta_LMD_1->SetMinimum(-phlim_lmd);
   hresphimcrec_theta_LMD_1->SetMaximum(phlim_lmd);
   hresphimcrec_theta_LMD_2->SetMinimum(0);
   hresphimcrec_theta_LMD_2->SetMaximum(phmax_lmd);
  
   hresphimcrec_theta_LMD_1->Fit(f1thetaMCREC,"qr");
   hresphimcrec_theta_LMD_1->SetMarkerSize(3.0);
   hresphimcrec_theta_LMD_1->SetMarkerColor(kBlack);
   hresphimcrec_theta_LMD_1->SetMarkerStyle(24);
   hresphimcrec_theta_LMD_2->SetMarkerSize(3.0);
   hresphimcrec_theta_LMD_2->SetMarkerColor(kBlack);
   hresphimcrec_theta_LMD_2->SetMarkerStyle(24);

   hresphimclmdrec_theta_LMD->SetTitle("#phi_{MCLMD}-#phi_{REC} (near LMD)");
   hresphimclmdrec_theta_LMD->FitSlicesY();
   hresphimclmdrec_theta_LMD_1->SetMinimum(-phlim_lmd);
   hresphimclmdrec_theta_LMD_1->SetMaximum(phlim_lmd);
   hresphimclmdrec_theta_LMD_2->SetMinimum(0);
   hresphimclmdrec_theta_LMD_2->SetMaximum(phmax_lmd);
  
   hresphimclmdrec_theta_LMD_1->Fit(f1thetaMCLMDREC,"qr");
   hresphimclmdrec_theta_LMD_1->SetMarkerSize(3.0);
   hresphimclmdrec_theta_LMD_1->SetMarkerColor(kGreen+2);
   hresphimclmdrec_theta_LMD_1->SetMarkerStyle(24);
   hresphimclmdrec_theta_LMD_2->SetMarkerSize(3.0);
   hresphimclmdrec_theta_LMD_2->SetMarkerColor(kGreen+2);
   hresphimclmdrec_theta_LMD_2->SetMarkerStyle(24);

   hresphimcmclmd_theta_LMD->SetTitle("#phi_{MCLMD}-#phi_{MC} (near LMD)");
   hresphimcmclmd_theta_LMD->FitSlicesY();
   hresphimcmclmd_theta_LMD_1->SetMinimum(-phlim_lmd);
   hresphimcmclmd_theta_LMD_1->SetMaximum(phlim_lmd);
   hresphimcmclmd_theta_LMD_2->SetMinimum(0);
   hresphimcmclmd_theta_LMD_2->SetMaximum(phmax_lmd);
  
   hresphimcmclmd_theta_LMD_1->Fit(f1thetaMCMCLMD,"qr");
   hresphimcmclmd_theta_LMD_1->SetMarkerSize(3.0);
   hresphimcmclmd_theta_LMD_1->SetMarkerColor(kRed+1);
   hresphimcmclmd_theta_LMD_1->SetMarkerStyle(24);
   hresphimcmclmd_theta_LMD_2->SetMarkerSize(3.0);
   hresphimcmclmd_theta_LMD_2->SetMarkerColor(kRed+1);
   hresphimcmclmd_theta_LMD_2->SetMarkerStyle(24);

   TCanvas cPHthmeanLMD;
   hresphimcrec_theta_LMD_1->SetTitle("");
   hresphimcrec_theta_LMD_1->Draw();
   hresphimclmdrec_theta_LMD_1->Draw("same");
   hresphimcmclmd_theta_LMD_1->Draw("same");
   hresphimcrec_theta_LMD_1->Draw("same");
   cPHthmeanLMD.SaveAs(namePHthmeanLMDPDF);
   cPHthmeanLMD.SaveAs(namePHthmeanLMDROOT);

   TString namePHthmeanIPPDF = storePathOUT+"/Phi_theta_mean_IP.pdf";
   TString namePHthmeanIPROOT = storePathOUT+"/Phi_theta_mean_IP.root";
   
   hresphimcrec_theta_IP->SetTitle("#phi_{MC}-#phi_{REC} (near IP)");
   hresphimcrec_theta_IP->FitSlicesY();
   hresphimcrec_theta_IP_1->GetYaxis()->SetTitle("#Delta#phi, mean [mrad]");
   hresphimcrec_theta_IP_2->GetYaxis()->SetTitle("#Delta#phi, sigma [mrad]");
   hresphimcrec_theta_IP_1->SetMinimum(-phlim_ip);
   hresphimcrec_theta_IP_1->SetMaximum(phlim_ip);
   hresphimcrec_theta_IP_2->SetMinimum(0);
   hresphimcrec_theta_IP_2->SetMaximum(phmax_ip);
  
   hresphimcrec_theta_IP_1->Fit(f1thetaMCREC,"qr");
   hresphimcrec_theta_IP_1->SetMarkerSize(3.0);
   hresphimcrec_theta_IP_1->SetMarkerColor(kBlack);
   hresphimcrec_theta_IP_1->SetMarkerStyle(20);
   hresphimcrec_theta_IP_2->SetMarkerSize(3.0);
   hresphimcrec_theta_IP_2->SetMarkerColor(kBlack);
   hresphimcrec_theta_IP_2->SetMarkerStyle(20);

   hresphimclmdrec_theta_IP->SetTitle("#phi_{MCLMD}-#phi_{REC} (near IP)");
   hresphimclmdrec_theta_IP->FitSlicesY();
   hresphimclmdrec_theta_IP_1->SetMinimum(-phlim_ip);
   hresphimclmdrec_theta_IP_1->SetMaximum(phlim_ip);
   hresphimclmdrec_theta_IP_2->SetMinimum(0);
   hresphimclmdrec_theta_IP_2->SetMaximum(phmax_ip);
  
   hresphimclmdrec_theta_IP_1->Fit(f1thetaMCLMDREC,"qr");
   hresphimclmdrec_theta_IP_1->SetMarkerSize(3.0);
   hresphimclmdrec_theta_IP_1->SetMarkerColor(kGreen+2);
   hresphimclmdrec_theta_IP_1->SetMarkerStyle(20);
   hresphimclmdrec_theta_IP_2->SetMarkerSize(3.0);
   hresphimclmdrec_theta_IP_2->SetMarkerColor(kGreen+2);
   hresphimclmdrec_theta_IP_2->SetMarkerStyle(20);
   
   hresphimcmclmd_theta_IP->SetTitle("#phi_{MCLMD}-#phi_{MC} (near IP)");
   hresphimcmclmd_theta_IP->FitSlicesY();
   hresphimcmclmd_theta_IP_1->SetMinimum(-phlim_ip);
   hresphimcmclmd_theta_IP_1->SetMaximum(phlim_ip);
   hresphimcmclmd_theta_IP_2->SetMinimum(0);
   hresphimcmclmd_theta_IP_2->SetMaximum(phmax_ip);
  
   hresphimcmclmd_theta_IP_1->Fit(f1thetaMCMCLMD,"qr");
   hresphimcmclmd_theta_IP_1->SetMarkerSize(3.0);
   hresphimcmclmd_theta_IP_1->SetMarkerColor(kRed+1);
   hresphimcmclmd_theta_IP_1->SetMarkerStyle(20);
   hresphimcmclmd_theta_IP_2->SetMarkerSize(3.0);
   hresphimcmclmd_theta_IP_2->SetMarkerColor(kRed+1);
   hresphimcmclmd_theta_IP_2->SetMarkerStyle(20);
   TCanvas cPHthmeanIP;
   hresphimcrec_theta_IP_1->SetTitle("");
   hresphimcrec_theta_IP_1->Draw();
   hresphimclmdrec_theta_IP_1->Draw("same");
   hresphimcmclmd_theta_IP_1->Draw("same");
   hresphimcrec_theta_IP_1->Draw("same");
   cPHthmeanIP.SaveAs(namePHthmeanIPPDF);
   cPHthmeanIP.SaveAs(namePHthmeanIPROOT);

   //SIGMAS! ########################################
  
 
   TString namePHphSigmaLMDPDF = storePathOUT+"/Phi_phi_sigma_LMD.pdf";
   TString namePHphSigmaLMDROOT = storePathOUT+"/Phi_phi_sigma_LMD.root";
   TCanvas cPHphsigmaLMD;
   hresphimcrec_phi_LMD_2->SetTitle("");
   hresphimcrec_phi_LMD_2->Draw();
   hresphimclmdrec_phi_LMD_2->Draw("same");
   hresphimcmclmd_phi_LMD_2->Draw("same");
   hresphimcrec_phi_LMD_2->Draw("same");
   cPHphsigmaLMD.SaveAs(namePHphSigmaLMDPDF);
   cPHphsigmaLMD.SaveAs(namePHphSigmaLMDROOT);

   TString namePHphSigmaIPPDF = storePathOUT+"/Phi_phi_sigma_IP.pdf";
   TString namePHphSigmaIPROOT = storePathOUT+"/Phi_phi_sigma_IP.root";
   TCanvas cPHphsigmaIP;
   hresphimcrec_phi_IP_2->SetTitle("");
   hresphimcrec_phi_IP_2->Draw();
   hresphimclmdrec_phi_IP_2->Draw("same");
   hresphimcmclmd_phi_IP_2->Draw("same");
   hresphimcrec_phi_IP_2->Draw("same");
   cPHphsigmaIP.SaveAs(namePHphSigmaIPPDF);
   cPHphsigmaIP.SaveAs(namePHphSigmaIPROOT);
   ///-----------------------------------
   TString namePHthsigmaLMDPDF = storePathOUT+"/Phi_theta_sigma_LMD.pdf";
   TString namePHthsigmaLMDROOT = storePathOUT+"/Phi_theta_sigma_LMD.root";
   TCanvas cPHthsigmaLMD;
   hresphimcrec_theta_LMD_2->SetTitle("");
   hresphimcrec_theta_LMD_2->Draw();
   hresphimclmdrec_theta_LMD_2->Draw("same");
   hresphimcmclmd_theta_LMD_2->Draw("same");
   hresphimcrec_theta_LMD_2->Draw("same");
   cPHthsigmaLMD.SaveAs(namePHthsigmaLMDPDF);
   cPHthsigmaLMD.SaveAs(namePHthsigmaLMDROOT);

   TString namePHthsigmaIPPDF = storePathOUT+"/Phi_theta_sigma_IP.pdf";
   TString namePHthsigmaIPROOT = storePathOUT+"/Phi_theta_sigma_IP.root";
   TCanvas cPHthsigmaIP;
   hresphimcrec_theta_IP_2->SetTitle("");
   hresphimcrec_theta_IP_2->Draw();
   hresphimclmdrec_theta_IP_2->Draw("same");
   hresphimcmclmd_theta_IP_2->Draw("same");
   hresphimcrec_theta_IP_2->Draw("same");
   cPHthsigmaIP.SaveAs(namePHthsigmaIPPDF);
   cPHthsigmaIP.SaveAs(namePHthsigmaIPROOT);
  fout->Close();


  

}
