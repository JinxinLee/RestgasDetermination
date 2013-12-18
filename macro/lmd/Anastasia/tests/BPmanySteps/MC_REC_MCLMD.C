//void ShiftsBP(TString storePath="/panda/pandaroot/macro/lmd/testPixel_sensorsOnlyLMD_4_8_mrad_KalmanGEANE_Geant3_20_bendAngleForGEANE_NewMapsSolenoid_ERROR_PROP/mom_1_5/"){
//void ShiftsBP(TString storePath="/panda/pandaroot/macro/lmd/testPixel_FullLMD_4_8_mrad_0_360_degree_newMinuit_Geant3_20_bendAngleForGEANE_10_100stepsBP_MapsSolenoid_ERROR_PROP/mom_1_5/"){
void MC_REC_MCLMD(TString storePath="/panda/pandaroot/macro/lmd/testPixel_newMinuit_BPgeane_FullLMD_160steps_NOpipe_large/mom_1_5/"){

  //style ----------------------------------
  gROOT->Macro("/home/karavdina/Disser/materials/Style_Thesis.C");
  gROOT->SetStyle("Thesis");
  //(end) style---------------------------

  // ---- Input file ----------------------------------------------------------------
  TString in=storePath+"/Lumi_Geane_0.root";
  TFile *fin = new TFile(in,"READ");
  // ---------------------------------------------------------------------------------

 // ---- Output file ----------------------------------------------------------------
  TString out=storePath+"/plotsShiftsBP.root";
  TFile *fout = new TFile(out,"RECREATE");
  TString resname_pdf =  storePath+"/plotsShiftsBP.pdf";
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
  //  TString condition = "abs(prec-pmc)<pmc*1e-5 && abs(zmc-zrec)<5e-4 && abs(zmclmd-zmc)<5e-4 && tvrec_err<1.5e-4 && wrec_err<0.1"; //15 GeV
  //  TString condition = "abs(prec-pmc)<pmc*1e-4"; //1.5 GeV
  //    TString condition = "abs(prec-pmc)<pmc*2e-6 && abs(pmclmd-pmc)<pmc*2e-6"; //1.5 GeV
  //    TString condition = "abs(prec-pmc)<pmc*5e-6 && abs(pmclmd-pmc)<pmc*5e-6"; //1.5 GeV
  //TString condition = "abs(prec-pmc)<pmc*2e-3 && abs(pmclmd-pmc)<pmc*2e-3"; //1.5 GeV
  //  TString condition = "prec<100."; //1.5 GeV
  //  TString condition = "abs(prec-pmc)<pmc*1e-2 && abs(pmclmd-pmc)<pmc*1e-2"; //1.5 GeV
  //  TString condition = "abs(prec-pmc)<pmc*1e2 && abs(pmclmd-pmc)<pmc*1e2"; //1.5 GeV
  //    TString condition = "abs(prec-pmc)<pmc*2e-6 && abs(pmclmd-pmc)<pmc*2e-6 && abs(pmc-1.5)<2e-6"; //1.5 GeV, fixed th=6, ph=0
  //  TString condition = "abs(prec-pmc)<pmc*1e-5 && abs(zmclmd-zmc)<5e-4"; //15 GeV
  //  TString condition = "(prec-pmc)/pmc>0";
  TString condition = "";

  TString conditionLMD = "zmc>1110.";
  TString conditionIP = "zmc<0.01";
//    cout<<conditionLMD.Data()<<endl;


  double thlim = 20;//4.06GeV
  double phiresmax = 20;//4.06GeV
  //  double xlim = 0.1;//4.06GeV
  //  double xlim = 0.05;//4.06GeV
  double xlim = 10;//4.06GeV
  double philim = 0.05;//4.06GeV
  double thresmax=20;//4.06GeV
  double xresmax=1;//4.06GeV
  double momlim=10.;//4.06GeV
  double momresmax=10.;//15GeV 
  //MC trk from IP vs. REC -----------------------------------------------------------------------------------------------------------------------
  TH2D *hthz = new TH2D("hthz",";z_{MC}, cm;#theta_{MC}-#theta_{REC}, mrad",45.,0,1200,1e3,-thlim,thlim);//4.06
  //  TH2D *hthz = new TH2D("hthz",";z_{MC}, cm;#theta_{MC}-#theta_{REC}, mrad",45.,0,1200,3e3,-0.8,0.8);//15
  tbp->Project("hthz","1e3*(thetamc-thetarec):zmc",condition);
  hthz->FitSlicesY();
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

  hthz_1->Write();
 dip1.Write("dip1_th_mean");
  dip2.Write("dip2_th_mean");
  trans.Write("trans_th_mean");

  c1.cd(2);
  hthz_2->SetMinimum(0);
  // hthz_2->SetMaximum(thresmax);//4.06
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
  hthz_2->Write();

  c1.cd(3);
  hthz_chi2->Draw();
  c1.cd(4);
  hthz->Draw("colz");
  //  c1.Draw();
  hthz->Write();
  c1.Print(resname_pdf_o);
  c1.Write();
  c1.Close();
  double plim = 1;
  double presmax = 1;
  TH2D *hpmcrecz = new TH2D("hpmcrecz",";z_{MC}, cm;(P_{REC}-P_{MC})/P_{MC}",45.,0,1200,1e3,-0.5,1.5);//4.06
  tbp->Project("hpmcrecz","(prec-pmc)/pmc:zmc",condition);
  hpmcrecz->FitSlicesY();
  hpmcrecz_1->SetMinimum(-plim);
  hpmcrecz_1->SetMaximum(plim);
  hpmcrecz_1->GetYaxis()->SetTitle("(P_{REC}-P_{MC})/P_{MC}, mean");
  TCanvas cmomfb;
  cmomfb.Divide(2,2);
  cmomfb.cd(1);
  hpmcrecz_1->SetTitle("MC-REC");
  hpmcrecz_2->SetTitle("");
  hpmcrecz_chi2->SetTitle("");
   hpmcrecz_1->Draw();
   dip1.SetY1(-plim);
   dip1.SetY2(plim);
  
  dip1.Draw();
  dip2.SetY1(-plim);
  dip2.SetY2(plim);
  dip2.Draw();
  trans.SetY1(-plim);
  trans.SetY2(plim);
  trans.Draw();

  cmomfb.cd(2);
  hpmcrecz_2->SetMinimum(0);
  hpmcrecz_2->SetMaximum(presmax);//15
  hpmcrecz_2->GetYaxis()->SetTitle("(P_{REC}-P_{MC})/P_{MC}, sigma");
  hpmcrecz_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hpmcrecz_2->Draw();
  TBox dip1_s(560.,0.,660.,presmax);
  dip1_s.SetFillStyle(3017);
  dip1_s.SetFillColor(kBlue-6);
  TBox dip2_s(342.,0.,602.,presmax);
  dip2_s.SetFillStyle(3018);
  dip2_s.SetFillColor(kBlue-3);
  TBox trans_s(283,0.,343,presmax);
  trans_s.SetFillStyle(3003);
  trans_s.SetFillColor(kGreen-5);
  dip1_s.Draw();
  dip2_s.Draw();
  trans_s.Draw();
 
  cmomfb.cd(3);
  hpmcrecz_chi2->Draw();
  cmomfb.cd(4);
  hpmcrecz->Draw("colz");
  hpmcrecz->Write();
  hpmcrecz_1->Write();
  hpmcrecz_2->Write();
  cmomfb.Print(resname_pdf_o);
  cmomfb.Write();
  cmomfb.Close();


  //  TH2D *hxz = new TH2D("hxz",";z_{MC}, cm;x_{MC} - x_{REC}, cm",45.,0,1200,1e3,-5.,5.);//4.06
  TH2D *hxz = new TH2D("hxz",";z_{MC}, cm;x_{MC} - x_{REC}, cm",45.,0,1200,1e3,-xlim,xlim);//15
  tbp->Project("hxz","(xmc-xrec):zmc",condition);
  hxz->FitSlicesY();
  // hxz_1->SetMinimum(-0.1);//4.06
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
  hxz_1->Write();
  c2.cd(2);
  hxz_2->SetMinimum(0);
  //  hxz_2->SetMaximum(0.7);
  hxz_2->SetMaximum(xresmax);//15
  hxz_2->GetYaxis()->SetTitle("x_{MC}-x_{REC}, sigma [cm]");
  hxz_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hxz_2->Draw();
  hxz_2->Write();
  c2.cd(3);
  hxz_chi2->Draw();
  c2.cd(4);
  hxz->Draw("colz");
  //  c2.Draw();
  hxz->Write();
  c2.Print(resname_pdf_o);
  c2.Write();
  c2.Close();
  TH2D *hyz = new TH2D("hyz",";z_{MC}, cm;y_{MC} - y_{REC}, cm",45.,0,1200,1e3,-xlim,xlim);//1.5
  //  TH2D *hyz = new TH2D("hyz",";z_{MC}, cm;y_{MC} - y_{REC}, cm",45.,0,1200,1e3,-1.,1.);//15
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
  hyz_1->Write();
  c3.cd(2);
  hyz_2->SetMinimum(0);
  //  hyz_2->SetMaximum(0.7);//1.5
  hyz_2->SetMaximum(xresmax);//15
  hyz_2->GetYaxis()->SetTitle("y_{MC}-y_{REC}, sigma [cm]");
  hyz_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hyz_2->Draw();
  hyz_2->Write();
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
  hphz_1->SetMinimum(-philim);//15
  hphz_1->SetMaximum(philim);
  hphz_1->GetYaxis()->SetTitle("#phi_{MC}-#phi_{REC}, mean [cm]");
  TCanvas c5;
  c5.Divide(2,2);
  c5.cd(1);
  hphz_1->SetTitle("REC vs. MC (near IP)");
  hphz_2->SetTitle("");
  hphz_chi2->SetTitle("");
  hphz_1->Draw();
  hphz_1->Write();
  c5.cd(2);
  hphz_2->SetMinimum(0);
  //  hphz_2->SetMaximum(0.2);
  hphz_2->SetMaximum(phiresmax);//15
  hphz_2->GetYaxis()->SetTitle("#phi_{MC}-#phi_{REC}, sigma [cm]");
  hphz_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hphz_2->Draw();
  hphz_2->Write();
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
 

  // hmomz->FitSlicesY();
  // // hmomz_1->SetMinimum(-0.1);//1.5
  // // hmomz_1->SetMaximum(0.1);
  // hmomz_1->SetMinimum(-momlim);
  // hmomz_1->SetMaximum(momlim);
  // hmomz_1->GetYaxis()->SetTitle("mom_{MC}-mom_{REC}, mean [MeV]");
  // TCanvas c6;
  // c6.Divide(2,2);
  // c6.cd(1);
  // hmomz_1->SetTitle("REC vs. MC (near IP)");
  // hmomz_2->SetTitle("");
  // hmomz_chi2->SetTitle("");
  // hmomz_1->Draw();
  // c6.cd(2);
  // hmomz_2->SetMinimum(0);
  // //  hmomz_2->SetMaximum(0.5);//1.5
  // hmomz_2->SetMaximum(momresmax);
  // hmomz_2->GetYaxis()->SetTitle("mom_{MC}-mom_{REC}, sigma [MeV]");
  // hmomz_chi2->GetYaxis()->SetTitle("#chi^{2}");
  // hmomz_2->Draw();
  // c6.cd(3);
  // hmomz_chi2->Draw();
  // c6.cd(4);
  // hmomz->Draw("colz");
  // //  c6.Draw();
  
  // hmomz->Write();
  // c6.Print(resname_pdf_o);
  // c6.Write();
  // c6.Close();
 TH2D *hthmcz = new TH2D("hthmcz",";z_{MC}, cm;#theta_{MC}, mrad",45.,0,1200,6e2,-10.,50.);
  tbp->Project("hthmcz","1e3*(thetamc):zmc",condition);

  TH2D *hthrecz = new TH2D("hthrecz",";z_{MC}, cm;#theta_{REC}, mrad",45.,0,1200,6e2,-10.,50.);
  tbp->Project("hthrecz","1e3*(thetarec):zmc",condition);
  TCanvas c7;
  c7.Divide(3,3);
  c7.cd(1);
  //  TH2D *hmomrecz = new TH2D("hmomrecz",";z_{MC}, cm;mom_{REC}, GeV",45.,0,1200,2e2,14.,16.);//15
  TH2D *hmomrecz = new TH2D("hmomrecz",";z_{MC}, cm;mom_{REC}, GeV",45.,0,1200,2e2,1.4,1.6);//1.5
  tbp->Project("hmomrecz","prec:zmc",condition);
  hmomrecz->Draw("colz");
  hmomrecz->Write();
  c7.cd(2);
  //  TH2D *hmommcz = new TH2D("hmommcz",";z_{MC}, cm;mom_{MC}, GeV",45.,0,1200,2e2,14.,16.);//15
  TH2D *hmommcz = new TH2D("hmommcz",";z_{MC}, cm;mom_{MC}, GeV",45.,0,1200,2e2,1.4,1.6);//1.5
  tbp->Project("hmommcz","pmc:zmc",condition);
  hmommcz->Draw("colz");
  hmommcz->Write();
  c7.cd(3);
  //TH2D *hmommclmdz = new TH2D("hmommclmdz",";z_{MC}, cm;mom_{MCLMD}, GeV",45.,0,1200,2e2,14.,16.);//15
  TH2D *hmommclmdz = new TH2D("hmommclmdz",";z_{MC}, cm;mom_{MCLMD}, GeV",45.,0,1200,2e2,1.4,1.6);//1/5
  tbp->Project("hmommclmdz","pmclmd:zmc",condition);
  hmommclmdz->Draw("colz");
  hmommclmdz->Write();
  c7.cd(4);
  //  TH2D *hdiffmomz_lmd = new TH2D("hdiffmomz_lmd",";z_{MCLMD}, cm;mom_{MCLMD} - mom_{REC}, keV",45.,0,1200,4e2,-100.,100.);//15
  TH2D *hdiffmomz_lmd = new TH2D("hdiffmomz_lmd",";z_{MCLMD}, cm;mom_{MCLMD} - mom_{REC}, keV",45.,0,1200,4e2,-5.,5.);//1.5
  tbp->Project("hdiffmomz_lmd","1e6*(pmclmd-prec):zmc",condition);
  hdiffmomz_lmd->Draw("colz");
  hdiffmomz_lmd->Write();
  c7.cd(5);
  // TH2D *hdiffmomz_mc = new TH2D("hdiffmomz_mc",";z_{MCLMD}, cm;mom_{MCLMD} - mom_{MC}, keV",45.,0,1200,4e2,-100.,100.);//15
  TH2D *hdiffmomz_mc = new TH2D("hdiffmomz_mc",";z_{MCLMD}, cm;mom_{MCLMD} - mom_{MC}, keV",45.,0,1200,4e2,-5.,5.);//1.5
  tbp->Project("hdiffmomz_mc","1e6*(pmclmd-pmc):zmc","thetamc!=0 && abs(zmc-zmc)<100.");
  hdiffmomz_mc->Draw("colz");
  hdiffmomz_mc->Write();
  c7.cd(6);
  // TH2D *hdiffmomz = new TH2D("hdiffmomz",";z_{MC}, cm;mom_{MC} - mom_{REC}, keV",45.,0,1200,4e2,-100.,100.);//15
  TH2D *hdiffmomz = new TH2D("hdiffmomz",";z_{MC}, cm;mom_{MC} - mom_{REC}, keV",45.,0,1200,4e2,-5.,5.);//1.5
  tbp->Project("hdiffmomz","1e6*(pmc-prec):zmc",condition);
  hdiffmomz->Draw("colz");
  hdiffmomz->Write();
  c7.cd(7);
  TH2D *hbxz = new TH2D("hbxz",";z_{MC}, cm;Bx, kG",45.,0,1200,2e2,-0.1,0.1);
  tbp->Project("hbxz","Bx:zmc",condition);
  hbxz->Draw("colz");
  c7.cd(8);
  TH2D *hbyz = new TH2D("hbyz",";z_{MC}, cm;By, kG",45.,0,1200,2e2,-1.5,1.5);//1.5
  //  TH2D *hbyz = new TH2D("hbyz",";z_{MC}, cm;By, kG",45.,0,1200,2e2,-10.5,10.5);//15
  tbp->Project("hbyz","By:zmc",condition);
  hbyz->Draw("colz");
  c7.cd(9);
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
  TH2D *hthz_lmd = new TH2D("hthz_lmd",";z_{MC}, cm;#theta_{MCLMD}-#theta_{REC}, mrad",45.,0,1200,1e3,-5.,5.);//1.5
  //  TH2D *hthz_lmd = new TH2D("hthz_lmd",";z_{MC}, cm;#theta_{MCLMD}-#theta_{REC}, mrad",45.,0,1200,1e3,-1.,1.);//15
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
  hthz_lmd_1->Write();
  dip1.Write("dip1_th_mean_mclmdrec");
  dip2.Write("dip2_th_mean_mclmdrec");
  trans.Write("trans_th_mean_mclmdrec");
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
  hthz_lmd_2->Write();
  c1_lmd.cd(3);
  hthz_lmd_chi2->Draw();
  c1_lmd.cd(4);
  hthz_lmd->Draw("colz");
  //  c1_lmd.Draw();

  hthz_lmd->Write();
  c1_lmd.Print(resname_pdf_o);
  c1_lmd.Write();
  c1_lmd.Close();
  TH2D *hxz_lmd = new TH2D("hxz_lmd",";z_{MC}, cm;x_{MCLMD} - x_{REC}, cm",45.,0,1200,1e3,-5.,5.);//1.5
  //  TH2D *hxz_lmd = new TH2D("hxz_lmd",";z_{MCLMD}, cm;x_{MCLMD} - x_{REC}, cm",45.,0,1200,1e3,-1.,1.);//15
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
  hxz_lmd_1->Write();
  c2_lmd.cd(2);
  hxz_lmd_2->SetMinimum(0);
  //  hxz_lmd_2->SetMaximum(0.7);
  hxz_lmd_2->SetMaximum(xresmax);//15
  hxz_lmd_2->GetYaxis()->SetTitle("x_{MCLMD}-x_{REC}, sigma [cm]");
  hxz_lmd_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hxz_lmd_2->Draw();
  hxz_lmd_2->Write();
  c2_lmd.cd(3);
  hxz_lmd_chi2->Draw();
  c2_lmd.cd(4);
  hxz_lmd->Draw("colz");
  //  c2_lmd.Draw();

  hxz_lmd->Write();
  c2_lmd.Print(resname_pdf_o);
  c2_lmd.Write();
  c2_lmd.Close();
  TH2D *hyz_lmd = new TH2D("hyz_lmd",";z_{MC}, cm;y_{MCLMD} - y_{REC}, cm",45.,0,1200,1e3,-5.,5.);//1.5
  //  TH2D *hyz_lmd = new TH2D("hyz_lmd",";z_{MCLMD}, cm;y_{MCLMD} - y_{REC}, cm",45.,0,1200,1e3,-1.,1.);//15
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
  hyz_lmd_1->Write();
  c3_lmd.cd(2);
  hyz_lmd_2->SetMinimum(0);
  //  hyz_lmd_2->SetMaximum(0.7);//1.5
  hyz_lmd_2->SetMaximum(xresmax);//15
  hyz_lmd_2->GetYaxis()->SetTitle("y_{MCLMD}-y_{REC}, sigma [cm]");
  hyz_lmd_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hyz_lmd_2->Draw();
  hyz_lmd_2->Write();
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
  hphz_lmd_1->SetMinimum(-philim);//15
  hphz_lmd_1->SetMaximum(philim);
  hphz_lmd_1->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{REC}, mean [cm]");
  TCanvas c5_lmd;
  c5_lmd.Divide(2,2);
  c5_lmd.cd(1);
  hphz_lmd_1->SetTitle("REC vs. MC (near LMD)");
  hphz_lmd_2->SetTitle("");
  hphz_lmd_chi2->SetTitle("");
  hphz_lmd_1->Draw();
  hphz_lmd_1->Write();
  c5_lmd.cd(2);
  hphz_lmd_2->SetMinimum(0);
  //  hphz_lmd_2->SetMaximum(0.2);
  hphz_lmd_2->SetMaximum(phiresmax);//15
  hphz_lmd_2->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{REC}, sigma [cm]");
  hphz_lmd_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hphz_lmd_2->Draw();
  hphz_lmd_2->Write();
  c5_lmd.cd(3);
  hphz_lmd_chi2->Draw();
  c5_lmd.cd(4);
  hphz_lmd->Draw("colz");
  //  c5_lmd.Draw();

  hphz_lmd->Write();
  c5_lmd.Print(resname_pdf_o);
  c5_lmd.Write();
  c5_lmd.Close();
  // //  TH2D *hmomz_lmd = new TH2D("hmomz_lmd",";z_{MC}, cm;mom_{MCLMD} - mom_{REC}, MeV",45.,0,1200,2e2,-1.,1.);//1.5
 
  // hmomz_lmd->FitSlicesY();
  // // hmomz_lmd_1->SetMinimum(-0.1);//1.5
  // // hmomz_lmd_1->SetMaximum(0.1);
  // hmomz_lmd_1->SetMinimum(-momlim);
  // hmomz_lmd_1->SetMaximum(momlim);
  // hmomz_lmd_1->GetYaxis()->SetTitle("mom_{MCLMD}-mom_{REC}, mean [MeV]");
  // TCanvas c6_lmd;
  // c6_lmd.Divide(2,2);
  // c6_lmd.cd(1);
  // hmomz_lmd_1->SetTitle("REC vs. MC (near LMD)");
  // hmomz_lmd_2->SetTitle("");
  // hmomz_lmd_chi2->SetTitle("");
  // hmomz_lmd_1->Draw();
  // c6_lmd.cd(2);
  // hmomz_lmd_2->SetMinimum(0);
  // //  hmomz_lmd_2->SetMaximum(0.5);//1.5
  // hmomz_lmd_2->SetMaximum(momresmax);
  // hmomz_lmd_2->GetYaxis()->SetTitle("mom_{MCLMD}-mom_{REC}, sigma [MeV]");
  // hmomz_lmd_chi2->GetYaxis()->SetTitle("#chi^{2}");
  // hmomz_lmd_2->Draw();
  // c6_lmd.cd(3);
  // hmomz_lmd_chi2->Draw();
  // c6_lmd.cd(4);
  // hmomz_lmd->Draw("colz");
  // //  c6_lmd.Draw();

  // hmomz_lmd->Write();
  // c6_lmd.Print(resname_pdf_o);
  // c6_lmd.Write();
  // c6_lmd.Close();
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
  TH2D *hthz_mc = new TH2D("hthz_mc",";z_{MC}, cm;#theta_{MCLMD}-#theta_{MC}, mrad",45.,0,1200,1e3,-5.,5.);//1.5
  //  TH2D *hthz_mc = new TH2D("hthz_mc",";z_{MC}, cm;#theta_{MCLMD}-#theta_{MC}, mrad",45.,0,1200,1e3,-1.,1.);//15
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
  hthz_mc_1->Write();
  dip1.Write("dip1_th_mean_mclmd_mc");
  dip2.Write("dip2_th_mean_mclmd_mc");
  trans.Write("trans_th_mean_mclmd_mc");
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
  hthz_mc_2->Write();
  c1_mc.cd(3);
  hthz_mc_chi2->Draw();
  c1_mc.cd(4);
  hthz_mc->Draw("colz");
  //  c1_mc.Draw();

  hthz_mc->Write();
  c1_mc.Print(resname_pdf_o);
  c1_mc.Write();
  c1_mc.Close();
  TH2D *hxz_mc = new TH2D("hxz_mc",";z_{MC}, cm;x_{MCLMD} - x_{MC}, cm",45.,0,1200,1e3,-5.,5.);//1.5
  //  TH2D *hxz_mc = new TH2D("hxz_mc",";z_{MCLMD}, cm;x_{MCLMD} - x_{MC}, cm",45.,0,1200,1e3,-1.,1.);//15
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
  hxz_mc_1->Write();
  c2_mc.cd(2);
  hxz_mc_2->SetMinimum(0);
  //  hxz_mc_2->SetMaximum(0.7);
  hxz_mc_2->SetMaximum(xresmax);//15
  hxz_mc_2->GetYaxis()->SetTitle("x_{MCLMD}-x_{MC}, sigma [cm]");
  hxz_mc_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hxz_mc_2->Draw();
  hxz_mc_2->Write();
  c2_mc.cd(3);
  hxz_mc_chi2->Draw();
  c2_mc.cd(4);
  hxz_mc->Draw("colz");
  //  c2_mc.Draw();

  hxz_mc->Write();
  c2_mc.Print(resname_pdf_o);
  c2_mc.Write();
  c2_mc.Close();
  TH2D *hyz_mc = new TH2D("hyz_mc",";z_{MC}, cm;y_{MCLMD} - y_{MC}, cm",45.,0,1200,1e3,-5.,5.);//1.5
  //  TH2D *hyz_mc = new TH2D("hyz_mc",";z_{MCLMD}, cm;y_{MCLMD} - y_{MC}, cm",45.,0,1200,1e3,-1.,1.);//15
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
  hyz_mc_1->Write();
  c3_mc.cd(2);
  hyz_mc_2->SetMinimum(0);
  //  hyz_mc_2->SetMaximum(0.7);//1.5
  hyz_mc_2->SetMaximum(xresmax);//15
  hyz_mc_2->GetYaxis()->SetTitle("y_{MCLMD}-y_{MC}, sigma [cm]");
  hyz_mc_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hyz_mc_2->Draw();
  hyz_mc_2->Write();
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
  hphz_mc_1->SetMinimum(-philim);//15
  hphz_mc_1->SetMaximum(philim);
  hphz_mc_1->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{MC}, mean [cm]");
  TCanvas c5_mc;
  c5_mc.Divide(2,2);
  c5_mc.cd(1);
  hphz_mc_1->SetTitle("MC(near LMD) vs. MC (near IP)");
  hphz_mc_2->SetTitle("");
  hphz_mc_chi2->SetTitle("");
  hphz_mc_1->Draw();
  hphz_mc_1->Write();
  c5_mc.cd(2);
  hphz_mc_2->SetMinimum(0);
  //  hphz_mc_2->SetMaximum(0.2);
  hphz_mc_2->SetMaximum(phiresmax);//15
  hphz_mc_2->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{MC}, sigma [cm]");
  hphz_mc_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hphz_mc_2->Draw();
  hphz_mc_2->Write();
  c5_mc.cd(3);
  hphz_mc_chi2->Draw();
  c5_mc.cd(4);
  hphz_mc->Draw("colz");
  //  c5_mc.Draw();

  hphz_mc->Write();
  c5_mc.Print(resname_pdf_c);
  c5_mc.Write();
  c5_mc.Close();
//   //  TH2D *hmomz_mc = new TH2D("hmomz_mc",";z_{MC}, cm;mom_{MCLMD} - mom_{MC}, MeV",45.,0,1200,2e2,-1.,1.);//1.5
 
//   // hmomz_mc->FitSlicesY();
//   // // hmomz_mc_1->SetMinimum(-0.1);//1.5
//   // // hmomz_mc_1->SetMaximum(0.1);
//   // hmomz_mc_1->SetMinimum(-momlim);
//   // hmomz_mc_1->SetMaximum(momlim);
//   // hmomz_mc_1->GetYaxis()->SetTitle("mom_{MCLMD}-mom_{MC}, mean [MeV]");
//   // TCanvas c6_mc;
//   // c6_mc.Divide(2,2);
//   // c6_mc.cd(1);
//   // hmomz_mc_1->SetTitle("MC(near LMD) vs. MC (near IP)");
//   // hmomz_mc_2->SetTitle("");
//   // hmomz_mc_chi2->SetTitle("");
//   // hmomz_mc_1->Draw();
//   // c6_mc.cd(2);
//   // hmomz_mc_2->SetMinimum(0);
//   // //  hmomz_mc_2->SetMaximum(0.5);//1.5
//   // hmomz_mc_2->SetMaximum(momresmax);
//   // hmomz_mc_2->GetYaxis()->SetTitle("mom_{MCLMD}-mom_{MC}, sigma [MeV]");
//   // hmomz_mc_chi2->GetYaxis()->SetTitle("#chi^{2}");
//   // hmomz_mc_2->Draw();
//   // c6_mc.cd(3);
//   // hmomz_mc_chi2->Draw();
//   // c6_mc.cd(4);
//   // hmomz_mc->Draw("colz");
//   // //  c6_mc.Draw();

//   // hmomz_mc->Write();
//   // //  c6_mc.Print(resname_pdf_c);
//   // c6_mc.Print(resname_pdf_o);
//   // c6_mc.Write();
//   // c6_mc.Close();
//   //END:MC trk from LMD vs. MC in IP -----------------------------------------------------------------------------------------------------------------------

  fout->Close();


}
