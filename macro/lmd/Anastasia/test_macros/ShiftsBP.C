//void ShiftsBP(TString storePath="/panda/pandaroot/macro/lmd/testPixel_sensorsOnlyLMD_4_8_mrad_KalmanGEANE_Geant3_20_bendAngleForGEANE_NewMapsSolenoid_ERROR_PROP/mom_1_5/"){
//void ShiftsBP(TString storePath="/panda/pandaroot/macro/lmd/testPixel_FullLMD_4_8_mrad_0_360_degree_newMinuit_Geant3_20_bendAngleForGEANE_10_100stepsBP_MapsSolenoid_ERROR_PROP/mom_1_5/"){
void ShiftsBP(TString storePath="/panda/pandaroot/macro/lmd/testPixel_newMinuit_BPgeane_FullLMD_160steps_NOpipe_large/mom_1_5/"){

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
  TString condition = "abs(prec-pmc)<pmc*1e2 && abs(pmclmd-pmc)<pmc*1e2"; //1.5 GeV
  //    TString condition = "abs(prec-pmc)<pmc*2e-6 && abs(pmclmd-pmc)<pmc*2e-6 && abs(pmc-1.5)<2e-6"; //1.5 GeV, fixed th=6, ph=0
  //  TString condition = "abs(prec-pmc)<pmc*1e-5 && abs(zmclmd-zmc)<5e-4"; //15 GeV



//   // and ad v,w,v',w' params for track representation
  double vsigmamax = 2*0.5;//1.5
  double tvsigmamax = 3e-3;//1.5
  double vlim = 2.;//1.5
  double tvlim = 5*0.001;//1.5
  //v
  TH2D *hvrecz = new TH2D("hvrecz",";z_{MC}, cm; v_{MC} - v_{REC}",45.,0,1200,2e2,-vlim,vlim);//1.5
  //  tbp->Project("hvrecz","-vrec+vmc:zmc",condition);
  tbp->Project("hvrecz","-vrec+vmc:zmc",condition);
  TH2D *hvmclmdz = new TH2D("hvmclmdz",";z_{MC}, cm; v_{MC} - v_{MCLMD}",45.,0,1200,2e2,-vlim,vlim);//1.5
  //  tbp->Project("hvmclmdz","-vmclmd+vmc:zmc","thetarec!=0 && abs(zmc-zrec)<100. && vmclmd_err==0 && vmclmd_err==0");
  tbp->Project("hvmclmdz","-vmclmd+vmc:zmc",condition);
  TH2D *hvrecmclmdz = new TH2D("hvrecmclmdz",";z_{MC}, cm; v_{MCLMD}-v_{REC}",45.,0,1200,2e2,-vlim,vlim);//1.5
  //  tbp->Project("hvrecmclmdz","-vrec+vmclmd:zmc","thetarec!=0 && abs(zmc-zrec)<100. && vmclmd_err==0 && vmclmd_err==0");
  tbp->Project("hvrecmclmdz","-vrec+vmclmd:zmc",condition);
  TH2D *hdvrec = new TH2D("hdvrec",";z_{MC}, cm; #sigma v_{REC}",45.,0,1200,4e2,7e-3,vsigmamax);//1.5
  //  tbp->Project("hdvrec","vrec_err:zmc","thetarec!=0 && abs(zmc-zrec)<100. && vmclmd_err==0 && vmclmd_err==0");
  tbp->Project("hdvrec","vrec_err:zmc",condition);

  //pulls v
  TH2D *hvreczpull = new TH2D("hvreczpull",";z_{MC}, cm; (v_{MC} - v_{REC})/#sigma_{v_{REC}}",45.,0,1200,2e2,-10,10);//1.5
  tbp->Project("hvreczpull","(-vrec+vmc)/vrec_err:zmc",condition);
  TH2D *hvmclmdzpull = new TH2D("hvmclmdzpull",";z_{MC}, cm; (v_{MC} - v_{MCLMD})/#sigma_{v_{REC}}",45.,0,1200,2e2,-10,10);//1.5
  tbp->Project("hvmclmdzpull","(-vmclmd+vmc)/vrec_err:zmc",condition);
  TH2D *hvrecmclmdzpull = new TH2D("hvrecmclmdzpull",";z_{MC}, cm; (v_{MCLMD}-v_{REC})/#sigma_{v_{REC}}",45.,0,1200,2e2,-10,10);//1.5
  tbp->Project("hvrecmclmdzpull","(-vrec+vmclmd)/vrec_err:zmc",condition);

  TCanvas c8("vpar");
  c8.Divide(3,4);
  c8.cd(1);
  hvrecz->FitSlicesY();
  hvrecz_1->GetYaxis()->SetTitle("v_{MC}-v_{REC}, mean");
  hvrecz_2->GetYaxis()->SetTitle("v_{MC}-v_{REC}, sigma");
  hvrecz_1->SetTitle("v_{MC}-v_{REC}");
  hvrecz_2->SetTitle("");
  hvrecz_1->SetMinimum(-7e-3);
  hvrecz_1->SetMaximum(7e-3);
  hvrecz_1->Draw();
  c8.cd(2);
  hvrecz_2->SetMinimum(0.);
  hvrecz_2->SetMaximum(vsigmamax);
  hvrecz_2->Draw();
  c8.cd(3);
  hvrecz->Draw("colz");
  c8.cd(4);
  hvrecmclmdz->FitSlicesY();
  hvrecmclmdz_1->GetYaxis()->SetTitle("v_{MCLMD}-v_{REC}, mean");
  hvrecmclmdz_2->GetYaxis()->SetTitle("v_{MCLMD}-v_{REC}, sigma");
  hvrecmclmdz_1->SetTitle("v_{MCLMD}-v_{REC}");
  hvrecmclmdz_2->SetTitle("");
  hvrecmclmdz_1->SetMinimum(-7e-3);
  hvrecmclmdz_1->SetMaximum(7e-3);
  hvrecmclmdz_1->Draw();
  c8.cd(5);
  hvrecmclmdz_2->SetMinimum(0.);
  hvrecmclmdz_2->SetMaximum(vsigmamax);
  hvrecmclmdz_2->Draw();
  c8.cd(6);
  hvrecmclmdz->Draw("colz");

  c8.cd(7);
  hvreczpull->FitSlicesY();
  hvreczpull_1->GetYaxis()->SetTitle("(v_{MC}-v_{REC})/#sigma_{v_{REC}}, mean");
  hvreczpull_2->GetYaxis()->SetTitle("(v_{MC}-v_{REC})#sigma_{v_{REC}}, sigma");
  hvreczpull_1->SetTitle("(v_{MC}-v_{REC})/#sigma_{v_{REC}}");
  hvreczpull_2->SetTitle("");
  hvreczpull_1->SetMinimum(-8e-3);
  hvreczpull_1->SetMaximum(8e-3);
  hvreczpull_1->Draw();
  c8.cd(8);
  hvreczpull_2->SetMinimum(0.);
  hvreczpull_2->SetMaximum(1.1);
  hvreczpull_2->Draw();
  c8.cd(9);
  hvreczpull->Draw("colz");
  c8.cd(10);
  hvrecmclmdzpull->FitSlicesY();
  hvrecmclmdzpull_1->GetYaxis()->SetTitle("(v_{MCLMD}-v_{REC})/#sigma_{v_{REC}}, mean");
  hvrecmclmdzpull_2->GetYaxis()->SetTitle("(v_{MCLMD}-v_{REC})/#sigma_{v_{REC}}, sigma");
  hvrecmclmdzpull_1->SetTitle("(v_{MCLMD}-v_{REC})/#sigma_{v_{REC}}");
  hvrecmclmdzpull_2->SetTitle("");
  hvrecmclmdzpull_1->SetMinimum(-8e-3);
  hvrecmclmdzpull_1->SetMaximum(8e-3);
  hvrecmclmdzpull_1->Draw();
  c8.cd(11);
  hvrecmclmdzpull_2->SetMinimum(0.);
  hvrecmclmdzpull_2->SetMaximum(1.1);
  hvrecmclmdzpull_2->Draw();
  c8.cd(12);
  hvrecmclmdzpull->Draw("colz");
 c8.Print(resname_pdf_o);
  c8.Write();
  c8.Close();



  
  //v'
  TH2D *htvrecz = new TH2D("htvrecz",";z_{MC}, cm; tv_{MC} - tv_{REC}",45.,0,1200,2e2,-0.002,0.002);//1.5
  tbp->Project("htvrecz","-tvrec+tvmc:zmc",condition);
  TH2D *htvmclmdz = new TH2D("htvmclmdz",";z_{MC}, cm; tv_{MC} - tv_{MCLMD}",45.,0,1200,2e2,-tvlim,tvlim);//1.5
  tbp->Project("htvmclmdz","-tvmclmd+tvmc:zmc",condition);
  TH2D *htvrecmclmdz = new TH2D("htvrecmclmdz",";z_{MC}, cm; tv_{MCLMD}-tv_{REC}",45.,0,1200,2e2,-0.002,0.002);//1.5
  tbp->Project("htvrecmclmdz","-tvrec+tvmclmd:zmc",condition);
 TH2D *htvreczpull = new TH2D("htvreczpull",";z_{MC}, cm; (tv_{MC} - tv_{REC})/#sigma_{tv_{REC}}",45.,0,1200,2e2,-10.,10.);//1.5
  tbp->Project("htvreczpull","(-tvrec+tvmc)/tvrec_err:zmc",condition);
  TH2D *htvrecmclmdzpull = new TH2D("htvrecmclmdzpull",";z_{MC}, cm; (tv_{MCLMD}-tv_{REC})/#sigma_{tv_{REC}}",45.,0,1200,2e2,-10.,10);//1.5
  tbp->Project("htvrecmclmdzpull","(-tvrec+tvmclmd)/tvrec_err:zmc",condition);

  // TH2D *hdtvmc = new TH2D("hdtvmc",";z_{MC}, cm; #sigma tv_{MC}",45.,0,1200,2e2,0,5e-4);//1.5
  // //  tbp->Project("hdtvmc","tvmc_err:zmc","thetarec!=0 && abs(zmc-zrec)<100. && vmclmd_err==0 && vmclmd_err==0");
  // tbp->Project("hdtvmc","tvmc_err:zmc",condition);
  // TH2D *hdtvmclmd = new TH2D("hdtvmclmd",";z_{MC}, cm; #sigma tv_{MCLMD}",45.,0,1200,2e2,0,5e-4);//1.5
  // //  tbp->Project("hdtvmclmd","tvmclmd_err:zmc",condition);
  // tbp->Project("hdtvmclmd","tvmclmd_err:zmc",condition);
  TH2D *hdtvrec = new TH2D("hdtvrec",";z_{MC}, cm; #sigma tv_{REC}",45.,0,1200,1e2,0.05*tvsigmamax,tvsigmamax);//1.5
  //  tbp->Project("hdtvrec","tvrec_err:zmc",condition);
  tbp->Project("hdtvrec","tvrec_err:zmc",condition);
  TCanvas c10("tvpar");
  c10.Divide(3,4);
  c10.cd(1);
  htvrecz->FitSlicesY();
  htvrecz_1->GetYaxis()->SetTitle("v'_{MC}-v'_{REC}, mean");
  htvrecz_2->GetYaxis()->SetTitle("v'_{MC}-v'_{REC}, sigma");
  htvrecz_1->SetTitle("v'_{MC}-v'_{REC}");
  htvrecz_2->SetTitle("");
  htvrecz_1->SetMinimum(-2e-5);
  htvrecz_1->SetMaximum(2e-5);
  htvrecz_1->Draw();
  c10.cd(2);
  htvrecz_2->SetMinimum(0);
  htvrecz_2->SetMaximum(1.2*tvsigmamax);
  htvrecz_2->Draw();
  c10.cd(3);
  htvrecz->Draw("colz");
   c10.cd(4);
  htvrecmclmdz->FitSlicesY();
  htvrecmclmdz_1->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{REC}, mean");
  htvrecmclmdz_2->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{REC}, sigma");
  htvrecmclmdz_1->SetTitle("v'_{MCLMD}-v'_{REC}");
  htvrecmclmdz_2->SetTitle("");
  htvrecmclmdz_1->SetMinimum(-2e-5);
  htvrecmclmdz_1->SetMaximum(2e-5);
  htvrecmclmdz_1->Draw();
  c10.cd(5);
  htvrecmclmdz_2->SetMinimum(0);
  htvrecmclmdz_2->SetMaximum(1.2*tvsigmamax);
  htvrecmclmdz_2->Draw();
  c10.cd(6);
  htvrecmclmdz->Draw("colz");
  c10.cd(7);
  htvreczpull->FitSlicesY();
  htvreczpull_1->GetYaxis()->SetTitle("(v'_{MC}-v'_{REC})/#sigma_{v'_{REC}}, mean");
  htvreczpull_2->GetYaxis()->SetTitle("v'_{MC}-v'_{REC}/#sigma_{v'_{REC}}, sigma");
  htvreczpull_1->SetTitle("(v'_{MC}-v'_{REC})/#sigma_{v'_{REC}}");
  htvreczpull_2->SetTitle("");
  htvreczpull_1->SetMinimum(-8e-3);
  htvreczpull_1->SetMaximum(8e-3);
  htvreczpull_1->Draw();
  c10.cd(8);
  htvreczpull_2->SetMinimum(0);
  htvreczpull_2->SetMaximum(1.1);
  htvreczpull_2->Draw();
  c10.cd(9);
  htvreczpull->Draw("colz");
   c10.cd(10);
  htvrecmclmdzpull->FitSlicesY();
  htvrecmclmdzpull_1->GetYaxis()->SetTitle("(v'_{MCLMD}-v'_{REC})/#sigma_{v'_{REC}}, mean");
  htvrecmclmdzpull_2->GetYaxis()->SetTitle("(v'_{MCLMD}-v'_{REC})/#sigma_{v'_{REC}}, sigma");
  htvrecmclmdzpull_1->SetTitle("(v'_{MCLMD}-v'_{REC})/#sigma_{v'_{REC}}");
  htvrecmclmdzpull_2->SetTitle("");
  htvrecmclmdzpull_1->SetMinimum(-8e-3);
  htvrecmclmdzpull_1->SetMaximum(8e-3);
  htvrecmclmdzpull_1->Draw();
  c10.cd(11);
  htvrecmclmdzpull_2->SetMinimum(0);
  htvrecmclmdzpull_2->SetMaximum(1.1);
  htvrecmclmdzpull_2->Draw();
  c10.cd(12);
  htvrecmclmdzpull->Draw("colz");

  c10.Print(resname_pdf_o);
  c10.Write();
  c10.Close();

  //w
  TH2D *hwrecz = new TH2D("hwrecz",";z_{MC}, cm; w_{MC} - w_{REC}",45.,0,1200,2e2,-vlim,vlim);//1.5
  //  tbp->Project("hwrecz","-wrec+wmc:zmc",condition);
  tbp->Project("hwrecz","-wrec+wmc:zmc",condition);
  TH2D *hwmclmdz = new TH2D("hwmclmdz",";z_{MC}, cm; w_{MC} - w_{MCLMD}",45.,0,1200,2e2,-vlim,vlim);//1.5
  //  tbp->Project("hwmclmdz","-wmclmd+wmc:zmc",condition);
  tbp->Project("hwmclmdz","-wmclmd+wmc:zmc",condition);
  TH2D *hwrecmclmdz = new TH2D("hwrecmclmdz",";z_{MC}, cm; w_{MCLMD}-w_{REC}",45.,0,1200,1e3,-vlim,vlim);//1.5
  //  tbp->Project("hwrecmclmdz","-wrec+wmclmd:zmc",condition);
  tbp->Project("hwrecmclmdz","-wrec+wmclmd:zmc",condition);
  // TH2D *hdwmc = new TH2D("hdwmc",";z_{MC}, cm; #sigma w_{MC}",45.,0,1200,2e2,0,vlim);//1.5
  // //  tbp->Project("hdwmc","wmc_err:zmc",condition);
  // tbp->Project("hdwmc","wmc_err:zmc",condition);
  // TH2D *hdwmclmd = new TH2D("hdwmclmd",";z_{MC}, cm; #sigma w_{MCLMD}",45.,0,1200,2e2,0,vlim);//1.5
  // //  tbp->Project("hdwmclmd","wmclmd_err:zmc",condition);
  // tbp->Project("hdwmclmd","wmclmd_err:zmc",condition);
  TH2D *hdwrec = new TH2D("hdwrec",";z_{MC}, cm; #sigma w_{REC}",45.,0,1200,4e2,7e-3,vsigmamax);//1.5
  //  tbp->Project("hdwrec","wrec_err:zmc",condition);
  tbp->Project("hdwrec","wrec_err:zmc",condition);

  //pulls w
  TH2D *hwreczpull = new TH2D("hwreczpull",";z_{MC}, cm; (w_{MC} - w_{REC})/#sigma_{w_{REC}}",45.,0,1200,2e2,-10,10);//1.5
  tbp->Project("hwreczpull","(-wrec+wmc)/wrec_err:zmc",condition);
  TH2D *hwmclmdzpull = new TH2D("hwmclmdzpull",";z_{MC}, cm; (w_{MC} - w_{MCLMD})/#sigma_{w_{REC}}",45.,0,1200,2e2,-10,10);//1.5
  tbp->Project("hwmclmdzpull","(-wmclmd+wmc)/wrec_err:zmc",condition);
  TH2D *hwrecmclmdzpull = new TH2D("hwrecmclmdzpull",";z_{MC}, cm; (w_{MCLMD}-w_{REC})/#sigma_{w_{REC}}",45.,0,1200,2e2,-10,10);//1.5
  tbp->Project("hwrecmclmdzpull","(-wrec+wmclmd)/wrec_err:zmc",condition);

  TCanvas c9("wpar");
  c9.Divide(3,4);
  c9.cd(1);
  hwrecz->FitSlicesY();
  hwrecz_1->GetYaxis()->SetTitle("w_{MC}-w_{REC}, mean");
  hwrecz_2->GetYaxis()->SetTitle("w_{MC}-w_{REC}, sigma");
  hwrecz_1->SetTitle("w_{MC}-w_{REC}");
  hwrecz_2->SetTitle("");
  hwrecz_1->SetMinimum(-7e-3);
  hwrecz_1->SetMaximum(7e-3);
  hwrecz_1->Draw();
  c9.cd(2);
  hwrecz_2->SetMinimum(0);
  hwrecz_2->SetMaximum(vsigmamax);
  hwrecz_2->Draw();
  c9.cd(3);
  hwrecz->Draw("colz");
  c9.cd(4);
  hwrecmclmdz->FitSlicesY();
  hwrecmclmdz_1->GetYaxis()->SetTitle("w_{MCLMD}-w_{REC}, mean");
  hwrecmclmdz_2->GetYaxis()->SetTitle("w_{MCLMD}-w_{REC}, sigma");
  hwrecmclmdz_1->SetTitle("w_{MCLMD}-w_{REC}");
  hwrecmclmdz_2->SetTitle("");
  hwrecmclmdz_1->Draw();
  hwrecmclmdz_1->SetMinimum(-7e-3);
  hwrecmclmdz_1->SetMaximum(7e-3);
  c9.cd(5);
  hwrecmclmdz_2->SetMinimum(0);
  hwrecmclmdz_2->SetMaximum(vsigmamax);
  hwrecmclmdz_2->Draw();
  c9.cd(6);
  hwrecmclmdz->Draw("colz");
  c9.cd(7);
  hwreczpull->FitSlicesY();
  hwreczpull_1->GetYaxis()->SetTitle("(w_{MC}-w_{REC})/#sigma_{w_{REC}}, mean");
  hwreczpull_2->GetYaxis()->SetTitle("(w_{MC}-w_{REC})/#sigma_{w_{REC}}, sigma");
  hwreczpull_1->SetTitle("(w_{MC}-w_{REC})//#sigma_{w_{REC}}");
  hwreczpull_2->SetTitle("");
  hwreczpull_1->SetMinimum(-8e-3);
  hwreczpull_1->SetMaximum(8e-3);
  hwreczpull_1->Draw();
  c9.cd(8);
  hwreczpull_2->SetMinimum(0);
  hwreczpull_2->SetMaximum(1.1);
  hwreczpull_2->Draw();
  c9.cd(9);
  hwreczpull->Draw("colz");
  c9.cd(10);
  hwrecmclmdzpull->FitSlicesY();
  hwrecmclmdzpull_1->GetYaxis()->SetTitle("(w_{MCLMD}-w_{REC})/#sigma_{w_{REC}}, mean");
  hwrecmclmdzpull_2->GetYaxis()->SetTitle("(w_{MCLMD}-w_{REC})/#sigma_{w_{REC}}, sigma");
  hwrecmclmdzpull_1->SetTitle("(w_{MCLMD}-w_{REC})/#sigma_{w_{REC}}");
  hwrecmclmdzpull_2->SetTitle("");
  hwrecmclmdzpull_1->Draw();
  hwrecmclmdzpull_1->SetMinimum(-8e-3);
  hwrecmclmdzpull_1->SetMaximum(8e-3);
  c9.cd(11);
  hwrecmclmdzpull_2->SetMinimum(0);
  hwrecmclmdzpull_2->SetMaximum(1.1);
  hwrecmclmdzpull_2->Draw();
  c9.cd(12);
  hwrecmclmdzpull->Draw("colz");

 c9.Print(resname_pdf_o);
  c9.Write();
  c9.Close();


  //w'
  TH2D *htwrecz = new TH2D("htwrecz",";z_{MC}, cm; w'_{MC} - w'_{REC}",45.,0,1200,2e2,-0.002,0.002);//1.5
  //  tbp->Project("htwrecz","-twrec+twmc:zmc",condition);
  tbp->Project("htwrecz","-twrec+twmc:zmc",condition);
  TH2D *htwmclmdz = new TH2D("htwmclmdz",";z_{MC}, cm; w'_{MC} - w'_{MCLMD}",45.,0,1200,2e2,-tvlim,tvlim);//1.5
  //  tbp->Project("htwmclmdz","-twmclmd+twmc:zmc",condition);
  tbp->Project("htwmclmdz","-twmclmd+twmc:zmc",condition);
  TH2D *htwrecmclmdz = new TH2D("htwrecmclmdz",";z_{MC}, cm; w'_{MCLMD}-w'_{REC}",45.,0,1200,2e2,-0.002,0.002);//1.5
  tbp->Project("htwrecmclmdz","-twrec+twmclmd:zmc",condition);
  //   TH2D *hdtwmc = new TH2D("hdtwmc",";z_{MC}, cm; #sigma w'_{MC}",45.,0,1200,2e2,0,5e-4);//1.5
  // //  tbp->Project("hdtwmc","twmc_err:zmc",condition);
  // tbp->Project("hdtwmc","twmc_err:zmc",condition);
  // TH2D *hdtwmclmd = new TH2D("hdtwmclmd",";z_{MC}, cm; #sigma w'_{MCLMD}",45.,0,1200,2e2,0,5e-4);//1.5
  // //  tbp->Project("hdtwmclmd","twmclmd_err:zmc",condition);
  // tbp->Project("hdtwmclmd","twmclmd_err:zmc",condition);
  TH2D *hdtwrec = new TH2D("hdtwrec",";z_{MC}, cm; #sigma w'_{REC}",45.,0,1200,1e2,0.05*tvsigmamax,tvsigmamax);//1.5
  //  tbp->Project("hdtwrec","twrec_err:zmc",condition);
  tbp->Project("hdtwrec","twrec_err:zmc",condition);
 TH2D *htwreczpull = new TH2D("htwreczpull",";z_{MC}, cm; (tw_{MC} - tw_{REC})/#sigma_{tw_{REC}}",45.,0,1200,2e2,-10.,10.);//1.5
  tbp->Project("htwreczpull","(-twrec+twmc)/twrec_err:zmc",condition);
  TH2D *htwrecmclmdzpull = new TH2D("htwrecmclmdzpull",";z_{MC}, cm; (tw_{MCLMD}-tw_{REC})/#sigma_{tw_{REC}}",45.,0,1200,2e2,-10.,10);//1.5
  tbp->Project("htwrecmclmdzpull","(-twrec+twmclmd)/twrec_err:zmc",condition);
  TCanvas c11("twpar");
  c11.Divide(3,4);
  c11.cd(1);
  htwrecz->FitSlicesY();
  htwrecz_1->GetYaxis()->SetTitle("w'_{MC}-w'_{REC}, mean");
  htwrecz_2->GetYaxis()->SetTitle("w'_{MC}-w'_{REC}, sigma");
  htwrecz_1->SetTitle("w'_{MC}-w'_{REC}");
  htwrecz_2->SetTitle("");
  htwrecz_1->SetMinimum(-2e-5);
  htwrecz_1->SetMaximum(2e-5);
  htwrecz_1->Draw();
  c11.cd(2);
  htwrecz_2->SetMinimum(0);
  htwrecz_2->SetMaximum(1.2*tvsigmamax);
  htwrecz_2->Draw();
  c11.cd(3);
  htwrecz->Draw("colz");
  c11.cd(4);
  htwrecmclmdz->FitSlicesY();
  htwrecmclmdz_1->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{REC}, mean");
  htwrecmclmdz_2->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{REC}, sigma");
  htwrecmclmdz_1->SetTitle("w'_{MCLMD}-w'_{REC}");
  htwrecmclmdz_2->SetTitle("");
  htwrecmclmdz_1->SetMinimum(-2e-5);
  htwrecmclmdz_1->SetMaximum(2e-5);
  htwrecmclmdz_1->Draw();
  c11.cd(5);
  htwrecmclmdz_2->SetMinimum(0);
  htwrecmclmdz_2->SetMaximum(1.2*tvsigmamax);
  htwrecmclmdz_2->Draw();
  c11.cd(6);
  htwrecmclmdz->Draw("colz");
  c11.cd(7);
  htwreczpull->FitSlicesY();
  htwreczpull_1->GetYaxis()->SetTitle("(w'_{MC}-w'_{REC})/#sigma_{w'_{REC}}, mean");
  htwreczpull_2->GetYaxis()->SetTitle("(w'_{MC}-w'_{REC})/#sigma_{w'_{REC}}, sigma");
  htwreczpull_1->SetTitle("(w'_{MC}-w'_{REC})/#sigma_{w'_{REC}}");
  htwreczpull_2->SetTitle("");
  htwreczpull_1->SetMinimum(-8e-3);
  htwreczpull_1->SetMaximum(8e-3);
  htwreczpull_1->Draw();
  c11.cd(8);
  htwreczpull_2->SetMinimum(0);
  htwreczpull_2->SetMaximum(1.1);
  htwreczpull_2->Draw();
  c11.cd(9);
  htwreczpull->Draw("colz");
  c11.cd(10);
  htwrecmclmdzpull->FitSlicesY();
  htwrecmclmdzpull_1->GetYaxis()->SetTitle("(w'_{MCLMD}-w'_{REC})/#sigma_{w'_{REC}}, mean");
  htwrecmclmdzpull_2->GetYaxis()->SetTitle("(w'_{MCLMD}-w'_{REC})/#sigma_{w'_{REC}}, sigma");
  htwrecmclmdzpull_1->SetTitle("(w'_{MCLMD}-w'_{REC})/#sigma_{w'_{REC}}");
  htwrecmclmdzpull_2->SetTitle("");
  htwrecmclmdzpull_1->SetMinimum(-2e-1);
  htwrecmclmdzpull_1->SetMaximum(2e-1);
  htwrecmclmdzpull_1->Draw();
  c11.cd(11);
  htwrecmclmdzpull_2->SetMinimum(0);
  htwrecmclmdzpull_2->SetMaximum(1.1);
  htwrecmclmdzpull_2->Draw();
  c11.cd(12);
  htwrecmclmdzpull->Draw("colz");
   c11.Print(resname_pdf_o);
   c11.Write();
   c11.Close();

//    // TString conditionLMD = condition + " && zmc>1118.";
//    // TString conditionIP = condition + " && zmc<0.01";
  TString conditionLMD = "zmc>1110.";
  TString conditionIP = "zmc<0.01";
//    cout<<conditionLMD.Data()<<endl;

   TH2D *hresvmcrec_phi_LMD = new TH2D("hresvmcrec_phi_LMD",";#phi, rad;v_{MC} - v_{REC}, cm",5e1,-3.15,3.15,5e1,-0.05,0.05);
   TH2D *hresvmcrec_phi_IP = new TH2D("hresvmcrec_phi_IP",";#phi, rad;v_{MC} - v_{REC}, cm",5e1,-3.15,3.15,5e1,-5,5);
   tbp->Project("hresvmcrec_phi_LMD","(vmc-vrec):phiMCip",conditionLMD);
   tbp->Project("hresvmcrec_phi_IP","(vmc-vrec):phiMCip",conditionIP);
   TH2D *hreswmcrec_phi_LMD = new TH2D("hreswmcrec_phi_LMD",";#phi, rad;w_{MC} - w_{REC}, cm",5e1,-3.15,3.15,5e1,-0.05,0.05);
   TH2D *hreswmcrec_phi_IP = new TH2D("hreswmcrec_phi_IP",";#phi, rad;w_{MC} - w_{REC}, cm",5e1,-3.15,3.15,5e1,-5,5);
   tbp->Project("hreswmcrec_phi_LMD","(wmc - wrec):phiMCip",conditionLMD);
   tbp->Project("hreswmcrec_phi_IP","(wmc - wrec):phiMCip",conditionIP);
   TH2D *hrestvmcrec_phi_LMD = new TH2D("hrestvmcrec_phi_LMD",";#phi, rad;v'_{MC} - v'_{REC}",5e1,-3.15,3.15,5e1,-0.005,0.005);
   TH2D *hrestvmcrec_phi_IP = new TH2D("hrestvmcrec_phi_IP",";#phi, rad;v'_{MC} - v'_{REC}",5e1,-3.15,3.15,5e1,-0.005,0.005);
   tbp->Project("hrestvmcrec_phi_LMD","(tvmc - tvrec):phiMCip",conditionLMD);
   tbp->Project("hrestvmcrec_phi_IP","(tvmc - tvrec):phiMCip",conditionIP);
   TH2D *hrestwmcrec_phi_LMD = new TH2D("hrestwmcrec_phi_LMD",";#phi, rad; w'_{MC} - w'_{REC}",5e1,-3.15,3.15,5e1,-0.005,0.005);
   TH2D *hrestwmcrec_phi_IP = new TH2D("hrestwmcrec_phi_IP",";#phi, rad; w'_{MC} - w'_{REC}",5e1,-3.15,3.15,5e1,-0.005,0.005);
   tbp->Project("hrestwmcrec_phi_LMD","(twmc - twrec):phiMCip",conditionLMD);
   tbp->Project("hrestwmcrec_phi_IP","(twmc - twrec):phiMCip",conditionIP);
   
   TH2D *hresvmclmdrec_phi_LMD = new TH2D("hresvmclmdrec_phi_LMD",";#phi, rad;v_{MCLMD} - v_{REC}, cm",5e1,-3.15,3.15,5e1,-0.05,0.05);
   TH2D *hresvmclmdrec_phi_IP = new TH2D("hresvmclmdrec_phi_IP",";#phi, rad;v_{MCLMD} - v_{REC}, cm",5e1,-3.15,3.15,5e1,-5,5);
    tbp->Project("hresvmclmdrec_phi_LMD","(vmclmd-vrec):phiMCip",conditionLMD);
   tbp->Project("hresvmclmdrec_phi_IP","(vmclmd-vrec):phiMCip",conditionIP);
   TH2D *hreswmclmdrec_phi_LMD = new TH2D("hreswmclmdrec_phi_LMD",";#phi, rad;w_{MCLMD} - w_{REC}, cm",5e1,-3.15,3.15,5e1,-0.05,0.05);
   TH2D *hreswmclmdrec_phi_IP = new TH2D("hreswmclmdrec_phi_IP",";#phi, rad;w_{MCLMD} - w_{REC}, cm",5e1,-3.15,3.15,5e1,-5,5);
   tbp->Project("hreswmclmdrec_phi_LMD","(wmclmd - wrec):phiMCip",conditionLMD);
   tbp->Project("hreswmclmdrec_phi_IP","(wmclmd - wrec):phiMCip",conditionIP);
   TH2D *hrestvmclmdrec_phi_LMD = new TH2D("hrestvmclmdrec_phi_LMD",";#phi, rad;v'_{MCLMD} - v'_{REC}",5e1,-3.15,3.15,5e1,-0.005,0.005);
   TH2D *hrestvmclmdrec_phi_IP = new TH2D("hrestvmclmdrec_phi_IP",";#phi, rad;v'_{MCLMD} - v'_{REC}",5e1,-3.15,3.15,5e1,-0.005,0.005);
   tbp->Project("hrestvmclmdrec_phi_LMD","(tvmclmd - tvrec):phiMCip",conditionLMD);
   tbp->Project("hrestvmclmdrec_phi_IP","(tvmclmd - tvrec):phiMCip",conditionIP);
   TH2D *hrestwmclmdrec_phi_LMD = new TH2D("hrestwmclmdrec_phi_LMD",";#phi, rad; w'_{MCLMD} - w'_{REC}",5e1,-3.15,3.15,5e1,-0.005,0.005);
   TH2D *hrestwmclmdrec_phi_IP = new TH2D("hrestwmclmdrec_phi_IP",";#phi, rad; w'_{MCLMD} - w'_{REC}",5e1,-3.15,3.15,5e1,-0.005,0.005);
   tbp->Project("hrestwmclmdrec_phi_LMD","(twmclmd - twrec):phiMCip",conditionLMD);
   tbp->Project("hrestwmclmdrec_phi_IP","(twmclmd - twrec):phiMCip",conditionIP);

  TH2D *hresvmcmclmd_phi_LMD = new TH2D("hresvmcmclmd_phi_LMD",";#phi, rad;v_{MCLMD} - v_{MC}, cm",5e1,-3.15,3.15,5e1,-0.05,0.05);
   TH2D *hresvmcmclmd_phi_IP = new TH2D("hresvmcmclmd_phi_IP",";#phi, rad;v_{MCLMD} - v_{MC}, cm",5e1,-3.15,3.15,5e1,-5,5);
    tbp->Project("hresvmcmclmd_phi_LMD","(vmclmd-vmc):phiMCip",conditionLMD);
   tbp->Project("hresvmcmclmd_phi_IP","(vmclmd-vmc):phiMCip",conditionIP);
   TH2D *hreswmcmclmd_phi_LMD = new TH2D("hreswmcmclmd_phi_LMD",";#phi, rad;w_{MCLMD} - w_{MC}, cm",5e1,-3.15,3.15,5e1,-0.05,0.05);
   TH2D *hreswmcmclmd_phi_IP = new TH2D("hreswmcmclmd_phi_IP",";#phi, rad;w_{MCLMD} - w_{MC}, cm",5e1,-3.15,3.15,5e1,-5,5);
   tbp->Project("hreswmcmclmd_phi_LMD","(wmclmd - wmc):phiMCip",conditionLMD);
   tbp->Project("hreswmcmclmd_phi_IP","(wmclmd - wmc):phiMCip",conditionIP);
   TH2D *hrestvmcmclmd_phi_LMD = new TH2D("hrestvmcmclmd_phi_LMD",";#phi, rad;v'_{MCLMD} - v'_{MC}",5e1,-3.15,3.15,5e1,-0.005,0.005);
   TH2D *hrestvmcmclmd_phi_IP = new TH2D("hrestvmcmclmd_phi_IP",";#phi, rad;v'_{MCLMD} - v'_{MC}",5e1,-3.15,3.15,5e1,-0.005,0.005);
   tbp->Project("hrestvmcmclmd_phi_LMD","(tvmclmd - tvmc):phiMCip",conditionLMD);
   tbp->Project("hrestvmcmclmd_phi_IP","(tvmclmd - tvmc):phiMCip",conditionIP);
   TH2D *hrestwmcmclmd_phi_LMD = new TH2D("hrestwmcmclmd_phi_LMD",";#phi, rad; w'_{MCLMD} - w'_{MC}",5e1,-3.15,3.15,5e1,-0.005,0.005);
   TH2D *hrestwmcmclmd_phi_IP = new TH2D("hrestwmcmclmd_phi_IP",";#phi, rad; w'_{MCLMD} - w'_{MC}",5e1,-3.15,3.15,5e1,-0.005,0.005);
   tbp->Project("hrestwmcmclmd_phi_LMD","(twmclmd - twmc):phiMCip",conditionLMD);
   tbp->Project("hrestwmcmclmd_phi_IP","(twmclmd - twmc):phiMCip",conditionIP);

   TCanvas cSD_Vres_phi("cSD_Vres_phi");
   cSD_Vres_phi.Divide(3,6);
   cSD_Vres_phi.cd(1);
   hresvmcrec_phi_LMD->FitSlicesY();
   hresvmcrec_phi_LMD_1->GetYaxis()->SetTitle("v_{MC}-v_{REC}, mean");
   hresvmcrec_phi_LMD_2->GetYaxis()->SetTitle("v_{MC}-v_{REC}, sigma");
   hresvmcrec_phi_LMD->SetTitle("v_{MC}-v_{REC} (near LMD)");
   hresvmcrec_phi_LMD->Draw("colz");
   cSD_Vres_phi.cd(2);
   hresvmcrec_phi_LMD_1->SetMinimum(-0.01);
   hresvmcrec_phi_LMD_1->SetMaximum(0.01);
   hresvmcrec_phi_LMD_1->Draw();
   cSD_Vres_phi.cd(3);
   hresvmcrec_phi_LMD_2->SetMinimum(0);
   hresvmcrec_phi_LMD_2->SetMaximum(0.01);
   hresvmcrec_phi_LMD_2->Draw();
   cSD_Vres_phi.cd(4);
   hresvmcrec_phi_IP->FitSlicesY();
   hresvmcrec_phi_IP_1->GetYaxis()->SetTitle("v_{MC}-v_{REC}, mean");
   hresvmcrec_phi_IP_2->GetYaxis()->SetTitle("v_{MC}-v_{REC}, sigma");
   hresvmcrec_phi_IP->SetTitle("v_{MC}-v_{REC} (near IP)");
   hresvmcrec_phi_IP->Draw("colz");
   cSD_Vres_phi.cd(5);
   hresvmcrec_phi_IP_1->SetMinimum(-0.8);
   hresvmcrec_phi_IP_1->SetMaximum(0.8);
   hresvmcrec_phi_IP_1->Draw();
   cSD_Vres_phi.cd(6);
   hresvmcrec_phi_IP_2->SetMinimum(0);
   hresvmcrec_phi_IP_2->SetMaximum(0.8);
   hresvmcrec_phi_IP_2->Draw();
   cSD_Vres_phi.cd(7);
   hresvmclmdrec_phi_LMD->FitSlicesY();
   hresvmclmdrec_phi_LMD_1->GetYaxis()->SetTitle("v_{MCLMD}-v_{REC}, mean");
   hresvmclmdrec_phi_LMD_2->GetYaxis()->SetTitle("v_{MCLMD}-v_{REC}, sigma");
   hresvmclmdrec_phi_LMD->SetTitle("v_{MCLMD}-v_{REC} (near LMD)");
   hresvmclmdrec_phi_LMD->Draw("colz");
   cSD_Vres_phi.cd(8);
   hresvmclmdrec_phi_LMD_1->SetMinimum(-0.01);
   hresvmclmdrec_phi_LMD_1->SetMaximum(0.01);
   hresvmclmdrec_phi_LMD_1->Draw();
   cSD_Vres_phi.cd(9);
   hresvmclmdrec_phi_LMD_2->SetMinimum(0);
   hresvmclmdrec_phi_LMD_2->SetMaximum(0.01);
   hresvmclmdrec_phi_LMD_2->Draw();
   cSD_Vres_phi.cd(10);
   hresvmclmdrec_phi_IP->FitSlicesY();
   hresvmclmdrec_phi_IP_1->GetYaxis()->SetTitle("v_{MCLMD}-v_{REC}, mean");
   hresvmclmdrec_phi_IP_2->GetYaxis()->SetTitle("v_{MCLMD}-v_{REC}, sigma");
   hresvmclmdrec_phi_IP->SetTitle("v_{MCLMD}-v_{REC} (near IP)");
   hresvmclmdrec_phi_IP->Draw("colz");
   cSD_Vres_phi.cd(11);
   hresvmclmdrec_phi_IP_1->SetMinimum(-0.8);
   hresvmclmdrec_phi_IP_1->SetMaximum(0.8);
   hresvmclmdrec_phi_IP_1->Draw();
   cSD_Vres_phi.cd(12);
   hresvmclmdrec_phi_IP_2->SetMinimum(0);
   hresvmclmdrec_phi_IP_2->SetMaximum(0.8);
   hresvmclmdrec_phi_IP_2->Draw();
   cSD_Vres_phi.cd(13);
   hresvmcmclmd_phi_LMD->FitSlicesY();
   hresvmcmclmd_phi_LMD_1->GetYaxis()->SetTitle("v_{MCLMD}-v_{MC}, mean");
   hresvmcmclmd_phi_LMD_2->GetYaxis()->SetTitle("v_{MCLMD}-v_{MC}, sigma");
   hresvmcmclmd_phi_LMD->SetTitle("v_{MCLMD}-v_{MC} (near LMD)");
   hresvmcmclmd_phi_LMD->Draw("colz");
   cSD_Vres_phi.cd(14);
   hresvmcmclmd_phi_LMD_1->SetMinimum(-0.01);
   hresvmcmclmd_phi_LMD_1->SetMaximum(0.01);
   hresvmcmclmd_phi_LMD_1->Draw();
   cSD_Vres_phi.cd(15);
   hresvmcmclmd_phi_LMD_2->SetMinimum(0);
   hresvmcmclmd_phi_LMD_2->SetMaximum(0.01);
   hresvmcmclmd_phi_LMD_2->Draw();
   cSD_Vres_phi.cd(16);
   hresvmcmclmd_phi_IP->FitSlicesY();
   hresvmcmclmd_phi_IP_1->GetYaxis()->SetTitle("v_{MCLMD}-v_{MC}, mean");
   hresvmcmclmd_phi_IP_2->GetYaxis()->SetTitle("v_{MCLMD}-v_{MC}, sigma");
   hresvmcmclmd_phi_IP->SetTitle("v_{MCLMD}-v_{MC} (near IP)");
   hresvmcmclmd_phi_IP->Draw("colz");
   cSD_Vres_phi.cd(17);
   hresvmcmclmd_phi_IP_1->SetMinimum(-0.8);
   hresvmcmclmd_phi_IP_1->SetMaximum(0.8);
   hresvmcmclmd_phi_IP_1->Draw();
   cSD_Vres_phi.cd(18);
   hresvmcmclmd_phi_IP_2->SetMinimum(0);
   hresvmcmclmd_phi_IP_2->SetMaximum(0.8);
   hresvmcmclmd_phi_IP_2->Draw();
   cSD_Vres_phi.Print(resname_pdf_o);
   cSD_Vres_phi.Write();
   cSD_Vres_phi.Close();

 TCanvas cSD_TVres_phi("cSD_TVres_phi");
   cSD_TVres_phi.Divide(3,6);
   cSD_TVres_phi.cd(1);
   hrestvmcrec_phi_LMD->FitSlicesY();
   hrestvmcrec_phi_LMD_1->GetYaxis()->SetTitle("v'_{MC}-v'_{REC}, mean");
   hrestvmcrec_phi_LMD_2->GetYaxis()->SetTitle("v'_{MC}-v'_{REC}, sigma");
   hrestvmcrec_phi_LMD->SetTitle("v'_{MC}-v'_{REC} (near LMD)");
   hrestvmcrec_phi_LMD->Draw("colz");
   cSD_TVres_phi.cd(2);
   hrestvmcrec_phi_LMD_1->SetMinimum(-0.001);
   hrestvmcrec_phi_LMD_1->SetMaximum(0.001);
   hrestvmcrec_phi_LMD_1->Draw();
   cSD_TVres_phi.cd(3);
   hrestvmcrec_phi_LMD_2->SetMinimum(0);
   hrestvmcrec_phi_LMD_2->SetMaximum(0.002);
   hrestvmcrec_phi_LMD_2->Draw();
   cSD_TVres_phi.cd(4);
   hrestvmcrec_phi_IP->FitSlicesY();
   hrestvmcrec_phi_IP_1->GetYaxis()->SetTitle("v'_{MC}-v'_{REC}, mean");
   hrestvmcrec_phi_IP_2->GetYaxis()->SetTitle("v'_{MC}-v'_{REC}, sigma");
   hrestvmcrec_phi_IP->SetTitle("v'_{MC}-v'_{REC} (near IP)");
   hrestvmcrec_phi_IP->Draw("colz");
   cSD_TVres_phi.cd(5);
   hrestvmcrec_phi_IP_1->SetMinimum(-0.002);
   hrestvmcrec_phi_IP_1->SetMaximum(0.002);
   hrestvmcrec_phi_IP_1->Draw();
   cSD_TVres_phi.cd(6);
   hrestvmcrec_phi_IP_2->SetMinimum(0);
   hrestvmcrec_phi_IP_2->SetMaximum(0.002);
   hrestvmcrec_phi_IP_2->Draw();
   cSD_TVres_phi.cd(7);
   hrestvmclmdrec_phi_LMD->FitSlicesY();
   hrestvmclmdrec_phi_LMD_1->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{REC}, mean");
   hrestvmclmdrec_phi_LMD_2->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{REC}, sigma");
   hrestvmclmdrec_phi_LMD->SetTitle("v'_{MCLMD}-v'_{REC} (near LMD)");
   hrestvmclmdrec_phi_LMD->Draw("colz");
   cSD_TVres_phi.cd(8);
   hrestvmclmdrec_phi_LMD_1->SetMinimum(-0.001);
   hrestvmclmdrec_phi_LMD_1->SetMaximum(0.001);
   hrestvmclmdrec_phi_LMD_1->Draw();
   cSD_TVres_phi.cd(9);
   hrestvmclmdrec_phi_LMD_2->SetMinimum(0);
   hrestvmclmdrec_phi_LMD_2->SetMaximum(0.002);
   hrestvmclmdrec_phi_LMD_2->Draw();
   cSD_TVres_phi.cd(10);
   hrestvmclmdrec_phi_IP->FitSlicesY();
   hrestvmclmdrec_phi_IP_1->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{REC}, mean");
   hrestvmclmdrec_phi_IP_2->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{REC}, sigma");
   hrestvmclmdrec_phi_IP->SetTitle("v'_{MCLMD}-v'_{REC} (near IP)");
   hrestvmclmdrec_phi_IP->Draw("colz");
   cSD_TVres_phi.cd(11);
   hrestvmclmdrec_phi_IP_1->SetMinimum(-0.002);
   hrestvmclmdrec_phi_IP_1->SetMaximum(0.002);
   hrestvmclmdrec_phi_IP_1->Draw();
   cSD_TVres_phi.cd(12);
   hrestvmclmdrec_phi_IP_2->SetMinimum(0);
   hrestvmclmdrec_phi_IP_2->SetMaximum(0.002);
   hrestvmclmdrec_phi_IP_2->Draw();
   cSD_TVres_phi.cd(13);
   hrestvmcmclmd_phi_LMD->FitSlicesY();
   hrestvmcmclmd_phi_LMD_1->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{MC}, mean");
   hrestvmcmclmd_phi_LMD_2->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{MC}, sigma");
   hrestvmcmclmd_phi_LMD->SetTitle("v'_{MCLMD}-v'_{MC} (near LMD)");
   hrestvmcmclmd_phi_LMD->Draw("colz");
   cSD_TVres_phi.cd(14);
   hrestvmcmclmd_phi_LMD_1->SetMinimum(-0.001);
   hrestvmcmclmd_phi_LMD_1->SetMaximum(0.001);
   hrestvmcmclmd_phi_LMD_1->Draw();
   cSD_TVres_phi.cd(15);
   hrestvmcmclmd_phi_LMD_2->SetMinimum(0);
   hrestvmcmclmd_phi_LMD_2->SetMaximum(0.002);
   hrestvmcmclmd_phi_LMD_2->Draw();
   cSD_TVres_phi.cd(16);
   hrestvmcmclmd_phi_IP->FitSlicesY();
   hrestvmcmclmd_phi_IP_1->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{MC}, mean");
   hrestvmcmclmd_phi_IP_2->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{MC}, sigma");
   hrestvmcmclmd_phi_IP->SetTitle("v'_{MCLMD}-v'_{MC} (near IP)");
   hrestvmcmclmd_phi_IP->Draw("colz");
   cSD_TVres_phi.cd(17);
   hrestvmcmclmd_phi_IP_1->SetMinimum(-0.002);
   hrestvmcmclmd_phi_IP_1->SetMaximum(0.002);
   hrestvmcmclmd_phi_IP_1->Draw();
   cSD_TVres_phi.cd(18);
   hrestvmcmclmd_phi_IP_2->SetMinimum(0);
   hrestvmcmclmd_phi_IP_2->SetMaximum(0.002);
   hrestvmcmclmd_phi_IP_2->Draw();
   cSD_TVres_phi.Print(resname_pdf_o);
   cSD_TVres_phi.Write();
   cSD_TVres_phi.Close();


   TCanvas cSD_Wres_phi("cSD_Wres_phi");
   cSD_Wres_phi.Divide(3,6);
   cSD_Wres_phi.cd(1);
   hreswmcrec_phi_LMD->FitSlicesY();
   hreswmcrec_phi_LMD_1->GetYaxis()->SetTitle("w_{MC}-w_{REC}, mean");
   hreswmcrec_phi_LMD_2->GetYaxis()->SetTitle("w_{MC}-w_{REC}, sigma");
   hreswmcrec_phi_LMD->SetTitle("w_{MC}-w_{REC} (near LMD)");
   hreswmcrec_phi_LMD->Draw("colz");
   cSD_Wres_phi.cd(2);
   hreswmcrec_phi_LMD_1->SetMinimum(-0.01);
   hreswmcrec_phi_LMD_1->SetMaximum(0.01);
   hreswmcrec_phi_LMD_1->Draw();
   cSD_Wres_phi.cd(3);
   hreswmcrec_phi_LMD_2->SetMinimum(0);
   hreswmcrec_phi_LMD_2->SetMaximum(0.01);
   hreswmcrec_phi_LMD_2->Draw();
   cSD_Wres_phi.cd(4);
   hreswmcrec_phi_IP->FitSlicesY();
   hreswmcrec_phi_IP_1->GetYaxis()->SetTitle("w_{MC}-w_{REC}, mean");
   hreswmcrec_phi_IP_2->GetYaxis()->SetTitle("w_{MC}-w_{REC}, sigma");
   hreswmcrec_phi_IP->SetTitle("w_{MC}-w_{REC} (near IP)");
   hreswmcrec_phi_IP->Draw("colz");
   cSD_Wres_phi.cd(5);
   hreswmcrec_phi_IP_1->SetMinimum(-0.8);
   hreswmcrec_phi_IP_1->SetMaximum(0.8);
   hreswmcrec_phi_IP_1->Draw();
   cSD_Wres_phi.cd(6);
   hreswmcrec_phi_IP_2->SetMinimum(0);
   hreswmcrec_phi_IP_2->SetMaximum(0.8);
   hreswmcrec_phi_IP_2->Draw();
   cSD_Wres_phi.cd(7);
   hreswmclmdrec_phi_LMD->FitSlicesY();
   hreswmclmdrec_phi_LMD_1->GetYaxis()->SetTitle("w_{MCLMD}-w_{REC}, mean");
   hreswmclmdrec_phi_LMD_2->GetYaxis()->SetTitle("w_{MCLMD}-w_{REC}, sigma");
   hreswmclmdrec_phi_LMD->SetTitle("w_{MCLMD}-w_{REC} (near LMD)");
   hreswmclmdrec_phi_LMD->Draw("colz");
   cSD_Wres_phi.cd(8);
   hreswmclmdrec_phi_LMD_1->SetMinimum(-0.01);
   hreswmclmdrec_phi_LMD_1->SetMaximum(0.01);
   hreswmclmdrec_phi_LMD_1->Draw();
   cSD_Wres_phi.cd(9);
   hreswmclmdrec_phi_LMD_2->SetMinimum(0);
   hreswmclmdrec_phi_LMD_2->SetMaximum(0.01);
   hreswmclmdrec_phi_LMD_2->Draw();
   cSD_Wres_phi.cd(10);
   hreswmclmdrec_phi_IP->FitSlicesY();
   hreswmclmdrec_phi_IP_1->GetYaxis()->SetTitle("w_{MCLMD}-w_{REC}, mean");
   hreswmclmdrec_phi_IP_2->GetYaxis()->SetTitle("w_{MCLMD}-w_{REC}, sigma");
   hreswmclmdrec_phi_IP->SetTitle("w_{MCLMD}-w_{REC} (near IP)");
   hreswmclmdrec_phi_IP->Draw("colz");
   cSD_Wres_phi.cd(11);
   hreswmclmdrec_phi_IP_1->SetMinimum(-0.8);
   hreswmclmdrec_phi_IP_1->SetMaximum(0.8);
   hreswmclmdrec_phi_IP_1->Draw();
   cSD_Wres_phi.cd(12);
   hreswmclmdrec_phi_IP_2->SetMinimum(0);
   hreswmclmdrec_phi_IP_2->SetMaximum(0.8);
   hreswmclmdrec_phi_IP_2->Draw();
   cSD_Wres_phi.cd(13);
   hreswmcmclmd_phi_LMD->FitSlicesY();
   hreswmcmclmd_phi_LMD_1->GetYaxis()->SetTitle("w_{MCLMD}-w_{MC}, mean");
   hreswmcmclmd_phi_LMD_2->GetYaxis()->SetTitle("w_{MCLMD}-w_{MC}, sigma");
   hreswmcmclmd_phi_LMD->SetTitle("w_{MCLMD}-w_{MC} (near LMD)");
   hreswmcmclmd_phi_LMD->Draw("colz");
   cSD_Wres_phi.cd(14);
   hreswmcmclmd_phi_LMD_1->SetMinimum(-0.01);
   hreswmcmclmd_phi_LMD_1->SetMaximum(0.01);
   hreswmcmclmd_phi_LMD_1->Draw();
   cSD_Wres_phi.cd(15);
   hreswmcmclmd_phi_LMD_2->SetMinimum(0);
   hreswmcmclmd_phi_LMD_2->SetMaximum(0.01);
   hreswmcmclmd_phi_LMD_2->Draw();
   cSD_Wres_phi.cd(16);
   hreswmcmclmd_phi_IP->FitSlicesY();
   hreswmcmclmd_phi_IP_1->GetYaxis()->SetTitle("w_{MCLMD}-w_{MC}, mean");
   hreswmcmclmd_phi_IP_2->GetYaxis()->SetTitle("w_{MCLMD}-w_{MC}, sigma");
   hreswmcmclmd_phi_IP->SetTitle("w_{MCLMD}-w_{MC} (near IP)");
   hreswmcmclmd_phi_IP->Draw("colz");
   cSD_Wres_phi.cd(17);
   hreswmcmclmd_phi_IP_1->SetMinimum(-0.8);
   hreswmcmclmd_phi_IP_1->SetMaximum(0.8);
   hreswmcmclmd_phi_IP_1->Draw();
   cSD_Wres_phi.cd(18);
   hreswmcmclmd_phi_IP_2->SetMinimum(0);
   hreswmcmclmd_phi_IP_2->SetMaximum(0.8);
   hreswmcmclmd_phi_IP_2->Draw();
   cSD_Wres_phi.Print(resname_pdf_o);
   cSD_Wres_phi.Write();
   cSD_Wres_phi.Close();

TCanvas cSD_TWres_phi("cSD_TWres_phi");
   cSD_TWres_phi.Divide(3,6);
   cSD_TWres_phi.cd(1);
   hrestwmcrec_phi_LMD->FitSlicesY();
   hrestwmcrec_phi_LMD_1->GetYaxis()->SetTitle("w'_{MC}-w'_{REC}, mean");
   hrestwmcrec_phi_LMD_2->GetYaxis()->SetTitle("w'_{MC}-w'_{REC}, sigma");
   hrestwmcrec_phi_LMD->SetTitle("w'_{MC}-w'_{REC} (near LMD)");
   hrestwmcrec_phi_LMD->Draw("colz");
   cSD_TWres_phi.cd(2);
   hrestwmcrec_phi_LMD_1->SetMinimum(-0.001);
   hrestwmcrec_phi_LMD_1->SetMaximum(0.001);
   hrestwmcrec_phi_LMD_1->Draw();
   cSD_TWres_phi.cd(3);
   hrestwmcrec_phi_LMD_2->SetMinimum(0);
   hrestwmcrec_phi_LMD_2->SetMaximum(0.002);
   hrestwmcrec_phi_LMD_2->Draw();
   cSD_TWres_phi.cd(4);
   hrestwmcrec_phi_IP->FitSlicesY();
   hrestwmcrec_phi_IP_1->GetYaxis()->SetTitle("w'_{MC}-w'_{REC}, mean");
   hrestwmcrec_phi_IP_2->GetYaxis()->SetTitle("w'_{MC}-w'_{REC}, sigma");
   hrestwmcrec_phi_IP->SetTitle("w'_{MC}-w'_{REC} (near IP)");
   hrestwmcrec_phi_IP->Draw("colz");
   cSD_TWres_phi.cd(5);
   hrestwmcrec_phi_IP_1->SetMinimum(-0.002);
   hrestwmcrec_phi_IP_1->SetMaximum(0.002);
   hrestwmcrec_phi_IP_1->Draw();
   cSD_TWres_phi.cd(6);
   hrestwmcrec_phi_IP_2->SetMinimum(0);
   hrestwmcrec_phi_IP_2->SetMaximum(0.002);
   hrestwmcrec_phi_IP_2->Draw();
   cSD_TWres_phi.cd(7);
   hrestwmclmdrec_phi_LMD->FitSlicesY();
   hrestwmclmdrec_phi_LMD_1->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{REC}, mean");
   hrestwmclmdrec_phi_LMD_2->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{REC}, sigma");
   hrestwmclmdrec_phi_LMD->SetTitle("w'_{MCLMD}-w'_{REC} (near LMD)");
   hrestwmclmdrec_phi_LMD->Draw("colz");
   cSD_TWres_phi.cd(8);
   hrestwmclmdrec_phi_LMD_1->SetMinimum(-0.001);
   hrestwmclmdrec_phi_LMD_1->SetMaximum(0.001);
   hrestwmclmdrec_phi_LMD_1->Draw();
   cSD_TWres_phi.cd(9);
   hrestwmclmdrec_phi_LMD_2->SetMinimum(0);
   hrestwmclmdrec_phi_LMD_2->SetMaximum(0.002);
   hrestwmclmdrec_phi_LMD_2->Draw();
   cSD_TWres_phi.cd(10);
   hrestwmclmdrec_phi_IP->FitSlicesY();
   hrestwmclmdrec_phi_IP_1->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{REC}, mean");
   hrestwmclmdrec_phi_IP_2->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{REC}, sigma");
   hrestwmclmdrec_phi_IP->SetTitle("w'_{MCLMD}-w'_{REC} (near IP)");
   hrestwmclmdrec_phi_IP->Draw("colz");
   cSD_TWres_phi.cd(11);
   hrestwmclmdrec_phi_IP_1->SetMinimum(-0.002);
   hrestwmclmdrec_phi_IP_1->SetMaximum(0.002);
   hrestwmclmdrec_phi_IP_1->Draw();
   cSD_TWres_phi.cd(12);
   hrestwmclmdrec_phi_IP_2->SetMinimum(0);
   hrestwmclmdrec_phi_IP_2->SetMaximum(0.002);
   hrestwmclmdrec_phi_IP_2->Draw();
   cSD_TWres_phi.cd(13);
   hrestwmcmclmd_phi_LMD->FitSlicesY();
   hrestwmcmclmd_phi_LMD_1->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{MC}, mean");
   hrestwmcmclmd_phi_LMD_2->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{MC}, sigma");
   hrestwmcmclmd_phi_LMD->SetTitle("w'_{MCLMD}-w'_{MC} (near LMD)");
   hrestwmcmclmd_phi_LMD->Draw("colz");
   cSD_TWres_phi.cd(14);
   hrestwmcmclmd_phi_LMD_1->SetMinimum(-0.001);
   hrestwmcmclmd_phi_LMD_1->SetMaximum(0.001);
   hrestwmcmclmd_phi_LMD_1->Draw();
   cSD_TWres_phi.cd(15);
   hrestwmcmclmd_phi_LMD_2->SetMinimum(0);
   hrestwmcmclmd_phi_LMD_2->SetMaximum(0.002);
   hrestwmcmclmd_phi_LMD_2->Draw();
   cSD_TWres_phi.cd(16);
   hrestwmcmclmd_phi_IP->FitSlicesY();
   hrestwmcmclmd_phi_IP_1->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{MC}, mean");
   hrestwmcmclmd_phi_IP_2->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{MC}, sigma");
   hrestwmcmclmd_phi_IP->SetTitle("w'_{MCLMD}-w'_{MC} (near IP)");
   hrestwmcmclmd_phi_IP->Draw("colz");
   cSD_TWres_phi.cd(17);
   hrestwmcmclmd_phi_IP_1->SetMinimum(-0.002);
   hrestwmcmclmd_phi_IP_1->SetMaximum(0.002);
   hrestwmcmclmd_phi_IP_1->Draw();
   cSD_TWres_phi.cd(18);
   hrestwmcmclmd_phi_IP_2->SetMinimum(0);
   hrestwmcmclmd_phi_IP_2->SetMaximum(0.002);
   hrestwmcmclmd_phi_IP_2->Draw();
   cSD_TWres_phi.Print(resname_pdf_o);
   cSD_TWres_phi.Write();
   cSD_TWres_phi.Close();




   TH2D *hresvmcrec_theta_LMD = new TH2D("hresvmcrec_theta_LMD",";#theta, mrad;v_{MC} - v_{REC}, cm",4e1,2,11,5e1,-0.05,0.05);
   TH2D *hresvmcrec_theta_IP = new TH2D("hresvmcrec_theta_IP",";#theta, mrad;v_{MC} - v_{REC}, cm",4e1,2,11,5e1,-5,5);
    tbp->Project("hresvmcrec_theta_LMD","(vmc-vrec):1e3*thetaMCip",conditionLMD);
   tbp->Project("hresvmcrec_theta_IP","(vmc-vrec):1e3*thetaMCip",conditionIP);
   TH2D *hreswmcrec_theta_LMD = new TH2D("hreswmcrec_theta_LMD",";#theta, mrad;w_{MC} - w_{REC}, cm",4e1,2,11,5e1,-0.05,0.05);
   TH2D *hreswmcrec_theta_IP = new TH2D("hreswmcrec_theta_IP",";#theta, mrad;w_{MC} - w_{REC}, cm",4e1,2,11,5e1,-5,5);
   tbp->Project("hreswmcrec_theta_LMD","(wmc - wrec):1e3*thetaMCip",conditionLMD);
   tbp->Project("hreswmcrec_theta_IP","(wmc - wrec):1e3*thetaMCip",conditionIP);
   TH2D *hrestvmcrec_theta_LMD = new TH2D("hrestvmcrec_theta_LMD",";#theta, mrad;v'_{MC} - v'_{REC}",4e1,2,11,5e1,-0.005,0.005);
   TH2D *hrestvmcrec_theta_IP = new TH2D("hrestvmcrec_theta_IP",";#theta, mrad;v'_{MC} - v'_{REC}",4e1,2,11,5e1,-0.005,0.005);
   tbp->Project("hrestvmcrec_theta_LMD","(tvmc - tvrec):1e3*thetaMCip",conditionLMD);
   tbp->Project("hrestvmcrec_theta_IP","(tvmc - tvrec):1e3*thetaMCip",conditionIP);
   TH2D *hrestwmcrec_theta_LMD = new TH2D("hrestwmcrec_theta_LMD",";#theta, mrad; w'_{MC} - w'_{REC}",4e1,2,11,5e1,-0.005,0.005);
   TH2D *hrestwmcrec_theta_IP = new TH2D("hrestwmcrec_theta_IP",";#theta, mrad; w'_{MC} - w'_{REC}",4e1,2,11,5e1,-0.005,0.005);
   tbp->Project("hrestwmcrec_theta_LMD","(twmc - twrec):1e3*thetaMCip",conditionLMD);
   tbp->Project("hrestwmcrec_theta_IP","(twmc - twrec):1e3*thetaMCip",conditionIP);
   
   TH2D *hresvmclmdrec_theta_LMD = new TH2D("hresvmclmdrec_theta_LMD",";#theta, mrad;v_{MCLMD} - v_{REC}, cm",4e1,2,11,5e1,-0.05,0.05);
   TH2D *hresvmclmdrec_theta_IP = new TH2D("hresvmclmdrec_theta_IP",";#theta, mrad;v_{MCLMD} - v_{REC}, cm",4e1,2,11,5e1,-5,5);
    tbp->Project("hresvmclmdrec_theta_LMD","(vmclmd-vrec):1e3*thetaMCip",conditionLMD);
   tbp->Project("hresvmclmdrec_theta_IP","(vmclmd-vrec):1e3*thetaMCip",conditionIP);
   TH2D *hreswmclmdrec_theta_LMD = new TH2D("hreswmclmdrec_theta_LMD",";#theta, mrad;w_{MCLMD} - w_{REC}, cm",4e1,2,11,5e1,-0.05,0.05);
   TH2D *hreswmclmdrec_theta_IP = new TH2D("hreswmclmdrec_theta_IP",";#theta, mrad;w_{MCLMD} - w_{REC}, cm",4e1,2,11,5e1,-5,5);
   tbp->Project("hreswmclmdrec_theta_LMD","(wmclmd - wrec):1e3*thetaMCip",conditionLMD);
   tbp->Project("hreswmclmdrec_theta_IP","(wmclmd - wrec):1e3*thetaMCip",conditionIP);
   TH2D *hrestvmclmdrec_theta_LMD = new TH2D("hrestvmclmdrec_theta_LMD",";#theta, mrad;v'_{MCLMD} - v'_{REC}",4e1,2,11,5e1,-0.005,0.005);
   TH2D *hrestvmclmdrec_theta_IP = new TH2D("hrestvmclmdrec_theta_IP",";#theta, mrad;v'_{MCLMD} - v'_{REC}",4e1,2,11,5e1,-0.005,0.005);
   tbp->Project("hrestvmclmdrec_theta_LMD","(tvmclmd - tvrec):1e3*thetaMCip",conditionLMD);
   tbp->Project("hrestvmclmdrec_theta_IP","(tvmclmd - tvrec):1e3*thetaMCip",conditionIP);
   TH2D *hrestwmclmdrec_theta_LMD = new TH2D("hrestwmclmdrec_theta_LMD",";#theta, mrad; w'_{MCLMD} - w'_{REC}",4e1,2,11,5e1,-0.005,0.005);
   TH2D *hrestwmclmdrec_theta_IP = new TH2D("hrestwmclmdrec_theta_IP",";#theta, mrad; w'_{MCLMD} - w'_{REC}",4e1,2,11,5e1,-0.005,0.005);
   tbp->Project("hrestwmclmdrec_theta_LMD","(twmclmd - twrec):1e3*thetaMCip",conditionLMD);
   tbp->Project("hrestwmclmdrec_theta_IP","(twmclmd - twrec):1e3*thetaMCip",conditionIP);

  TH2D *hresvmcmclmd_theta_LMD = new TH2D("hresvmcmclmd_theta_LMD",";#theta, mrad;v_{MCLMD} - v_{MC}, cm",4e1,2,11,5e1,-0.05,0.05);
   TH2D *hresvmcmclmd_theta_IP = new TH2D("hresvmcmclmd_theta_IP",";#theta, mrad;v_{MCLMD} - v_{MC}, cm",4e1,2,11,5e1,-5,5);
    tbp->Project("hresvmcmclmd_theta_LMD","(vmclmd-vmc):1e3*thetaMCip",conditionLMD);
   tbp->Project("hresvmcmclmd_theta_IP","(vmclmd-vmc):1e3*thetaMCip",conditionIP);
   TH2D *hreswmcmclmd_theta_LMD = new TH2D("hreswmcmclmd_theta_LMD",";#theta, mrad;w_{MCLMD} - w_{MC}, cm",4e1,2,11,5e1,-0.05,0.05);
   TH2D *hreswmcmclmd_theta_IP = new TH2D("hreswmcmclmd_theta_IP",";#theta, mrad;w_{MCLMD} - w_{MC}, cm",4e1,2,11,5e1,-5,5);
   tbp->Project("hreswmcmclmd_theta_LMD","(wmclmd - wmc):1e3*thetaMCip",conditionLMD);
   tbp->Project("hreswmcmclmd_theta_IP","(wmclmd - wmc):1e3*thetaMCip",conditionIP);
   TH2D *hrestvmcmclmd_theta_LMD = new TH2D("hrestvmcmclmd_theta_LMD",";#theta, mrad;v'_{MCLMD} - v'_{MC}",4e1,2,11,5e1,-0.005,0.005);
   TH2D *hrestvmcmclmd_theta_IP = new TH2D("hrestvmcmclmd_theta_IP",";#theta, mrad;v'_{MCLMD} - v'_{MC}",4e1,2,11,5e1,-0.005,0.005);
   tbp->Project("hrestvmcmclmd_theta_LMD","(tvmclmd - tvmc):1e3*thetaMCip",conditionLMD);
   tbp->Project("hrestvmcmclmd_theta_IP","(tvmclmd - tvmc):1e3*thetaMCip",conditionIP);
   TH2D *hrestwmcmclmd_theta_LMD = new TH2D("hrestwmcmclmd_theta_LMD",";#theta, mrad; w'_{MCLMD} - w'_{MC}",4e1,2,11,5e1,-0.005,0.005);
   TH2D *hrestwmcmclmd_theta_IP = new TH2D("hrestwmcmclmd_theta_IP",";#theta, mrad; w'_{MCLMD} - w'_{MC}",4e1,2,11,5e1,-0.005,0.005);
   tbp->Project("hrestwmcmclmd_theta_LMD","(twmclmd - twmc):1e3*thetaMCip",conditionLMD);
   tbp->Project("hrestwmcmclmd_theta_IP","(twmclmd - twmc):1e3*thetaMCip",conditionIP);

   TCanvas cSD_Vres_theta("cSD_Vres_theta");
   cSD_Vres_theta.Divide(3,6);
   cSD_Vres_theta.cd(1);
   hresvmcrec_theta_LMD->FitSlicesY();
   hresvmcrec_theta_LMD_1->GetYaxis()->SetTitle("v_{MC}-v_{REC}, mean [cm]");
   hresvmcrec_theta_LMD_2->GetYaxis()->SetTitle("v_{MC}-v_{REC}, sigma [cm]");
   hresvmcrec_theta_LMD->SetTitle("v_{MC}-v_{REC} (near LMD)");
   hresvmcrec_theta_LMD->Draw("colz");
   cSD_Vres_theta.cd(2);
   hresvmcrec_theta_LMD_1->SetMinimum(-0.005);
   hresvmcrec_theta_LMD_1->SetMaximum(0.005);
   hresvmcrec_theta_LMD_1->Draw();
   cSD_Vres_theta.cd(3);
   hresvmcrec_theta_LMD_2->SetMinimum(0);
   hresvmcrec_theta_LMD_2->SetMaximum(0.01);
   hresvmcrec_theta_LMD_2->Draw();
   cSD_Vres_theta.cd(4);
   hresvmcrec_theta_IP->FitSlicesY();
   hresvmcrec_theta_IP_1->GetYaxis()->SetTitle("v_{MC}-v_{REC}, mean [cm]");
   hresvmcrec_theta_IP_2->GetYaxis()->SetTitle("v_{MC}-v_{REC}, sigma [cm]");
   hresvmcrec_theta_IP->SetTitle("v_{MC}-v_{REC} (near IP)");
   hresvmcrec_theta_IP->Draw("colz");
   cSD_Vres_theta.cd(5);
   hresvmcrec_theta_IP_1->SetMinimum(-0.5);
   hresvmcrec_theta_IP_1->SetMaximum(0.5);
   hresvmcrec_theta_IP_1->Draw();
   cSD_Vres_theta.cd(6);
   hresvmcrec_theta_IP_2->SetMinimum(0);
   hresvmcrec_theta_IP_2->SetMaximum(1);
   hresvmcrec_theta_IP_2->Draw();
   cSD_Vres_theta.cd(7);
   hresvmclmdrec_theta_LMD->FitSlicesY();
   hresvmclmdrec_theta_LMD_1->GetYaxis()->SetTitle("v_{MCLMD}-v_{REC}, mean [cm]");
   hresvmclmdrec_theta_LMD_2->GetYaxis()->SetTitle("v_{MCLMD}-v_{REC}, sigma [cm]");
   hresvmclmdrec_theta_LMD->SetTitle("v_{MCLMD}-v_{REC} (near LMD)");
   hresvmclmdrec_theta_LMD->Draw("colz");
   cSD_Vres_theta.cd(8);
   hresvmclmdrec_theta_LMD_1->SetMinimum(-0.005);
   hresvmclmdrec_theta_LMD_1->SetMaximum(0.005);
   hresvmclmdrec_theta_LMD_1->Draw();
   cSD_Vres_theta.cd(9);
   hresvmclmdrec_theta_LMD_2->SetMinimum(0);
   hresvmclmdrec_theta_LMD_2->SetMaximum(0.01);
   hresvmclmdrec_theta_LMD_2->Draw();
   cSD_Vres_theta.cd(10);
   hresvmclmdrec_theta_IP->FitSlicesY();
   hresvmclmdrec_theta_IP_1->GetYaxis()->SetTitle("v_{MCLMD}-v_{REC}, mean [cm]");
   hresvmclmdrec_theta_IP_2->GetYaxis()->SetTitle("v_{MCLMD}-v_{REC}, sigma [cm]");
   hresvmclmdrec_theta_IP->SetTitle("v_{MCLMD}-v_{REC} (near IP)");
   hresvmclmdrec_theta_IP->Draw("colz");
   cSD_Vres_theta.cd(11);
   hresvmclmdrec_theta_IP_1->SetMinimum(-0.5);
   hresvmclmdrec_theta_IP_1->SetMaximum(0.5);
   hresvmclmdrec_theta_IP_1->Draw();
   cSD_Vres_theta.cd(12);
   hresvmclmdrec_theta_IP_2->SetMinimum(0);
   hresvmclmdrec_theta_IP_2->SetMaximum(1);
   hresvmclmdrec_theta_IP_2->Draw();
   cSD_Vres_theta.cd(13);
   hresvmcmclmd_theta_LMD->FitSlicesY();
   hresvmcmclmd_theta_LMD_1->GetYaxis()->SetTitle("v_{MCLMD}-v_{MC}, mean [cm]");
   hresvmcmclmd_theta_LMD_2->GetYaxis()->SetTitle("v_{MCLMD}-v_{MC}, sigma [cm]");
   hresvmcmclmd_theta_LMD->SetTitle("v_{MCLMD}-v_{MC} (near LMD)");
   hresvmcmclmd_theta_LMD->Draw("colz");
   cSD_Vres_theta.cd(14);
   hresvmcmclmd_theta_LMD_1->SetMinimum(-0.005);
   hresvmcmclmd_theta_LMD_1->SetMaximum(0.005);
   hresvmcmclmd_theta_LMD_1->Draw();
   cSD_Vres_theta.cd(15);
   hresvmcmclmd_theta_LMD_2->SetMinimum(0);
   hresvmcmclmd_theta_LMD_2->SetMaximum(0.01);
   hresvmcmclmd_theta_LMD_2->Draw();
   cSD_Vres_theta.cd(16);
   hresvmcmclmd_theta_IP->FitSlicesY();
   hresvmcmclmd_theta_IP_1->GetYaxis()->SetTitle("v_{MCLMD}-v_{MC}, mean [cm]");
   hresvmcmclmd_theta_IP_2->GetYaxis()->SetTitle("v_{MCLMD}-v_{MC}, sigma [cm]");
   hresvmcmclmd_theta_IP->SetTitle("v_{MCLMD}-v_{MC} (near IP)");
   hresvmcmclmd_theta_IP->Draw("colz");
   cSD_Vres_theta.cd(17);
   hresvmcmclmd_theta_IP_1->SetMinimum(-0.5);
   hresvmcmclmd_theta_IP_1->SetMaximum(0.5);
   hresvmcmclmd_theta_IP_1->Draw();
   cSD_Vres_theta.cd(18);
   hresvmcmclmd_theta_IP_2->SetMinimum(0);
   hresvmcmclmd_theta_IP_2->SetMaximum(1);
   hresvmcmclmd_theta_IP_2->Draw();
   cSD_Vres_theta.Print(resname_pdf_o);
   cSD_Vres_theta.Write();
   cSD_Vres_theta.Close();

 TCanvas cSD_TVres_theta("cSD_TVres_theta");
   cSD_TVres_theta.Divide(3,6);
   cSD_TVres_theta.cd(1);
   hrestvmcrec_theta_LMD->FitSlicesY();
   hrestvmcrec_theta_LMD_1->GetYaxis()->SetTitle("v'_{MC}-v'_{REC}, mean");
   hrestvmcrec_theta_LMD_2->GetYaxis()->SetTitle("v'_{MC}-v'_{REC}, sigma");
   hrestvmcrec_theta_LMD->SetTitle("v'_{MC}-v'_{REC} (near LMD)");
   hrestvmcrec_theta_LMD->Draw("colz");
   cSD_TVres_theta.cd(2);
   hrestvmcrec_theta_LMD_1->SetMinimum(-0.001);
   hrestvmcrec_theta_LMD_1->SetMaximum(0.001);
   hrestvmcrec_theta_LMD_1->Draw();
   cSD_TVres_theta.cd(3);
   hrestvmcrec_theta_LMD_2->SetMinimum(0);
   hrestvmcrec_theta_LMD_2->SetMaximum(0.0015);
   hrestvmcrec_theta_LMD_2->Draw();
   cSD_TVres_theta.cd(4);
   hrestvmcrec_theta_IP->FitSlicesY();
   hrestvmcrec_theta_IP_1->GetYaxis()->SetTitle("v'_{MC}-v'_{REC}, mean");
   hrestvmcrec_theta_IP_2->GetYaxis()->SetTitle("v'_{MC}-v'_{REC}, sigma");
   hrestvmcrec_theta_IP->SetTitle("v'_{MC}-v'_{REC} (near IP)");
   hrestvmcrec_theta_IP->Draw("colz");
   cSD_TVres_theta.cd(5);
   hrestvmcrec_theta_IP_1->SetMinimum(-0.0015);
   hrestvmcrec_theta_IP_1->SetMaximum(0.0015);
   hrestvmcrec_theta_IP_1->Draw();
   cSD_TVres_theta.cd(6);
   hrestvmcrec_theta_IP_2->SetMinimum(0);
   hrestvmcrec_theta_IP_2->SetMaximum(0.0015);
   hrestvmcrec_theta_IP_2->Draw();
   cSD_TVres_theta.cd(7);
   hrestvmclmdrec_theta_LMD->FitSlicesY();
   hrestvmclmdrec_theta_LMD_1->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{REC}, mean");
   hrestvmclmdrec_theta_LMD_2->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{REC}, sigma");
   hrestvmclmdrec_theta_LMD->SetTitle("v'_{MCLMD}-v'_{REC} (near LMD)");
   hrestvmclmdrec_theta_LMD->Draw("colz");
   cSD_TVres_theta.cd(8);
   hrestvmclmdrec_theta_LMD_1->SetMinimum(-0.001);
   hrestvmclmdrec_theta_LMD_1->SetMaximum(0.001);
   hrestvmclmdrec_theta_LMD_1->Draw();
   cSD_TVres_theta.cd(9);
   hrestvmclmdrec_theta_LMD_2->SetMinimum(0);
   hrestvmclmdrec_theta_LMD_2->SetMaximum(0.0015);
   hrestvmclmdrec_theta_LMD_2->Draw();
   cSD_TVres_theta.cd(10);
   hrestvmclmdrec_theta_IP->FitSlicesY();
   hrestvmclmdrec_theta_IP_1->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{REC}, mean");
   hrestvmclmdrec_theta_IP_2->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{REC}, sigma");
   hrestvmclmdrec_theta_IP->SetTitle("v'_{MCLMD}-v'_{REC} (near IP)");
   hrestvmclmdrec_theta_IP->Draw("colz");
   cSD_TVres_theta.cd(11);
   hrestvmclmdrec_theta_IP_1->SetMinimum(-0.0015);
   hrestvmclmdrec_theta_IP_1->SetMaximum(0.0015);
   hrestvmclmdrec_theta_IP_1->Draw();
   cSD_TVres_theta.cd(12);
   hrestvmclmdrec_theta_IP_2->SetMinimum(0);
   hrestvmclmdrec_theta_IP_2->SetMaximum(0.0015);
   hrestvmclmdrec_theta_IP_2->Draw();
   cSD_TVres_theta.cd(13);
   hrestvmcmclmd_theta_LMD->FitSlicesY();
   hrestvmcmclmd_theta_LMD_1->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{MC}, mean");
   hrestvmcmclmd_theta_LMD_2->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{MC}, sigma");
   hrestvmcmclmd_theta_LMD->SetTitle("v'_{MCLMD}-v'_{MC} (near LMD)");
   hrestvmcmclmd_theta_LMD->Draw("colz");
   cSD_TVres_theta.cd(14);
   hrestvmcmclmd_theta_LMD_1->SetMinimum(-0.001);
   hrestvmcmclmd_theta_LMD_1->SetMaximum(0.001);
   hrestvmcmclmd_theta_LMD_1->Draw();
   cSD_TVres_theta.cd(15);
   hrestvmcmclmd_theta_LMD_2->SetMinimum(0);
   hrestvmcmclmd_theta_LMD_2->SetMaximum(0.0015);
   hrestvmcmclmd_theta_LMD_2->Draw();
   cSD_TVres_theta.cd(16);
   hrestvmcmclmd_theta_IP->FitSlicesY();
   hrestvmcmclmd_theta_IP_1->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{MC}, mean");
   hrestvmcmclmd_theta_IP_2->GetYaxis()->SetTitle("v'_{MCLMD}-v'_{MC}, sigma");
   hrestvmcmclmd_theta_IP->SetTitle("v'_{MCLMD}-v'_{MC} (near IP)");
   hrestvmcmclmd_theta_IP->Draw("colz");
   cSD_TVres_theta.cd(17);
   hrestvmcmclmd_theta_IP_1->SetMinimum(-0.0015);
   hrestvmcmclmd_theta_IP_1->SetMaximum(0.0015);
   hrestvmcmclmd_theta_IP_1->Draw();
   cSD_TVres_theta.cd(18);
   hrestvmcmclmd_theta_IP_2->SetMinimum(0);
   hrestvmcmclmd_theta_IP_2->SetMaximum(0.0015);
   hrestvmcmclmd_theta_IP_2->Draw();
   cSD_TVres_theta.Print(resname_pdf_o);
   cSD_TVres_theta.Write();
   cSD_TVres_theta.Close();


   TCanvas cSD_Wres_theta("cSD_Wres_theta");
   cSD_Wres_theta.Divide(3,6);
   cSD_Wres_theta.cd(1);
   hreswmcrec_theta_LMD->FitSlicesY();
   hreswmcrec_theta_LMD_1->GetYaxis()->SetTitle("w_{MC}-w_{REC}, mean [cm]");
   hreswmcrec_theta_LMD_2->GetYaxis()->SetTitle("w_{MC}-w_{REC}, sigma [cm]");
   hreswmcrec_theta_LMD->SetTitle("w_{MC}-w_{REC} (near LMD)");
   hreswmcrec_theta_LMD->Draw("colz");
   cSD_Wres_theta.cd(2);
   hreswmcrec_theta_LMD_1->SetMinimum(-0.005);
   hreswmcrec_theta_LMD_1->SetMaximum(0.005);
   hreswmcrec_theta_LMD_1->Draw();
   cSD_Wres_theta.cd(3);
   hreswmcrec_theta_LMD_2->SetMinimum(0);
   hreswmcrec_theta_LMD_2->SetMaximum(0.01);
   hreswmcrec_theta_LMD_2->Draw();
   cSD_Wres_theta.cd(4);
   hreswmcrec_theta_IP->FitSlicesY();
   hreswmcrec_theta_IP_1->GetYaxis()->SetTitle("w_{MC}-w_{REC}, mean [cm]");
   hreswmcrec_theta_IP_2->GetYaxis()->SetTitle("w_{MC}-w_{REC}, sigma [cm]");
   hreswmcrec_theta_IP->SetTitle("w_{MC}-w_{REC} (near IP)");
   hreswmcrec_theta_IP->Draw("colz");
   cSD_Wres_theta.cd(5);
   hreswmcrec_theta_IP_1->SetMinimum(-0.5);
   hreswmcrec_theta_IP_1->SetMaximum(0.5);
   hreswmcrec_theta_IP_1->Draw();
   cSD_Wres_theta.cd(6);
   hreswmcrec_theta_IP_2->SetMinimum(0);
   hreswmcrec_theta_IP_2->SetMaximum(1);
   hreswmcrec_theta_IP_2->Draw();
   cSD_Wres_theta.cd(7);
   hreswmclmdrec_theta_LMD->FitSlicesY();
   hreswmclmdrec_theta_LMD_1->GetYaxis()->SetTitle("w_{MCLMD}-w_{REC}, mean [cm]");
   hreswmclmdrec_theta_LMD_2->GetYaxis()->SetTitle("w_{MCLMD}-w_{REC}, sigma [cm]");
   hreswmclmdrec_theta_LMD->SetTitle("w_{MCLMD}-w_{REC} (near LMD)");
   hreswmclmdrec_theta_LMD->Draw("colz");
   cSD_Wres_theta.cd(8);
   hreswmclmdrec_theta_LMD_1->SetMinimum(-0.005);
   hreswmclmdrec_theta_LMD_1->SetMaximum(0.005);
   hreswmclmdrec_theta_LMD_1->Draw();
   cSD_Wres_theta.cd(9);
   hreswmclmdrec_theta_LMD_2->SetMinimum(0);
   hreswmclmdrec_theta_LMD_2->SetMaximum(0.01);
   hreswmclmdrec_theta_LMD_2->Draw();
   cSD_Wres_theta.cd(10);
   hreswmclmdrec_theta_IP->FitSlicesY();
   hreswmclmdrec_theta_IP_1->GetYaxis()->SetTitle("w_{MCLMD}-w_{REC}, mean [cm]");
   hreswmclmdrec_theta_IP_2->GetYaxis()->SetTitle("w_{MCLMD}-w_{REC}, sigma [cm]");
   hreswmclmdrec_theta_IP->SetTitle("w_{MCLMD}-w_{REC} (near IP)");
   hreswmclmdrec_theta_IP->Draw("colz");
   cSD_Wres_theta.cd(11);
   hreswmclmdrec_theta_IP_1->SetMinimum(-0.5);
   hreswmclmdrec_theta_IP_1->SetMaximum(0.5);
   hreswmclmdrec_theta_IP_1->Draw();
   cSD_Wres_theta.cd(12);
   hreswmclmdrec_theta_IP_2->SetMinimum(0);
   hreswmclmdrec_theta_IP_2->SetMaximum(1);
   hreswmclmdrec_theta_IP_2->Draw();
   cSD_Wres_theta.cd(13);
   hreswmcmclmd_theta_LMD->FitSlicesY();
   hreswmcmclmd_theta_LMD_1->GetYaxis()->SetTitle("w_{MCLMD}-w_{MC}, mean [cm]");
   hreswmcmclmd_theta_LMD_2->GetYaxis()->SetTitle("w_{MCLMD}-w_{MC}, sigma [cm]");
   hreswmcmclmd_theta_LMD->SetTitle("w_{MCLMD}-w_{MC} (near LMD)");
   hreswmcmclmd_theta_LMD->Draw("colz");
   cSD_Wres_theta.cd(14);
   hreswmcmclmd_theta_LMD_1->SetMinimum(-0.005);
   hreswmcmclmd_theta_LMD_1->SetMaximum(0.005);
   hreswmcmclmd_theta_LMD_1->Draw();
   cSD_Wres_theta.cd(15);
   hreswmcmclmd_theta_LMD_2->SetMinimum(0);
   hreswmcmclmd_theta_LMD_2->SetMaximum(0.01);
   hreswmcmclmd_theta_LMD_2->Draw();
   cSD_Wres_theta.cd(16);
   hreswmcmclmd_theta_IP->FitSlicesY();
   hreswmcmclmd_theta_IP_1->GetYaxis()->SetTitle("w_{MCLMD}-w_{MC}, mean [cm]");
   hreswmcmclmd_theta_IP_2->GetYaxis()->SetTitle("w_{MCLMD}-w_{MC}, sigma [cm]");
   hreswmcmclmd_theta_IP->SetTitle("w_{MCLMD}-w_{MC} (near IP)");
   hreswmcmclmd_theta_IP->Draw("colz");
   cSD_Wres_theta.cd(17);
   hreswmcmclmd_theta_IP_1->SetMinimum(-0.5);
   hreswmcmclmd_theta_IP_1->SetMaximum(0.5);
   hreswmcmclmd_theta_IP_1->Draw();
   cSD_Wres_theta.cd(18);
   hreswmcmclmd_theta_IP_2->SetMinimum(0);
   hreswmcmclmd_theta_IP_2->SetMaximum(1);
   hreswmcmclmd_theta_IP_2->Draw();
   cSD_Wres_theta.Print(resname_pdf_o);
   cSD_Wres_theta.Write();
   cSD_Wres_theta.Close();

TCanvas cSD_TWres_theta("cSD_TWres_theta");
   cSD_TWres_theta.Divide(3,6);
   cSD_TWres_theta.cd(1);
   hrestwmcrec_theta_LMD->FitSlicesY();
   hrestwmcrec_theta_LMD_1->GetYaxis()->SetTitle("w'_{MC}-w'_{REC}, mean");
   hrestwmcrec_theta_LMD_2->GetYaxis()->SetTitle("w'_{MC}-w'_{REC}, sigma");
   hrestwmcrec_theta_LMD->SetTitle("w'_{MC}-w'_{REC} (near LMD)");
   hrestwmcrec_theta_LMD->Draw("colz");
   cSD_TWres_theta.cd(2);
   hrestwmcrec_theta_LMD_1->SetMinimum(-0.001);
   hrestwmcrec_theta_LMD_1->SetMaximum(0.001);
   hrestwmcrec_theta_LMD_1->Draw();
   cSD_TWres_theta.cd(3);
   hrestwmcrec_theta_LMD_2->SetMinimum(0);
   hrestwmcrec_theta_LMD_2->SetMaximum(0.0015);
   hrestwmcrec_theta_LMD_2->Draw();
   cSD_TWres_theta.cd(4);
   hrestwmcrec_theta_IP->FitSlicesY();
   hrestwmcrec_theta_IP_1->GetYaxis()->SetTitle("w'_{MC}-w'_{REC}, mean");
   hrestwmcrec_theta_IP_2->GetYaxis()->SetTitle("w'_{MC}-w'_{REC}, sigma");
   hrestwmcrec_theta_IP->SetTitle("w'_{MC}-w'_{REC} (near IP)");
   hrestwmcrec_theta_IP->Draw("colz");
   cSD_TWres_theta.cd(5);
   hrestwmcrec_theta_IP_1->SetMinimum(-0.0015);
   hrestwmcrec_theta_IP_1->SetMaximum(0.0015);
   hrestwmcrec_theta_IP_1->Draw();
   cSD_TWres_theta.cd(6);
   hrestwmcrec_theta_IP_2->SetMinimum(0);
   hrestwmcrec_theta_IP_2->SetMaximum(0.0015);
   hrestwmcrec_theta_IP_2->Draw();
   cSD_TWres_theta.cd(7);
   hrestwmclmdrec_theta_LMD->FitSlicesY();
   hrestwmclmdrec_theta_LMD_1->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{REC}, mean");
   hrestwmclmdrec_theta_LMD_2->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{REC}, sigma");
   hrestwmclmdrec_theta_LMD->SetTitle("w'_{MCLMD}-w'_{REC} (near LMD)");
   hrestwmclmdrec_theta_LMD->Draw("colz");
   cSD_TWres_theta.cd(8);
   hrestwmclmdrec_theta_LMD_1->SetMinimum(-0.001);
   hrestwmclmdrec_theta_LMD_1->SetMaximum(0.001);
   hrestwmclmdrec_theta_LMD_1->Draw();
   cSD_TWres_theta.cd(9);
   hrestwmclmdrec_theta_LMD_2->SetMinimum(0);
   hrestwmclmdrec_theta_LMD_2->SetMaximum(0.0015);
   hrestwmclmdrec_theta_LMD_2->Draw();
   cSD_TWres_theta.cd(10);
   hrestwmclmdrec_theta_IP->FitSlicesY();
   hrestwmclmdrec_theta_IP_1->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{REC}, mean");
   hrestwmclmdrec_theta_IP_2->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{REC}, sigma");
   hrestwmclmdrec_theta_IP->SetTitle("w'_{MCLMD}-w'_{REC} (near IP)");
   hrestwmclmdrec_theta_IP->Draw("colz");
   cSD_TWres_theta.cd(11);
   hrestwmclmdrec_theta_IP_1->SetMinimum(-0.0015);
   hrestwmclmdrec_theta_IP_1->SetMaximum(0.0015);
   hrestwmclmdrec_theta_IP_1->Draw();
   cSD_TWres_theta.cd(12);
   hrestwmclmdrec_theta_IP_2->SetMinimum(0);
   hrestwmclmdrec_theta_IP_2->SetMaximum(0.0015);
   hrestwmclmdrec_theta_IP_2->Draw();
   cSD_TWres_theta.cd(13);
   hrestwmcmclmd_theta_LMD->FitSlicesY();
   hrestwmcmclmd_theta_LMD_1->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{MC}, mean");
   hrestwmcmclmd_theta_LMD_2->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{MC}, sigma");
   hrestwmcmclmd_theta_LMD->SetTitle("w'_{MCLMD}-w'_{MC} (near LMD)");
   hrestwmcmclmd_theta_LMD->Draw("colz");
   cSD_TWres_theta.cd(14);
   hrestwmcmclmd_theta_LMD_1->SetMinimum(-0.001);
   hrestwmcmclmd_theta_LMD_1->SetMaximum(0.001);
   hrestwmcmclmd_theta_LMD_1->Draw();
   cSD_TWres_theta.cd(15);
   hrestwmcmclmd_theta_LMD_2->SetMinimum(0);
   hrestwmcmclmd_theta_LMD_2->SetMaximum(0.0015);
   hrestwmcmclmd_theta_LMD_2->Draw();
   cSD_TWres_theta.cd(16);
   hrestwmcmclmd_theta_IP->FitSlicesY();
   hrestwmcmclmd_theta_IP_1->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{MC}, mean");
   hrestwmcmclmd_theta_IP_2->GetYaxis()->SetTitle("w'_{MCLMD}-w'_{MC}, sigma");
   hrestwmcmclmd_theta_IP->SetTitle("w'_{MCLMD}-w'_{MC} (near IP)");
   hrestwmcmclmd_theta_IP->Draw("colz");
   cSD_TWres_theta.cd(17);
   hrestwmcmclmd_theta_IP_1->SetMinimum(-0.0015);
   hrestwmcmclmd_theta_IP_1->SetMaximum(0.0015);
   hrestwmcmclmd_theta_IP_1->Draw();
   cSD_TWres_theta.cd(18);
   hrestwmcmclmd_theta_IP_2->SetMinimum(0);
   hrestwmcmclmd_theta_IP_2->SetMaximum(0.0015);
   hrestwmcmclmd_theta_IP_2->Draw();
   cSD_TWres_theta.Print(resname_pdf_o);
   cSD_TWres_theta.Write();
   cSD_TWres_theta.Close();

   //theta resolution vs. theta and phi
   TH2D *hresthmcrec_phi_LMD = new TH2D("hresthmcrec_phi_LMD",";#phi, rad; (#theta_{MC} - #theta_{REC}), mrad",1e2,-3.15,3.15,1e2,-5.0,5.0);
   TH2D *hresthmcrec_phi_IP = new TH2D("hresthmcrec_phi_IP",";#phi, rad; (#theta_{MC} - #theta_{REC}), mrad",1e2,-3.15,3.15,1e2,-5.0,5.0);
   tbp->Project("hresthmcrec_phi_LMD","(1e3*(thetamc-thetarec)):phiMCip",conditionLMD);
   tbp->Project("hresthmcrec_phi_IP","(1e3*(thetamc-thetarec)):phiMCip",conditionIP);
   TH2D *hresthmclmdrec_phi_LMD = new TH2D("hresthmclmdrec_phi_LMD",";#phi, rad; (#theta_{MCLMD} - #theta_{REC}), mrad",1e2,-3.15,3.15,1e2,-5.0,5.0);
   TH2D *hresthmclmdrec_phi_IP = new TH2D("hresthmclmdrec_phi_IP",";#phi, rad; (#theta_{MCLMD} - #theta_{REC}), mrad",1e2,-3.15,3.15,1e2,-5.0,5.0);
   tbp->Project("hresthmclmdrec_phi_LMD","1e3*(thetamclmd-thetarec):phiMCip",conditionLMD);
   tbp->Project("hresthmclmdrec_phi_IP","1e3*(thetamclmd-thetarec):phiMCip",conditionIP);
   TH2D *hresthmcmclmd_phi_LMD = new TH2D("hresthmcmclmd_phi_LMD",";#phi, rad; (#theta_{MCLMD} - #theta_{MC}), mrad",1e2,-3.15,3.15,1e2,-5.0,5.0);
   TH2D *hresthmcmclmd_phi_IP = new TH2D("hresthmcmclmd_phi_IP",";#phi, rad; (#theta_{MCLMD} - #theta_{MC}), mrad",1e2,-3.15,3.15,1e2,-5.0,5.0);
   tbp->Project("hresthmcmclmd_phi_LMD","1e3*(thetamclmd-thetamc):phiMCip",conditionLMD);
   tbp->Project("hresthmcmclmd_phi_IP","1e3*(thetamclmd-thetamc):phiMCip",conditionIP);

   TH2D *hresthmcrec_theta_LMD = new TH2D("hresthmcrec_theta_LMD",";#theta, mrad;#theta_{MC} - #theta_{REC}, mrad",4e1,2,11,1e2,-5.0,5.0);
   TH2D *hresthmcrec_theta_IP = new TH2D("hresthmcrec_theta_IP",";#theta, mrad;#theta_{MC} - #theta_{REC}, mrad",4e1,2,11,1e2,-5.0,5.0);
   tbp->Project("hresthmcrec_theta_LMD","1e3*(thetamc-thetarec):1e3*thetaMCip",conditionLMD);
   tbp->Project("hresthmcrec_theta_IP","1e3*(thetamc-thetarec):1e3*thetaMCip",conditionIP);
   TH2D *hresthmclmdrec_theta_LMD = new TH2D("hresthmclmdrec_theta_LMD",";#theta, mrad;#theta_{MCLMD} - #theta_{REC}, mrad",4e1,2,11,1e2,-5.0,5.0);
   TH2D *hresthmclmdrec_theta_IP = new TH2D("hresthmclmdrec_theta_IP",";#theta, mrad;#theta_{MCLMD} - #theta_{REC}, mrad",4e1,2,11,1e2,-5.0,5.0);
   tbp->Project("hresthmclmdrec_theta_LMD","1e3*(thetamclmd-thetarec):1e3*thetaMCip",conditionLMD);
   tbp->Project("hresthmclmdrec_theta_IP","1e3*(thetamclmd-thetarec):1e3*thetaMCip",conditionIP);
   TH2D *hresthmcmclmd_theta_LMD = new TH2D("hresthmcmclmd_theta_LMD",";#theta, mrad;#theta_{MCLMD} - #theta_{MC}, mrad",4e1,2,11,1e2,-5.0,5.0);
   TH2D *hresthmcmclmd_theta_IP = new TH2D("hresthmcmclmd_theta_IP",";#theta, mrad;#theta_{MCLMD} - #theta_{MC}, mrad",4e1,2,11,1e2,-5.0,5.0);
   tbp->Project("hresthmcmclmd_theta_LMD","1e3*(thetamclmd-thetamc):1e3*thetaMCip",conditionLMD);
   tbp->Project("hresthmcmclmd_theta_IP","1e3*(thetamclmd-thetamc):1e3*thetaMCip",conditionIP);
   

 TCanvas cSD_Thres_phi("cSD_Thres_phi");
   cSD_Thres_phi.Divide(3,6);
   cSD_Thres_phi.cd(1);
   hresthmcrec_phi_LMD->FitSlicesY();
   hresthmcrec_phi_LMD_1->GetYaxis()->SetTitle("#theta_{MC}-#theta_{REC}, mean [mrad]");
   hresthmcrec_phi_LMD_2->GetYaxis()->SetTitle("#theta_{MC}-#theta_{REC}, sigma [mrad]");
   hresthmcrec_phi_LMD->SetTitle("#theta_{MC}-#theta_{REC} (near LMD)");
   hresthmcrec_phi_LMD->Draw("colz");
   cSD_Thres_phi.cd(2);
   hresthmcrec_phi_LMD_1->SetMinimum(-0.2);
   hresthmcrec_phi_LMD_1->SetMaximum(0.2);
   hresthmcrec_phi_LMD_1->Draw();
   cSD_Thres_phi.cd(3);
   hresthmcrec_phi_LMD_2->SetMinimum(0);
   hresthmcrec_phi_LMD_2->SetMaximum(0.7);
   hresthmcrec_phi_LMD_2->Draw();
   cSD_Thres_phi.cd(4);
   hresthmcrec_phi_IP->FitSlicesY();
   hresthmcrec_phi_IP_1->GetYaxis()->SetTitle("#theta_{MC}-#theta_{REC}, mean [mrad]");
   hresthmcrec_phi_IP_2->GetYaxis()->SetTitle("#theta_{MC}-#theta_{REC}, sigma [mrad]");
   hresthmcrec_phi_IP->SetTitle("#theta_{MC}-#theta_{REC} (near IP)");
   hresthmcrec_phi_IP->Draw("colz");
   cSD_Thres_phi.cd(5);
   hresthmcrec_phi_IP_1->SetMinimum(-0.2);
   hresthmcrec_phi_IP_1->SetMaximum(0.2);
   hresthmcrec_phi_IP_1->Draw();
   cSD_Thres_phi.cd(6);
   hresthmcrec_phi_IP_2->SetMinimum(0);
   hresthmcrec_phi_IP_2->SetMaximum(0.7);
   hresthmcrec_phi_IP_2->Draw();
   cSD_Thres_phi.cd(7);
   hresthmclmdrec_phi_LMD->FitSlicesY();
   hresthmclmdrec_phi_LMD_1->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{REC}, mean [mrad]");
   hresthmclmdrec_phi_LMD_2->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{REC}, sigma [mrad]");
   hresthmclmdrec_phi_LMD->SetTitle("#theta_{MCLMD}-#theta_{REC} (near LMD)");
   hresthmclmdrec_phi_LMD->Draw("colz");
   cSD_Thres_phi.cd(8);
   hresthmclmdrec_phi_LMD_1->SetMinimum(-0.2);
   hresthmclmdrec_phi_LMD_1->SetMaximum(0.2);
   hresthmclmdrec_phi_LMD_1->Draw();
   cSD_Thres_phi.cd(9);
   hresthmclmdrec_phi_LMD_2->SetMinimum(0);
   hresthmclmdrec_phi_LMD_2->SetMaximum(0.7);
   hresthmclmdrec_phi_LMD_2->Draw();
   cSD_Thres_phi.cd(10);
   hresthmclmdrec_phi_IP->FitSlicesY();
   hresthmclmdrec_phi_IP_1->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{REC}, mean [mrad]");
   hresthmclmdrec_phi_IP_2->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{REC}, sigma [mrad]");
   hresthmclmdrec_phi_IP->SetTitle("#theta_{MCLMD}-#theta_{REC} (near IP)");
   hresthmclmdrec_phi_IP->Draw("colz");
   cSD_Thres_phi.cd(11);
   hresthmclmdrec_phi_IP_1->SetMinimum(-0.2);
   hresthmclmdrec_phi_IP_1->SetMaximum(0.2);
   hresthmclmdrec_phi_IP_1->Draw();
   cSD_Thres_phi.cd(12);
   hresthmclmdrec_phi_IP_2->SetMinimum(0);
   hresthmclmdrec_phi_IP_2->SetMaximum(0.7);
   hresthmclmdrec_phi_IP_2->Draw();
   cSD_Thres_phi.cd(13);
   hresthmcmclmd_phi_LMD->FitSlicesY();
   hresthmcmclmd_phi_LMD_1->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{MC}, mean [mrad]");
   hresthmcmclmd_phi_LMD_2->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{MC}, sigma [mrad]");
   hresthmcmclmd_phi_LMD->SetTitle("#theta_{MCLMD}-#theta_{MC} (near LMD)");
   hresthmcmclmd_phi_LMD->Draw("colz");
   cSD_Thres_phi.cd(14);
   hresthmcmclmd_phi_LMD_1->SetMinimum(-0.2);
   hresthmcmclmd_phi_LMD_1->SetMaximum(0.2);
   hresthmcmclmd_phi_LMD_1->Draw();
   cSD_Thres_phi.cd(15);
   hresthmcmclmd_phi_LMD_2->SetMinimum(0);
   hresthmcmclmd_phi_LMD_2->SetMaximum(0.7);
   hresthmcmclmd_phi_LMD_2->Draw();
   cSD_Thres_phi.cd(16);
   hresthmcmclmd_phi_IP->FitSlicesY();
   hresthmcmclmd_phi_IP_1->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{MC}, mean [mrad]");
   hresthmcmclmd_phi_IP_2->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{MC}, sigma [mrad]");
   hresthmcmclmd_phi_IP->SetTitle("#theta_{MCLMD}-#theta_{MC} (near IP)");
   hresthmcmclmd_phi_IP->Draw("colz");
   cSD_Thres_phi.cd(17);
   hresthmcmclmd_phi_IP_1->SetMinimum(-0.2);
   hresthmcmclmd_phi_IP_1->SetMaximum(0.2);
   hresthmcmclmd_phi_IP_1->Draw();
   cSD_Thres_phi.cd(18);
   hresthmcmclmd_phi_IP_2->SetMinimum(0);
   hresthmcmclmd_phi_IP_2->SetMaximum(0.7);
   hresthmcmclmd_phi_IP_2->Draw();
   cSD_Thres_phi.Print(resname_pdf_o);
   cSD_Thres_phi.Write();
   cSD_Thres_phi.Close();

   TCanvas cSD_Thres_theta("cSD_Thres_theta");
   cSD_Thres_theta.Divide(3,6);
   cSD_Thres_theta.cd(1);
   hresthmcrec_theta_LMD->FitSlicesY();
   hresthmcrec_theta_LMD_1->GetYaxis()->SetTitle("#theta_{MC}-#theta_{REC}, mean [mrad]");
   hresthmcrec_theta_LMD_2->GetYaxis()->SetTitle("#theta_{MC}-#theta_{REC}, sigma [mrad]");
   hresthmcrec_theta_LMD->SetTitle("#theta_{MC}-#theta_{REC} (near LMD)");
   hresthmcrec_theta_LMD->Draw("colz");
   cSD_Thres_theta.cd(2);
   hresthmcrec_theta_LMD_1->SetMinimum(-0.2);
   hresthmcrec_theta_LMD_1->SetMaximum(0.2);
   hresthmcrec_theta_LMD_1->Draw();
   cSD_Thres_theta.cd(3);
   hresthmcrec_theta_LMD_2->SetMinimum(0);
   hresthmcrec_theta_LMD_2->SetMaximum(0.7);
   hresthmcrec_theta_LMD_2->Draw();
   cSD_Thres_theta.cd(4);
   hresthmcrec_theta_IP->FitSlicesY();
   hresthmcrec_theta_IP_1->GetYaxis()->SetTitle("#theta_{MC}-#theta_{REC}, mean [mrad]");
   hresthmcrec_theta_IP_2->GetYaxis()->SetTitle("#theta_{MC}-#theta_{REC}, sigma [mrad]");
   hresthmcrec_theta_IP->SetTitle("#theta_{MC}-#theta_{REC} (near IP)");
   hresthmcrec_theta_IP->Draw("colz");
   cSD_Thres_theta.cd(5);
   hresthmcrec_theta_IP_1->SetMinimum(-0.2);
   hresthmcrec_theta_IP_1->SetMaximum(0.2);
   hresthmcrec_theta_IP_1->Draw();
   cSD_Thres_theta.cd(6);
   hresthmcrec_theta_IP_2->SetMinimum(0);
   hresthmcrec_theta_IP_2->SetMaximum(0.7);
   hresthmcrec_theta_IP_2->Draw();
   cSD_Thres_theta.cd(7);
   hresthmclmdrec_theta_LMD->FitSlicesY();
   hresthmclmdrec_theta_LMD_1->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{REC}, mean [mrad]");
   hresthmclmdrec_theta_LMD_2->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{REC}, sigma [mrad]");
   hresthmclmdrec_theta_LMD->SetTitle("#theta_{MCLMD}-#theta_{REC} (near LMD)");
   hresthmclmdrec_theta_LMD->Draw("colz");
   cSD_Thres_theta.cd(8);
   hresthmclmdrec_theta_LMD_1->SetMinimum(-0.2);
   hresthmclmdrec_theta_LMD_1->SetMaximum(0.2);
   hresthmclmdrec_theta_LMD_1->Draw();
   cSD_Thres_theta.cd(9);
   hresthmclmdrec_theta_LMD_2->SetMinimum(0);
   hresthmclmdrec_theta_LMD_2->SetMaximum(0.7);
   hresthmclmdrec_theta_LMD_2->Draw();
   cSD_Thres_theta.cd(10);
   hresthmclmdrec_theta_IP->FitSlicesY();
   hresthmclmdrec_theta_IP_1->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{REC}, mean [mrad]");
   hresthmclmdrec_theta_IP_2->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{REC}, sigma [mrad]");
   hresthmclmdrec_theta_IP->SetTitle("#theta_{MCLMD}-#theta_{REC} (near IP)");
   hresthmclmdrec_theta_IP->Draw("colz");
   cSD_Thres_theta.cd(11);
   hresthmclmdrec_theta_IP_1->SetMinimum(-0.2);
   hresthmclmdrec_theta_IP_1->SetMaximum(0.2);
   hresthmclmdrec_theta_IP_1->Draw();
   cSD_Thres_theta.cd(12);
   hresthmclmdrec_theta_IP_2->SetMinimum(0);
   hresthmclmdrec_theta_IP_2->SetMaximum(0.7);
   hresthmclmdrec_theta_IP_2->Draw();
   cSD_Thres_theta.cd(13);
   hresthmcmclmd_theta_LMD->FitSlicesY();
   hresthmcmclmd_theta_LMD_1->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{MC}, mean [mrad]");
   hresthmcmclmd_theta_LMD_2->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{MC}, sigma [mrad]");
   hresthmcmclmd_theta_LMD->SetTitle("#theta_{MCLMD}-#theta_{MC} (near LMD)");
   hresthmcmclmd_theta_LMD->Draw("colz");
   cSD_Thres_theta.cd(14);
   hresthmcmclmd_theta_LMD_1->SetMinimum(-0.2);
   hresthmcmclmd_theta_LMD_1->SetMaximum(0.2);
   hresthmcmclmd_theta_LMD_1->Draw();
   cSD_Thres_theta.cd(15);
   hresthmcmclmd_theta_LMD_2->SetMinimum(0);
   hresthmcmclmd_theta_LMD_2->SetMaximum(0.5);
   hresthmcmclmd_theta_LMD_2->Draw();
   cSD_Thres_theta.cd(16);
   hresthmcmclmd_theta_IP->FitSlicesY();
   hresthmcmclmd_theta_IP_1->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{MC}, mean [mrad]");
   hresthmcmclmd_theta_IP_2->GetYaxis()->SetTitle("#theta_{MCLMD}-#theta_{MC}, sigma [mrad]");
   hresthmcmclmd_theta_IP->SetTitle("#theta_{MCLMD}-#theta_{MC} (near IP)");
   hresthmcmclmd_theta_IP->Draw("colz");
   cSD_Thres_theta.cd(17);
   hresthmcmclmd_theta_IP_1->SetMinimum(-0.2);
   hresthmcmclmd_theta_IP_1->SetMaximum(0.2);
   hresthmcmclmd_theta_IP_1->Draw();
   cSD_Thres_theta.cd(18);
   hresthmcmclmd_theta_IP_2->SetMinimum(0);
   hresthmcmclmd_theta_IP_2->SetMaximum(0.7);
   hresthmcmclmd_theta_IP_2->Draw();
   cSD_Thres_theta.Print(resname_pdf_o);
   cSD_Thres_theta.Write();
   cSD_Thres_theta.Close();


   //phi resolution vs. theta and phi
   TH2D *hresphimcrec_phi_LMD = new TH2D("hresphimcrec_phi_LMD",";#phi, rad; (#phi_{MC} - #phi_{REC}), mrad",1e2,-3.15,3.15,1e2,-100.0,100.0);
   TH2D *hresphimcrec_phi_IP = new TH2D("hresphimcrec_phi_IP",";#phi, rad; (#phi_{MC} - #phi_{REC}), mrad",1e2,-3.15,3.15,1e2,-100.0,100.0);
   tbp->Project("hresphimcrec_phi_LMD","1e3*(phimc-phirec):phiMCip",conditionLMD);
   tbp->Project("hresphimcrec_phi_IP","1e3*(phimc-phirec):phiMCip",conditionIP);
   TH2D *hresphimclmdrec_phi_LMD = new TH2D("hresphimclmdrec_phi_LMD",";#phi, rad; (#phi_{MCLMD} - #phi_{REC}), mrad",1e2,-3.15,3.15,1e2,-100.0,100.0);
   TH2D *hresphimclmdrec_phi_IP = new TH2D("hresphimclmdrec_phi_IP",";#phi, rad; (#phi_{MCLMD} - #phi_{REC}), mrad",1e2,-3.15,3.15,1e2,-100.0,100.0);
   tbp->Project("hresphimclmdrec_phi_LMD","1e3*(phimclmd-phirec):phiMCip",conditionLMD);
   tbp->Project("hresphimclmdrec_phi_IP","1e3*(phimclmd-phirec):phiMCip",conditionIP);
   TH2D *hresphimcmclmd_phi_LMD = new TH2D("hresphimcmclmd_phi_LMD",";#phi, rad; (#phi_{MCLMD} - #phi_{MC}), mrad",1e2,-3.15,3.15,1e2,-100.0,100.0);
   TH2D *hresphimcmclmd_phi_IP = new TH2D("hresphimcmclmd_phi_IP",";#phi, rad; (#phi_{MCLMD} - #phi_{MC}), mrad",1e2,-3.15,3.15,1e2,-100.0,100.0);
   tbp->Project("hresphimcmclmd_phi_LMD","1e3*(phimclmd-phimc):phiMCip",conditionLMD);
   tbp->Project("hresphimcmclmd_phi_IP","1e3*(phimclmd-phimc):phiMCip",conditionIP);

   TH2D *hresphimcrec_theta_LMD = new TH2D("hresphimcrec_theta_LMD",";#theta, mrad;#phi_{MC} - #phi_{REC}, mrad",4e1,2,11,1e2,-100.0,100.0);
   TH2D *hresphimcrec_theta_IP = new TH2D("hresphimcrec_theta_IP",";#theta, mrad;#phi_{MC} - #phi_{REC}, mrad",4e1,2,11,1e2,-100.0,100.0);
   tbp->Project("hresphimcrec_theta_LMD","1e3*(phimc-phirec):1e3*thetaMCip",conditionLMD);
   tbp->Project("hresphimcrec_theta_IP","1e3*(phimc-phirec):1e3*thetaMCip",conditionIP);
   TH2D *hresphimclmdrec_theta_LMD = new TH2D("hresphimclmdrec_theta_LMD",";#theta, mrad;#phi_{MCLMD} - #phi_{REC}, mrad",4e1,2,11,1e2,-100.0,100.0);
   TH2D *hresphimclmdrec_theta_IP = new TH2D("hresphimclmdrec_theta_IP",";#theta, mrad;#phi_{MCLMD} - #phi_{REC}, mrad",4e1,2,11,1e2,-100.0,100.0);
   tbp->Project("hresphimclmdrec_theta_LMD","1e3*(phimclmd-phirec):1e3*thetaMCip",conditionLMD);
   tbp->Project("hresphimclmdrec_theta_IP","1e3*(phimclmd-phirec):1e3*thetaMCip",conditionIP);
   TH2D *hresphimcmclmd_theta_LMD = new TH2D("hresphimcmclmd_theta_LMD",";#theta, mrad;#phi_{MCLMD} - #phi_{MC}, mrad",4e1,2,11,1e2,-100.0,100.0);
   TH2D *hresphimcmclmd_theta_IP = new TH2D("hresphimcmclmd_theta_IP",";#theta, mrad;#phi_{MCLMD} - #phi_{MC}, mrad",4e1,2,11,1e2,-100.0,100.0);
   tbp->Project("hresphimcmclmd_theta_LMD","1e3*(phimclmd-phimc):1e3*thetaMCip",conditionLMD);
   tbp->Project("hresphimcmclmd_theta_IP","1e3*(phimclmd-phimc):1e3*thetaMCip",conditionIP);
   

 TCanvas cSD_Phires_phi("cSD_Phires_phi");
   cSD_Phires_phi.Divide(3,6);
   cSD_Phires_phi.cd(1);
   hresphimcrec_phi_LMD->FitSlicesY();
   hresphimcrec_phi_LMD_1->GetYaxis()->SetTitle("#phi_{MC}-#phi_{REC}, mean [mrad]");
   hresphimcrec_phi_LMD_2->GetYaxis()->SetTitle("#phi_{MC}-#phi_{REC}, sigma [mrad]");
   hresphimcrec_phi_LMD->SetTitle("#phi_{MC}-#phi_{REC} (near LMD)");
   hresphimcrec_phi_LMD->Draw("colz");
   cSD_Phires_phi.cd(2);
   hresphimcrec_phi_LMD_1->SetMinimum(-40); 
   hresphimcrec_phi_LMD_1->SetMaximum(40);
   hresphimcrec_phi_LMD_1->Draw();
   cSD_Phires_phi.cd(3);
   hresphimcrec_phi_LMD_2->SetMinimum(0);
   hresphimcrec_phi_LMD_2->SetMaximum(200);
   hresphimcrec_phi_LMD_2->Draw();
   cSD_Phires_phi.cd(4);
   hresphimcrec_phi_IP->FitSlicesY();
   hresphimcrec_phi_IP_1->GetYaxis()->SetTitle("#phi_{MC}-#phi_{REC}, mean [mrad]");
   hresphimcrec_phi_IP_2->GetYaxis()->SetTitle("#phi_{MC}-#phi_{REC}, sigma [mrad]");
   hresphimcrec_phi_IP->SetTitle("#phi_{MC}-#phi_{REC} (near IP)");
   hresphimcrec_phi_IP->Draw("colz");
   cSD_Phires_phi.cd(5);
   hresphimcrec_phi_IP_1->SetMinimum(-40);
   hresphimcrec_phi_IP_1->SetMaximum(40);
   hresphimcrec_phi_IP_1->Draw();
   cSD_Phires_phi.cd(6);
   hresphimcrec_phi_IP_2->SetMinimum(0);
   hresphimcrec_phi_IP_2->SetMaximum(200);
   hresphimcrec_phi_IP_2->Draw();
   cSD_Phires_phi.cd(7);
   hresphimclmdrec_phi_LMD->FitSlicesY();
   hresphimclmdrec_phi_LMD_1->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{REC}, mean [mrad]");
   hresphimclmdrec_phi_LMD_2->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{REC}, sigma [mrad]");
   hresphimclmdrec_phi_LMD->SetTitle("#phi_{MCLMD}-#phi_{REC} (near LMD)");
   hresphimclmdrec_phi_LMD->Draw("colz");
   cSD_Phires_phi.cd(8);
   hresphimclmdrec_phi_LMD_1->SetMinimum(-40);
   hresphimclmdrec_phi_LMD_1->SetMaximum(40);
   hresphimclmdrec_phi_LMD_1->Draw();
   cSD_Phires_phi.cd(9);
   hresphimclmdrec_phi_LMD_2->SetMinimum(0);
   hresphimclmdrec_phi_LMD_2->SetMaximum(200);
   hresphimclmdrec_phi_LMD_2->Draw();
   cSD_Phires_phi.cd(10);
   hresphimclmdrec_phi_IP->FitSlicesY();
   hresphimclmdrec_phi_IP_1->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{REC}, mean [mrad]");
   hresphimclmdrec_phi_IP_2->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{REC}, sigma [mrad]");
   hresphimclmdrec_phi_IP->SetTitle("#phi_{MCLMD}-#phi_{REC} (near IP)");
   hresphimclmdrec_phi_IP->Draw("colz");
   cSD_Phires_phi.cd(11);
   hresphimclmdrec_phi_IP_1->SetMinimum(-40);
   hresphimclmdrec_phi_IP_1->SetMaximum(40);
   hresphimclmdrec_phi_IP_1->Draw();
   cSD_Phires_phi.cd(12);
   hresphimclmdrec_phi_IP_2->SetMinimum(0);
   hresphimclmdrec_phi_IP_2->SetMaximum(200);
   hresphimclmdrec_phi_IP_2->Draw();
   cSD_Phires_phi.cd(13);
   hresphimcmclmd_phi_LMD->FitSlicesY();
   hresphimcmclmd_phi_LMD_1->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{MC}, mean [mrad]");
   hresphimcmclmd_phi_LMD_2->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{MC}, sigma [mrad]");
   hresphimcmclmd_phi_LMD->SetTitle("#phi_{MCLMD}-#phi_{MC} (near LMD)");
   hresphimcmclmd_phi_LMD->Draw("colz");
   cSD_Phires_phi.cd(14);
   hresphimcmclmd_phi_LMD_1->SetMinimum(-40);
   hresphimcmclmd_phi_LMD_1->SetMaximum(40);
   hresphimcmclmd_phi_LMD_1->Draw();
   cSD_Phires_phi.cd(15);
   hresphimcmclmd_phi_LMD_2->SetMinimum(0);
   hresphimcmclmd_phi_LMD_2->SetMaximum(200);
   hresphimcmclmd_phi_LMD_2->Draw();
   cSD_Phires_phi.cd(16);
   hresphimcmclmd_phi_IP->FitSlicesY();
   hresphimcmclmd_phi_IP_1->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{MC}, mean [mrad]");
   hresphimcmclmd_phi_IP_2->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{MC}, sigma [mrad]");
   hresphimcmclmd_phi_IP->SetTitle("#phi_{MCLMD}-#phi_{MC} (near IP)");
   hresphimcmclmd_phi_IP->Draw("colz");
   cSD_Phires_phi.cd(17);
   hresphimcmclmd_phi_IP_1->SetMinimum(-40);
   hresphimcmclmd_phi_IP_1->SetMaximum(40);
   hresphimcmclmd_phi_IP_1->Draw();
   cSD_Phires_phi.cd(18);
   hresphimcmclmd_phi_IP_2->SetMinimum(0);
   hresphimcmclmd_phi_IP_2->SetMaximum(200);
   hresphimcmclmd_phi_IP_2->Draw();
   cSD_Phires_phi.Print(resname_pdf_o);
   cSD_Phires_phi.Write();
   cSD_Phires_phi.Close();

   TCanvas cSD_Phires_theta("cSD_Phires_theta");
   cSD_Phires_theta.Divide(3,6);
   cSD_Phires_theta.cd(1);
   hresphimcrec_theta_LMD->FitSlicesY();
   hresphimcrec_theta_LMD_1->GetYaxis()->SetTitle("#phi_{MC}-#phi_{REC}, mean [mrad]");
   hresphimcrec_theta_LMD_2->GetYaxis()->SetTitle("#phi_{MC}-#phi_{REC}, sigma [mrad]");
   hresphimcrec_theta_LMD->SetTitle("#phi_{MC}-#phi_{REC} (near LMD)");
   hresphimcrec_theta_LMD->Draw("colz");
   cSD_Phires_theta.cd(2);
   hresphimcrec_theta_LMD_1->SetMinimum(-40);
   hresphimcrec_theta_LMD_1->SetMaximum(40);
   hresphimcrec_theta_LMD_1->Draw();
   cSD_Phires_theta.cd(3);
   hresphimcrec_theta_LMD_2->SetMinimum(0);
   hresphimcrec_theta_LMD_2->SetMaximum(200);
   hresphimcrec_theta_LMD_2->Draw();
   cSD_Phires_theta.cd(4);
   hresphimcrec_theta_IP->FitSlicesY();
   hresphimcrec_theta_IP_1->GetYaxis()->SetTitle("#phi_{MC}-#phi_{REC}, mean [mrad]");
   hresphimcrec_theta_IP_2->GetYaxis()->SetTitle("#phi_{MC}-#phi_{REC}, sigma [mrad]");
   hresphimcrec_theta_IP->SetTitle("#phi_{MC}-#phi_{REC} (near IP)");
   hresphimcrec_theta_IP->Draw("colz");
   cSD_Phires_theta.cd(5);
   hresphimcrec_theta_IP_1->SetMinimum(-40);
   hresphimcrec_theta_IP_1->SetMaximum(40);
   hresphimcrec_theta_IP_1->Draw();
   cSD_Phires_theta.cd(6);
   hresphimcrec_theta_IP_2->SetMinimum(0);
   hresphimcrec_theta_IP_2->SetMaximum(200);
   hresphimcrec_theta_IP_2->Draw();
   cSD_Phires_theta.cd(7);
   hresphimclmdrec_theta_LMD->FitSlicesY();
   hresphimclmdrec_theta_LMD_1->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{REC}, mean [mrad]");
   hresphimclmdrec_theta_LMD_2->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{REC}, sigma [mrad]");
   hresphimclmdrec_theta_LMD->SetTitle("#phi_{MCLMD}-#phi_{REC} (near LMD)");
   hresphimclmdrec_theta_LMD->Draw("colz");
   cSD_Phires_theta.cd(8);
   hresphimclmdrec_theta_LMD_1->SetMinimum(-40);
   hresphimclmdrec_theta_LMD_1->SetMaximum(40);
   hresphimclmdrec_theta_LMD_1->Draw();
   cSD_Phires_theta.cd(9);
   hresphimclmdrec_theta_LMD_2->SetMinimum(0);
   hresphimclmdrec_theta_LMD_2->SetMaximum(200);
   hresphimclmdrec_theta_LMD_2->Draw();
   cSD_Phires_theta.cd(10);
   hresphimclmdrec_theta_IP->FitSlicesY();
   hresphimclmdrec_theta_IP_1->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{REC}, mean [mrad]");
   hresphimclmdrec_theta_IP_2->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{REC}, sigma [mrad]");
   hresphimclmdrec_theta_IP->SetTitle("#phi_{MCLMD}-#phi_{REC} (near IP)");
   hresphimclmdrec_theta_IP->Draw("colz");
   cSD_Phires_theta.cd(11);
   hresphimclmdrec_theta_IP_1->SetMinimum(-40);
   hresphimclmdrec_theta_IP_1->SetMaximum(40);
   hresphimclmdrec_theta_IP_1->Draw();
   cSD_Phires_theta.cd(12);
   hresphimclmdrec_theta_IP_2->SetMinimum(0);
   hresphimclmdrec_theta_IP_2->SetMaximum(200);
   hresphimclmdrec_theta_IP_2->Draw();
   cSD_Phires_theta.cd(13);
   hresphimcmclmd_theta_LMD->FitSlicesY();
   hresphimcmclmd_theta_LMD_1->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{MC}, mean [mrad]");
   hresphimcmclmd_theta_LMD_2->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{MC}, sigma [mrad]");
   hresphimcmclmd_theta_LMD->SetTitle("#phi_{MCLMD}-#phi_{MC} (near LMD)");
   hresphimcmclmd_theta_LMD->Draw("colz");
   cSD_Phires_theta.cd(14);
   hresphimcmclmd_theta_LMD_1->SetMinimum(-40);
   hresphimcmclmd_theta_LMD_1->SetMaximum(40);
   hresphimcmclmd_theta_LMD_1->Draw();
   cSD_Phires_theta.cd(15);
   hresphimcmclmd_theta_LMD_2->SetMinimum(0);
   hresphimcmclmd_theta_LMD_2->SetMaximum(0.5);
   hresphimcmclmd_theta_LMD_2->Draw();
   cSD_Phires_theta.cd(16);
   hresphimcmclmd_theta_IP->FitSlicesY();
   hresphimcmclmd_theta_IP_1->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{MC}, mean [mrad]");
   hresphimcmclmd_theta_IP_2->GetYaxis()->SetTitle("#phi_{MCLMD}-#phi_{MC}, sigma [mrad]");
   hresphimcmclmd_theta_IP->SetTitle("#phi_{MCLMD}-#phi_{MC} (near IP)");
   hresphimcmclmd_theta_IP->Draw("colz");
   cSD_Phires_theta.cd(17);
   hresphimcmclmd_theta_IP_1->SetMinimum(-40);
   hresphimcmclmd_theta_IP_1->SetMaximum(40);
   hresphimcmclmd_theta_IP_1->Draw();
   cSD_Phires_theta.cd(18);
   hresphimcmclmd_theta_IP_2->SetMinimum(0);
   hresphimcmclmd_theta_IP_2->SetMaximum(200);
   hresphimcmclmd_theta_IP_2->Draw();
   cSD_Phires_theta.Print(resname_pdf_o);
   cSD_Phires_theta.Write();
   cSD_Phires_theta.Close();


 //   /// compare MCLMD and MC
 //  //v

 //   TCanvas c8tra("vpar_mc_mclmd");
 //   c8tra.Divide(3,2);
 //   c8tra.cd(1);
 //   hvmclmdz->FitSlicesY();
 //   hvmclmdz_1->GetYaxis()->SetTitle("v_{MC}-v_{MCLMD}, mean");
 //   hvmclmdz_2->GetYaxis()->SetTitle("v_{MC}-v_{MCLMD}, sigma");
 //   hvmclmdz_1->SetTitle("v_{MC}-v_{MCLMD}");
 //   hvmclmdz_2->SetTitle("");
 //   hvmclmdz_1->SetMinimum(-1e-2);
 //   hvmclmdz_1->SetMaximum(1e-2);
 //   hvmclmdz_1->Draw();
 //   c8tra.cd(2);
 //   hvmclmdz_2->SetMinimum(0.);
 //   hvmclmdz_2->SetMaximum(0.5*vsigmamax);
 //   hvmclmdz_2->Draw();
 //   c8tra.cd(3);
 //   hvmclmdz->Draw("colz");
 //   c8tra.cd(4);
 //   hwmclmdz->FitSlicesY();
 //   hwmclmdz_1->GetYaxis()->SetTitle("w_{MC}-w_{MCLMD}, mean");
 //   hwmclmdz_2->GetYaxis()->SetTitle("w_{MC}-w_{MCLMD}, sigma");
 //   hwmclmdz_1->SetTitle("w_{MC}-w_{MCLMD}");
 //   hwmclmdz_2->SetTitle("");
 //   hwmclmdz_1->SetMinimum(-1e-2);
 //   hwmclmdz_1->SetMaximum(1e-2);
 //   hwmclmdz_1->Draw();
 //   c8tra.cd(5);
 //   hwmclmdz_2->SetMinimum(0.);
 //   hwmclmdz_2->SetMaximum(0.5*vsigmamax);
 //   hwmclmdz_2->Draw();
 //   c8tra.cd(6);
 //   hwmclmdz->Draw("colz");
 //   c8tra.Print(resname_pdf_o);
 //   c8tra.Write();
 //   c8tra.Close();
 //   TCanvas c9tra("tvpar_mc_mclmd");
 //   c9tra.Divide(3,2);
 //   c9tra.cd(1);
 //   htvmclmdz->FitSlicesY();
 //   htvmclmdz_1->GetYaxis()->SetTitle("v'_{MC}-v'_{MCLMD}, mean");
 //   htvmclmdz_2->GetYaxis()->SetTitle("v'_{MC}-v'_{MCLMD}, sigma");
 //   htvmclmdz_1->SetTitle("v'_{MC}-v'_{MCLMD}");
 //   htvmclmdz_2->SetTitle("");
 //   htvmclmdz_1->SetMinimum(-2e-5);
 //   htvmclmdz_1->SetMaximum(2e-5);
 //   htvmclmdz_1->Draw();
 //   c9tra.cd(2);
 //   htvmclmdz_2->SetMinimum(0.);
 //   htvmclmdz_2->SetMaximum(1.2*tvsigmamax);
 //   htvmclmdz_2->Draw();
 //   c9tra.cd(3);
 //   htvmclmdz->Draw("colz");
 //   c9tra.cd(4);
 //   htwmclmdz->FitSlicesY();
 //   htwmclmdz_1->GetYaxis()->SetTitle("w'_{MC}-w'_{MCLMD}, mean");
 //   htwmclmdz_2->GetYaxis()->SetTitle("w'_{MC}-w'_{MCLMD}, sigma");
 //   htwmclmdz_1->SetTitle("w'_{MC}-w'_{MCLMD}");
 //   htwmclmdz_2->SetTitle("");
 //   htwmclmdz_1->SetMinimum(-2e-5);
 //   htwmclmdz_1->SetMaximum(2e-5);
 //   htwmclmdz_1->Draw();
 //   c9tra.cd(5);
 //   htwmclmdz_2->SetMinimum(0.);
 //   htwmclmdz_2->SetMaximum(1.2*tvsigmamax);
 //   htwmclmdz_2->Draw();
 //   c9tra.cd(6);
 //   htwmclmdz->Draw("colz");
 //   c9tra.Print(resname_pdf_o);
 //   c9tra.Write();
 //   c9tra.Close();
 //  // END [compare MC and MCLMD]


 //  TCanvas c18zzz("Errors_SD");
 //  c18zzz.Divide(2,4);
 //  c18zzz.cd(1);
 //  hdvrec->Draw("colz");
 //  hdvrec->FitSlicesY();
 //  c18zzz.cd(2);
 //  hdvrec_1->SetTitle("#sigma v_{REC}, mean");
 //  hdvrec_1->SetMinimum(0);
 //  hdvrec_1->SetMaximum(vsigmamax);
 //  hdvrec_1->Draw();
 //  c18zzz.cd(3);
 //  hdtvrec->Draw("colz");
 //  c18zzz.cd(4);
 //  hdtvrec->FitSlicesY();
 //  hdtvrec_1->SetTitle("#sigma v'_{REC}, mean");
 //  hdtvrec_1->SetMinimum(0.1*tvsigmamax);
 //  hdtvrec_1->SetMaximum(0.5*tvsigmamax);
 //  hdtvrec_1->Draw();
 //  c18zzz.cd(5);
 // hdwrec->Draw("colz");
 // c18zzz.cd(6);
 // hdwrec->FitSlicesY();
 // hdwrec_1->SetTitle("#sigma w_{REC}, mean");
 // hdwrec_1->SetMinimum(0);
 // hdwrec_1->SetMaximum(vsigmamax);
 // hdwrec_1->Draw();
  
 //  c18zzz.cd(7);
 //  hdtwrec->Draw("colz");
 //  c18zzz.cd(8);
 //   hdtwrec->FitSlicesY();
 //  hdtwrec_1->SetTitle("#sigma w'_{REC}, mean");
 //  hdtwrec_1->SetMinimum(0.1*tvsigmamax);
 //  hdtwrec_1->SetMaximum(0.5*tvsigmamax); 
 //  hdtwrec_1->Draw();
  
 //  c18zzz.Print(resname_pdf_o);
 //  c18zzz.Write();
 //  c18zzz.Close();


 // //  TCanvas c18bbb("ErrorsRatio_SD");
 // //  c18bbb.Divide(2,4);
 // //  c18bbb.cd(1);
 // //  TH1  *hvrec_vmc = hvrecz_2;
 // //  hvrec_vmc->Divide(hdvrec_1);
 // //  hvrec_vmc->SetTitle("#sigma(v_{MC}-v_{REC})/#sigma(v_{REC})");
 // //  hvrec_vmc->GetYaxis()->SetTitle("expect/estim");
 // //  hvrec_vmc->GetXaxis()->SetTitle("z, cm");
 // //  hvrec_vmc->SetMinimum(0);
 // //  hvrec_vmc->SetMaximum(1.5);
 // //  hvrec_vmc->Draw();
 // //  c18bbb.cd(2);
 // //  TH1  *hvrec_vmclmd = hvrecmclmdz_2;
 // //  hvrec_vmclmd->Divide(hdvrec_1);
 // //  hvrec_vmclmd->SetTitle("#sigma(v_{MCLMD}-v_{REC})/#sigma(v_{REC})");
 // //  hvrec_vmclmd->GetYaxis()->SetTitle("expect/estim");
 // //  hvrec_vmclmd->GetXaxis()->SetTitle("z, cm");
 // //  hvrec_vmclmd->SetMinimum(0);
 // //  hvrec_vmclmd->SetMaximum(1.5);
 // //  hvrec_vmclmd->Draw();
 // //  c18bbb.cd(3);
 // //  TH1 *htvrec_tvmc =  htvrecz_2;
 // //  htvrec_tvmc->Divide(hdtvrec_1);
 // //  htvrec_tvmc->SetTitle("#sigma(v'_{MC}-v'_{REC})/#sigma(v'_{REC})");
 // //  htvrec_tvmc->GetYaxis()->SetTitle("expect/estim");
 // //  htvrec_tvmc->GetXaxis()->SetTitle("z, cm");
 // //  htvrec_tvmc->SetMinimum(0);
 // //  htvrec_tvmc->SetMaximum(1.5);
 // //  htvrec_tvmc->Draw();
 // //  c18bbb.cd(4);
 // //  TH1  *htvrec_tvmclmd = htvrecmclmdz_2;
 // //  htvrec_tvmclmd->Divide(hdtvrec_1);
 // //  htvrec_tvmclmd->SetTitle("#sigma(v'_{MCLMD}-v'_{REC})/#sigma(v'_{REC})");
 // //  htvrec_tvmclmd->GetYaxis()->SetTitle("expect/estim");
 // //  htvrec_tvmclmd->GetXaxis()->SetTitle("z, cm");
 // //  htvrec_tvmclmd->SetMinimum(0);
 // //  htvrec_tvmclmd->SetMaximum(1.5);
 // //  htvrec_tvmclmd->Draw();

 // //  c18bbb.cd(5);
 // //  TH1 *hwrec_wmc =  hwrecz_2;
 // //  hwrec_wmc->Divide(hdwrec_1);
 // //  hwrec_wmc->SetTitle("#sigma(w_{MC}-w_{REC})/#sigma(w_{REC})");
 // //  hwrec_wmc->GetYaxis()->SetTitle("expect/estim");
 // //  hwrec_wmc->GetXaxis()->SetTitle("z, cm");
 // //  hwrec_wmc->SetMinimum(0);
 // //  hwrec_wmc->SetMaximum(1.5);
 // //  hwrec_wmc->Draw();
 // //  c18bbb.cd(6);
 // // TH1  *hwrec_wmclmd = hwrecmclmdz_2;
 // //  hwrec_wmclmd->Divide(hdwrec_1);
 // //  hwrec_wmclmd->SetTitle("#sigma(w_{MCLMD}-w_{REC})/#sigma(w_{REC})");
 // //  hwrec_wmclmd->GetYaxis()->SetTitle("expect/estim");
 // //  hwrec_wmclmd->GetXaxis()->SetTitle("z, cm");
 // //  hwrec_wmclmd->SetMinimum(0);
 // //  hwrec_wmclmd->SetMaximum(1.5);
 // //  hwrec_wmclmd->Draw();
 
 // //  c18bbb.cd(7);
 // //  TH1 *htwrec_twmc = htwrecz_2;
 // //  htwrec_twmc->Divide(hdtwrec_1);
 // //  htwrec_twmc->SetTitle("#sigma(w'_{MC}-w'_{REC})/#sigma(w'_{REC})");
 // //  htwrec_twmc->GetYaxis()->SetTitle("expect/estim");
 // //  htwrec_twmc->GetXaxis()->SetTitle("z, cm");
 // //  htwrec_twmc->SetMinimum(0);
 // //  htwrec_twmc->SetMaximum(1.5);
 // //  htwrec_twmc->Draw();

 // //  c18bbb.cd(8);
 // //  TH1  *htwrec_twmclmd = htwrecmclmdz_2;
 // //  htwrec_twmclmd->Divide(hdtwrec_1);
 // //  htwrec_twmclmd->SetTitle("#sigma(w'_{MCLMD}-w'_{REC})/#sigma(w'_{REC})");
 // //  htwrec_twmclmd->GetYaxis()->SetTitle("expect/estim");
 // //  htwrec_twmclmd->GetXaxis()->SetTitle("z, cm");
 // //  htwrec_twmclmd->SetMinimum(0);
 // //  htwrec_twmclmd->SetMaximum(1.5);
 // //  htwrec_twmclmd->Draw();

 // //  c18bbb.Print(resname_pdf_o);
 // //  c18bbb.Write();
 // //  c18bbb.Close();


 //  // //  double thlim = 0.01;//15GeV
 //  // double thlim = 0.04;//15GeV
 //  // double phlim = 0.02;//15GeV
 //  // double xlim = 0.02;//15GeV
 //  // double thresmax=0.2;//15GeV
 //  // double xresmax=0.1;//15GeV
 //  // double momlim=0.5;//15GeV
 //  // double momresmax=1.;//15GeV


  double thlim = 0.2;//1.5GeV
  double phiresmax = 0.2;//1.5GeV
  //  double xlim = 0.1;//1.5GeV
  //  double xlim = 0.05;//1.5GeV
  double xlim = 0.01;//1.5GeV
  double philim = 0.005;//1.5GeV
  double thresmax=0.9;//1.5GeV
  double xresmax=0.7;//1.5GeV
  double momlim=1.;//1.5GeV
  double momresmax=1.;//15GeV 
  //MC trk from IP vs. REC -----------------------------------------------------------------------------------------------------------------------
  TH2D *hthz = new TH2D("hthz",";z_{MC}, cm;#theta_{MC}-#theta_{REC}, mrad",45.,0,1200,1e3,-5.,5.);//1.5
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
  double plim = 300;
  double presmax = 10;
  TH2D *hpmcrecz = new TH2D("hpmcrecz",";z_{MC}, cm;P_{MC}-P_{REC}, mrad",45.,0,1200,1e3,-500,500);//1.5
  tbp->Project("hpmcrecz","1e6*(pmc-prec):zmc",condition);
  hpmcrecz->FitSlicesY();
  hpmcrecz_1->SetMinimum(-plim);
  hpmcrecz_1->SetMaximum(plim);
  hpmcrecz_1->GetYaxis()->SetTitle("P_{MC}-P_{REC}, mean [keV/c]");
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
  hpmcrecz_2->GetYaxis()->SetTitle("P_{MC}-P_{REC}, sigma [keV/c]");
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

  TH2D *hthmcfbz = new TH2D("hthmcfbz",";z_{MC}, cm;#theta_{MCf}-#theta_{MCb}, mrad",45.,0,1200,1e3,-0.5,0.5);//1.5
  tbp->Project("hthmcfbz","1e3*(thetamc-thetamcb):zmc",condition);
  hthmcfbz->FitSlicesY();
  hthmcfbz_1->SetMinimum(-thlim);
  hthmcfbz_1->SetMaximum(thlim);
  hthmcfbz_1->GetYaxis()->SetTitle("#theta_{MCf}-#theta_{MCb}, mean [mrad]");
  TCanvas c1fb;
  c1fb.Divide(2,2);
  c1fb.cd(1);
  hthmcfbz_1->SetTitle("MC forward vs. backward");
  hthmcfbz_2->SetTitle("");
  hthmcfbz_chi2->SetTitle("");
   hthmcfbz_1->Draw();
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

  c1fb.cd(2);
  hthmcfbz_2->SetMinimum(0);
  // hthmcfbz_2->SetMaximum(thresmax);//1.5
  hthmcfbz_2->SetMaximum(thresmax);//15
  hthmcfbz_2->GetYaxis()->SetTitle("#theta_{MCf}-#theta_{MCb}, sigma [mrad]");
  hthmcfbz_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hthmcfbz_2->Draw();
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
 
  c1fb.cd(3);
  hthmcfbz_chi2->Draw();
  c1fb.cd(4);
  hthmcfbz->Draw("colz");
  //  c1fb.Draw();
  hthmcfbz->Write();
  c1fb.Print(resname_pdf_o);
  c1fb.Write();
  c1fb.Close();

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
  TH2D *hyz = new TH2D("hyz",";z_{MC}, cm;y_{MC} - y_{REC}, cm",45.,0,1200,1e3,-5.,5.);//1.5
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

//   // TCanvas c19zzz("vparZZZ");
//   // c19zzz.Divide(2,2);
//   // c19zzz.cd(1);
//   // hvmclmdz->Draw("colz");
//   // c19zzz.cd(2);
//   // hwmclmdz->Draw("colz");
//   // c19zzz.cd(3);
//   // htvmclmdz->Draw("colz");
//   // c19zzz.cd(4);
//   // htwmclmdz->Draw("colz");
//   // TString mcdiffname = storePath+"/MCip_vs_MClmd.pdf";
//   // c19zzz.SaveAs(mcdiffname);
//   // c19zzz.Print(resname_pdf_c);
//   // c19zzz.Write();
//   // c19zzz.Close();

// TCanvas c20zzz("vparErrZZZ");
//  c20zzz.Divide(2,2);
//  c20zzz.cd(1);
//  hdvrec->Draw("colz");
//  c20zzz.cd(2);
//  hdwrec->Draw("colz");
//  c20zzz.cd(3);
//  hdtvrec->Draw("colz");
//  c20zzz.cd(4);
//  hdtwrec->Draw("colz");
//  TString mcdiffname2 = storePath+"/MCip_vs_MClmd_ERR.pdf";
//  c20zzz.SaveAs(mcdiffname2);
//  c20zzz.Write();
//  c20zzz.Close();
//  hwrecz_1->Write();
//  hwrecz_2->Write();
//  hwrecz->Write();
//  hwmclmdz_1->Write();
//  hwmclmdz_2->Write();
//  hwmclmdz->Write();
//  hwrecmclmdz_1->Write();
//  hwrecmclmdz_2->Write();
//  hwrecmclmdz->Write();
//  // hdwmc->Write();
//  // hdwmclmd->Write();
//  hdwrec->Write();
//  htvrecz->Write();
//  htvmclmdz->Write();
//  htvrecmclmdz->Write();
//  htvrecz_1->Write();
//  htvmclmdz_1->Write();
//  htvrecmclmdz_1->Write();
//  htvrecz_2->Write();
//  htvmclmdz_2->Write();
//  htvrecmclmdz_2->Write();
//  // hdtvmc->Write();
//  // hdtvmclmd->Write();
//  hdtvrec->Write();
//  htwrecz->Write();
//  htwmclmdz->Write();
//  htwrecmclmdz->Write();
//  htwrecz_1->Write();
//  htwmclmdz_1->Write();
//  htwrecmclmdz_1->Write();
//  htwrecz_2->Write();
//  htwmclmdz_2->Write();
//  htwrecmclmdz_2->Write();
//  // hdtwmc->Write();
//  // hdtwmclmd->Write();
//  hdtwrec->Write();
//  hvrecz->Write();
//  hvmclmdz->Write();
//  hvrecmclmdz->Write();
//  hvrecz_1->Write();
//  hvmclmdz_1->Write();
//  hvrecmclmdz_1->Write();
//  hvrecz_2->Write();
//  hvmclmdz_2->Write();
//  hvrecmclmdz_2->Write();
//  // hdvmc->Write();
//  // hdvmclmd->Write();
//  hdvrec->Write();

//  hvreczpull->Write();
//  hwreczpull->Write();
//  htvreczpull->Write();
//  htwreczpull->Write();
//  hvrecmclmdzpull->Write();
//  hwrecmclmdzpull->Write();
//  htvrecmclmdzpull->Write();
//  htwrecmclmdzpull->Write();
//  hvreczpull_2->Write();
//  hwreczpull_2->Write();
//  htvreczpull_2->Write();
//  htwreczpull_2->Write();
//  hvrecmclmdzpull_2->Write();
//  hwrecmclmdzpull_2->Write();
//  htvrecmclmdzpull_2->Write();
//  htwrecmclmdzpull_2->Write();
  hresthmcrec_phi_LMD->Write();
  hresthmcrec_phi_IP->Write();
  hresthmcrec_theta_LMD->Write();
  hresthmcrec_theta_IP->Write();
  hresphimcrec_phi_LMD->Write();
  hresphimcrec_phi_IP->Write();
  hresphimcrec_theta_LMD->Write();
  hresphimcrec_theta_IP->Write();
  hresthmcrec_phi_LMD_1->Write();
  hresthmcrec_phi_IP_1->Write();
  hresthmcrec_theta_LMD_1->Write();
  hresthmcrec_theta_IP_1->Write();
  hresphimcrec_phi_LMD_1->Write();
  hresphimcrec_phi_IP_1->Write();
  hresphimcrec_theta_LMD_1->Write();
  hresphimcrec_theta_IP_1->Write();
  hresthmcrec_phi_LMD_2->Write();
  hresthmcrec_phi_IP_2->Write();
  hresthmcrec_theta_LMD_2->Write();
  hresthmcrec_theta_IP_2->Write();
  hresphimcrec_phi_LMD_2->Write();
  hresphimcrec_phi_IP_2->Write();
  hresphimcrec_theta_LMD_2->Write();
  hresphimcrec_theta_IP_2->Write();

  hresthmclmdrec_phi_LMD->Write();
  hresthmclmdrec_phi_IP->Write();
  hresthmclmdrec_theta_LMD->Write();
  hresthmclmdrec_theta_IP->Write();
  hresphimclmdrec_phi_LMD->Write();
  hresphimclmdrec_phi_IP->Write();
  hresphimclmdrec_theta_LMD->Write();
  hresphimclmdrec_theta_IP->Write();
  hresthmclmdrec_phi_LMD_1->Write();
  hresthmclmdrec_phi_IP_1->Write();
  hresthmclmdrec_theta_LMD_1->Write();
  hresthmclmdrec_theta_IP_1->Write();
  hresphimclmdrec_phi_LMD_1->Write();
  hresphimclmdrec_phi_IP_1->Write();
  hresphimclmdrec_theta_LMD_1->Write();
  hresphimclmdrec_theta_IP_1->Write();
  hresthmclmdrec_phi_LMD_2->Write();
  hresthmclmdrec_phi_IP_2->Write();
  hresthmclmdrec_theta_LMD_2->Write();
  hresthmclmdrec_theta_IP_2->Write();
  hresphimclmdrec_phi_LMD_2->Write();
  hresphimclmdrec_phi_IP_2->Write();
  hresphimclmdrec_theta_LMD_2->Write();
  hresphimclmdrec_theta_IP_2->Write();

  hresthmcmclmd_phi_LMD->Write();
  hresthmcmclmd_phi_IP->Write();
  hresthmcmclmd_theta_LMD->Write();
  hresthmcmclmd_theta_IP->Write();
  hresphimcmclmd_phi_LMD->Write();
  hresphimcmclmd_phi_IP->Write();
  hresphimcmclmd_theta_LMD->Write();
  hresphimcmclmd_theta_IP->Write();
  hresthmcmclmd_phi_LMD_1->Write();
  hresthmcmclmd_phi_IP_1->Write();
  hresthmcmclmd_theta_LMD_1->Write();
  hresthmcmclmd_theta_IP_1->Write();
  hresphimcmclmd_phi_LMD_1->Write();
  hresphimcmclmd_phi_IP_1->Write();
  hresphimcmclmd_theta_LMD_1->Write();
  hresphimcmclmd_theta_IP_1->Write();
  hresthmcmclmd_phi_LMD_2->Write();
  hresthmcmclmd_phi_IP_2->Write();
  hresthmcmclmd_theta_LMD_2->Write();
  hresthmcmclmd_theta_IP_2->Write();
  hresphimcmclmd_phi_LMD_2->Write();
  hresphimcmclmd_phi_IP_2->Write();
  hresphimcmclmd_theta_LMD_2->Write();
  hresphimcmclmd_theta_IP_2->Write();
  fout->Close();


}
