void ShiftsFBP(TString storePath="/panda/pandaroot/macro/lmd/testPixel_newMinuit_BPgeane_FullLMD_160steps_NOpipe_large/mom_1_5/"){

  //style ----------------------------------
  gROOT->Macro("/home/karavdina/Disser/materials/Style_Thesis.C");
  gROOT->SetStyle("Thesis");
  //(end) style---------------------------

  // ---- Input file ----------------------------------------------------------------
  TString in=storePath+"/Lumi_Geane_0.root";
  TFile *fin = new TFile(in,"READ");
  // ---------------------------------------------------------------------------------

 // ---- Output file ----------------------------------------------------------------
  TString out=storePath+"/plotsShiftsFBP.root";
  TFile *fout = new TFile(out,"RECREATE");
  TString resname_pdf =  storePath+"/plotsShiftsFBP.pdf";
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



  double thlim = 1e3*0.002;//1.5GeV
  double phiresmax = 1e6*0.0002;//1.5GeV
  double xlim = 1e4*0.001;//1.5GeV
  double philim = 1e6*0.00007;//1.5GeV
  double thresmax=1e3*0.0015;//1.5GeV
  double xresmax=1e4*0.0002;//1.5GeV

 TH2D *hthmcfbz = new TH2D("hthmcfbz",";z_{MC}, cm;#theta_{MCf}-#theta_{MCb}, mrad",45.,0,1200,1e3,-5,5);//1.5
  tbp->Project("hthmcfbz","1e6*(thetamc-thetamcb):zmc",condition);
  hthmcfbz->FitSlicesY();
  hthmcfbz_1->SetMinimum(-thlim);
  hthmcfbz_1->SetMaximum(thlim);
  hthmcfbz_1->GetYaxis()->SetTitle("#theta_{MCf}-#theta_{MCb}, mean [#murad]");
  TCanvas c1fb;
  c1fb.Divide(2,2);
  c1fb.cd(1);
  hthmcfbz_1->SetTitle("MC forward - backward");
  hthmcfbz_2->SetTitle("");
  hthmcfbz_chi2->SetTitle("");
   hthmcfbz_1->Draw();
   dip1.SetY1(-thlim);
   dip1.SetY2(thlim);
  
  dip1.Draw();
  dip2.SetY1(-thlim);
  dip2.SetY2(thlim);
  dip2.Draw();
  trans.SetY1(-thlim);
  trans.SetY2(thlim);
  trans.Draw();

  c1fb.cd(2);
  hthmcfbz_2->SetMinimum(0);
  hthmcfbz_2->SetMaximum(thresmax);//15
  hthmcfbz_2->GetYaxis()->SetTitle("#theta_{MCf}-#theta_{MCb}, sigma [#murad]");
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
  hthmcfbz->Write();
  hthmcfbz_1->Write();
  hthmcfbz_2->Write();
  c1fb.Print(resname_pdf_o);
  c1fb.Write();
  c1fb.Close();

  double plim = 100;
  double presmax = 50;
  TH2D *hpmcfbz = new TH2D("hpmcfbz",";z_{MC}, cm;P_{MCf}-P_{MCb}, mrad",45.,0,1200,1e3,-200,200);//1.5
  tbp->Project("hpmcfbz","1e6*(pmc-pmcb):zmc",condition);
  hpmcfbz->FitSlicesY();
  hpmcfbz_1->SetMinimum(-plim);
  hpmcfbz_1->SetMaximum(plim);
  hpmcfbz_1->GetYaxis()->SetTitle("P_{MCf}-P_{MCb}, mean [keV/c]");
  TCanvas cmomfb;
  cmomfb.Divide(2,2);
  cmomfb.cd(1);
  hpmcfbz_1->SetTitle("MC forward - backward");
  hpmcfbz_2->SetTitle("");
  hpmcfbz_chi2->SetTitle("");
   hpmcfbz_1->Draw();
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
  hpmcfbz_2->SetMinimum(0);
  hpmcfbz_2->SetMaximum(presmax);//15
  hpmcfbz_2->GetYaxis()->SetTitle("P_{MCf}-P_{MCb}, sigma [keV/c]");
  hpmcfbz_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hpmcfbz_2->Draw();
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
  hpmcfbz_chi2->Draw();
  cmomfb.cd(4);
  hpmcfbz->Draw("colz");
  hpmcfbz->Write();
  hpmcfbz_1->Write();
  hpmcfbz_2->Write();
  cmomfb.Print(resname_pdf_o);
  cmomfb.Write();
  cmomfb.Close();

  TH2D *hxz = new TH2D("hxz",";z_{MC}, cm;x_{MCf} - x_{MCb}, #mum",45.,0,1200,1e3,-100,100);//15
  tbp->Project("hxz","1e4*(xmc-xmcb):zmc",condition);
  hxz->FitSlicesY();
  hxz_1->SetMinimum(-xlim);//15
  hxz_1->SetMaximum(xlim);
  hxz_1->GetYaxis()->SetTitle("x_{MCf}-x_{MCb}, mean [#mum]");
  TCanvas c2;
  c2.Divide(2,2);
  c2.cd(1);
  hxz_1->SetTitle("MC forward - backward");
  hxz_2->SetTitle("");
  hxz_chi2->SetTitle("");
  hxz_1->Draw();
  c2.cd(2);
  hxz_2->SetMinimum(0);
  hxz_2->SetMaximum(xresmax);//15
  hxz_2->GetYaxis()->SetTitle("x_{MC}-x_{REC}, sigma [#mum]");
  hxz_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hxz_2->Draw();
  c2.cd(3);
  hxz_chi2->Draw();
  c2.cd(4);
  hxz->Draw("colz");
  //  c2.Draw();
  hxz->Write();
  hxz_1->Write();
  hxz_2->Write();
  c2.Print(resname_pdf_o);
  c2.Write();
  c2.Close();

  TH2D *hyz = new TH2D("hyz",";z_{MC}, cm;y_{MCf} - y_{MCb}, #mum",45.,0,1200,1e3,-100,100);//1.5
  tbp->Project("hyz","1e4*(ymc-ymcb):zmc",condition);
  hyz->FitSlicesY();
  hyz_1->SetMinimum(-xlim);//15
  hyz_1->SetMaximum(xlim);
  hyz_1->GetYaxis()->SetTitle("y_{MCf}-y_{RECb}, mean [#mum]");
  TCanvas c3;
  c3.Divide(2,2);
  c3.cd(1);
  hyz_1->SetTitle("MC forward - backward");
  hyz_2->SetTitle("");
  hyz_chi2->SetTitle("");
  hyz_1->Draw();
  c3.cd(2);
  hyz_2->SetMinimum(0);
  //  hyz_2->SetMaximum(0.7);//1.5
  hyz_2->SetMaximum(xresmax);//15
  hyz_2->GetYaxis()->SetTitle("y_{MCf}-y_{MCb}, sigma [#mum]");
  hyz_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hyz_2->Draw();
  c3.cd(3);
  hyz_chi2->Draw();
  c3.cd(4);
  hyz->Draw("colz");
  //  c3.Draw();
  hyz->Write();
  hyz_1->Write();
  hyz_2->Write();
  c3.Print(resname_pdf_o);
  c3.Write();
  c3.Close();

  TH2D *hphz = new TH2D("hphz",";z_{MC}, cm;#phi_{MCf} - #phi_{MCb}, mrad",45.,0,1200,1e6,-2000,2000);
  tbp->Project("hphz","1e6*(phimc-phimcb):zmc",condition);
  hphz->FitSlicesY();
  hphz_1->SetMinimum(-philim);//15
  hphz_1->SetMaximum(philim);
  hphz_1->GetYaxis()->SetTitle("#phi_{MCf}-#phi_{MCb}, mean [#murad]");
  TCanvas c5;
  c5.Divide(2,2);
  c5.cd(1);
  hphz_1->SetTitle("MC forward - backward");
  hphz_2->SetTitle("");
  hphz_chi2->SetTitle("");
  hphz_1->Draw();
  c5.cd(2);
  hphz_2->SetMinimum(0);
  hphz_2->SetMaximum(phiresmax);//15
  hphz_2->GetYaxis()->SetTitle("#phi_{MCf}-#phi_{MCb}, sigma [#murad]");
  hphz_chi2->GetYaxis()->SetTitle("#chi^{2}");
  hphz_2->Draw();
  c5.cd(3);
  hphz_chi2->Draw();
  c5.cd(4);
  hphz->Draw("colz");
  hphz->Write();
  hphz_1->Write();
  hphz_2->Write();
  c5.Print(resname_pdf_c);
  c5.Write();
  c5.Close();
 
  fout->Close();


}
