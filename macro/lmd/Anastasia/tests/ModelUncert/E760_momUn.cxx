// macro for comparision DPM model and E760 model with their uncertainty in LMD range
#include <model/PndLmdDPMAngModel1D.h>
#include <model/PndLmdDPMMTModel1D.h>
#include <model/PndLmdDPMModelParametrization.h>
#include <model/PndLmdE760LikeModelParametrization.h>
#include <model/PndLmdE760ModelParametrization.h>
//#include <PndLmdLumiFitOptions.h>
#include <model/PndLmdModelFactory.h>
//#include <Model1D.h>
#include <PndLmdLumiHelper.h>
// macro to repoduce DPM fit
#include <TMath.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TGraphAsymmErrors.h>
#include <TAxis.h>
#include <iostream>
#include <TMath.h>
#include <utility>
#include <vector>
#include <TROOT.h>
#include <TLegend.h>
#include "TVirtualFitter.h"
using namespace std;
//par[3]={sigT,b,rho,dsig/dt}
double df_dSigT(double &t, double par[]){
  double sigT = par[0];
  double b = par[1];
  double rho = par[2];
  double F = par[3];
  double res = 2*F/sigT;
  //  cout<<"res_sigT "<<res<<endl;
  return res;
}
double df_db(double &t, double par[]){
  double sigT = par[0];
  double b = par[1];
  double rho = par[2];
  double F = par[3];
  double res = -t*F;
  //  cout<<"res_b "<<res<<endl;
  return res;
}
double df_drho(double &t, double par[]){
  double sigT = par[0];
  double b = par[1];
  double rho = par[2];
  double F = par[3];
  // cout<<"F = "<<F<<endl;
  double res = F*2*rho/(1+rho*rho);
  //  cout<<"res_rho "<<res<<endl;
  return res;
}

