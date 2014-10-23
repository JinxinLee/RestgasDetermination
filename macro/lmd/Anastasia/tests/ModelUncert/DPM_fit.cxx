// macro for comparision DPM model and E760 model with their uncertainty in LMD range
#include <model/PndLmdDPMAngModel1D.h>
#include <model/PndLmdDPMMTModel1D.h>
#include <model/PndLmdDPMModelParametrization.h>
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
#include <TH1D.h>
#include <TLegend.h>
#include "TVirtualFitter.h"
using namespace std;
//par[5]={A1,A2,A3,t1,t2}
double df_dA1(double &x, double par[]){
  double t = -TMath::Abs(x);
  double A1 = par[0];
  double A2 = par[1];
  double A3 = par[2];
  double t1 = par[3];
  double t2 = par[4];
  double int_res = TMath::Exp(t/(2*t1))-A2*TMath::Exp(t/(2*t2));
  double res = TMath::Power(int_res,2);
  return res;
}
double df_dA2(double &x, double par[]){
  double t = -TMath::Abs(x);
  double A1 = par[0];
  double A2 = par[1];
  double A3 = par[2];
  double t1 = par[3];
  double t2 = par[4];
  double res = -2*A1*TMath::Exp(t/(2*t1)+t/(2*t2))+2*A1*A2*TMath::Exp(t/t2);
  return res;
}

double df_dA3(double &x, double par[]){
  double t = -TMath::Abs(x);
  double A1 = par[0];
  double A2 = par[1];
  double A3 = par[2];
  double t1 = par[3];
  double t2 = par[4];
  double res = TMath::Exp(t/t2);
  return res;
}

double df_dt1(double &x, double par[]){
  double t = -TMath::Abs(x);
  double A1 = par[0];
  double A2 = par[1];
  double A3 = par[2];
  double t1 = par[3];
  double t2 = par[4];
  double res = A1*(-TMath::Exp(t/t1)+A2*TMath::Exp(t/(2*t1)+t/(2*t2)))*(t/(t1*t1));
  return res;
}

double df_dt2(double &x, double par[]){
  double t = -TMath::Abs(x);
  double A1 = par[0];
  double A2 = par[1];
  double A3 = par[2];
  double t1 = par[3];
  double t2 = par[4];
  //double res = A1*(-A2*TMath::Exp(t/(2*t1)+t/(2*t2))+A2*A2*TMath::Exp(t/t2))*(-t/(t2*t2))+A3*TMath::Exp(t/t2)*(-t/t2);
  double res = A1*A2*(TMath::Exp(t/(2*t1)+t/(2*t2))-(A2+A3)*TMath::Exp(t/t2))*(-t/(t2*t2));
  return res;
}

double Df(double &t, double par[], double errpar[]){
  double errA1 = errpar[0];
  double errA2 = errpar[1];
  double errA3 = errpar[2];
  double errt1 = errpar[3];
  double errt2 = errpar[4];
  double int_res = TMath::Power(df_dA1(t,par)*errA1,2)+TMath::Power(df_dA2(t,par)*errA2,2)+TMath::Power(df_dA3(t,par)*errA3,2)+TMath::Power(df_dt1(t,par)*errt1,2)+TMath::Power(df_dt2(t,par)*errt2,2);
  double res = TMath::Sqrt(int_res);
  return res;
}

double DfA1(double &t, double par[], double errpar[]){
  double errA1 = errpar[0];
  double errA2 = errpar[1];
  double errA3 = errpar[2];
  double errt1 = errpar[3];
  double errt2 = errpar[4];
  double int_res = TMath::Power(df_dA1(t,par)*errA1,2);
  double res = TMath::Sqrt(int_res);
  return res;
}

double DfA2(double &t, double par[], double errpar[]){
  double errA1 = errpar[0];
  double errA2 = errpar[1];
  double errA3 = errpar[2];
  double errt1 = errpar[3];
  double errt2 = errpar[4];
  double int_res = TMath::Power(df_dA2(t,par)*errA2,2);
  double res = TMath::Sqrt(int_res);
  return res;
}
double DfA3(double &t, double par[], double errpar[]){
  double errA1 = errpar[0];
  double errA2 = errpar[1];
  double errA3 = errpar[2];
  double errt1 = errpar[3];
  double errt2 = errpar[4];
  double int_res = TMath::Power(df_dA3(t,par)*errA3,2);
  double res = TMath::Sqrt(int_res);
  return res;
}

double Dft1(double &t, double par[], double errpar[]){
  double errA1 = errpar[0];
  double errA2 = errpar[1];
  double errA3 = errpar[2];
  double errt1 = errpar[3];
  double errt2 = errpar[4];
  double int_res = TMath::Power(df_dt1(t,par)*errt1,2);
  double res = TMath::Sqrt(int_res);
  return res;
}

