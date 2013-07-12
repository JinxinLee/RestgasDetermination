//void ShiftsBP(TString storePath="/panda/pandaroot/macro/lmd/testPixel_sensorsOnlyLMD_4_8_mrad_KalmanGEANE_Geant3_20_bendAngleForGEANE_NewMapsSolenoid_ERROR_PROP/mom_1_5/"){
void ShiftsBP(TString storePath="/panda/pandaroot/macro/lmd/testPixel_sensorsOnlyLMD_4_8_mrad_0_360_degree_KalmanGEANE_Geant3_20_bendAngleForGEANE_OldMapsSolenoid_ERROR_PROP/mom_15/"){

  //style ----------------------------------
  gROOT->Macro("/home/karavdina/Disser/materials/Style_Thesis.C");
  gROOT->SetStyle("Thesis");
  //(end) style---------------------------

  // ---- Input file ----------------------------------------------------------------
  TString in=storePath+"/Lumi_Geane_0.root";
  TFile *fin = new TFile(in,"READ");
  // ---------------------------------------------------------------------------------

 // ---- Output file ----------------------------------------------------------------
  TString out=storePath+"/plotsShiftsBP_vwOnly.root";
  TFile *fout = new TFile(out,"RECREATE");
  TString resname_pdf =  storePath+"/plotsShiftsBP_vwOnly.pdf";
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
  TString condition = "abs(prec-pmc)<pmc*1e-5 && abs(zmc-zrec)<5e-4 && abs(zmclmd-zmc)<5e-4 && tvrec_err<0.1 && wrec_err<1."; //15 GeV
  //  double thlim = 0.01;//15GeV
  double thlim = 0.04;//15GeV
  double phlim = 0.02;//15GeV
  double xlim = 0.02;//15GeV
  double thresmax=0.2;//15GeV
  double xresmax=0.1;//15GeV
  double momlim=0.5;//15GeV
  double momresmax=1.;//15GeV

 //  double thlim = 0.2;//1.5GeV
 //  double phlim = 0.05;//1.5GeV
 //  double xlim = 0.1;//1.5GeV
 //  double thresmax=1.5;//1.5GeV
 //  double xresmax=0.7;//1.5GeV
 //  double momlim=1.;//1.5GeV
 //  double momresmax=1.;//15GeV
  //MC trk from IP vs. REC -----------------------------------------------------------------------------------------------------------------------
  //  TH2D *hthz = new TH2D("hthz",";z_{MC}, cm;#theta_{MC}-#theta_{REC}, mrad",45.,0,1200,1e3,-5.,5.);//1.5
  TH2D *hthz = new TH2D("hthz",";z_{MC}, cm;#theta_{MC}-#theta_{REC}, mrad",45.,0,1200,3e3,-0.8,0.8);//15
  tbp->Project("hthz","1e3*(thetamc-thetarec):zmc",condition);
  hthz->FitSlicesY();
  // hthz_1->SetMinimum(-0.2);
  // hthz_1->SetMaximum(0.2);
  hthz_1->SetMinimum(-thlim);
  hthz_1->SetMaximum(thlim);

  hthz_1->GetYaxis()->SetTitle("#theta_{MC}-#theta_{REC}, mean [mrad]");
  TCanvas c1;
  c1.Divide(2,2);
  c1.cd(1);
  hthz_1->SetTitle("REC vs. MC (near IP)");
  hthz_2->SetTitle("");
  hthz_chi2->SetTitle("");
   hthz_1->Draw();
  //  dip1.SetY1(-0.2);
  // dip1.SetY2(0.2);
   dip1.SetY1(-thlim);
   dip1.SetY2(thlim);
  
  dip1.Draw();
  // dip2.SetY1(-0.2);
  // dip2.SetY2(0.2);
  dip2.SetY1(-thlim);
  dip2.SetY2(thlim);
  dip2.Draw();
  // trans.SetY1(-0.2);
  // trans.SetY2(0.2);
  trans.SetY1(-thlim);
  trans.SetY2(thlim);
  trans.Draw();

  c1.cd(2);
  hthz_2->SetMinimum(0);
  // hthz_2->SetMaximum(thresmax);//1.5
  hthz_2->SetMaximum(thresmax);//15
  hthz_2->GetYaxis()->SetTitle("#theta_{MC}-#theta_{REC}, sigma [mrad]");
  hthz_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hthz_2->Draw();
  TBox dip1_s(560.,0.,660.,thresmax);
  dip1_s.SetFillStyle(3017);
  dip1_s.SetFillColor(kBlue-6);
  TBox dip2_s(342.,0.,602.,thresmax);
  dip2_s.SetFillStyle(3018);
  dip2_s.SetFillColor(kBlue-3);
  TBox trans_s(283,0.,343,thresmax);
  trans_s.SetFillStyle(3003);
  trans_s.SetFillColor(kGreen-5);
  dip1_s.Draw();
  dip2_s.Draw();
  trans_s.Draw();
 
  c1.cd(3);
  hthz_chi2->Draw();
  c1.cd(4);
  hthz->Draw("colz");
  //  c1.Draw();
  hthz->Write();
  c1.Print(resname_pdf_o);
  c1.Write();
  c1.Close();

  //  TH2D *hxz = new TH2D("hxz",";z_{MC}, cm;x_{MC} - x_{REC}, cm",45.,0,1200,1e3,-5.,5.);//1.5
  TH2D *hxz = new TH2D("hxz",";z_{MC}, cm;x_{MC} - x_{REC}, cm",45.,0,1200,1e3,-1.,1.);//15
  tbp->Project("hxz","(xmc-xrec):zmc",condition);
  hxz->FitSlicesY();
  // hxz_1->SetMinimum(-0.1);//1.5
  // hxz_1->SetMaximum(0.1);
  hxz_1->SetMinimum(-xlim);//15
  hxz_1->SetMaximum(xlim);
  hxz_1->GetYaxis()->SetTitle("x_{MC}-x_{REC}, mean [cm]");
  TCanvas c2;
  c2.Divide(2,2);
  c2.cd(1);
  hxz_1->SetTitle("REC vs. MC (near IP)");
  hxz_2->SetTitle("");
  hxz_chi2->SetTitle("");
  hxz_1->Draw();
  c2.cd(2);
  hxz_2->SetMinimum(0);
  //  hxz_2->SetMaximum(0.7);
  hxz_2->SetMaximum(xresmax);//15
  hxz_2->GetYaxis()->SetTitle("x_{MC}-x_{REC}, sigma [cm]");
  hxz_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hxz_2->Draw();
  c2.cd(3);
  hxz_chi2->Draw();
  c2.cd(4);
  hxz->Draw("colz");
  //  c2.Draw();
  hxz->Write();
  c2.Print(resname_pdf_o);
  c2.Write();
  c2.Close();
  //    TH2D *hyz = new TH2D("hyz",";z_{MC}, cm;y_{MC} - y_{REC}, cm",45.,0,1200,1e3,-5.,5.);//1.5
  TH2D *hyz = new TH2D("hyz",";z_{MC}, cm;y_{MC} - y_{REC}, cm",45.,0,1200,1e3,-1.,1.);//15
  tbp->Project("hyz","(ymc-yrec):zmc",condition);
  hyz->FitSlicesY();
  // hyz_1->SetMinimum(-0.1); //1.5
  // hyz_1->SetMaximum(0.1);
  hyz_1->SetMinimum(-xlim);//15
  hyz_1->SetMaximum(xlim);
  hyz_1->GetYaxis()->SetTitle("y_{MC}-y_{REC}, mean [cm]");
  TCanvas c3;
  c3.Divide(2,2);
  c3.cd(1);
  hyz_1->SetTitle("REC vs. MC (near IP)");
  hyz_2->SetTitle("");
  hyz_chi2->SetTitle("");
  hyz_1->Draw();
  c3.cd(2);
  hyz_2->SetMinimum(0);
  //  hyz_2->SetMaximum(0.7);//1.5
  hyz_2->SetMaximum(xresmax);//15
  hyz_2->GetYaxis()->SetTitle("y_{MC}-y_{REC}, sigma [cm]");
  hyz_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hyz_2->Draw();
  c3.cd(3);
  hyz_chi2->Draw();
  c3.cd(4);
  hyz->Draw("colz");
  //  c3.Draw();
  hyz->Write();
  c3.Print(resname_pdf_o);
  c3.Write();
  c3.Close();
  TH2D *hzz = new TH2D("hzz",";z_{MC}, cm;z_{MC} - z_{REC}, cm",45.,0,1200,1e3,-1e-2,1e-2);
  tbp->Project("hzz","(zmc-zrec):zmc",condition);
 

  TH2D *hphz = new TH2D("hphz",";z_{MC}, cm;#phi_{MC} - #phi_{REC}, rad",45.,0,1200,1e3,-2e-1,2e-1);
  tbp->Project("hphz","(phimc-phirec):zmc",condition);
  hphz->FitSlicesY();
  // hphz_1->SetMinimum(-0.05);//1.5
  // hphz_1->SetMaximum(0.05);
  hphz_1->SetMinimum(-xlim);//15
  hphz_1->SetMaximum(xlim);
  hphz_1->GetYaxis()->SetTitle("#phi_{MC}-#phi_{REC}, mean [cm]");
  TCanvas c5;
  c5.Divide(2,2);
  c5.cd(1);
  hphz_1->SetTitle("REC vs. MC (near IP)");
  hphz_2->SetTitle("");
  hphz_chi2->SetTitle("");
  hphz_1->Draw();
  c5.cd(2);
  hphz_2->SetMinimum(0);
  //  hphz_2->SetMaximum(0.2);
  hphz_2->SetMaximum(phlim);//15
  hphz_2->GetYaxis()->SetTitle("#phi_{MC}-#phi_{REC}, sigma [cm]");
  hphz_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hphz_2->Draw();
  c5.cd(3);
  hphz_chi2->Draw();
  c5.cd(4);
  hphz->Draw("colz");
  //  c5.Draw();

  hphz->Write();
  c5.Print(resname_pdf_o);
  c5.Write();
  c5.Close();
  //  TH2D *hmomz = new TH2D("hmomz",";z_{MC}, cm;mom_{MC} - mom_{REC}, MeV",45.,0,1200,2e2,-1.,1.);//1.5
  TH2D *hmomz = new TH2D("hmomz",";z_{MC}, cm;mom_{MC} - mom_{REC}, MeV",45.,0,1200,2e2,-20.,20.);//15
  tbp->Project("hmomz","1e3*(pmc-prec):zmc",condition);
  hmomz->FitSlicesY();
  // hmomz_1->SetMinimum(-0.1);//1.5
  // hmomz_1->SetMaximum(0.1);
  hmomz_1->SetMinimum(-momlim);
  hmomz_1->SetMaximum(momlim);
  hmomz_1->GetYaxis()->SetTitle("mom_{MC}-mom_{REC}, mean [MeV]");
  TCanvas c6;
  c6.Divide(2,2);
  c6.cd(1);
  hmomz_1->SetTitle("REC vs. MC (near IP)");
  hmomz_2->SetTitle("");
  hmomz_chi2->SetTitle("");
  hmomz_1->Draw();
  c6.cd(2);
  hmomz_2->SetMinimum(0);
  //  hmomz_2->SetMaximum(0.5);//1.5
  hmomz_2->SetMaximum(momresmax);
  hmomz_2->GetYaxis()->SetTitle("mom_{MC}-mom_{REC}, sigma [MeV]");
  hmomz_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hmomz_2->Draw();
  c6.cd(3);
  hmomz_chi2->Draw();
  c6.cd(4);
  hmomz->Draw("colz");
  //  c6.Draw();
  
  hmomz->Write();
  c6.Print(resname_pdf_o);
  c6.Write();
  c6.Close();
 TH2D *hthmcz = new TH2D("hthmcz",";z_{MC}, cm;#theta_{MC}, mrad",45.,0,1200,6e2,-10.,50.);
  tbp->Project("hthmcz","1e3*(thetamc):zmc",condition);

  TH2D *hthrecz = new TH2D("hthrecz",";z_{MC}, cm;#theta_{REC}, mrad",45.,0,1200,6e2,-10.,50.);
  tbp->Project("hthrecz","1e3*(thetarec):zmc",condition);
  TCanvas c7;
  c7.Divide(3,2);
  // c7.cd(1);
  // // hzz->SetTitle("REC vs. MC (near IP)");
  // hzz->Draw("colz");

  // c7.cd(2);
  // hthmcz->SetTitle("REC vs. MC (near IP)");
  // hthmcz->Draw("colz");
  // //  hthrecz->Draw("colz");
  // c7.cd(3);
  c7.cd(4);
  TH2D *hbxz = new TH2D("hbxz",";z_{MC}, cm;Bx, kG",45.,0,1200,2e2,-0.1,0.1);
  tbp->Project("hbxz","Bx:zmc",condition);
  hbxz->Draw("colz");
  c7.cd(5);
  //  //  TH2D *hbyz = new TH2D("hbyz",";z_{MC}, cm;By, kG",45.,0,1200,2e2,-1.5,1.5);//1.5
  TH2D *hbyz = new TH2D("hbyz",";z_{MC}, cm;By, kG",45.,0,1200,2e2,-10.5,10.5);//15
  tbp->Project("hbyz","By:zmc",condition);
  hbyz->Draw("colz");
  c7.cd(6);
  TH2D *hbzz = new TH2D("hbzz",";z_{MC}, cm;Bz, kG",45.,0,1200,1e2,-25.,25.);
  tbp->Project("hbzz","Bz:zmc",condition);
  hbzz->Draw("colz");

  // hthmcz->Write();
  // hthrecz->Write();
  // hzz->Write();
  hbxz->Write();
  hbyz->Write();
  hbzz->Write();
  //  c7.Print(resname_pdf_c);//close
  c7.Print(resname_pdf_o);//don't close
  c7.Write();
  c7.Close();
 //  //END:MC trk from IP vs. REC -----------------------------------------------------------------------------------------------------------------------

  //MC trk from LMD vs. REC -----------------------------------------------------------------------------------------------------------------------
  //  TH2D *hthz_lmd = new TH2D("hthz_lmd",";z_{MC}, cm;#theta_{MCLMD}-#theta_{REC}, mrad",45.,0,1200,1e3,-5.,5.);//1.5
  TH2D *hthz_lmd = new TH2D("hthz_lmd",";z_{MC}, cm;#theta_{MCLMD}-#theta_{REC}, mrad",45.,0,1200,1e3,-1.,1.);//15
  tbp->Project("hthz_lmd","1e3*(thetamclmd-thetarec):zmc",condition);
  hthz_lmd->FitSlicesY();
  // hthz_lmd_1->SetMinimum(-0.2);
  // hthz_lmd_1->SetMaximum(0.2);
  hthz_lmd_1->SetMinimum(-thlim);
  hthz_lmd_1->SetMaximum(thlim);

  hthz_lmd_1->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{REC}, mean [mrad]");
  TCanvas c1_lmd;
  c1_lmd.Divide(2,2);
  c1_lmd.cd(1);

  hthz_lmd_1->SetTitle("REC vs. MC (near LMD)");
  hthz_lmd_2->SetTitle("");
  hthz_lmd_chi2->SetTitle("");
  hthz_lmd_1->Draw();
  dip1.SetY1(-thlim);
  dip1.SetY2(thlim);
  dip1.Draw();
  dip2.SetY1(-thlim);
  dip2.SetY2(thlim);
  dip2.Draw();
  trans.SetY1(-thlim);
  trans.SetY2(thlim);
  trans.Draw();
  c1_lmd.cd(2);
  hthz_lmd_2->SetMinimum(0);
  //  hthz_lmd_2->SetMaximum(1.5);//1.5
  hthz_lmd_2->SetMaximum(thresmax);//15
  hthz_lmd_2->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{REC}, sigma [mrad]");
  hthz_lmd_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hthz_lmd_2->Draw();
  TBox dip1_s(560.,0.,660.,thresmax);
  dip1_s.SetFillStyle(3017);
  dip1_s.SetFillColor(kBlue-6);
  TBox dip2_s(342.,0.,602.,thresmax);
  dip2_s.SetFillStyle(3018);
  dip2_s.SetFillColor(kBlue-3);
  TBox trans_s(283,0.,343,thresmax);
  trans_s.SetFillStyle(3003);
  trans_s.SetFillColor(kGreen-5);
  dip1_s.Draw();
  dip2_s.Draw();
  trans_s.Draw();
 
  c1_lmd.cd(3);
  hthz_lmd_chi2->Draw();
  c1_lmd.cd(4);
  hthz_lmd->Draw("colz");
  //  c1_lmd.Draw();

  hthz_lmd->Write();
  c1_lmd.Print(resname_pdf_o);
  c1_lmd.Write();
  c1_lmd.Close();
  //  TH2D *hxz_lmd = new TH2D("hxz_lmd",";z_{MC}, cm;x_{MCLMD} - x_{REC}, cm",45.,0,1200,1e3,-5.,5.);//1.5
  TH2D *hxz_lmd = new TH2D("hxz_lmd",";z_{MCLMD}, cm;x_{MCLMD} - x_{REC}, cm",45.,0,1200,1e3,-1.,1.);//15
  tbp->Project("hxz_lmd","(xmclmd-xrec):zmc",condition);
  hxz_lmd->FitSlicesY();
  // hxz_lmd_1->SetMinimum(-0.1);//1.5
  // hxz_lmd_1->SetMaximum(0.1);
  hxz_lmd_1->SetMinimum(-xlim);//15
  hxz_lmd_1->SetMaximum(xlim);
  hxz_lmd_1->GetYaxis()->SetTitle("x_{MCLMD}-x_{REC}, mean [cm]");
  TCanvas c2_lmd;
  c2_lmd.Divide(2,2);
  c2_lmd.cd(1);
  hxz_lmd_1->SetTitle("REC vs. MC (near LMD)");
  hxz_lmd_2->SetTitle("");
  hxz_lmd_chi2->SetTitle("");
  hxz_lmd_1->Draw();
  c2_lmd.cd(2);
  hxz_lmd_2->SetMinimum(0);
  //  hxz_lmd_2->SetMaximum(0.7);
  hxz_lmd_2->SetMaximum(xresmax);//15
  hxz_lmd_2->GetYaxis()->SetTitle("x_{MCLMD}-x_{REC}, sigma [cm]");
  hxz_lmd_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hxz_lmd_2->Draw();
  c2_lmd.cd(3);
  hxz_lmd_chi2->Draw();
  c2_lmd.cd(4);
  hxz_lmd->Draw("colz");
  //  c2_lmd.Draw();

  hxz_lmd->Write();
  c2_lmd.Print(resname_pdf_o);
  c2_lmd.Write();
  c2_lmd.Close();
  //  TH2D *hyz_lmd = new TH2D("hyz_lmd",";z_{MC}, cm;y_{MCLMD} - y_{REC}, cm",45.,0,1200,1e3,-5.,5.);//1.5
  TH2D *hyz_lmd = new TH2D("hyz_lmd",";z_{MCLMD}, cm;y_{MCLMD} - y_{REC}, cm",45.,0,1200,1e3,-1.,1.);//15
  tbp->Project("hyz_lmd","(ymclmd-yrec):zmc",condition);
  hyz_lmd->FitSlicesY();
  // hyz_lmd_1->SetMinimum(-0.1); //1.5
  // hyz_lmd_1->SetMaximum(0.1);
  hyz_lmd_1->SetMinimum(-xlim);//15
  hyz_lmd_1->SetMaximum(xlim);
  hyz_lmd_1->GetYaxis()->SetTitle("y_{MCLMD}-y_{REC}, mean [cm]");
  TCanvas c3_lmd;
  c3_lmd.Divide(2,2);
  c3_lmd.cd(1);
  hyz_lmd_1->SetTitle("REC vs. MC (near LMD)");
  hyz_lmd_2->SetTitle("");
  hyz_lmd_chi2->SetTitle("");
  hyz_lmd_1->Draw();
  c3_lmd.cd(2);
  hyz_lmd_2->SetMinimum(0);
  //  hyz_lmd_2->SetMaximum(0.7);//1.5
  hyz_lmd_2->SetMaximum(xresmax);//15
  hyz_lmd_2->GetYaxis()->SetTitle("y_{MCLMD}-y_{REC}, sigma [cm]");
  hyz_lmd_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hyz_lmd_2->Draw();
  c3_lmd.cd(3);
  hyz_lmd_chi2->Draw();
  c3_lmd.cd(4);
  hyz_lmd->Draw("colz");
  //  c3_lmd.Draw();
  hyz_lmd->Write();
  c3_lmd.Print(resname_pdf_o);
  c3_lmd.Write();
  c3_lmd.Close();
  TH2D *hzz_lmd = new TH2D("hzz_lmd",";z_{MC}, cm;z_{MCLMD} - z_{REC}, cm",45.,0,1200,1e3,-1e-2,1e-2);
  tbp->Project("hzz_lmd","(zmclmd-zrec):zmc",condition);
 

  TH2D *hphz_lmd = new TH2D("hphz_lmd",";z_{MC}, cm;#phi_{MCLMD} - #phi_{REC}, rad",45.,0,1200,1e3,-2e-1,2e-1);
  tbp->Project("hphz_lmd","(phimclmd-phirec):zmc",condition);
  hphz_lmd->FitSlicesY();
  // hphz_lmd_1->SetMinimum(-0.05);//1.5
  // hphz_lmd_1->SetMaximum(0.05);
  hphz_lmd_1->SetMinimum(-xlim);//15
  hphz_lmd_1->SetMaximum(xlim);
  hphz_lmd_1->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{REC}, mean [cm]");
  TCanvas c5_lmd;
  c5_lmd.Divide(2,2);
  c5_lmd.cd(1);
  hphz_lmd_1->SetTitle("REC vs. MC (near LMD)");
  hphz_lmd_2->SetTitle("");
  hphz_lmd_chi2->SetTitle("");
  hphz_lmd_1->Draw();
  c5_lmd.cd(2);
  hphz_lmd_2->SetMinimum(0);
  //  hphz_lmd_2->SetMaximum(0.2);
  hphz_lmd_2->SetMaximum(phlim);//15
  hphz_lmd_2->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{REC}, sigma [cm]");
  hphz_lmd_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hphz_lmd_2->Draw();
  c5_lmd.cd(3);
  hphz_lmd_chi2->Draw();
  c5_lmd.cd(4);
  hphz_lmd->Draw("colz");
  //  c5_lmd.Draw();

  hphz_lmd->Write();
  c5_lmd.Print(resname_pdf_o);
  c5_lmd.Write();
  c5_lmd.Close();
  //  TH2D *hmomz_lmd = new TH2D("hmomz_lmd",";z_{MC}, cm;mom_{MCLMD} - mom_{REC}, MeV",45.,0,1200,2e2,-1.,1.);//1.5
  TH2D *hmomz_lmd = new TH2D("hmomz_lmd",";z_{MCLMD}, cm;mom_{MCLMD} - mom_{REC}, keV",45.,0,1200,2e1,-20.,20.);//15
  tbp->Project("hmomz_lmd","1e3*(pmclmd-prec):zmc",condition);
  hmomz_lmd->FitSlicesY();
  // hmomz_lmd_1->SetMinimum(-0.1);//1.5
  // hmomz_lmd_1->SetMaximum(0.1);
  hmomz_lmd_1->SetMinimum(-momlim);
  hmomz_lmd_1->SetMaximum(momlim);
  hmomz_lmd_1->GetYaxis()->SetTitle("mom_{MCLMD}-mom_{REC}, mean [MeV]");
  TCanvas c6_lmd;
  c6_lmd.Divide(2,2);
  c6_lmd.cd(1);
  hmomz_lmd_1->SetTitle("REC vs. MC (near LMD)");
  hmomz_lmd_2->SetTitle("");
  hmomz_lmd_chi2->SetTitle("");
  hmomz_lmd_1->Draw();
  c6_lmd.cd(2);
  hmomz_lmd_2->SetMinimum(0);
  //  hmomz_lmd_2->SetMaximum(0.5);//1.5
  hmomz_lmd_2->SetMaximum(momresmax);
  hmomz_lmd_2->GetYaxis()->SetTitle("mom_{MCLMD}-mom_{REC}, sigma [MeV]");
  hmomz_lmd_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hmomz_lmd_2->Draw();
  c6_lmd.cd(3);
  hmomz_lmd_chi2->Draw();
  c6_lmd.cd(4);
  hmomz_lmd->Draw("colz");
  //  c6_lmd.Draw();

  hmomz_lmd->Write();
  c6_lmd.Print(resname_pdf_o);
  c6_lmd.Write();
  c6_lmd.Close();
 // TH2D *hthmcz_lmd = new TH2D("hthmcz_lmd",";z_{MCLMD}, cm;#theta_{MCLMD}, mrad",45.,0,1200,6e2,-10.,50.);
 //  tbp->Project("hthmcz_lmd","1e3*(thetamclmd):zmc","thetarec!=0 && abs(zmclmd-zrec)<100.");

 //  // TH2D *hthrecz_lmd = new TH2D("hthrecz_lmd",";z_{MCLMD}, cm;#theta_{REC}, mrad",45.,0,1200,6e2,-10.,50.);
 //  // tbp->Project("hthrecz_lmd","1e3*(thetarec):zmc","thetarec!=0 && abs(zmclmd-zrec)<100.");
 //  TCanvas c7_lmd;
 //  c7_lmd.Divide(3,2);
 //  c7_lmd.cd(1);
 //  hzz_lmd->Draw("colz");

 //  c7_lmd.cd(2);
 //  hthmcz_lmd->Draw("colz");
 //  c7_lmd.Write();
 //  hthmcz_lmd->Write();
 //  hzz_lmd->Write();
 //  c7_lmd.Print(resname_pdf_c);


  //END:MC trk from LMD vs. REC -----------------------------------------------------------------------------------------------------------------------

  //MC trk from LMD vs. MC in IP -----------------------------------------------------------------------------------------------------------------------
  //  TH2D *hthz_mc = new TH2D("hthz_mc",";z_{MC}, cm;#theta_{MCLMD}-#theta_{MC}, mrad",45.,0,1200,1e3,-5.,5.);//1.5
  TH2D *hthz_mc = new TH2D("hthz_mc",";z_{MC}, cm;#theta_{MCLMD}-#theta_{MC}, mrad",45.,0,1200,1e3,-1.,1.);//15
  tbp->Project("hthz_mc","1e3*(thetamclmd-thetamc):zmc","thetamc!=0 && abs(zmc-zmclmd)<1.");
  hthz_mc->FitSlicesY();
  // hthz_mc_1->SetMinimum(-0.2);
  // hthz_mc_1->SetMaximum(0.2);
  hthz_mc_1->SetMinimum(-thlim);
  hthz_mc_1->SetMaximum(thlim);

  hthz_mc_1->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{MC}, mean [mrad]");
  TCanvas c1_mc;
  c1_mc.Divide(2,2);
  c1_mc.cd(1);
  hthz_mc_1->SetTitle("MC(near LMD) vs. MC (near IP)");
  hthz_mc_2->SetTitle("");
  hthz_mc_chi2->SetTitle("");
  hthz_mc_1->Draw();
  dip1.SetY1(-thlim);
  dip1.SetY2(thlim);
  dip1.Draw();
  dip2.SetY1(-thlim);
  dip2.SetY2(thlim);
  dip2.Draw();
  trans.SetY1(-thlim);
  trans.SetY2(thlim);
  trans.Draw();
  c1_mc.cd(2);
  hthz_mc_2->SetMinimum(0);
  //  hthz_mc_2->SetMaximum(1.5);//1.5
  hthz_mc_2->SetMaximum(thresmax);//15
  hthz_mc_2->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{MC}, sigma [mrad]");
  hthz_mc_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hthz_mc_2->Draw();
  TBox dip1_s(560.,0.,660.,thresmax);
  dip1_s.SetFillStyle(3017);
  dip1_s.SetFillColor(kBlue-6);
  TBox dip2_s(342.,0.,602.,thresmax);
  dip2_s.SetFillStyle(3018);
  dip2_s.SetFillColor(kBlue-3);
  TBox trans_s(283,0.,343,thresmax);
  trans_s.SetFillStyle(3003);
  trans_s.SetFillColor(kGreen-5);
  dip1_s.Draw();
  dip2_s.Draw();
  trans_s.Draw();
 
  c1_mc.cd(3);
  hthz_mc_chi2->Draw();
  c1_mc.cd(4);
  hthz_mc->Draw("colz");
  //  c1_mc.Draw();

  hthz_mc->Write();
  c1_mc.Print(resname_pdf_o);
  c1_mc.Write();
  c1_mc.Close();
  //  TH2D *hxz_mc = new TH2D("hxz_mc",";z_{MC}, cm;x_{MCLMD} - x_{MC}, cm",45.,0,1200,1e3,-5.,5.);//1.5
  TH2D *hxz_mc = new TH2D("hxz_mc",";z_{MCLMD}, cm;x_{MCLMD} - x_{MC}, cm",45.,0,1200,1e3,-1.,1.);//15
  tbp->Project("hxz_mc","(xmclmd-xmc):zmc","thetamc!=0 && abs(zmc-zmc)<100.");
  hxz_mc->FitSlicesY();
  // hxz_mc_1->SetMinimum(-0.1);//1.5
  // hxz_mc_1->SetMaximum(0.1);
  hxz_mc_1->SetMinimum(-xlim);//15
  hxz_mc_1->SetMaximum(xlim);
  hxz_mc_1->GetYaxis()->SetTitle("x_{MCLMD}-x_{MC}, mean [cm]");
  TCanvas c2_mc;
  c2_mc.Divide(2,2);
  c2_mc.cd(1);
  hxz_mc_1->SetTitle("MC(near LMD) vs. MC (near IP)");
  hxz_mc_2->SetTitle("");
  hxz_mc_chi2->SetTitle("");
  hxz_mc_1->Draw();
  c2_mc.cd(2);
  hxz_mc_2->SetMinimum(0);
  //  hxz_mc_2->SetMaximum(0.7);
  hxz_mc_2->SetMaximum(xresmax);//15
  hxz_mc_2->GetYaxis()->SetTitle("x_{MCLMD}-x_{MC}, sigma [cm]");
  hxz_mc_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hxz_mc_2->Draw();
  c2_mc.cd(3);
  hxz_mc_chi2->Draw();
  c2_mc.cd(4);
  hxz_mc->Draw("colz");
  //  c2_mc.Draw();

  hxz_mc->Write();
  c2_mc.Print(resname_pdf_o);
  c2_mc.Write();
  c2_mc.Close();
  //  TH2D *hyz_mc = new TH2D("hyz_mc",";z_{MC}, cm;y_{MCLMD} - y_{MC}, cm",45.,0,1200,1e3,-5.,5.);//1.5
  TH2D *hyz_mc = new TH2D("hyz_mc",";z_{MCLMD}, cm;y_{MCLMD} - y_{MC}, cm",45.,0,1200,1e3,-1.,1.);//15
  tbp->Project("hyz_mc","(ymclmd-ymc):zmc","thetamc!=0 && abs(zmc-zmc)<100.");
  hyz_mc->FitSlicesY();
  // hyz_mc_1->SetMinimum(-0.1); //1.5
  // hyz_mc_1->SetMaximum(0.1);
  hyz_mc_1->SetMinimum(-xlim);//15
  hyz_mc_1->SetMaximum(xlim);
  hyz_mc_1->GetYaxis()->SetTitle("y_{MCLMD}-y_{MC}, mean [cm]");
  TCanvas c3_mc;
  c3_mc.Divide(2,2);
  c3_mc.cd(1);
  hyz_mc_1->SetTitle("MC(near LMD) vs. MC (near IP)");
  hyz_mc_2->SetTitle("");
  hyz_mc_chi2->SetTitle("");
  hyz_mc_1->Draw();
  c3_mc.cd(2);
  hyz_mc_2->SetMinimum(0);
  //  hyz_mc_2->SetMaximum(0.7);//1.5
  hyz_mc_2->SetMaximum(xresmax);//15
  hyz_mc_2->GetYaxis()->SetTitle("y_{MCLMD}-y_{MC}, sigma [cm]");
  hyz_mc_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hyz_mc_2->Draw();
  c3_mc.cd(3);
  hyz_mc_chi2->Draw();
  c3_mc.cd(4);
  hyz_mc->Draw("colz");
  //  c3_mc.Draw();

  hyz_mc->Write();
  c3_mc.Print(resname_pdf_o);
  c3_mc.Write();
  c3_mc.Close();
  TH2D *hzz_mc = new TH2D("hzz_mc",";z_{MC}, cm;z_{MCLMD} - z_{MC}, cm",45.,0,1200,1e3,-1e-2,1e-2);
  tbp->Project("hzz_mc","(zmclmd-zmc):zmc","thetamc!=0 && abs(zmc-zmc)<100.");
 

  TH2D *hphz_mc = new TH2D("hphz_mc",";z_{MC}, cm;#phi_{MCLMD} - #phi_{MC}, rad",45.,0,1200,1e3,-2e-1,2e-1);
  tbp->Project("hphz_mc","(phimclmd-phimc):zmc","thetamc!=0 && abs(zmc-zmc)<100.");
  hphz_mc->FitSlicesY();
  // hphz_mc_1->SetMinimum(-0.05);//1.5
  // hphz_mc_1->SetMaximum(0.05);
  hphz_mc_1->SetMinimum(-xlim);//15
  hphz_mc_1->SetMaximum(xlim);
  hphz_mc_1->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{MC}, mean [cm]");
  TCanvas c5_mc;
  c5_mc.Divide(2,2);
  c5_mc.cd(1);
  hphz_mc_1->SetTitle("MC(near LMD) vs. MC (near IP)");
  hphz_mc_2->SetTitle("");
  hphz_mc_chi2->SetTitle("");
  hphz_mc_1->Draw();
  c5_mc.cd(2);
  hphz_mc_2->SetMinimum(0);
  //  hphz_mc_2->SetMaximum(0.2);
  hphz_mc_2->SetMaximum(phlim);//15
  hphz_mc_2->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{MC}, sigma [cm]");
  hphz_mc_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hphz_mc_2->Draw();
  c5_mc.cd(3);
  hphz_mc_chi2->Draw();
  c5_mc.cd(4);
  hphz_mc->Draw("colz");
  //  c5_mc.Draw();

  hphz_mc->Write();
  c5_mc.Print(resname_pdf_o);
  c5_mc.Write();
  c5_mc.Close();
  //  TH2D *hmomz_mc = new TH2D("hmomz_mc",";z_{MC}, cm;mom_{MCLMD} - mom_{MC}, MeV",45.,0,1200,2e2,-1.,1.);//1.5
  TH2D *hmomz_mc = new TH2D("hmomz_mc",";z_{MCLMD}, cm;mom_{MCLMD} - mom_{MC}, keV",45.,0,1200,2e1,-20.,20.);//15
  tbp->Project("hmomz_mc","1e3*(pmclmd-pmc):zmc","thetamc!=0 && abs(zmc-zmc)<100.");
  hmomz_mc->FitSlicesY();
  // hmomz_mc_1->SetMinimum(-0.1);//1.5
  // hmomz_mc_1->SetMaximum(0.1);
  hmomz_mc_1->SetMinimum(-momlim);
  hmomz_mc_1->SetMaximum(momlim);
  hmomz_mc_1->GetYaxis()->SetTitle("mom_{MCLMD}-mom_{MC}, mean [MeV]");
  TCanvas c6_mc;
  c6_mc.Divide(2,2);
  c6_mc.cd(1);
  hmomz_mc_1->SetTitle("MC(near LMD) vs. MC (near IP)");
  hmomz_mc_2->SetTitle("");
  hmomz_mc_chi2->SetTitle("");
  hmomz_mc_1->Draw();
  c6_mc.cd(2);
  hmomz_mc_2->SetMinimum(0);
  //  hmomz_mc_2->SetMaximum(0.5);//1.5
  hmomz_mc_2->SetMaximum(momresmax);
  hmomz_mc_2->GetYaxis()->SetTitle("mom_{MCLMD}-mom_{MC}, sigma [MeV]");
  hmomz_mc_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hmomz_mc_2->Draw();
  c6_mc.cd(3);
  hmomz_mc_chi2->Draw();
  c6_mc.cd(4);
  hmomz_mc->Draw("colz");
  //  c6_mc.Draw();

  hmomz_mc->Write();
  //  c6_mc.Print(resname_pdf_c);
  c6_mc.Print(resname_pdf_o);
  c6_mc.Write();
  c6_mc.Close();
  //END:MC trk from LMD vs. MC in IP -----------------------------------------------------------------------------------------------------------------------

  // and ad v,w,v',w' params for track representation
  //v
  TH2D *hvrecz = new TH2D("hvrecz",";z_{MC}, cm; v_{MC} - v_{REC}",45.,0,1200,2e2,-1.1,1.1);//1.5
  //  tbp->Project("hvrecz","-vrec+vmc:zmc",condition);
  tbp->Project("hvrecz","-vrec+vmc:zmc",condition);
  TH2D *hvmclmdz = new TH2D("hvmclmdz",";z_{MC}, cm; v_{MC} - v_{MCLMD}",45.,0,1200,2e3,-0.03,0.03);//1.5
  //  tbp->Project("hvmclmdz","-vmclmd+vmc:zmc","thetarec!=0 && abs(zmc-zrec)<100. && vmclmd_err==0 && vmclmd_err==0");
  tbp->Project("hvmclmdz","-vmclmd+vmc:zmc",condition);
  TH2D *hvrecmclmdz = new TH2D("hvrecmclmdz",";z_{MC}, cm; v_{MCLMD}-v_{REC}",45.,0,1200,2e3,-1.1,1.1);//1.5
  //  tbp->Project("hvrecmclmdz","-vrec+vmclmd:zmc","thetarec!=0 && abs(zmc-zrec)<100. && vmclmd_err==0 && vmclmd_err==0");
  tbp->Project("hvrecmclmdz","-vrec+vmclmd:zmc",condition);
  TH2D *hdvmc = new TH2D("hdvmc",";z_{MC}, cm; #sigma v_{MC}",45.,0,1200,2e2,0,1.1);//1.5
  //  tbp->Project("hdvmc","vmc_err:zmc","thetarec!=0 && abs(zmc-zrec)<100. && vmclmd_err==0 && vmclmd_err==0");
  tbp->Project("hdvmc","vmc_err:zmc",condition);
  TH2D *hdvmclmd = new TH2D("hdvmclmd",";z_{MC}, cm; #sigma v_{MCLMD}",45.,0,1200,2e3,0,1.1);//1.5
  //  tbp->Project("hdvmclmd","vmclmd_err:zmc","thetarec!=0 && abs(zmc-zrec)<100. && vmclmd_err==0 && vmclmd_err==0");
  tbp->Project("hdvmclmd","vmclmd_err:zmc",condition);
  TH2D *hdvrec = new TH2D("hdvrec",";z_{MC}, cm; #sigma v_{REC}",45.,0,1200,1e2,0,0.1);//1.5
  //  tbp->Project("hdvrec","vrec_err:zmc","thetarec!=0 && abs(zmc-zrec)<100. && vmclmd_err==0 && vmclmd_err==0");
  tbp->Project("hdvrec","vrec_err:zmc",condition);
  TCanvas c8("vpar");
  c8.Divide(3,2);
  c8.cd(1);
  hvrecz->FitSlicesY();
  hvrecz_1->GetYaxis()->SetTitle("v_{MC}-v_{REC}, mean");
  hvrecz_2->GetYaxis()->SetTitle("v_{MC}-v_{REC}, sigma");
  hvrecz_1->SetTitle("v_{MC}-v_{REC}");
  hvrecz_2->SetTitle("");
  hvrecz_1->SetMinimum(-1.2e-1);
  hvrecz_1->SetMaximum(1.2e-1);
  hvrecz_1->Draw();
  c8.cd(2);
  hvrecz_2->SetMinimum(0.);
  hvrecz_2->SetMaximum(0.09);
  hvrecz_2->Draw();
  c8.cd(3);
  hvrecz->Draw("colz");
  // c8.cd(4);
  // hvmclmdz->FitSlicesY();
  // hvmclmdz_1->GetYaxis()->SetTitle("v_{MC}-v_{MCLMD}, mean");
  // hvmclmdz_2->GetYaxis()->SetTitle("v_{MC}-v_{MCLMD}, sigma");
  // hvmclmdz_1->SetTitle("v_{MC}-v_{MCLMD}");
  // hvmclmdz_2->SetTitle("");
  // // hvmclmdz_1->SetMinimum(-1.1e-1);
  // // hvmclmdz_1->SetMaximum(1.1e-1);
  // hvmclmdz_1->Draw();
  // c8.cd(5);
  // // hvmclmdz_2->SetMinimum(0.);
  // // hvmclmdz_2->SetMaximum(3e-5);
  // hvmclmdz_2->Draw();
  // c8.cd(6);
  // hvmclmdz->Draw("colz");
  c8.cd(4);
  hvrecmclmdz->FitSlicesY();
  hvrecmclmdz_1->GetYaxis()->SetTitle("v_{MCLMD}-v_{REC}, mean");
  hvrecmclmdz_2->GetYaxis()->SetTitle("v_{MCLMD}-v_{REC}, sigma");
  hvrecmclmdz_1->SetTitle("v_{MCLMD}-v_{REC}");
  hvrecmclmdz_2->SetTitle("");
  hvrecmclmdz_1->SetMinimum(-1.2e-1);
  hvrecmclmdz_1->SetMaximum(1.2e-1);
  hvrecmclmdz_1->Draw();
  c8.cd(5);
  hvrecmclmdz_2->SetMinimum(0.);
  hvrecmclmdz_2->SetMaximum(0.09);
  hvrecmclmdz_2->Draw();
  c8.cd(6);
  hvrecmclmdz->Draw("colz");
 c8.Print(resname_pdf_o);
  c8.Write();
  c8.Close();

  
  //v'
  TH2D *htvrecz = new TH2D("htvrecz",";z_{MC}, cm; tv_{MC} - tv_{REC}",45.,0,1200,2e2,-0.002,0.002);//1.5
  //  tbp->Project("htvrecz","-tvrec+tvmc:zmc","thetarec!=0 && abs(zmc-zrec)<100. && vmclmd_err==0 && vmclmd_err==0");
  tbp->Project("htvrecz","-tvrec+tvmc:zmc",condition);
  TH2D *htvmclmdz = new TH2D("htvmclmdz",";z_{MC}, cm; tv_{MC} - tv_{MCLMD}",45.,0,1200,2e3,-1e-4,1e-4);//1.5
  //  tbp->Project("htvmclmdz","-tvmclmd+tvmc:zmc","thetarec!=0 && abs(zmc-zrec)<100. && vmclmd_err==0 && vmclmd_err==0");
  tbp->Project("htvmclmdz","-tvmclmd+tvmc:zmc",condition);
  TH2D *htvrecmclmdz = new TH2D("htvrecmclmdz",";z_{MC}, cm; tv_{MCLMD}-tv_{REC}",45.,0,1200,2e2,-0.002,0.002);//1.5
  //  tbp->Project("htvrecmclmdz","-tvrec+tvmclmd:zmc","thetarec!=0 && abs(zmc-zrec)<100. && vmclmd_err==0 && vmclmd_err==0");
  tbp->Project("htvrecmclmdz","-tvrec+tvmclmd:zmc",condition);
  TH2D *hdtvmc = new TH2D("hdtvmc",";z_{MC}, cm; #sigma tv_{MC}",45.,0,1200,2e2,0,5e-4);//1.5
  //  tbp->Project("hdtvmc","tvmc_err:zmc","thetarec!=0 && abs(zmc-zrec)<100. && vmclmd_err==0 && vmclmd_err==0");
  tbp->Project("hdtvmc","tvmc_err:zmc",condition);
  TH2D *hdtvmclmd = new TH2D("hdtvmclmd",";z_{MC}, cm; #sigma tv_{MCLMD}",45.,0,1200,2e3,0,5e-4);//1.5
  //  tbp->Project("hdtvmclmd","tvmclmd_err:zmc",condition);
  tbp->Project("hdtvmclmd","tvmclmd_err:zmc",condition);
  TH2D *hdtvrec = new TH2D("hdtvrec",";z_{MC}, cm; #sigma tv_{REC}",45.,0,1200,1e2,0,1e-4);//1.5
  //  tbp->Project("hdtvrec","tvrec_err:zmc",condition);
  tbp->Project("hdtvrec","tvrec_err:zmc",condition);
  TCanvas c10("tvpar");
  c10.Divide(3,2);
  c10.cd(1);
  htvrecz->FitSlicesY();
  htvrecz_1->GetYaxis()->SetTitle("tv_{MC}-tv_{REC}, mean");
  htvrecz_2->GetYaxis()->SetTitle("tv_{MC}-tv_{REC}, sigma");
  htvrecz_1->SetTitle("tv_{MC}-tv_{REC}");
  htvrecz_2->SetTitle("");
  htvrecz_1->SetMinimum(-5e-5);
  htvrecz_1->SetMaximum(5e-5);
  htvrecz_1->Draw();
  c10.cd(2);
  htvrecz_2->SetMinimum(6e-5);
  htvrecz_2->SetMaximum(1.5e-4);
  htvrecz_2->Draw();
  c10.cd(3);
  htvrecz->Draw("colz");
  // c10.cd(4);
  // htvmclmdz->FitSlicesY();
  // htvmclmdz_1->GetYaxis()->SetTitle("tv_{MC}-tv_{MCLMD}, mean");
  // htvmclmdz_2->GetYaxis()->SetTitle("tv_{MC}-tv_{MCLMD}, sigma");
  // htvmclmdz_1->SetTitle("tv_{MC}-tv_{MCLMD}");
  // htvmclmdz_2->SetTitle("");
  // // htvmclmdz_1->SetMinimum(-2e-6);
  // // htvmclmdz_1->SetMaximum(2e-6);
  // htvmclmdz_1->Draw();
  // c10.cd(5);
  // // htvmclmdz_2->SetMinimum(0.);
  // // htvmclmdz_2->SetMaximum(2e-4);
  // htvmclmdz_2->Draw();
  // c10.cd(6);
  // htvmclmdz->Draw("colz");
  c10.cd(4);
  htvrecmclmdz->FitSlicesY();
  htvrecmclmdz_1->GetYaxis()->SetTitle("tv_{MCLMD}-tv_{REC}, mean");
  htvrecmclmdz_2->GetYaxis()->SetTitle("tv_{MCLMD}-tv_{REC}, sigma");
  htvrecmclmdz_1->SetTitle("tv_{MCLMD}-tv_{REC}");
  htvrecmclmdz_2->SetTitle("");
  htvrecmclmdz_1->SetMinimum(-5e-5);
  htvrecmclmdz_1->SetMaximum(5e-5);
  htvrecmclmdz_1->Draw();
  c10.cd(5);
  htvrecmclmdz_2->SetMinimum(6e-5);
  htvrecmclmdz_2->SetMaximum(1.5e-4);
  htvrecmclmdz_2->Draw();
  c10.cd(6);
  htvrecmclmdz->Draw("colz");
  c10.Print(resname_pdf_o);
  c10.Write();
  c10.Close();

 // TCanvas c10a("tvpar2");
 //  c10a.Divide(3,2);
 //  c10a.cd(1);
 //  hdtvrec->Draw("colz");
 //  c10a.cd(2);
 //  hdtvmclmd->Draw("colz");
 //  c10a.cd(3);
 //  hdtvmc->Draw("colz");
 //  c10a.cd(4);
 //  hdtvrec->FitSlicesY();
 //  hdtvrec_1->SetTitle("#sigma tv_{REC}, mean");
 //  hdtvrec_1->SetMinimum(0);
 //  hdtvrec_1->SetMaximum(2e-4);
 //  hdtvrec_1->Draw();
 //  c10a.cd(5);
 //  hdtvmclmd->FitSlicesY();
 //  hdtvmclmd_1->SetTitle("#sigma tv_{MCLMD}, mean");
 //  hdtvmclmd_1->SetMinimum(0);
 //  hdtvmclmd_1->SetMaximum(2e-4);
 //  hdtvmclmd_1->Draw();
 //  c10a.cd(6);
 //  hdtvmc->FitSlicesY();
 //  hdtvmc_1->SetTitle("#sigma tv_{MC}, mean");
 //  hdtvmc_1->SetMinimum(0);
 //  hdtvmc_1->SetMaximum(2e-4);
 //  hdtvmc_1->Draw();
 //  c10a.Print(resname_pdf_o);
 //  c10a.Write();
 //  c10a.Close();


  //w
  TH2D *hwrecz = new TH2D("hwrecz",";z_{MC}, cm; w_{MC} - w_{REC}",45.,0,1200,2e2,-1.1,1.1);//1.5
  //  tbp->Project("hwrecz","-wrec+wmc:zmc",condition);
  tbp->Project("hwrecz","-wrec+wmc:zmc",condition);
  TH2D *hwmclmdz = new TH2D("hwmclmdz",";z_{MC}, cm; w_{MC} - w_{MCLMD}",45.,0,1200,2e3,-0.03,0.03);//1.5
  //  tbp->Project("hwmclmdz","-wmclmd+wmc:zmc",condition);
  tbp->Project("hwmclmdz","-wmclmd+wmc:zmc",condition);
  TH2D *hwrecmclmdz = new TH2D("hwrecmclmdz",";z_{MC}, cm; w_{MCLMD}-w_{REC}",45.,0,1200,1e3,-1.1,1.1);//1.5
  //  tbp->Project("hwrecmclmdz","-wrec+wmclmd:zmc",condition);
  tbp->Project("hwrecmclmdz","-wrec+wmclmd:zmc",condition);
  TH2D *hdwmc = new TH2D("hdwmc",";z_{MC}, cm; #sigma w_{MC}",45.,0,1200,2e2,0,1.1);//1.5
  //  tbp->Project("hdwmc","wmc_err:zmc",condition);
  tbp->Project("hdwmc","wmc_err:zmc",condition);
  TH2D *hdwmclmd = new TH2D("hdwmclmd",";z_{MC}, cm; #sigma w_{MCLMD}",45.,0,1200,2e3,0,1.1);//1.5
  //  tbp->Project("hdwmclmd","wmclmd_err:zmc",condition);
  tbp->Project("hdwmclmd","wmclmd_err:zmc",condition);
  TH2D *hdwrec = new TH2D("hdwrec",";z_{MC}, cm; #sigma w_{REC}",45.,0,1200,1e2,0,0.2);//1.5
  //  tbp->Project("hdwrec","wrec_err:zmc",condition);
  tbp->Project("hdwrec","wrec_err:zmc",condition);
  TCanvas c9("wpar");
  c9.Divide(3,2);
  c9.cd(1);
  hwrecz->FitSlicesY();
  hwrecz_1->GetYaxis()->SetTitle("w_{MC}-w_{REC}, mean");
  hwrecz_2->GetYaxis()->SetTitle("w_{MC}-w_{REC}, sigma");
  hwrecz_1->SetTitle("w_{MC}-w_{REC}");
  hwrecz_2->SetTitle("");
  hwrecz_1->SetMinimum(-1.2e-1);
  hwrecz_1->SetMaximum(1.2e-1);
  hwrecz_1->Draw();
  c9.cd(2);
  hwrecz_2->SetMinimum(0);
  hwrecz_2->SetMaximum(0.09);
  hwrecz_2->Draw();
  c9.cd(3);
  hwrecz->Draw("colz");
  // c9.cd(4);
  // hwmclmdz->FitSlicesY();
  // hwmclmdz_1->GetYaxis()->SetTitle("w_{MC}-w_{MCLMD}, mean");
  // hwmclmdz_2->GetYaxis()->SetTitle("w_{MC}-w_{MCLMD}, sigma");
  // hwmclmdz_1->SetTitle("w_{MC}-w_{MCLMD}");
  // hwmclmdz_2->SetTitle("");
  // // hwmclmdz_1->SetMinimum(-5e-6);
  // // hwmclmdz_1->SetMaximum(5e-6);
  // hwmclmdz_1->Draw();
  // c9.cd(5);
  // // hwmclmdz_2->SetMinimum(0);
  // // hwmclmdz_2->SetMaximum(0.09);
  // hwmclmdz_2->Draw();
  // c9.cd(6);
  // hwmclmdz->Draw("colz");
  c9.cd(4);
  hwrecmclmdz->FitSlicesY();
  hwrecmclmdz_1->GetYaxis()->SetTitle("w_{MCLMD}-w_{REC}, mean");
  hwrecmclmdz_2->GetYaxis()->SetTitle("w_{MCLMD}-w_{REC}, sigma");
  hwrecmclmdz_1->SetTitle("w_{MCLMD}-w_{REC}");
  hwrecmclmdz_2->SetTitle("");
  hwrecmclmdz_1->Draw();
  hwrecmclmdz_1->SetMinimum(-1.2e-1);
  hwrecmclmdz_1->SetMaximum(1.2e-1);
  c9.cd(5);
  hwrecmclmdz_2->SetMinimum(0);
  hwrecmclmdz_2->SetMaximum(0.09);
  hwrecmclmdz_2->Draw();
  c9.cd(6);
  hwrecmclmdz->Draw("colz");
 c9.Print(resname_pdf_o);
  c9.Write();
  c9.Close();

  // TCanvas c9a("wpar2");
  // c9a.Divide(3,2);
  // c9a.cd(1);
  // hdwrec->Draw("colz");
  // c9a.cd(2);
  // hdwmclmd->Draw("colz");
  // c9a.cd(3);
  // hdwmc->Draw("colz");
  // c9a.cd(4);
  // hdwrec->FitSlicesY();
  // hdwrec_1->SetTitle("#sigma w_{REC}, mean");
  // hdwrec_1->SetMinimum(0);
  // hdwrec_1->SetMaximum(0.9);
  // hdwrec_1->Draw();
  // c9a.cd(5);
  // hdwmclmd->FitSlicesY();
  // hdwmclmd_1->SetTitle("#sigma w_{MCLMD}, mean");
  // hdwmclmd_1->SetMinimum(0);
  // hdwmclmd_1->SetMaximum(0.9);
  // hdwmclmd_1->Draw();
  // c9a.cd(6);
  // hdwmc->FitSlicesY();
  // hdwmc_1->SetTitle("#sigma w_{MC}, mean");
  // hdwmc_1->SetMinimum(0);
  // hdwmc_1->SetMaximum(0.9);
  // hdwmc_1->Draw();
  // c9a.Print(resname_pdf_o);
  // c9a.Write();
  // c9a.Close();


  //w'
  TH2D *htwrecz = new TH2D("htwrecz",";z_{MC}, cm; tw_{MC} - tw_{REC}",45.,0,1200,2e2,-0.002,0.002);//1.5
  //  tbp->Project("htwrecz","-twrec+twmc:zmc",condition);
  tbp->Project("htwrecz","-twrec+twmc:zmc",condition);
  TH2D *htwmclmdz = new TH2D("htwmclmdz",";z_{MC}, cm; tw_{MC} - tw_{MCLMD}",45.,0,1200,2e3,-3e-5,3e-5);//1.5
  //  tbp->Project("htwmclmdz","-twmclmd+twmc:zmc",condition);
  tbp->Project("htwmclmdz","-twmclmd+twmc:zmc",condition);
  TH2D *htwrecmclmdz = new TH2D("htwrecmclmdz",";z_{MC}, cm; tw_{MCLMD}-tw_{REC}",45.,0,1200,2e2,-0.002,0.002);//1.5
  //  tbp->Project("htwrecmclmdz","-twrec+twmclmd:zmc",condition);
  tbp->Project("htwrecmclmdz","-twrec+twmclmd:zmc"," abs(zmc-zrec)<100. && vmclmd_err==0");
  TH2D *hdtwmc = new TH2D("hdtwmc",";z_{MC}, cm; #sigma tw_{MC}",45.,0,1200,2e2,0,5e-4);//1.5
  //  tbp->Project("hdtwmc","twmc_err:zmc",condition);
  tbp->Project("hdtwmc","twmc_err:zmc",condition);
  TH2D *hdtwmclmd = new TH2D("hdtwmclmd",";z_{MC}, cm; #sigma tw_{MCLMD}",45.,0,1200,2e3,0,5e-4);//1.5
  //  tbp->Project("hdtwmclmd","twmclmd_err:zmc",condition);
  tbp->Project("hdtwmclmd","twmclmd_err:zmc",condition);
  TH2D *hdtwrec = new TH2D("hdtwrec",";z_{MC}, cm; #sigma tw_{REC}",45.,0,1200,1e2,0,1e-4);//1.5
  //  tbp->Project("hdtwrec","twrec_err:zmc",condition);
  tbp->Project("hdtwrec","twrec_err:zmc",condition);
  TCanvas c11("twpar");
  c11.Divide(3,2);
  c11.cd(1);
  htwrecz->FitSlicesY();
  htwrecz_1->GetYaxis()->SetTitle("tw_{MC}-tw_{REC}, mean");
  htwrecz_2->GetYaxis()->SetTitle("tw_{MC}-tw_{REC}, sigma");
  htwrecz_1->SetTitle("tw_{MC}-tw_{REC}");
  htwrecz_2->SetTitle("");
  htwrecz_1->SetMinimum(-5e-5);
  htwrecz_1->SetMaximum(5e-5);
  htwrecz_1->Draw();
  c11.cd(2);
  htwrecz_2->SetMinimum(6e-5);
  htwrecz_2->SetMaximum(1.5e-4);
  htwrecz_2->Draw();
  c11.cd(3);
  htwrecz->Draw("colz");
  // c11.cd(4);
  // htwmclmdz->FitSlicesY();
  // htwmclmdz_1->GetYaxis()->SetTitle("tw_{MC}-tw_{MCLMD}, mean");
  // htwmclmdz_2->GetYaxis()->SetTitle("tw_{MC}-tw_{MCLMD}, sigma");
  // htwmclmdz_1->SetTitle("tw_{MC}-tw_{MCLMD}");
  // htwmclmdz_2->SetTitle("");
  // // htwmclmdz_1->SetMinimum(-2e-6);
  // // htwmclmdz_1->SetMaximum(2e-6);
  // htwmclmdz_1->Draw();
  // c11.cd(5);
  // // htwmclmdz_2->SetMinimum(2e-4);
  // // htwmclmdz_2->SetMaximum(3e-4);
  // htwmclmdz_2->Draw();
  // c11.cd(6);
  // htwmclmdz->Draw("colz");
  c11.cd(4);
  htwrecmclmdz->FitSlicesY();
  htwrecmclmdz_1->GetYaxis()->SetTitle("tw_{MCLMD}-tw_{REC}, mean");
  htwrecmclmdz_2->GetYaxis()->SetTitle("tw_{MCLMD}-tw_{REC}, sigma");
  htwrecmclmdz_1->SetTitle("tw_{MCLMD}-tw_{REC}");
  htwrecmclmdz_2->SetTitle("");
  htwrecmclmdz_1->SetMinimum(-5e-5);
  htwrecmclmdz_1->SetMaximum(5e-5);
  htwrecmclmdz_1->Draw();
  c11.cd(5);
  htwrecmclmdz_2->SetMinimum(6e-5);
  htwrecmclmdz_2->SetMaximum(1.5e-4);
  htwrecmclmdz_2->Draw();
  c11.cd(6);
  htwrecmclmdz->Draw("colz");
   c11.Print(resname_pdf_o);
   c11.Write();
   c11.Close();

 // TCanvas c11a("twpar2");
 //  c11a.Divide(3,2);
 //  c11a.cd(1);
 //  hdtwrec->Draw("colz");
 //  c11a.cd(2);
 //  hdtwmclmd->Draw("colz");
 //  c11a.cd(3);
 //  hdtwmc->Draw("colz");
 //  c11a.cd(4);
 //  hdtwrec->FitSlicesY();
 //  hdtwrec_1->SetTitle("#sigma tw_{REC}, mean");
 //  hdtwrec_1->SetMinimum(0);
 //  hdtwrec_1->SetMaximum(5e-4);
 //  hdtwrec_1->Draw();
 //  c11a.cd(5);
 //  hdtwmclmd->FitSlicesY();
 //  hdtwmclmd_1->SetTitle("#sigma tw_{MCLMD}, mean");
 //  hdtwmclmd_1->SetMinimum(0);
 //  hdtwmclmd_1->SetMaximum(5e-4);
 //  hdtwmclmd_1->Draw();
 //  c11a.cd(6);
 //  hdtwmc->FitSlicesY();
 //  hdtwmc_1->SetTitle("#sigma tw_{MC}, mean");
 //  hdtwmc_1->SetMinimum(0);
 //  hdtwmc_1->SetMaximum(5e-4);
 //  hdtwmc_1->Draw();
 //  c11a.Print(resname_pdf_o);
 //  c11a.Write();
 //  c11a.Close();

  TCanvas c18zzz("vparZZZ");
  c18zzz.Divide(3,4);
  c18zzz.cd(1);
  hdvrec->Draw("colz");
  hdvrec->FitSlicesY();
  c18zzz.cd(2);
  hdvrec_1->SetTitle("#sigma v_{REC}, mean");
  hdvrec_1->SetMinimum(0);
  hdvrec_1->SetMaximum(0.2);
  hdvrec_1->Draw();
  c18zzz.cd(3);
  //  TH1D hvrec_vmc("hvrec_vmc",";z_{MC}, cm",45.,0,1200);//1.5
  //  hvrecz_2->Copy(hvrec_vmc);
  TH1  *hvrec_vmc = hvrecz_2;
  hvrec_vmc->Divide(hdvrec_1);
  hvrec_vmc->SetTitle("#sigma(v_{MC}-v_{REC})/#sigma(v_{REC})");
  hvrec_vmc->GetYaxis()->SetTitle("expect/estim");
  hvrec_vmc->GetXaxis()->SetTitle("z, cm");
  hvrec_vmc->SetMinimum(0);
  hvrec_vmc->SetMaximum(1.5);
  hvrec_vmc->Draw();
  c18zzz.cd(4);
  hdtvrec->Draw("colz");
  c18zzz.cd(5);
  hdtvrec->FitSlicesY();
  hdtvrec_1->SetTitle("#sigma tv_{REC}, mean");
  hdtvrec_1->SetMinimum(0);
  hdtvrec_1->SetMaximum(2e-4);
  hdtvrec_1->Draw();
  c18zzz.cd(6);
  TH1 *htvrec_tvmc =  htvrecz_2;
  htvrec_tvmc->Divide(hdtvrec_1);
  htvrec_tvmc->SetTitle("#sigma(tv_{MC}-tv_{REC})/#sigma(tv_{REC})");
  htvrec_tvmc->GetYaxis()->SetTitle("expect/estim");
  htvrec_tvmc->GetXaxis()->SetTitle("z, cm");
  htvrec_tvmc->SetMinimum(0);
  htvrec_tvmc->SetMaximum(1.5);
  htvrec_tvmc->Draw();
 c18zzz.cd(7);
 hdwrec->Draw("colz");
 c18zzz.cd(8);
 hdwrec->FitSlicesY();
 hdwrec_1->SetTitle("#sigma w_{REC}, mean");
 hdwrec_1->SetMinimum(0);
 hdwrec_1->SetMaximum(0.2);
 hdwrec_1->Draw();
  c18zzz.cd(9);
  TH1 *hwrec_wmc =  hwrecz_2;
  hwrec_wmc->Divide(hdwrec_1);
  hwrec_wmc->SetTitle("#sigma(w_{MC}-w_{REC})/#sigma(w_{REC})");
  hwrec_wmc->GetYaxis()->SetTitle("expect/estim");
  hwrec_wmc->GetXaxis()->SetTitle("z, cm");
  hwrec_wmc->SetMinimum(0);
  hwrec_wmc->SetMaximum(1.5);
  hwrec_wmc->Draw();
  c18zzz.cd(10);
  hdtwrec->Draw("colz");
  c18zzz.cd(11);
   hdtwrec->FitSlicesY();
  hdtwrec_1->SetTitle("#sigma tw_{REC}, mean");
  hdtwrec_1->SetMinimum(0);
  hdtwrec_1->SetMaximum(2e-4); 
  hdtwrec_1->Draw();
  c18zzz.cd(12);
  TH1 *htwrec_twmc = htwrecz_2;
  htwrec_twmc->Divide(hdtwrec_1);
  htwrec_twmc->SetTitle("#sigma(tw_{MC}-tw_{REC})/#sigma(tw_{REC})");
  htwrec_twmc->GetYaxis()->SetTitle("expect/estim");
  htwrec_twmc->GetXaxis()->SetTitle("z, cm");
  htwrec_twmc->SetMinimum(0);
  htwrec_twmc->SetMaximum(1.5);
  htwrec_twmc->Draw();
  c18zzz.Print(resname_pdf_o);
  c18zzz.Write();
  c18zzz.Close();

  TCanvas c19zzz("vparZZZ");
  c19zzz.Divide(2,2);
  c19zzz.cd(1);
  hvmclmdz->Draw("colz");
  c19zzz.cd(2);
  hwmclmdz->Draw("colz");
  c19zzz.cd(3);
  htvmclmdz->Draw("colz");
  c19zzz.cd(4);
  htwmclmdz->Draw("colz");
  //TString mcdiffname = storePath+"/MCip_vs_MClmd.pdf";
  //  c19zzz.SaveAs(mcdiffname);
  c19zzz.Print(resname_pdf_c);
  c19zzz.Write();
  c19zzz.Close();



 hwrecz->Write();
 hwmclmdz->Write();
 hwrecmclmdz->Write();
 hdwmc->Write();
 hdwmclmd->Write();
 hdwrec->Write();
 htvrecz->Write();
 htvmclmdz->Write();
 htvrecmclmdz->Write();
 hdtvmc->Write();
 hdtvmclmd->Write();
 hdtvrec->Write();
 htwrecz->Write();
 htwmclmdz->Write();
 htwrecmclmdz->Write();
 hdtwmc->Write();
 hdtwmclmd->Write();
 hdtwrec->Write();
 hvrecz->Write();
 hvmclmdz->Write();
 hvrecmclmdz->Write();
 hdvmc->Write();
 hdvmclmd->Write();
 hdvrec->Write();
 
  fout->Close();

}