double Df(double &t, double par[], double errpar[]){
  double errSigT = errpar[0];
  double errb = errpar[1];
  double errrho = errpar[2];
  double int_res = TMath::Power(df_dSigT(t,par)*errSigT,2)+TMath::Power(df_db(t,par)*errb,2)
    +TMath::Power(df_drho(t,par)*errrho,2);
  double res = TMath::Sqrt(int_res);
   // cout<<"res_Df "<<res<<" = "<<TMath::Power(df_dSigT(t,par)*errSigT,2)<<" + "<<TMath::Power(df_db(t,par)*errb,2)
   //     <<" + "<<TMath::Power(df_drho(t,par)*errrho,2)<<endl;
  return res;
}
double DfsigT(double &t, double par[], double errpar[]){
  double errSigT = errpar[0];
  double errb = errpar[1];
  double errrho = errpar[2];
  double int_res = TMath::Power(df_dSigT(t,par)*errSigT,2);
  double res = TMath::Sqrt(int_res);
   // cout<<"res_Df "<<res<<" = "<<TMath::Power(df_dSigT(t,par)*errSigT,2)<<" + "<<TMath::Power(df_db(t,par)*errb,2)
   //     <<" + "<<TMath::Power(df_drho(t,par)*errrho,2)<<endl;
  return res;
}
double Dfb(double &t, double par[], double errpar[]){
  double errSigT = errpar[0];
  double errb = errpar[1];
  double errrho = errpar[2];
  double int_res = TMath::Power(df_db(t,par)*errb,2);
  double res = TMath::Sqrt(int_res);
   // cout<<"res_Df "<<res<<" = "<<TMath::Power(df_dSigT(t,par)*errSigT,2)<<" + "<<TMath::Power(df_db(t,par)*errb,2)
   //     <<" + "<<TMath::Power(df_drho(t,par)*errrho,2)<<endl;
  return res;
}
double Dfrho(double &t, double par[], double errpar[]){
  double errSigT = errpar[0];
  double errb = errpar[1];
  double errrho = errpar[2];
  double int_res = TMath::Power(df_drho(t,par)*errrho,2);
  double res = TMath::Sqrt(int_res);
   // cout<<"res_Df "<<res<<" = "<<TMath::Power(df_dSigT(t,par)*errSigT,2)<<" + "<<TMath::Power(df_db(t,par)*errb,2)
   //     <<" + "<<TMath::Power(df_drho(t,par)*errrho,2)<<endl;
  return res;
}
int main(){
  gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/test_macros/rootlogon.C");

  // part1: fit energy dependence of DPM parameters ---------------
  double mom[6]={3.70,4.07,5.60,5.72,5.94,6.23};
  // //values for fixed sigma_total!
  double sigT[6]={70.3,68.0,61.3,60.9,60.2,59.4};
  double errsigT[6]={1e-2,1e-2,1e-2,1e-2,1e-2,1e-2};
  double b[6]={12.9,12.8,12.5,12.2,12.6,12.2};
  double errb[6]={0.4,0.7,0.3,0.4,0.3,0.6};
  double rho[6]={0.006,-0.007,-0.030,-0.018,-0.035,-0.029};
  double errrho[6]={0.008,0.012,0.007,0.008,0.008,0.010};
  // //values for free sigma_total!
  // double rho[6]={0.018,-0.015,-0.047,-0.051,-0.063,-0.006};
  // double errrho[6]={0.014,0.024,0.007,0.011,0.008,0.020};
  // double b[6] = { 12.6, 12.9, 12.6, 12.7, 13.0, 11.7 };
  // double errb[6] = { 0.4, 0.7, 0.2, 0.3, 0.2, 0.5 };
  // double sigT[6] = { 71.9, 67.8, 60.9, 59.5, 59.1, 61.5 };
  // double errsigT[6] = { 0.9, 1.5, 0.4, 0.6, 0.5, 1.2 };
  TCanvas c1("c1","canvas",1200,800);
  c1.Divide(3,2);
  // TCanvas c1("c1","canvas",600,800);
  // c1.Divide(1,2);
  TGraphErrors *grSIGT = new TGraphErrors(6,mom,sigT,0,errsigT); 
  grSIGT->SetMarkerStyle(21);
  grSIGT->SetMarkerSize(0.9); 
  TF1 *fsigt = new TF1("fsigt","[0]+[1]*TMath::Power(x,[2])",mom[0]-0.5,mom[5]+0.5);
  fsigt->SetParNames("par0","par1","par2");
  fsigt->SetParameter(0,34);
  fsigt->SetParameter(1,89.7);
  fsigt->SetParameter(2,-0.7);
  grSIGT->Fit(fsigt,"R");

  TF1 *fsigt_dpm = new TF1("fsigt_dpm","[0]+[1]*TMath::Power(x,[2])",mom[0]-0.5,mom[5]+0.5);
  fsigt_dpm->SetParNames("par0","par1","par2");
  fsigt_dpm->SetParameter(0,34.48);
  fsigt_dpm->SetParameter(1,89.7);
  fsigt_dpm->SetParameter(2,-0.7);
  fsigt_dpm->SetLineColor(4);
  //Create a TGraphErrors to hold the confidence intervals
  int np=1e4;
  // int np=1e3;
  TGraphErrors *grint_sigt = new TGraphErrors(np);
  TGraphErrors *grdiff_sigt = new TGraphErrors(np);
  // grint->SetTitle("Fitted line with .95 conf. band");
  grint_sigt->SetTitle("");
  grdiff_sigt->SetTitle("");
  for (int i=0; i<np; i++){
    //   double pcur= mom[0]-1.0+((mom[5]+1.0)-(mom[0]-1.0))*i/np;
    double pcur= mom[0]-2.3+((mom[5]+8.9)-(mom[0]-2.3))*i/np;
    grint_sigt->SetPoint(i, pcur, 0);
    double myfit = fsigt->Eval(pcur);
    double dpmfit = fsigt_dpm->Eval(pcur);
    double difffit = 100.*(dpmfit-myfit)/dpmfit;
    grdiff_sigt->SetPoint(i, pcur, difffit);
  }
  //Compute the confidence intervals at the x points of the created graph
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(grint_sigt,0.68);
  grint_sigt->SetLineColor(2);
  grint_sigt->SetLineWidth(3);
  grint_sigt->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grint_sigt->GetYaxis()->SetTitle("#sigma_{T}");
  grint_sigt->SetFillColor(2);
  grint_sigt->SetFillStyle(3005);

  //TLegend *leg_sigt = new TLegend(0.62,0.35,0.85,0.55);
  TLegend *leg_sigt = new TLegend(0.72,0.5,0.68,0.7);
  //  TLegend *leg = new TLegend(0.22,0.15,0.37,0.35);
  leg_sigt->SetFillColor(0);
  leg_sigt->SetTextFont(42);
  leg_sigt->SetTextSize(0.05);
  leg_sigt->AddEntry(grSIGT,"data","ep");
  leg_sigt->AddEntry(fsigt_dpm,"E760 fit","l");
  leg_sigt->AddEntry(fsigt,"our fit","l");

  c1.cd(1);
  grint_sigt->Draw("AP4");
  grSIGT->Draw("psame");
  fsigt_dpm->Draw("same");
  leg_sigt->Draw();
  c1.cd(4);
  //c1.cd(2);
  grdiff_sigt->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grdiff_sigt->GetYaxis()->SetTitle("(E760-fit)/E760, %");
  grdiff_sigt->SetMarkerStyle(20);
  grdiff_sigt->SetMarkerSize(0.5);
  grdiff_sigt->Draw("AP");

  //B
 TGraphErrors *grB = new TGraphErrors(6,mom,b,0,errb); 
  grB->SetMarkerStyle(21);
  grB->SetMarkerSize(0.9); 
  TF1 *fb = new TF1("fb","[0]+[1]*x",mom[0]-0.5,mom[5]+0.5);
  fb->SetParNames("par0","par1");
  fb->SetParameter(0,13.64);
  fb->SetParameter(1,-0.2);
  grB->Fit(fb,"R");

  TF1 *fb_dpm = new TF1("fb_dpm","[0]+[1]*x",mom[0]-0.5,mom[5]+0.5);
  fb_dpm->SetParNames("par0","par1");
  fb_dpm->SetParameter(0,13.64);
  fb_dpm->SetParameter(1,-0.2);
  fb_dpm->SetLineColor(4);
  //Create a TGraphErrors to hold the confidence intervals
  TGraphErrors *grint_b = new TGraphErrors(np);
  TGraphErrors *grdiff_b = new TGraphErrors(np);
  // grint->SetTitle("Fitted line with .95 conf. band");
  grint_b->SetTitle("");
  grdiff_b->SetTitle("");
  for (int i=0; i<np; i++){
    //    double pcur= mom[0]-1.0+((mom[5]+1.0)-(mom[0]-1.0))*i/np;
    double pcur= mom[0]-2.3+((mom[5]+8.9)-(mom[0]-2.3))*i/np;
    grint_b->SetPoint(i, pcur, 0);
    double myfit = fb->Eval(pcur);
    double dpmfit = fb_dpm->Eval(pcur);
    double difffit = 100.*(dpmfit-myfit)/dpmfit;
    grdiff_b->SetPoint(i, pcur, difffit);
  }
  //Compute the confidence intervals at the x points of the created graph
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(grint_b,0.68);
  grint_b->SetLineColor(kRed);
  grint_b->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grint_b->GetYaxis()->SetTitle("b");
  grint_b->SetFillColor(2);
  grint_b->SetFillStyle(3005);


  c1.cd(2);
  // c1.cd(1);
  grint_b->Draw("AP4");
  grB->Draw("psame");
  fb_dpm->Draw("same");
  leg_sigt->Draw();
  c1.cd(5);
  //c1.cd(2);
  grdiff_b->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grdiff_b->GetYaxis()->SetTitle("(E760-fit)/E760, %");
  grdiff_b->SetMarkerStyle(20);
  grdiff_b->SetMarkerSize(0.5);
  grdiff_b->Draw("AP");

 //rho
 TGraphErrors *grRHO = new TGraphErrors(6,mom,rho,0,errrho); 
  grRHO->SetMarkerStyle(21);
  grRHO->SetMarkerSize(0.9); 
  TF1 *frho = new TF1("frho","[0]+[1]*x",mom[0]-0.5,mom[5]+0.5);
  frho->SetParNames("par0","par1");
  frho->SetParameter(0,-0.12);
  frho->SetParameter(1,0.03);
  grRHO->Fit(frho,"R");

  TF1 *frho_dpm = new TF1("frho_dpm","[0]+[1]*x",mom[0]-0.5,mom[5]+0.5);
  frho_dpm->SetParNames("par0","par1");
  frho_dpm->SetParameter(0,-0.12);
  frho_dpm->SetParameter(1,0.03);
  frho_dpm->SetLineColor(4);
  //Create a TGraphErrors to hold the confidence intervals
  TGraphErrors *grint_rho = new TGraphErrors(np);
  TGraphErrors *grdiff_rho = new TGraphErrors(np);
  // grint->SetTitle("Fitted line with .95 conf. band");
  grint_rho->SetTitle("");
  grdiff_rho->SetTitle("");
 
  
 for (int i=0; i<np; i++){
   //   double pcur= mom[0]-1.0+((mom[5]+1.0)-(mom[0]-1.0))*i/np;
   double pcur= mom[0]-2.3+((mom[5]+8.9)-(mom[0]-2.3))*i/np;
    grint_rho->SetPoint(i, pcur, 0);
    double myfit = frho->Eval(pcur);
    double dpmfit = frho_dpm->Eval(pcur);
    double difffit = 100.*(dpmfit-myfit)/dpmfit;
    grdiff_rho->SetPoint(i, pcur, difffit);
    // double y = grint_rho->GetErrorY(i);
    // double un = TMath::Sqrt(TMath::Power(dpmfit-myfit,2)+TMath::Power(y,2));
    // cout<<"OLD: "<<y<<" NEW: "<<un<<endl;
    // grint_rho->SetPointError(i, 0, un);//!TEST
  }
//Compute the confidence intervals at the x points of the created graph
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(grint_rho,0.68);
  grint_rho->SetLineColor(kRed);
  grint_rho->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grint_rho->GetYaxis()->SetTitle("#rho");
  grint_rho->SetFillColor(2);
  grint_rho->SetFillStyle(3005);

  c1.cd(3);
  // c1.cd(1);
  grint_rho->Draw("AP4");
  grRHO->Draw("psame");
  frho_dpm->Draw("same");
  leg_sigt->Draw();
  c1.cd(6);
  // c1.cd(2);
  grdiff_rho->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grdiff_rho->GetYaxis()->SetTitle("(E760-fit)/E760, %");
  grdiff_rho->SetMarkerStyle(20);
  grdiff_rho->SetMarkerSize(0.5);
  grdiff_rho->SetMinimum(-9e2);
  grdiff_rho->SetMaximum(9e2);
  grdiff_rho->Draw("AP");


  c1.SaveAs("E760_fit.pdf");
  c1.SaveAs("E760_fit.root");

  //[end] part1: fit energy dependence of E760 parameters ---------------------

  // parrho: calculate differencial cross-section and its uncertanty ----------
  double Plab_min=1.5;
  double Plab_max=15;
  int Plabstep= 30;
  // double Plab_min=1.5;
  // double Plab_max=15;
  //  int Plabstep= 2;
  TGraphErrors *resUn2mrad = new TGraphErrors(Plabstep);
  resUn2mrad->SetMarkerStyle(20);
  resUn2mrad->SetMarkerColor(32);
  resUn2mrad->SetMarkerSize(2.0);
  TGraphErrors *resUn2mradmn = new TGraphErrors(Plabstep);
  //  resUn2mradmn->SetMarkerStyle(25);
  resUn2mradmn->SetMarkerStyle(21);
  resUn2mradmn->SetMarkerColor(32);
  resUn2mradmn->SetMarkerSize(2.0);
  TGraphErrors *resUn2mradpl = new TGraphErrors(Plabstep);
  //  resUn2mradpl->SetMarkerStyle(26);
  resUn2mradpl->SetMarkerStyle(22);
  resUn2mradpl->SetMarkerColor(32);
  resUn2mradpl->SetMarkerSize(2.0);

  for(int pstep=0;pstep<=Plabstep;pstep++){
    double plab = Plab_min+pstep*(Plab_max-Plab_min)/Plabstep;
    double dP = 1e-5*plab;
    double plab2 = plab-dP;
    double plab3 = plab+dP;
    cout<<"plab2 = "<<plab2<<" plab3 = "<<plab3<<endl;
    //for main mom value ----------------------------------
  //get patameters for E760 model from the fit
  //par[3]={sigT,b,rho,dsig/dt}
  double par[4],errpar[4];
  par[0] = fsigt->Eval(plab);
  par[1] = fb->Eval(plab);
  par[2] = frho->Eval(plab);
  for(int i=0;i<grint_sigt->GetN();i++){
    double xval,yval;
    grint_sigt->GetPoint(i,xval,yval);
    double errpar_0 = grint_sigt->GetErrorY(i);
    if(fabs(xval-plab)<1e-2){
      errpar[0] = grint_sigt->GetErrorY(i);
      errpar[1] = grint_b->GetErrorY(i);
      errpar[2] = grint_rho->GetErrorY(i);
    }
  }
  cout<<"E760-like Params for Plab="<<plab<<endl;
  for(int jdpm=0;jdpm<3;jdpm++){
    cout<<par[jdpm]<<" +- "<<errpar[jdpm]<<endl;
  }

  LumiFit::PndLmdFitModelOptions model_options;
  model_options.dpm_elastic_parts = LumiFit::ALL_RHO_B_SIGTOT;
  PndLmdDPMAngModel1D modelE760("e760_angular_1d", model_options.dpm_elastic_parts);
  shared_ptr<Parametrization> para(new PndLmdE760LikeModelParametrization(modelE760.getModelParameterSet()));
  modelE760.getModelParameterHandler().registerParametrizations(
  							    modelE760.getModelParameterSet(), para);
  modelE760.getModelParameterSet().setModelParameterValue("p_lab", plab);
  modelE760.getModelParameterSet().setModelParameterValue("luminosity", 1);
  ((Model1D*) &modelE760)->init();

 PndLmdDPMAngModel1D modelE760mn("e760_angular_1d_mn", model_options.dpm_elastic_parts);
  shared_ptr<Parametrization> para2(new PndLmdE760LikeModelParametrization(modelE760mn.getModelParameterSet()));
  modelE760mn.getModelParameterHandler().registerParametrizations(
  							    modelE760mn.getModelParameterSet(), para2);
  modelE760mn.getModelParameterSet().setModelParameterValue("p_lab", plab2);
  modelE760mn.getModelParameterSet().setModelParameterValue("luminosity", 1);
  ((Model1D*) &modelE760mn)->init();

  PndLmdDPMAngModel1D modelE760pl("e760_angular_1d_pl", model_options.dpm_elastic_parts);
  shared_ptr<Parametrization> para3(new PndLmdE760LikeModelParametrization(modelE760pl.getModelParameterSet()));
  modelE760pl.getModelParameterHandler().registerParametrizations(
  							    modelE760mn.getModelParameterSet(), para3);
  modelE760pl.getModelParameterSet().setModelParameterValue("p_lab", plab3);
  modelE760pl.getModelParameterSet().setModelParameterValue("luminosity", 1);
  ((Model1D*) &modelE760pl)->init();


  double lower_bound = 2.0;
  double upper_bound = 10.0 ;
  std::vector<std::pair<double, double> > integral_ranges;
  integral_ranges.push_back(std::make_pair(lower_bound / 1000.0, upper_bound / 1000.0));
  double integral_2_10 = modelE760.Integral(integral_ranges, 0.0001);
  cout<<" integral (2-10) = "<<integral_2_10<<endl;
  double th_dw = 0.0015;//
  double th_up = 0.015;
  const int nst = 1e4;
  double cs_loc = 0;
  double cs_val[nst], cs_uncert[nst];//e760
  double cs_valmn[nst], rel_cs_mn[nst], diff_cs_mn[nst];//e760
  double cs_valpl[nst], rel_cs_pl[nst], diff_cs_pl[nst];//e760

  double rel_err[nst];
  double th_val[nst],  t_val[nst];

  for(int i=0;i<nst;i++){
    double dth = (th_up-th_dw)/nst;
    double theta =  th_dw +i*dth;
    double th1 = theta;
    double th2 = theta + dth;
    PndLmdLumiHelper *lmd_help  = new PndLmdLumiHelper();
    double t_cur = (-1)*(lmd_help->getMomentumTransferFromTheta(plab, th1));
    t_val[i] = (-1)*t_cur;
    th_val[i] = 1e3*th1;
    cs_val[i] = modelE760.getRawRhoBSigtotFullElastic(&t_cur);
    cs_valmn[i] = modelE760mn.getRawRhoBSigtotFullElastic(&t_cur);
    cs_valpl[i] = modelE760pl.getRawRhoBSigtotFullElastic(&t_cur);
    rel_cs_mn[i] = 100.*(cs_val[i]-cs_valmn[i])/cs_val[i];
    rel_cs_pl[i] = 100.*(cs_val[i]-cs_valpl[i])/cs_val[i];
    diff_cs_mn[i] = (cs_val[i]-cs_valmn[i]);
    diff_cs_pl[i] = (cs_val[i]-cs_valpl[i]);
    par[3] = cs_val[i];
    cs_uncert[i] = Df(t_cur,par,errpar);
    rel_err[i] = 100*(cs_uncert[i]/cs_val[i]);
  }


  //TGraph *gr_cs_th_plain = new TGraph(nst,th_val,cs_val);
  TGraphErrors *gr_cs_th = new TGraphErrors(nst,th_val,cs_val,0,cs_uncert);
  gr_cs_th->GetXaxis()->SetTitle("#theta, mrad");
  gr_cs_th->GetYaxis()->SetTitle("d#sigma/dt, mb/(GeV/c)^{2}");
  gr_cs_th->SetFillColor(1);
  gr_cs_th->SetLineColor(1);
  gr_cs_th->SetFillStyle(3001);

 TGraphErrors *gr_cs_thpl = new TGraphErrors(nst,th_val,cs_valpl,0,cs_uncert);
  gr_cs_thpl->GetXaxis()->SetTitle("#theta, mrad");
  gr_cs_thpl->GetYaxis()->SetTitle("d#sigma/dt, mb/(GeV/c)^{2}");
  gr_cs_thpl->SetFillColor(4);
  gr_cs_thpl->SetLineColor(4);
  gr_cs_thpl->SetFillStyle(3001);

 TGraphErrors *gr_cs_thmn = new TGraphErrors(nst,th_val,cs_valmn,0,cs_uncert);
  gr_cs_thmn->GetXaxis()->SetTitle("#theta, mrad");
  gr_cs_thmn->GetYaxis()->SetTitle("d#sigma/dt, mb/(GeV/c)^{2}");
  gr_cs_thmn->SetFillColor(2);
  gr_cs_thmn->SetLineColor(2);
  gr_cs_thmn->SetFillStyle(3001);

  TMultiGraph *mgr_cs_th = new TMultiGraph();
  mgr_cs_th->Add(gr_cs_th);
  mgr_cs_th->Add(gr_cs_thpl);
  mgr_cs_th->Add(gr_cs_thmn);


  TGraphErrors *gr_cs_t = new TGraphErrors(nst,t_val,cs_val,0,cs_uncert);
  gr_cs_t->SetTitle("");
  gr_cs_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  gr_cs_t->GetYaxis()->SetTitle("d#sigma/dt, mb/(GeV/c)^{2}");
  gr_cs_t->SetFillColor(4);
  gr_cs_t->SetFillStyle(3001);



  int x_2mrad=0;   int x_10mrad=0;
  PndLmdLumiHelper *lmd_help  = new PndLmdLumiHelper();
  double t_2mrad = (lmd_help->getMomentumTransferFromTheta(plab, 2e-3));
  double t_10mrad = (lmd_help->getMomentumTransferFromTheta(plab, 1e-2));
  for(int igrp = 0; igrp<gr_cs_t->GetN();igrp++){
    double x,y;
    gr_cs_t->GetPoint(igrp,x,y);
    if(fabs(x-t_2mrad)<0.01*t_2mrad) x_2mrad=igrp;
    if(fabs(x-t_10mrad)<0.01*t_10mrad) x_10mrad=igrp;
  }
  double CSint_2_10 = 0;
  for(int ik=x_2mrad;ik<x_10mrad;ik++){
    double x1,y1;
    gr_cs_t->GetPoint(ik,x1,y1);
    double x2,y2;
    gr_cs_t->GetPoint(ik+1,x2,y2);
    CSint_2_10 += y1*(x2-x1);
  }
  

 cout<<" CSint_2_10 = "<<CSint_2_10<<endl;

  TGraph *gr_cs_un_th = new TGraph(nst,th_val,cs_uncert);
  gr_cs_un_th->SetTitle("");
  gr_cs_un_th->GetXaxis()->SetTitle("#theta, mrad");
  gr_cs_un_th->GetYaxis()->SetTitle("#Delta(d#sigma/dt), mb/(GeV/c)^{2}");

  TGraph *gr_cs_un_t = new TGraph(nst,t_val,cs_uncert);
  gr_cs_un_t->SetTitle("");
  gr_cs_un_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  gr_cs_un_t->GetYaxis()->SetTitle("#Delta(d#sigma/dt), mb/(GeV/c)^{2}");

  TGraph *gr_cs_un_t_mn = new TGraph(nst,t_val,diff_cs_mn);
  TGraph *gr_cs_un_t_pl = new TGraph(nst,t_val,diff_cs_pl);

  TMultiGraph *mgr_cs_un_t = new TMultiGraph();
  mgr_cs_un_t->Add(gr_cs_un_t,"L");
  TLegend *legUn = new TLegend(0.77,0.64,0.99,0.99);
  legUn->SetFillColor(0);
  legUn->SetTextFont(42);
  legUn->SetTextSize(0.05);
  legUn->AddEntry(gr_cs_un_t,"total uncertainty","l");

  double errCSint_2_10 = 0;
  double errCSint_2_10_mn = 0;
  double errCSint_2_10_pl = 0;
  for(int ik=x_2mrad;ik<x_10mrad;ik++){
    double x1,y1;
    gr_cs_un_t->GetPoint(ik,x1,y1);
    double x2,y2;
    gr_cs_un_t->GetPoint(ik+1,x2,y2);
    errCSint_2_10 += y1*(x2-x1);

    gr_cs_un_t_mn->GetPoint(ik,x1,y1);
    gr_cs_un_t_mn->GetPoint(ik+1,x2,y2);
    errCSint_2_10_mn += y1*(x2-x1);
    gr_cs_un_t_pl->GetPoint(ik,x1,y1);
    gr_cs_un_t_pl->GetPoint(ik+1,x2,y2);
    errCSint_2_10_pl += y1*(x2-x1);
  }
 
  cout<<" errCSint_2_10 = "<<errCSint_2_10<<" errCSint_2_10_mn = "
      <<errCSint_2_10_mn<<" errCSint_2_10_pl = "<<errCSint_2_10_pl<<endl;
 double integ_un_2_10 = 100.*errCSint_2_10/CSint_2_10;
 resUn2mrad->SetPoint(pstep, plab,integ_un_2_10);
 double errIntegCS=0.5*fabs(CSint_2_10-integral_2_10)/integral_2_10;
 resUn2mrad->SetPointError(pstep,0,errIntegCS);

 double integ_un_2_10mn = 100.*errCSint_2_10_mn/CSint_2_10;
 resUn2mradmn->SetPoint(pstep, plab,integ_un_2_10mn);
 resUn2mradmn->SetPointError(pstep,0,errIntegCS);
 double integ_un_2_10pl = 100.*errCSint_2_10_pl/CSint_2_10;
 resUn2mradpl->SetPoint(pstep, plab,integ_un_2_10pl);
 resUn2mradpl->SetPointError(pstep,0,errIntegCS);

 TGraph *gr_cs_relun_th = new TGraph(nst,th_val,rel_err);
 gr_cs_relun_th->SetTitle("");
  gr_cs_relun_th->GetXaxis()->SetTitle("#theta, mrad");
  gr_cs_relun_th->GetYaxis()->SetTitle("#Delta(d#sigma/dt)/(d#sigma/dt), %");

  TGraph *gr_cs_relun_th_mn = new TGraph(nst,th_val,rel_cs_mn);
  gr_cs_relun_th_mn->SetLineColor(2);
  TGraph *gr_cs_relun_th_pl = new TGraph(nst,th_val,rel_cs_pl);
  gr_cs_relun_th_pl->SetLineColor(4);
  TMultiGraph *mg_cs_relun_th = new TMultiGraph();
  mg_cs_relun_th->Add(gr_cs_relun_th);
  mg_cs_relun_th->Add(gr_cs_relun_th_mn);
  mg_cs_relun_th->Add(gr_cs_relun_th_pl);

  TGraph *gr_cs_relun_t = new TGraph(nst,t_val,rel_err);
  gr_cs_relun_t->SetTitle("");
  gr_cs_relun_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  gr_cs_relun_t->GetYaxis()->SetTitle("#Delta(d#sigma/dt)/(d#sigma/dt), %");


  TCanvas c2;
  c2.Divide(2,3);
  c2.cd(1);
  //  gr_cs_th->Draw("AP4");
  mgr_cs_th->Draw("AP4");
  c2.cd(2);
  gr_cs_t->Draw("AP4");
  c2.cd(3);
  gr_cs_un_th->Draw("AL");
  c2.cd(4);
  mgr_cs_un_t->Draw("A");
  legUn->Draw();
  mgr_cs_un_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  mgr_cs_un_t->GetYaxis()->SetTitle("#Delta(d#sigma/dt), mb/(GeV/c)^{2}");
  c2.cd(5);
  //  gr_cs_relun_th->Draw("AL");
  mg_cs_relun_th->Draw("AL");
  mg_cs_relun_th->GetXaxis()->SetTitle("#theta, mrad");
  mg_cs_relun_th->GetYaxis()->SetTitle("#Delta(d#sigma/dt)/(d#sigma/dt), %");
  TLegend *legUn2 = new TLegend(0.87,0.64,0.99,0.99);
  legUn2->SetFillColor(0);
  legUn2->SetTextFont(42);
  legUn2->SetTextSize(0.05);
  legUn2->AddEntry(gr_cs_un_th,"model","l");
  legUn2->AddEntry(gr_cs_relun_th_mn,"P_{lab}-#DeltaP","l");
  legUn2->AddEntry(gr_cs_relun_th_pl,"P_{lab}+#DeltaP","l");
  legUn2->Draw();
  c2.cd(6);
  gr_cs_relun_t->Draw("AL");
  TString fname = "cs_and_uncert_Plab_";
  fname +=plab;
  TString fnamepdf = fname + ".pdf";
  TString fnameroot = fname + ".root";
  c2.SaveAs(fnamepdf);
  c2.SaveAs(fnameroot);
  // [end] parrho: calculate differencial cross-section and its uncertanty ----
  }
  TCanvas c3("c3","canvas",800,600);
  TMultiGraph *mg_res = new TMultiGraph();
  // mg_res->Add(resUn2mrad);
  mg_res->Add(resUn2mradmn);
  mg_res->Add(resUn2mradpl);
  //  TLegend *leg = new TLegend(0.27,0.65,0.65,0.87);
  TLegend *leg = new TLegend(0.8,0.77,0.99,0.99);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.05);
  //  leg->AddEntry(resUn2mrad,"model uncertainty","ep");
  leg->AddEntry(resUn2mradmn,"P_{lab}-#DeltaP","ep");
  leg->AddEntry(resUn2mradpl,"P_{lab}+#DeltaP","ep");
  mg_res->Draw("AP");
  //  mg_res->SetMinimum(-1);
  mg_res->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  mg_res->GetYaxis()->SetTitle("#Delta#sigma/#sigma, %");
  mg_res->SetMaximum(0.012);
  mg_res->SetMinimum(-0.012);
  leg->Draw();
  c3.SaveAs("uncertE760Integ_vs_Plab.pdf");
  c3.SaveAs("uncertE760Integ_vs_Plab.root");
  return 0;
}