double Dft2(double &t, double par[], double errpar[]){
  double errA1 = errpar[0];
  double errA2 = errpar[1];
  double errA3 = errpar[2];
  double errt1 = errpar[3];
  double errt2 = errpar[4];
  double int_res = TMath::Power(df_dt2(t,par)*errt2,2);
  double res = TMath::Sqrt(int_res);
  return res;
}

// double cross_sec(double &t, double par[]){
// //par[5]={A1,A2,A3,t1,t2}
//   double A1 = par[0];
//   double A2 = par[1];
//   double A3 = par[2];
//   double t1 = par[3];
//   double t2 = par[4];
// //\frac{d\sigma_{had}}{dt} & = A_{1}\cdot[e^{t/2t_{1}}-A_{2}\cdot e^{t/2t_{2}}]^{2} + A_{3}\cdot e^{t/t_{2}}
//   double cs = A1*TMath::Power((exp(t/(2*t1))-A2*exp(t/(2*t2)));2)+A3*exp(t/t2);
//   return cs;
// }

int main(){
  gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/test_macros/rootlogon.C");

  // part1: fit energy dependence of DPM parameters ---------------
  double mom[6]={2.33,2.85,3.55,5.7,10.40,15.95};
  double A1[6]={582,426,382,232,171,113};
  double errA1[6]={17,8.5,50.2,7.4,1.9,4.2};
  double A2[6]={0.196,0.153,0.137,0.11,0.074,0.049};
  double errA2[6]={0.008,0.005,0.021,0.010,0.004,0.013};
  double T2[6]={0.322,0.394, 0.392, 0.351, 0.293,0.289};
  double errT2[6]={0.012, 0.012, 0.060, 0.028, 0.015, 0.049};
  double A3[6]={2.72,1.78,1.33,0.77,0.89,0.69};
  double errA3[6]={0.38,0.19,0.42,0.20,0.12,0.32};
  TCanvas c1("c1","canvas",1200,800);
  c1.Divide(4,2);

  TGraphErrors *grA1 = new TGraphErrors(6,mom,A1,0,errA1); 
  grA1->SetMarkerStyle(21);
  grA1->SetMarkerSize(0.9); 
  TF1 *fa1 = new TF1("fa1","[0]+[1]*exp(-x/[2])",mom[0]-0.5,mom[5]+0.5);
  fa1->SetParNames("par0","par1","par2");
  fa1->SetParameter(0,115);
  fa1->SetParameter(1,650);
  fa1->SetParameter(2,4.08);
  grA1->Fit(fa1,"R");

  TF1 *fa1_dpm = new TF1("fa1_dpm","[0]+[1]*exp(-x/[2])",mom[0]-0.5,mom[5]+0.5);
  fa1_dpm->SetParNames("par0","par1","par2");
  fa1_dpm->SetParameter(0,115);
  fa1_dpm->SetParameter(1,650);
  fa1_dpm->SetParameter(2,4.08);
  fa1_dpm->SetLineColor(4);
  //Create a TGraphErrors to hold the confidence intervals
  int np=1e4;
  TGraphErrors *grint_a1 = new TGraphErrors(np);
  TGraphErrors *grdiff_a1 = new TGraphErrors(np);
  // grint->SetTitle("Fitted line with .95 conf. band");
  grint_a1->SetTitle("");
  grdiff_a1->SetTitle("");
  for (int i=0; i<np; i++){
    double pcur= mom[0]-1.0+((mom[5]+1.0)-(mom[0]-1.0))*i/np;
    grint_a1->SetPoint(i, pcur, 0);
    double myfit = fa1->Eval(pcur);
    double dpmfit = fa1_dpm->Eval(pcur);
    double difffit = 100.*(dpmfit-myfit)/dpmfit;
    grdiff_a1->SetPoint(i, pcur, difffit);
  }
  //Compute the confidence intervals at the x points of the created graph
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(grint_a1,0.68);
  grint_a1->SetLineColor(kRed);
  grint_a1->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grint_a1->GetYaxis()->SetTitle("A_{1}");
  grint_a1->SetFillColor(2);
  grint_a1->SetFillStyle(3005);


  c1.cd(1);
  grint_a1->Draw("AP4");
  grA1->Draw("psame");
  fa1_dpm->Draw("same");
  TLegend *leg_a1 = new TLegend(0.42,0.35,0.65,0.55);
  //  TLegend *leg = new TLegend(0.22,0.15,0.37,0.35);
  leg_a1->SetFillColor(0);
  leg_a1->SetTextFont(42);
  leg_a1->SetTextSize(0.05);
  leg_a1->AddEntry(grA1,"data","ep");
  leg_a1->AddEntry(fa1_dpm,"DPM fit","l");
  leg_a1->AddEntry(fa1,"our fit","l");
  leg_a1->Draw();
  c1.cd(5);
  grdiff_a1->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grdiff_a1->GetYaxis()->SetTitle("(DPM-fit)/DPM, %");
  grdiff_a1->SetMarkerStyle(20);
  grdiff_a1->SetMarkerSize(0.5);
  grdiff_a1->Draw("AP");

  //A2
 TGraphErrors *grA2 = new TGraphErrors(6,mom,A2,0,errA2); 
  grA2->SetMarkerStyle(21);
  grA2->SetMarkerSize(0.9); 
  TF1 *fa2 = new TF1("fa2","[0]+[1]*exp(-x/[2])",mom[0]-0.5,mom[5]+0.5);
  fa2->SetParNames("par0","par1","par2");
  fa2->SetParameter(0,0.0687);
  fa2->SetParameter(1,0.307);
  fa2->SetParameter(2,2.367);
  grA2->Fit(fa2,"R");

  TF1 *fa2_dpm = new TF1("fa2_dpm","[0]+[1]*exp(-x/[2])",mom[0]-0.5,mom[5]+0.5);
  fa2_dpm->SetParNames("par0","par1","par2");
  fa2_dpm->SetParameter(0,0.0687);
  fa2_dpm->SetParameter(1,0.307);
  fa2_dpm->SetParameter(2,2.367);
  fa2_dpm->SetLineColor(4);
  //Create a TGraphErrors to hold the confidence intervals
  TGraphErrors *grint_a2 = new TGraphErrors(np);
  TGraphErrors *grdiff_a2 = new TGraphErrors(np);
  // grint->SetTitle("Fitted line with .95 conf. band");
  grint_a2->SetTitle("");
  grdiff_a2->SetTitle("");
  for (int i=0; i<np; i++){
    double pcur= mom[0]-1.0+((mom[5]+1.0)-(mom[0]-1.0))*i/np;
    grint_a2->SetPoint(i, pcur, 0);
    double myfit = fa2->Eval(pcur);
    double dpmfit = fa2_dpm->Eval(pcur);
    double difffit = 100.*(dpmfit-myfit)/dpmfit;
    grdiff_a2->SetPoint(i, pcur, difffit);
  }
  //Compute the confidence intervals at the x points of the created graph
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(grint_a2,0.68);
  grint_a2->SetLineColor(kRed);
  grint_a2->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grint_a2->GetYaxis()->SetTitle("A_{2}");
  grint_a2->SetFillColor(2);
  grint_a2->SetFillStyle(3005);


  c1.cd(2);
  grint_a2->Draw("AP4");
  grA2->Draw("psame");
  fa2_dpm->Draw("same");
  leg_a1->Draw();
  c1.cd(6);
  grdiff_a2->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grdiff_a2->GetYaxis()->SetTitle("(DPM-fit)/DPM, %");
  grdiff_a2->SetMarkerStyle(20);
  grdiff_a2->SetMarkerSize(0.5);
  grdiff_a2->Draw("AP");

 //t2
 TGraphErrors *grT2 = new TGraphErrors(6,mom,T2,0,errT2); 
  grT2->SetMarkerStyle(21);
  grT2->SetMarkerSize(0.9); 
  TF1 *ft2 = new TF1("ft2","[0]+[1]*exp(-x/[2])",mom[0]-0.5,mom[5]+0.5);
  ft2->SetParNames("par0","par1","par2");
  ft2->SetParameter(0,-2.979);
  ft2->SetParameter(1,3.353);
  ft2->SetParameter(2,483.4);
  grT2->Fit(ft2,"R");

  TF1 *ft2_dpm = new TF1("ft2_dpm","[0]+[1]*exp(-x/[2])",mom[0]-0.5,mom[5]+0.5);
  ft2_dpm->SetParNames("par0","par1","par2");
  ft2_dpm->SetParameter(0,-2.979);
  ft2_dpm->SetParameter(1,3.353);
  ft2_dpm->SetParameter(2,483.4);
  ft2_dpm->SetLineColor(4);
  //Create a TGraphErrors to hold the confidence intervals
  TGraphErrors *grint_t2 = new TGraphErrors(np);
  TGraphErrors *grdiff_t2 = new TGraphErrors(np);
  // grint->SetTitle("Fitted line with .95 conf. band");
  grint_t2->SetTitle("");
  grdiff_t2->SetTitle("");
  for (int i=0; i<np; i++){
    double pcur= mom[0]-1.0+((mom[5]+1.0)-(mom[0]-1.0))*i/np;
    grint_t2->SetPoint(i, pcur, 0);
    double myfit = ft2->Eval(pcur);
    double dpmfit = ft2_dpm->Eval(pcur);
    double difffit = 100.*(dpmfit-myfit)/dpmfit;
    grdiff_t2->SetPoint(i, pcur, difffit);
  }
  //Compute the confidence intervals at the x points of the created graph
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(grint_t2,0.68);
  grint_t2->SetLineColor(kRed);
  grint_t2->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grint_t2->GetYaxis()->SetTitle("t_{2}");
  grint_t2->SetFillColor(2);
  grint_t2->SetFillStyle(3005);


  c1.cd(3);
  grint_t2->Draw("AP4");
  grT2->Draw("psame");
  ft2_dpm->Draw("same");
  leg_a1->Draw();
  c1.cd(7);
  grdiff_t2->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grdiff_t2->GetYaxis()->SetTitle("(DPM-fit)/DPM, %");
  grdiff_t2->SetMarkerStyle(20);
  grdiff_t2->SetMarkerSize(0.5);
  grdiff_t2->Draw("AP");

 //A3
 TGraphErrors *grA3 = new TGraphErrors(6,mom,A3,0,errA3); 
  grA3->SetMarkerStyle(21);
  grA3->SetMarkerSize(0.9); 
  TF1 *fa3 = new TF1("fa3","[0]+[1]*exp(-x/[2])",mom[0]-0.5,mom[5]+0.5);
  fa3->SetParNames("par0","par1","par2");
  fa3->SetParameter(0,0.8372);
  fa3->SetParameter(1,39.53);
  fa3->SetParameter(2,0.765);
  grA3->Fit(fa3,"R");

  TF1 *fa3_dpm = new TF1("fa3_dpm","[0]+[1]*exp(-x/[2])",mom[0]-0.5,mom[5]+0.5);
  fa3_dpm->SetParNames("par0","par1","par2");
  fa3_dpm->SetParameter(0,0.8372);
  fa3_dpm->SetParameter(1,39.53);
  fa3_dpm->SetParameter(2,0.765);
  fa3_dpm->SetLineColor(4);
  //Create a TGraphErrors to hold the confidence intervals
  TGraphErrors *grint_a3 = new TGraphErrors(np);
  TGraphErrors *grdiff_a3 = new TGraphErrors(np);
  // grint->SetTitle("Fitted line with .95 conf. band");
  grint_a3->SetTitle("");
  grdiff_a3->SetTitle("");
  for (int i=0; i<np; i++){
    double pcur= mom[0]-1.0+((mom[5]+1.0)-(mom[0]-1.0))*i/np;
    grint_a3->SetPoint(i, pcur, 0);
    double myfit = fa3->Eval(pcur);
    double dpmfit = fa3_dpm->Eval(pcur);
    double difffit = 100.*(dpmfit-myfit)/dpmfit;
    grdiff_a3->SetPoint(i, pcur, difffit);
  }
  //Compute the confidence intervals at the x points of the created graph
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(grint_a3,0.68);
  grint_a3->SetLineColor(kRed);
  grint_a3->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grint_a3->GetYaxis()->SetTitle("A_{3}");
  grint_a3->SetFillColor(2);
  grint_a3->SetFillStyle(3005);


  c1.cd(4);
  grint_a3->Draw("AP4");
  grA3->Draw("psame");
  fa3_dpm->Draw("same");
  leg_a1->Draw();
  c1.cd(8);
  grdiff_a3->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grdiff_a3->GetYaxis()->SetTitle("(DPM-fit)/DPM, %");
  grdiff_a3->SetMarkerStyle(20);
  grdiff_a3->SetMarkerSize(0.5);
  grdiff_a3->Draw("AP");

  c1.SaveAs("DPM_fit.pdf");
  c1.SaveAs("DPM_fit.root");

  //[end] part1: fit energy dependence of DPM parameters ---------------------

  // part2: calculate differencial cross-section and its uncertanty ----------

  double Plab_min=1.5;
  double Plab_max=15;
  int Plabstep= 30;
  // int Plabstep= 100;
  //int Plabstep= 2;
 TGraphErrors *resUn2mrad = new TGraphErrors(Plabstep);
  resUn2mrad->SetMarkerStyle(20);
  resUn2mrad->SetMarkerColor(32);
  resUn2mrad->SetMarkerSize(2.0);
  TGraphErrors  *resUn3mrad = new TGraphErrors(Plabstep);
  resUn3mrad->SetMarkerStyle(21);
  resUn3mrad->SetMarkerColor(46); resUn3mrad->SetMarkerSize(1.7);
  TGraphErrors  *resUn4mrad = new TGraphErrors(Plabstep);
  resUn4mrad->SetMarkerStyle(22);
  resUn4mrad->SetMarkerColor(38);  resUn4mrad->SetMarkerSize(1.5);

  for(int pstep=0;pstep<=Plabstep;pstep++){
    double plab = Plab_min+pstep*(Plab_max-Plab_min)/Plabstep;
  //get patameters for DPM model from the fit
  // double plab = 10.40;
//par[5]={A1,A2,A3,t1,t2}
  double par[5],errpar[5];
  par[0] = fa1_dpm->Eval(plab);
  par[1] = fa2_dpm->Eval(plab);
  par[2] = fa3_dpm->Eval(plab);
  par[3] = 0.0899;
  errpar[3] = 0.010;
  par[4] = ft2_dpm->Eval(plab);
  // grint_a1->Print();
  for(int i=0;i<grint_a1->GetN();i++){
    double xval,yval;
    grint_a1->GetPoint(i,xval,yval);
    double errpar_0 = grint_a1->GetErrorY(i);
    //  if(fabs(xval-plab)<1e-1 && fabs(par[0]-yval)<1e-1){
    //    cout<<par[0]<<" "<<yval<<endl;
    //   if(fabs(xval-plab)<1e-2 && fabs(par[0]-yval)<0.1*errpar_0){
    //    if(fabs(xval-plab)<5e-4 && fabs(par[0]-yval)<errpar_0){
    if(fabs(xval-plab)<1e-3){
      // cout<<par[0]<<" "<<yval<<endl;
      //      grint_a2->GetPoint(i,xval,yval);
      //cout<<par[1]<<" "<<yval<<endl;
      // grint_t2->GetPoint(i,xval,yval);
      //  cout<<par[4]<<" "<<yval<<endl;
      errpar[0] = grint_a1->GetErrorY(i);
      errpar[1] = grint_a2->GetErrorY(i);
      errpar[2] = grint_a3->GetErrorY(i);
      errpar[4] = grint_t2->GetErrorY(i);
    }
    //    grint_a1->Print();
  }
  cout<<"DPM Params for Plab="<<plab<<endl;
  for(int jdpm=0;jdpm<5;jdpm++){
    cout<<par[jdpm]<<" +- "<<errpar[jdpm]<<endl;
  }
  LumiFit::PndLmdFitModelOptions model_options;
  // model_options.momentum_transfer_active = true; // will calc cross-section as a func from (t)
  // //model_options.momentum_transfer_active = false; // will calc cross-section as a func from (theta), theta in rad
  // model_options.acceptance_correction_active = false;
  // model_options.momentum_transfer_active = false;
  // model_options.resolution_smearing_active = false;
  // PndLmdModelFactory model_factory;
  // shared_ptr<Model1D> model1d = model_factory.generate1DModel(model_options, plab);

  PndLmdDPMAngModel1D modelDPM("dpm_angular_1d", LumiFit::ALL);
  shared_ptr<Parametrization> para(
  				   new PndLmdDPMModelParametrization(modelDPM.getModelParameterSet()));
  modelDPM.getModelParameterHandler().registerParametrizations(
  							    modelDPM.getModelParameterSet(), para);
  modelDPM.getModelParameterSet().setModelParameterValue("p_lab", plab);
  modelDPM.getModelParameterSet().setModelParameterValue("luminosity", 1);
  // modelDPM->init();
  ((Model1D*) &modelDPM)->init();
double lower_bound = 2.0;
  double upper_bound = 10.0 ;
  std::vector<std::pair<double, double> > integral_ranges;
  integral_ranges.push_back(std::make_pair(lower_bound / 1000.0, upper_bound / 1000.0));
  double integral_2_10 = modelDPM.Integral(integral_ranges, 0.0001);
  lower_bound = 3.0;
  upper_bound = 9.0 ;
  std::vector<std::pair<double, double> > integral_ranges2;
  integral_ranges2.push_back(std::make_pair(lower_bound / 1000.0, upper_bound / 1000.0));
  double integral_3_9 = modelDPM.Integral(integral_ranges2, 0.0001);
  lower_bound = 4.0;
  upper_bound = 8.0 ;
  std::vector<std::pair<double, double> > integral_ranges3;
  integral_ranges3.push_back(std::make_pair(lower_bound / 1000.0, upper_bound / 1000.0));
  double integral_4_8 = modelDPM.Integral(integral_ranges3, 0.0001);
  cout<<" integral (2-10) = "<<integral_2_10<<endl;

// double th_dw = 1.04719994E-03;//rad -> 0.06 DPM
  double th_dw = 0.0015;//
  //  double th_dw = 0.003;//
 //double th_up = TMath::Pi();
  // double th_up = 0.02;
  double th_up = 0.015;
   const int nst = 1e4;
   // const int nst = 2e1;
  double cs_loc = 0;
  double cs_val[nst], cs_uncert[nst];//dpm
  double cs_uncertA1[nst],cs_uncertA2[nst],cs_uncertA3[nst],cs_uncertt1[nst],cs_uncertt2[nst];
  double diff_cs_val[nst], diff_cs_uncert[nst];// dpm - e760 
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
    //    cs_val[i] = modelDPM.getRawFullElastic(&t_val[i]);
    cs_val[i] = modelDPM.getRawFullElastic(&t_cur);
    cs_uncert[i] = Df(t_cur,par,errpar);
    cs_uncertA1[i] = DfA1(t_cur,par,errpar);
    cs_uncertA2[i] = DfA2(t_cur,par,errpar);
    cs_uncertA3[i] = DfA3(t_cur,par,errpar);
    cs_uncertt1[i] = Dft1(t_cur,par,errpar);
    cs_uncertt2[i] = Dft2(t_cur,par,errpar);

    rel_err[i] = 100*(cs_uncert[i]/cs_val[i]);
    // if(fabs(th_val[i]-2.)<1e-3){ 
    //   resUn2mrad->SetPoint(pstep, plab, rel_err[i]);
    //   //    cout<<th_val[i]<<endl;
    // }
    // if(fabs(th_val[i]-5.)<1e-2){
    //   resUn5mrad->SetPoint(pstep, plab, rel_err[i]);
    //   //   cout<<th_val[i]<<endl;
    // }
    // if(fabs(th_val[i]-10.)<1e-2){
    //   resUn10mrad->SetPoint(pstep, plab, rel_err[i]);
    //   //   cout<<th_val[i]<<endl;
    // }
    // rel_err[i] = 100*(cs_uncert[i]/cs_val[i]);
    // //TEST: calculate total cross-section
    // vector<pair<double, double> > range;
    // range.push_back(make_pair(th1,th2));
    // double dcs =  model1d->Integral(range,1e-6);
    // cs_loc += dcs;
    // cout<<"cs_val = "<<cs_val[i]<<" +/- "<<cs_uncert[i]<<endl;
  }

  TGraphErrors *gr_cs_th = new TGraphErrors(nst,th_val,cs_val,0,cs_uncert);
  gr_cs_th->GetXaxis()->SetTitle("#theta, mrad");
  gr_cs_th->GetYaxis()->SetTitle("d#sigma/dt, mb/(GeV/c)^{2}");
  gr_cs_th->SetFillColor(4);
  gr_cs_th->SetFillStyle(3001);



  TGraphErrors *gr_cs_t = new TGraphErrors(nst,t_val,cs_val,0,cs_uncert);
  
  int x_2mrad=0;   int x_10mrad=0;
  int x_3mrad=0;   int x_9mrad=0;
  int x_4mrad=0;   int x_8mrad=0;
  PndLmdLumiHelper *lmd_help  = new PndLmdLumiHelper();
  double t_2mrad = (lmd_help->getMomentumTransferFromTheta(plab, 2e-3));
  double t_3mrad = (lmd_help->getMomentumTransferFromTheta(plab, 3e-3));
  double t_4mrad = (lmd_help->getMomentumTransferFromTheta(plab, 4e-3));
  double t_8mrad = (lmd_help->getMomentumTransferFromTheta(plab, 8e-3));
  double t_9mrad = (lmd_help->getMomentumTransferFromTheta(plab, 9e-3));
  double t_10mrad = (lmd_help->getMomentumTransferFromTheta(plab, 1e-2));
  cout<<" t_2mrad = "<<t_2mrad<<" t_3mrad = "<<t_3mrad<<" t_4mrad = "<<t_4mrad<<endl;
  cout<<" t_10mrad = "<<t_10mrad<<" t_9mrad = "<<t_9mrad<<" t_8mrad = "<<t_8mrad<<endl;
  for(int igrp = 0; igrp<gr_cs_t->GetN();igrp++){
    double x,y;
    gr_cs_t->GetPoint(igrp,x,y);
    if(fabs(x-t_2mrad)<0.01*t_2mrad) x_2mrad=igrp;
    if(fabs(x-t_3mrad)<0.01*t_3mrad) x_3mrad=igrp;
    if(fabs(x-t_4mrad)<0.01*t_4mrad) x_4mrad=igrp;
    if(fabs(x-t_10mrad)<0.01*t_10mrad) x_10mrad=igrp;
    if(fabs(x-t_8mrad)<0.01*t_8mrad) x_8mrad=igrp;
    if(fabs(x-t_9mrad)<0.01*t_9mrad) x_9mrad=igrp;
  }
  double CSint_2_10 = 0;
  for(int ik=x_2mrad;ik<x_10mrad;ik++){
    double x1,y1;
    gr_cs_t->GetPoint(ik,x1,y1);
    double x2,y2;
    gr_cs_t->GetPoint(ik+1,x2,y2);
    CSint_2_10 += y1*(x2-x1);
  }
  double CSint_4_8 = 0;
 for(int ik=x_4mrad;ik<x_8mrad;ik++){
    double x1,y1;
    gr_cs_t->GetPoint(ik,x1,y1);
    double x2,y2;
    gr_cs_t->GetPoint(ik+1,x2,y2);
    CSint_4_8 += y1*(x2-x1);
  }
 double CSint_3_9 = 0;
 for(int ik=x_3mrad;ik<x_9mrad;ik++){
    double x1,y1;
    gr_cs_t->GetPoint(ik,x1,y1);
    double x2,y2;
    gr_cs_t->GetPoint(ik+1,x2,y2);
    CSint_3_9 += y1*(x2-x1);
  }

 //  //  cout<<"4="<<x_4mrad<<" 8="<<x_8mrad<<" 2="<<x_2mrad<<" 10="<<x_10mrad
 cout<<" CSint_2_10 = "<<CSint_2_10<<" CSint_3_9 = "<<CSint_3_9<<" CSint_4_8 = "<<CSint_4_8<<endl;
  // double norm_2_10 = double(x_10mrad-x_2mrad)/(gr_cs_t->GetN());
  // cout<<"norm_2_10 = "<<norm_2_10<<endl;
  gr_cs_t->SetTitle("");
  gr_cs_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  gr_cs_t->GetYaxis()->SetTitle("d#sigma/dt, mb/(GeV/c)^{2}");
  gr_cs_t->SetFillColor(4);
  gr_cs_t->SetFillStyle(3001);

  TGraph *gr_cs_un_th = new TGraph(nst,th_val,cs_uncert);
  gr_cs_un_th->SetTitle("");
  gr_cs_un_th->GetXaxis()->SetTitle("#theta, mrad");
  gr_cs_un_th->GetYaxis()->SetTitle("#Delta(d#sigma/dt), mb/(GeV/c)^{2}");

  TMultiGraph *mgr_cs_un_t = new TMultiGraph();
  TGraph *gr_cs_un_t = new TGraph(nst,t_val,cs_uncert);
  TGraph *gr_cs_unA1_t = new TGraph(nst,t_val,cs_uncertA1);
  gr_cs_unA1_t->SetLineColor(46);
  gr_cs_unA1_t->SetMarkerColor(46);
  gr_cs_unA1_t->SetMarkerStyle(20);
  gr_cs_unA1_t->SetMarkerSize(0.3);
  TGraph *gr_cs_unA2_t = new TGraph(nst,t_val,cs_uncertA2);
  gr_cs_unA2_t->SetMarkerColor(7);
  gr_cs_unA2_t->SetLineColor(7);
  gr_cs_unA2_t->SetMarkerStyle(20);
  gr_cs_unA2_t->SetMarkerSize(0.3);
  TGraph *gr_cs_unA3_t = new TGraph(nst,t_val,cs_uncertA3);
  gr_cs_unA3_t->SetMarkerColor(8);
  gr_cs_unA3_t->SetLineColor(8);
  gr_cs_unA3_t->SetMarkerStyle(20);
  gr_cs_unA3_t->SetMarkerSize(0.3);
  TGraph *gr_cs_unt1_t = new TGraph(nst,t_val,cs_uncertt1);
  gr_cs_unt1_t->SetMarkerColor(12);
  gr_cs_unt1_t->SetLineColor(12);
  gr_cs_unt1_t->SetMarkerStyle(20);
  gr_cs_unt1_t->SetMarkerSize(0.3);
  TGraph *gr_cs_unt2_t = new TGraph(nst,t_val,cs_uncertt2);
  gr_cs_unt2_t->SetMarkerColor(28);
  gr_cs_unt2_t->SetLineColor(28);
  gr_cs_unt2_t->SetMarkerStyle(20);
  gr_cs_unt2_t->SetMarkerSize(0.3);
  TLegend *legUn = new TLegend(0.77,0.64,0.99,0.99);
  legUn->SetFillColor(0);
  legUn->SetTextFont(42);
  legUn->SetTextSize(0.05);
  legUn->AddEntry(gr_cs_un_t,"total uncertainty","l");
  legUn->AddEntry(gr_cs_unA1_t,"A1 contribution","l");
  legUn->AddEntry(gr_cs_unA2_t,"A2 contribution","l");
  legUn->AddEntry(gr_cs_unA3_t,"A3 contribution","l");
  legUn->AddEntry(gr_cs_unt1_t,"t1 contribution","l");
  legUn->AddEntry(gr_cs_unt2_t,"t2 contribution","l");
  mgr_cs_un_t->Add(gr_cs_un_t,"L");
  mgr_cs_un_t->Add(gr_cs_unA1_t,"L");
  mgr_cs_un_t->Add(gr_cs_unA2_t,"L");
  mgr_cs_un_t->Add(gr_cs_unA3_t,"L");
  mgr_cs_un_t->Add(gr_cs_unt1_t,"L");
  mgr_cs_un_t->Add(gr_cs_unt2_t,"L");
  double errCSint_2_10 = 0;
  for(int ik=x_2mrad;ik<x_10mrad;ik++){
    double x1,y1;
    gr_cs_un_t->GetPoint(ik,x1,y1);
    double x2,y2;
    gr_cs_un_t->GetPoint(ik+1,x2,y2);
    errCSint_2_10 += y1*(x2-x1);
  }
  double errCSint_4_8 = 0;
 for(int ik=x_4mrad;ik<x_8mrad;ik++){
    double x1,y1;
    gr_cs_un_t->GetPoint(ik,x1,y1);
    double x2,y2;
    gr_cs_un_t->GetPoint(ik+1,x2,y2);
    errCSint_4_8 += y1*(x2-x1);
  }
 double errCSint_3_9 = 0;
 for(int ik=x_3mrad;ik<x_9mrad;ik++){
    double x1,y1;
    gr_cs_un_t->GetPoint(ik,x1,y1);
    double x2,y2;
    gr_cs_un_t->GetPoint(ik+1,x2,y2);
    errCSint_3_9 += y1*(x2-x1);
  }

cout<<" errCSint_2_10 = "<<errCSint_2_10<<" errCSint_3_9 = "<<errCSint_3_9<<" errCSint_4_8 = "<<errCSint_4_8<<endl;
 double integ_un_2_10 = 100.*errCSint_2_10/CSint_2_10;
 resUn2mrad->SetPoint(pstep, plab,integ_un_2_10);
 double errIntegCS=70*fabs(CSint_2_10-integral_2_10)/integral_2_10;
 resUn2mrad->SetPointError(pstep,0,errIntegCS);
 double integ_un_3_9 = 100.*errCSint_3_9/CSint_3_9;
 resUn3mrad->SetPoint(pstep, plab,integ_un_3_9);
 errIntegCS=70*fabs(CSint_3_9-integral_3_9)/integral_3_9;
 resUn3mrad->SetPointError(pstep,0,errIntegCS);
 double integ_un_4_8 = 100.*errCSint_4_8/CSint_4_8;
 resUn4mrad->SetPoint(pstep, plab,integ_un_4_8);
 errIntegCS=70*fabs(CSint_4_8-integral_4_8)/integral_4_8;
 resUn4mrad->SetPointError(pstep,0,errIntegCS);

  gr_cs_un_t->SetTitle("");
  gr_cs_un_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  gr_cs_un_t->GetYaxis()->SetTitle("#Delta(d#sigma/dt), mb/(GeV/c)^{2}");

 TGraph *gr_cs_relun_th = new TGraph(nst,th_val,rel_err);
 gr_cs_relun_th->SetTitle("");
  gr_cs_relun_th->GetXaxis()->SetTitle("#theta, mrad");
  gr_cs_relun_th->GetYaxis()->SetTitle("#Delta(d#sigma/dt)/(d#sigma/dt), %");

  TGraph *gr_cs_relun_t = new TGraph(nst,t_val,rel_err);
  gr_cs_relun_t->SetTitle("");
  gr_cs_relun_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  gr_cs_relun_t->GetYaxis()->SetTitle("#Delta(d#sigma/dt)/(d#sigma/dt), %");


  TCanvas c2;
  c2.Divide(2,3);
  c2.cd(1);
  gr_cs_th->Draw("AP4");
  c2.cd(2);
  gr_cs_t->Draw("AP4");
  c2.cd(3);
  gr_cs_un_th->Draw("AL");
  c2.cd(4);
  mgr_cs_un_t->Draw("A");
  mgr_cs_un_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  mgr_cs_un_t->GetYaxis()->SetTitle("#Delta(d#sigma/dt), mb/(GeV/c)^{2}");
  legUn->Draw();
  // gr_cs_un_t->Draw("AL");
  c2.cd(5);
  gr_cs_relun_th->Draw("AL");
  c2.cd(6);
  gr_cs_relun_t->Draw("AL");
  TString fname = "cs_and_uncert_Plab_";
  fname +=plab;
  TString fnamepdf = fname + ".pdf";
  TString fnameroot = fname + ".root";
  c2.SaveAs(fnamepdf);
  c2.SaveAs(fnameroot);
  // [end] part2: calculate differencial cross-section and its uncertanty ----

 
  // TCanvas c4("c4","canvas",800,600);
  // h_cs_th->Draw();
  // c4.SaveAs("HIST.pdf");
  }


  TCanvas c3("c3","canvas",800,600);
  TMultiGraph *mg_res = new TMultiGraph();
  mg_res->Add(resUn2mrad);
  mg_res->Add(resUn3mrad);
  mg_res->Add(resUn4mrad);

  TLegend *leg = new TLegend(0.17,0.65,0.52,0.85);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.05);
  leg->AddEntry(resUn2mrad,"#theta #in [2-10] mrad","ep");
  leg->AddEntry(resUn3mrad,"#theta #in [3-9] mrad","ep");
  leg->AddEntry(resUn4mrad,"#theta #in [4-8] mrad","ep");
  mg_res->Draw("AP");
  mg_res->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  mg_res->SetMinimum(-1.);
  //  mg_res->GetYaxis()->SetTitle("#Delta(d#sigma/dt)/(d#sigma/dt), %");
  mg_res->GetYaxis()->SetTitle("#Delta#sigma/#sigma, %");
  leg->Draw();
  c3.SaveAs("uncertDPMInteg_vs_Plab.pdf");
   c3.SaveAs("uncertDPMInteg_vs_Plab.root");
  return 0;
}
