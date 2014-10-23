// macro for comparision DPM model and E760 model with their uncertainty in LMD range
#include <model/PndLmdDPMAngModel1D.h>
#include <model/PndLmdDPMMTModel1D.h>
#include <model/PndLmdDPMModelParametrization.h>
#include <model/PndLmdDPMLikeModelParametrization.h>
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

double s_inv(double pl){// s
  double m_p = 0.938;//GeV/c barp mass
 double res = 2*TMath::Power(m_p,2) + 2*m_p*TMath::Sqrt(m_p*m_p+pl*pl);
 return res;
}

double p_cm(double pl){// Plab -> Pcm
  double m_p = 0.938;//GeV/c barp mass
  double s = s_inv(pl);
  double res = pl*m_p/sqrt(s);
  return res;
}

double t_cosCM(double cosCM, double pl){ // cos theta in CM -> t
  double pcm = p_cm(pl);
  double res = -2*TMath::Power(pcm,2)*(1-cosCM);
  return res;
}

double cosCM_t(double x, double pl){ // t -> cos theta in CM
  double t =  -TMath::Abs(x);
  double pcm = p_cm(pl);
  double res = 1+0.5*t/TMath::Power(pcm,2);
  return res;
}

double thLAB_t(double pl, double t){ // t -> theta in LAB
  double cosTh1 = cosCM_t(t,pl);
  double ThCM = TMath::ACos(cosTh1);
  double res = 0.5*ThCM;
  // cout<<"t= "<<t<<" cosTh = "<<cosTh1<<endl;
  return res;
}

double ds_dt(double ds_do, double pl){
  double pcm = p_cm(pl);
  double res = TMath::Pi()*ds_do/TMath::Power(pcm,2);
  return res;
}

double df_dSigT(double t, double par[]){
  double sigT = par[0];
  double b = par[1];
  double rho = par[2];
  double F = par[3];
  double res = 2*F/sigT;
  //  cout<<"res_sigT "<<res<<endl;
  return res;
}
double df_db(const double t, double par[]){
  double sigT = par[0];
  double b = par[1];
  double rho = par[2];
  double F = par[3];
  double res = -t*F;
  //  cout<<"res_b "<<res<<endl;
  return res;
}
double df_drho(const double t, double par[]){
  double sigT = par[0];
  double b = par[1];
  double rho = par[2];
  double F = par[3];
  // cout<<"F = "<<F<<endl;
  double res = F*2*rho/(1+rho*rho);
  //  cout<<"res_rho "<<res<<endl;
  return res;
}

double DfE760(const double t, double par[], double errpar[]){
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


int E760fit(const double plab, double &parsigT, double &parb, double &parrho, 
	    double &errparsigT, double &errparb, double &errparrho){
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
  // grint->SetTitle("Fitted line with .68 conf. band");
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
  c1.SaveAs("E760_fit.pdf"); // CONTROL Plots
  // c1.SaveAs("E760_fit.root");

  //[end] part1: fit energy dependence of E760 parameters ---------------------

  // part2: model params uncertanty at given Plab --------------------------
  double par[4],errpar[4];
  par[0] = fsigt->Eval(plab);
  par[1] = fb->Eval(plab);
  par[2] = frho->Eval(plab);
  parsigT = par[0];
  parb = par[1];
  parrho = par[2];
  for(int i=0;i<grint_sigt->GetN();i++){
    double xval,yval;
    grint_sigt->GetPoint(i,xval,yval);
    double errpar_0 = grint_sigt->GetErrorY(i);
    //  if(fabs(xval-plab)<1e-2 && fabs(par[0]-yval)<2*errpar_0){
    if(fabs(xval-plab)<1e-3){
      errpar[0] = grint_sigt->GetErrorY(i);
      errpar[1] = grint_b->GetErrorY(i);
      errpar[2] = grint_rho->GetErrorY(i);
    }
  }
  errparsigT = errpar[0];
  errparb =  errpar[1];
  errparrho = errpar[2];
  // cout<<"E760-like Params for Plab="<<plab<<endl;
  // for(int jdpm=0;jdpm<3;jdpm++){
  //   cout<<par[jdpm]<<" +- "<<errpar[jdpm]<<endl;
  // }
  //[end] part2: model params uncertanty at given Plab ---------------------
  return 0;
}
double df_dA1(const double x, double par[]){
  double t =  -TMath::Abs(x);
  double A1 = par[0];
  double A2 = par[1];
  double A3 = par[2];
  double t1 = par[3];
  double t2 = par[4];
  double int_res = TMath::Exp(t/(2*t1))-A2*TMath::Exp(t/(2*t2));
  double res = TMath::Power(int_res,2);
  return res;
}
double df_dA2(const double x, double par[]){
  double t =  -TMath::Abs(x);
  double A1 = par[0];
  double A2 = par[1];
  double A3 = par[2];
  double t1 = par[3];
  double t2 = par[4];
  double res = -2*A1*TMath::Exp(t/(2*t1)+t/(2*t2))+2*A1*A2*TMath::Exp(t/t2);
  return res;
}

double df_dA3(const double x, double par[]){
  double t =  -TMath::Abs(x);
  double A1 = par[0];
  double A2 = par[1];
  double A3 = par[2];
  double t1 = par[3];
  double t2 = par[4];
  double res = TMath::Exp(t/t2);
  return res;
}

double df_dt1(const double x, double par[]){
  double t =  -TMath::Abs(x);
  double A1 = par[0];
  double A2 = par[1];
  double A3 = par[2];
  double t1 = par[3];
  double t2 = par[4];
  double res = A1*(-TMath::Exp(t/t1)+A2*TMath::Exp(t/(2*t1)+t/(2*t2)))*(t/(t1*t1));
  return res;
}

double df_dt2(const double x, double par[]){
  double t =  -TMath::Abs(x);
  double A1 = par[0];
  double A2 = par[1];
  double A3 = par[2];
  double t1 = par[3];
  double t2 = par[4];
  //double res = A1*(-A2*TMath::Exp(t/(2*t1)+t/(2*t2))+A2*A2*TMath::Exp(t/t2))*(-t/(t2*t2))+A3*TMath::Exp(t/t2)*(-t/t2);
  double res = A1*A2*(TMath::Exp(t/(2*t1)+t/(2*t2))-(A2+A3)*TMath::Exp(t/t2))*(-t/(t2*t2));
  return res;
}

double DfDPM(const double t, double par[], double errpar[]){
  double errA1 = errpar[0];
  double errA2 = errpar[1];
  double errA3 = errpar[2];
  double errt1 = errpar[3];
  double errt2 = errpar[4];
  double int_res = TMath::Power(df_dA1(t,par)*errA1,2)+TMath::Power(df_dA2(t,par)*errA2,2)+TMath::Power(df_dA3(t,par)*errA3,2)+TMath::Power(df_dt1(t,par)*errt1,2)+TMath::Power(df_dt2(t,par)*errt2,2);
  double res = TMath::Sqrt(int_res);
  return res;
}

int DPMfit(double plab, double &para1, double &para2, double &para3, double &part1, double &part2,
	   double &errpara1, double &errpara2, double &errpara3, double &errpart1, double &errpart2){
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

  c1.SaveAs("DPM_fit.pdf"); //CONTROL plots
  //[end] part1: fit energy dependence of DPM parameters -------------------

  //part2: model params uncertanty at given Plab ---------------------------
  double par[5],errpar[5];
  par[0] = fa1->Eval(plab);
  par[1] = fa2->Eval(plab);
  par[2] = fa3->Eval(plab);
  par[3] = 0.0899;
  errpar[3] = 0.010;
  par[4] = ft2->Eval(plab);
  para1 = par[0];
  para2 = par[1];
  para3 = par[2];
  part1 = par[3];
  part2 = par[4];
 for(int i=0;i<grint_a1->GetN();i++){
    double xval,yval;
    grint_a1->GetPoint(i,xval,yval);
    double errpar_0 = grint_a1->GetErrorY(i);
    //  if(fabs(xval-plab)<1e-1 && fabs(par[0]-yval)<1e-1){
    //    cout<<par[0]<<" "<<yval<<endl;
    //    if(fabs(xval-plab)<5e-4 && fabs(par[0]-yval)<errpar_0){
    if(fabs(xval-plab)<1e-3){
    //  if(fabs(xval-plab)<1e-2){
      // cout<<par[0]<<" "<<yval<<endl;
      //      grint_a2->GetPoint(i,xval,yval);
      // cout<<par[1]<<" "<<yval<<endl;
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
  errpara1 = errpar[0];
  errpara2 = errpar[1];
  errpara3 = errpar[2];
  errpart1 = errpar[3];
  errpart2 = errpar[4];
  //[end] part2: model params uncertanty at given Plab ---------------------
  return 0;
}
int DPMlikefit(double plab, double &para1, double &para2, double &para3, double &part1, double &part2,
	   double &errpara1, double &errpara2, double &errpara3, double &errpart1, double &errpart2){
 //DPM-like
  double mom[4]={2.33,2.85,5.7,10.40};
  double A1[4]={582,426,232,171};
  double errA1[4]={17,8.5,7.4,1.9};
  double A2[4]={0.196,0.153,0.11,0.074};
  double errA2[4]={0.008,0.005,0.010,0.004};
  double T2[4]={0.322,0.394, 0.351, 0.293};
  double errT2[4]={0.012, 0.012, 0.028, 0.015};
  double A3[4]={2.72,1.78,0.77,0.89};
  double errA3[4]={0.38,0.19,0.20,0.12};
  TCanvas c1("c1","canvas",1200,800);
  c1.Divide(4,2);

  TGraphErrors *grA1 = new TGraphErrors(5,mom,A1,0,errA1); 
  grA1->SetMarkerStyle(21);
  grA1->SetMarkerSize(0.9); 
  TF1 *fa1 = new TF1("fa1","[0]+[1]*exp(-x/[2])",mom[0]-1.5,mom[3]+1.5);
  fa1->SetParNames("par0","par1","par2");
  fa1->SetParameter(0,115);
  fa1->SetParameter(1,650);
  fa1->SetParameter(2,4.08);
  grA1->Fit(fa1,"R");

  TF1 *fa1_dpm = new TF1("fa1_dpm","[0]+[1]*exp(-x/[2])",mom[0]-1.5,mom[3]+1.5);
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
    double pcur= mom[0]-1.0+((mom[3]+4.6)-(mom[0]-1.0))*i/np;
    grint_a1->SetPoint(i, pcur, 0);
    double myfit = fa1->Eval(pcur);
    double dpmfit = fa1_dpm->Eval(pcur);
    double difffit = 100.*(dpmfit-myfit)/dpmfit;
    grdiff_a1->SetPoint(i, pcur, difffit);
  }
  // Compute the confidence intervals at the x points of the created graph
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(grint_a1,0.68);
  grint_a1->SetLineColor(kRed);
  grint_a1->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grint_a1->GetYaxis()->SetTitle("A_{1}");
  grint_a1->SetFillColor(2);
  grint_a1->SetFillStyle(3005);


  c1.cd(1);
  grint_a1->Draw("AP4");
  grA1->Draw("psame");
  // grA1->Draw("AP");
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
 TGraphErrors *grA2 = new TGraphErrors(5,mom,A2,0,errA2); 
  grA2->SetMarkerStyle(21);
  grA2->SetMarkerSize(0.9); 
  TF1 *fa2 = new TF1("fa2","[0]+[1]*exp(-x/[2])",mom[0]-1.5,mom[3]+1.5);
  fa2->SetParNames("par0","par1","par2");
  fa2->SetParameter(0,0.0687);
  fa2->SetParameter(1,0.307);
  fa2->SetParameter(2,2.367);
  grA2->Fit(fa2,"R");

  TF1 *fa2_dpm = new TF1("fa2_dpm","[0]+[1]*exp(-x/[2])",mom[0]-1.5,mom[3]+1.5);
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
    double pcur= mom[0]-1.0+((mom[3]+4.6)-(mom[0]-1.0))*i/np;
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
 TGraphErrors *grT2 = new TGraphErrors(5,mom,T2,0,errT2); 
  grT2->SetMarkerStyle(21);
  grT2->SetMarkerSize(0.9); 
  TF1 *ft2 = new TF1("ft2","[0]+[1]*exp(-x/[2])",mom[0]-1.5,mom[3]+1.5);
  ft2->SetParNames("par0","par1","par2");
  ft2->SetParameter(0,-2.979);
  ft2->SetParameter(1,3.353);
  ft2->SetParameter(2,483.4);
  grT2->Fit(ft2,"R");

  TF1 *ft2_dpm = new TF1("ft2_dpm","[0]+[1]*exp(-x/[2])",mom[0]-1.5,mom[3]+1.5);
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
    double pcur= mom[0]-1.0+((mom[3]+4.6)-(mom[0]-1.0))*i/np;
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
 TGraphErrors *grA3 = new TGraphErrors(5,mom,A3,0,errA3); 
  grA3->SetMarkerStyle(21);
  grA3->SetMarkerSize(0.9); 
  TF1 *fa3 = new TF1("fa3","[0]+[1]*exp(-x/[2])",mom[0]-1.5,mom[3]+1.5);
  fa3->SetParNames("par0","par1","par2");
  fa3->SetParameter(0,0.8372);
  fa3->SetParameter(1,39.53);
  fa3->SetParameter(2,0.765);
  grA3->Fit(fa3,"R");

  TF1 *fa3_dpm = new TF1("fa3_dpm","[0]+[1]*exp(-x/[2])",mom[0]-1.5,mom[3]+1.5);
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
    double pcur= mom[0]-1.0+((mom[3]+4.6)-(mom[0]-1.0))*i/np;
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

  c1.SaveAs("DPM_fit.pdf"); //CONTROL plots

  //[end] part1: fit energy dependence of DPM parameters ---------------------

 //part2: model params uncertanty at given Plab ---------------------------
  double par[5],errpar[5];
  par[0] = fa1->Eval(plab);
  par[1] = fa2->Eval(plab);
  par[2] = fa3->Eval(plab);
  par[3] = 0.0899;
  errpar[3] = 0.010;
  par[4] = ft2->Eval(plab);
  para1 = par[0];
  para2 = par[1];
  para3 = par[2];
  part1 = par[3];
  part2 = par[4];
 for(int i=0;i<grint_a1->GetN();i++){
    double xval,yval;
    grint_a1->GetPoint(i,xval,yval);
    double errpar_0 = grint_a1->GetErrorY(i);
    //  if(fabs(xval-plab)<1e-1 && fabs(par[0]-yval)<1e-1){
    //    cout<<par[0]<<" "<<yval<<endl;
    // if(fabs(xval-plab)<1e-2 && fabs(par[0]-yval)<errpar_0){
    if(fabs(xval-plab)<1e-2){
      // cout<<par[0]<<" "<<yval<<endl;
      //      grint_a2->GetPoint(i,xval,yval);
      // cout<<par[1]<<" "<<yval<<endl;
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
  errpara1 = errpar[0];
  errpara2 = errpar[1];
  errpara3 = errpar[2];
  errpart1 = errpar[3];
  errpart2 = errpar[4];
  //[end] part2: model params uncertanty at given Plab ---------------------
  return 0;

}
int ExpDataBase(const double plab, TGraphErrors &expdata, int sampl=0){
 

  if(fabs(plab-10.1)<1e-4){
    //http://durpdg.dur.ac.uk/view/ins133174
    //-T IN GEV**2	D(SIG)/DT IN MUB/GEV**2
    cout<<"Data from BERGLUND 1980 — K- p AND anti-p p ELASTIC SCATTERING AT 10.1-GeV/c, added 5% system"<<endl;
 double p2686_d2x1y1_xval[] = { 0.19, 0.21000000000000002, 0.22999999999999998, 0.25, 0.27, 0.29000000000000004, 0.31, 0.33, 0.35, 
    0.37, 0.39, 0.41000000000000003, 0.43, 0.45, 0.47, 0.49, 0.525, 0.575, 0.625, 
    0.675, 0.725, 0.775, 0.825, 0.875, 0.925, 1.0, 1.1, 1.2, 1.3, 
    1.4500000000000002, 1.7000000000000002, 2.0, 2.3, 2.6, 3.0 };
  double p2686_d2x1y1_xerr[] = { 0.010000000000000009, 0.010000000000000009, 0.009999999999999981, 0.010000000000000009, 0.010000000000000009, 0.010000000000000009, 0.010000000000000009, 0.010000000000000009, 0.009999999999999953, 
    0.010000000000000009, 0.010000000000000009, 0.010000000000000009, 0.010000000000000009, 0.010000000000000009, 0.009999999999999953, 0.010000000000000009, 0.025000000000000022, 0.02499999999999991, 0.025000000000000022, 
    0.025000000000000022, 0.025000000000000022, 0.025000000000000022, 0.02499999999999991, 0.025000000000000022, 0.025000000000000022, 0.050000000000000044, 0.050000000000000044, 0.050000000000000044, 0.050000000000000044, 
    0.10000000000000009, 0.15000000000000013, 0.1499999999999999, 0.1499999999999999, 0.1499999999999999, 0.25 };
 
  double p2686_d2x1y1_yval[] = { 19180.0, 13120.0, 9510.0, 6960.0, 5970.0, 4630.0, 3550.0, 3128.0, 2012.0, 
    1491.0, 1295.0, 900.0, 659.0, 576.0, 487.0, 350.0, 248.0, 128.0, 128.0, 
    104.0, 104.0, 98.0, 100.0, 79.0, 73.0, 54.3, 42.7, 29.9, 14.7, 
    4.8, 2.0, 1.2, 2.6, 2.0, 2.4 };
  double p2686_d2x1y1_yerr[] = { 420.0, 320.0, 260.0, 210.0, 190.0, 170.0, 150.0, 140.0, 110.0, 
    88.0, 81.0, 67.0, 50.0, 53.0, 47.0, 40.0, 21.0, 15.0, 15.0, 
    14.0, 13.0, 13.0, 13.0, 12.0, 11.0, 6.4, 5.7, 4.7, 3.4, 
    1.4, 0.8, 0.6, 0.9, 0.9, 1.2 };
  for(int it=0;it<35;it++){ // add systematic 5%
    double system = 5e-2*p2686_d2x1y1_yval[it];
    p2686_d2x1y1_yerr[it] = sqrt(p2686_d2x1y1_yerr[it]*p2686_d2x1y1_yerr[it]+system*system);
    p2686_d2x1y1_yval[it] *=1e-3;
    p2686_d2x1y1_yerr[it] *=1e-3;
  }
 
  int p2686_d2x1y1_numpoints = 35;//only small t
  expdata = TGraphErrors(p2686_d2x1y1_numpoints, p2686_d2x1y1_xval, p2686_d2x1y1_yval, p2686_d2x1y1_xerr,
			      p2686_d2x1y1_yerr);
    return 0;
  }

  if(fabs(plab-8.0)<1e-4 && sampl==0){
//http://durpdg.dur.ac.uk/view/ins124704
    cout<<"Data from RUSS 1977 — Elastic Scattering of pi-, K-, and anti-p from Hydrogen at 8-GeV/c and 16-GeV/c, added syst. 5%"<<endl;
    //-T IN GEV**2	D(SIG)/DT IN MB/GEV**2
 double p4722_d4x1y1_xval[] = { 0.025, 0.035, 0.045, 0.055, 0.065, 0.075, 0.085, 0.095, 0.105, 
    0.115, 0.125, 0.135, 0.145, 0.155, 0.165, 0.175, 0.185, 0.195, 0.205, 
    0.215, 0.225, 0.235, 0.245, 0.255, 0.265, 0.275, 0.285, 0.295, 0.31, 
    0.33, 0.35, 0.37, 0.39, 0.435, 0.535, 0.585, 0.635, 0.685, 0.735, 
    0.785, 0.835 };
  double p4722_d4x1y1_yval[] = { 116.0, 103.29, 90.07, 81.36, 71.39, 62.76, 54.91, 48.81, 43.74, 
    39.07, 33.94, 30.28, 27.52, 24.02, 21.01, 17.61, 15.97, 14.01, 12.86, 
    10.96, 10.23, 8.99, 7.44, 7.02, 5.86, 4.954, 4.966, 3.992, 3.15, 
    2.6, 1.951, 1.404, 1.121, 0.6, 0.14, 0.107, 0.068, 0.067, 0.074, 
    0.09, 0.094 };
  double p4722_d4x1y1_yerr[] = { 1.3, 0.88, 0.52, 0.43, 0.39, 0.41, 0.41, 0.37, 0.37, 
    0.33, 0.32, 0.3, 0.29, 0.28, 0.26, 0.22, 0.22, 0.21, 0.28, 
    0.27, 0.26, 0.24, 0.39, 0.39, 0.35, 0.325, 0.316, 0.335, 0.192, 
    0.192, 0.192, 0.134, 0.125, 0.039, 0.042, 0.042, 0.032, 0.021, 0.021, 
    0.021, 0.021 };

 for(int it=0;it<41;it++){ // add systematic 5%
    double system = 5e-2*p4722_d4x1y1_yval[it];
    p4722_d4x1y1_yerr[it] = sqrt(p4722_d4x1y1_yerr[it]*p4722_d4x1y1_yerr[it]+system*system);
  }

 int p4722_d4x1y1_numpoints = 41;
 // int p4722_d4x1y1_numpoints = 20;
  expdata =  TGraphErrors(p4722_d4x1y1_numpoints, p4722_d4x1y1_xval, p4722_d4x1y1_yval, 0, p4722_d4x1y1_yerr);
    
  return 0;
  }

  if(fabs(plab-6.2)<1e-2){
    //http://durpdg.dur.ac.uk/view/ins99557
    //-T IN GEV**2	D(SIG)/DT IN MUB/GEV**2
    cout<<"Data from BURAN 1976 — Anti-Proton-Proton Elastic Scattering at 6.2-GeV/c, added syst. 20%"<<endl;
  double p2447_d1x1y1_xval[] = { 0.31, 0.33, 0.35, 0.37, 0.39, 0.41, 0.43, 0.45, 0.47, 
    0.49, 0.51, 0.53, 0.55, 0.57, 0.59, 0.61, 0.63, 0.65, 0.67, 
    0.69, 0.71, 0.73, 0.75, 0.77, 0.79, 0.81, 0.83, 0.85, 0.87, 
    0.89, 0.91, 0.93, 0.95, 0.97, 0.99, 1.02, 1.06, 1.1, 1.14, 
    1.18, 1.22, 1.26, 1.3, 1.34, 1.38, 1.42, 1.46, 1.49, 1.55, 
    1.65, 1.75, 1.85, 1.95, 2.05, 2.15, 2.25, 2.35, 2.45, 2.55, 
    2.65, 2.75, 2.85, 2.95, 3.05, 3.15, 3.35, 4.0, 5.5, 7.75, 
    9.5 };
  double p2447_d1x1y1_xerr[] = { 0.020000000000000018, 0.020000000000000018, 0.019999999999999962, 0.020000000000000018, 0.020000000000000018, 0.019999999999999962, 0.020000000000000018, 0.020000000000000018, 0.019999999999999962, 
    0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.019999999999999907, 0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 
    0.019999999999999907, 0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.019999999999999907, 0.020000000000000018, 0.020000000000000018, 
    0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.019999999999999907, 0.020000000000000018, 0.020000000000000018, 0.040000000000000036, 0.040000000000000036, 0.040000000000000036, 0.039999999999999813, 
    0.040000000000000036, 0.040000000000000036, 0.040000000000000036, 0.040000000000000036, 0.040000000000000036, 0.039999999999999813, 0.040000000000000036, 0.040000000000000036, 0.020000000000000018, 0.10000000000000009, 
    0.010000000000000009, 0.010000000000000009, 0.010000000000000009, 0.010000000000000009, 0.009999999999999787, 0.009999999999999787, 0.009999999999999787, 0.010000000000000231, 0.010000000000000231, 0.009999999999999787, 
    0.009999999999999787, 0.009999999999999787, 0.010000000000000231, 0.010000000000000231, 0.009999999999999787, 0.009999999999999787, 0.30000000000000027, 1.0, 2.0, 2.5, 
    1.0 };
  
  double p2447_d1x1y1_yval[] = { 2377.0, 1771.0, 1253.0, 918.0, 647.0, 466.0, 342.0, 261.0, 193.0, 
    160.0, 147.0, 132.0, 134.0, 136.0, 141.0, 145.0, 159.0, 169.0, 177.0, 
    183.0, 167.0, 184.0, 181.0, 172.0, 166.0, 176.0, 165.0, 170.0, 151.0, 
    150.0, 153.0, 136.0, 133.0, 115.0, 123.0, 112.0, 100.0, 83.4, 68.4, 
    60.6, 50.8, 44.6, 41.4, 29.8, 24.9, 21.1, 18.7, 15.7, 12.0, 
    10.4, 7.94, 7.7, 5.47, 5.15, 5.32, 5.16, 4.6, 3.78, 3.28, 
    2.68, 2.59, 1.69, 1.44, 1.04, 0.93, 0.33, 0.077, 0.026, 0.0074, 
    0.03 };
  double p2447_d1x1y1_yerr[] = { 64.0, 49.0, 36.0, 16.0, 12.0, 11.0, 10.0, 8.0, 7.0, 
    6.0, 6.0, 5.0, 5.0, 5.0, 5.0, 5.0, 6.0, 6.0, 7.0, 
    7.0, 6.0, 7.0, 7.0, 7.0, 6.0, 7.0, 7.0, 7.0, 6.0, 
    6.0, 7.0, 6.0, 6.0, 6.0, 6.0, 4.0, 4.0, 2.8, 2.8, 
    2.7, 2.5, 2.4, 2.2, 1.9, 1.7, 1.6, 1.5, 2.0, 0.73, 
    0.7, 0.6, 0.6, 0.48, 0.43, 0.47, 0.47, 0.46, 0.42, 0.42, 
    0.4, 0.4, 0.4, 0.34, 0.33, 0.35, 0.15, 0.046, 0.021, 0.0059, 
    0.015 };
 
  for(int it=0;it<70;it++){ // add systematic 20%
    double system = 0.2*p2447_d1x1y1_yval[it];
    p2447_d1x1y1_yerr[it] = sqrt(p2447_d1x1y1_yerr[it]*p2447_d1x1y1_yerr[it]+system*system);
    //conver mub into mb
    p2447_d1x1y1_yval[it] *=1e-3;
    p2447_d1x1y1_yerr[it] *=1e-3;
  }
  // int p2447_d1x1y1_numpoints = 70;
  int p2447_d1x1y1_numpoints = 35;
  expdata =  TGraphErrors(p2447_d1x1y1_numpoints, p2447_d1x1y1_xval, p2447_d1x1y1_yval, p2447_d1x1y1_xerr, p2447_d1x1y1_yerr);
    return 0;
  }
  if(fabs(plab-1.5)<1e-3){
    //http://durpdg.dur.ac.uk/view/ins109426
    cout<<"Data from EISENHANDLER 1976 — Differential Cross-Sections for anti-Proton-Proton Elastic Scattering Between 0.69-GeV/c and 2.43-GeV/c, added 4% system.error!"<<endl;
    //COS(THETA)	D(SIG)/DOMEGA IN MB/SR
    //!!!Additional systematic error: ± 4.0% (OVERALL NORMALIZATION UNCERTAINTY,NOT INCLUDED)!!!
    double p7011_d3x1y3_xval[] = { 0.93, 0.91, 0.89, 0.87, 0.85, 0.83, 0.81, 0.79, 0.77, 
    0.75, 0.73, 0.71, 0.69, 0.67, 0.65, 0.63, 0.61, 0.59, 0.57, 
    0.55, 0.53, 0.51, 0.49, 0.47, 0.45, 0.43, 0.41, 0.39, 0.37, 
    0.35, 0.33, 0.31, 0.29, 0.27, 0.25, 0.23, 0.21, 0.19, 0.17, 
    0.15, 0.13, 0.11, 0.09, 0.07, 0.05, 0.03, 0.01, -0.01, -0.03, 
    -0.05, -0.07, -0.09, -0.11, -0.13, -0.15, -0.17, -0.19, -0.21, -0.23, 
    -0.25, -0.27, -0.29, -0.31, -0.33, -0.35, -0.37, -0.39, -0.41, -0.43, 
    -0.45, -0.47, -0.49, -0.51, -0.53, -0.55, -0.57, -0.59, -0.61, -0.63, 
    -0.65, -0.67, -0.69, -0.71, -0.73, -0.75, -0.77, -0.79, -0.81, -0.83, 
    -0.85, -0.87, -0.89, -0.91, -0.93, -0.95 };
 
  double p7011_d3x1y3_yval[] = { 30.136, 24.391, 19.308, 15.567, 12.172, 9.903, 7.883, 6.005, 4.613, 
    3.468, 2.427, 1.9, 1.496, 1.113, 0.772, 0.548, 0.394, 0.294, 0.24, 
    0.193, 0.181, 0.121, 0.165, 0.15, 0.174, 0.216, 0.222, 0.238, 0.275, 
    0.265, 0.262, 0.266, 0.283, 0.284, 0.279, 0.287, 0.275, 0.277, 0.291, 
    0.291, 0.254, 0.228, 0.243, 0.226, 0.211, 0.205, 0.181, 0.173, 0.158, 
    0.159, 0.147, 0.128, 0.138, 0.131, 0.105, 0.101, 0.094, 0.088, 0.09, 
    0.076, 0.058, 0.072, 0.066, 0.053, 0.065, 0.054, 0.057, 0.046, 0.049, 
    0.048, 0.042, 0.044, 0.047, 0.043, 0.043, 0.037, 0.046, 0.039, 0.044, 
    0.048, 0.048, 0.051, 0.054, 0.048, 0.054, 0.06, 0.066, 0.074, 0.058, 
    0.075, 0.076, 0.09, 0.098, 0.099, 0.094 };
  double p7011_d3x1y3_yerr[] = { 1.23, 0.806, 0.679, 0.486, 0.342, 0.241, 0.206, 0.175, 0.149, 
    0.129, 0.069, 0.058, 0.043, 0.036, 0.029, 0.024, 0.02, 0.017, 0.013, 
    0.011, 0.011, 0.008, 0.01, 0.011, 0.01, 0.011, 0.012, 0.013, 0.014, 
    0.013, 0.012, 0.012, 0.012, 0.012, 0.011, 0.01, 0.01, 0.01, 0.01, 
    0.01, 0.009, 0.009, 0.009, 0.009, 0.008, 0.008, 0.008, 0.007, 0.007, 
    0.007, 0.007, 0.006, 0.006, 0.006, 0.006, 0.005, 0.005, 0.005, 0.005, 
    0.005, 0.004, 0.005, 0.005, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 
    0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.005, 0.004, 0.006, 
    0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 0.006, 0.006, 0.007, 0.006, 
    0.006, 0.007, 0.007, 0.009, 0.008, 0.008 };
  for(int it=0;it<95;it++){ // add systematic 4%
    double system = 0.04*p7011_d3x1y3_yval[it];
    p7011_d3x1y3_yerr[it] = sqrt(p7011_d3x1y3_yerr[it]*p7011_d3x1y3_yerr[it]+system*system);
  }

  //transform data in t and dsig/dt --------
  //step 1: cos(theta_cm) -> t
  for(int it=0;it<95;it++){
    double cosCm = p7011_d3x1y3_xval[it];
    double t_val = t_cosCM(cosCm, plab);
    p7011_d3x1y3_xval[it] = fabs(t_val);
  }
  
  //step 2: dsig/dOmega -> dsig/dt
 for(int it=0;it<95;it++){
   double ds_do = p7011_d3x1y3_yval[it];
   double ds_dt_val = ds_dt(ds_do,plab);
   p7011_d3x1y3_yval[it] = ds_dt_val;

   double err_ds_do = p7011_d3x1y3_yerr[it];
   double err_ds_dt_val = ds_dt(err_ds_do,plab);
   p7011_d3x1y3_yerr[it] = err_ds_dt_val;
 }
  //----------------------------------------
  int p7011_d3x1y3_numpoints = 95;
  expdata = TGraphErrors(p7011_d3x1y3_numpoints, p7011_d3x1y3_xval, p7011_d3x1y3_yval,0,p7011_d3x1y3_yerr);
  return 0;
  }
  if(fabs(plab-1.6)<1e-3){
    //http://durpdg.dur.ac.uk/view/ins109426
    cout<<"Data from EISENHANDLER 1976 — Differential Cross-Sections for anti-Proton-Proton Elastic Scattering Between 0.69-GeV/c and 2.43-GeV/c, added 4% system.error!"<<endl;
    //COS(THETA)	D(SIG)/DOMEGA IN MB/SR
    double p7011_d3x1y4_xval[] = { 0.93, 0.91, 0.89, 0.87, 0.85, 0.83, 0.81, 0.79, 0.77, 
    0.75, 0.73, 0.71, 0.69, 0.67, 0.65, 0.63, 0.61, 0.59, 0.57, 
    0.55, 0.53, 0.51, 0.49, 0.47, 0.45, 0.43, 0.41, 0.39, 0.37, 
    0.35, 0.33, 0.31, 0.29, 0.27, 0.25, 0.23, 0.21, 0.19, 0.17, 
    0.15, 0.13, 0.11, 0.09, 0.07, 0.05, 0.03, 0.01, -0.01, -0.03, 
    -0.05, -0.07, -0.09, -0.11, -0.13, -0.15, -0.17, -0.19, -0.21, -0.23, 
    -0.25, -0.27, -0.29, -0.31, -0.33, -0.35, -0.37, -0.39, -0.41, -0.43, 
    -0.45, -0.47, -0.49, -0.51, -0.53, -0.55, -0.57, -0.59, -0.61, -0.63, 
    -0.65, -0.67, -0.69, -0.71, -0.73, -0.75, -0.77, -0.79, -0.81, -0.83, 
    -0.85, -0.87, -0.89, -0.91, -0.93, -0.95 };
  
  double p7011_d3x1y4_yval[] = { 30.859, 23.892, 20.044, 15.751, 11.829, 8.706, 7.065, 5.163, 3.883, 
    2.66, 2.215, 1.429, 1.073, 0.732, 0.535, 0.398, 0.283, 0.206, 0.181, 
    0.155, 0.167, 0.171, 0.158, 0.188, 0.219, 0.222, 0.242, 0.292, 0.31, 
    0.302, 0.306, 0.282, 0.293, 0.292, 0.296, 0.288, 0.298, 0.284, 0.281, 
    0.283, 0.247, 0.22, 0.222, 0.221, 0.196, 0.198, 0.183, 0.139, 0.136, 
    0.13, 0.111, 0.111, 0.087, 0.092, 0.081, 0.079, 0.067, 0.076, 0.054, 
    0.056, 0.047, 0.056, 0.056, 0.056, 0.054, 0.05, 0.056, 0.046, 0.055, 
    0.047, 0.05, 0.052, 0.045, 0.049, 0.04, 0.044, 0.045, 0.051, 0.046, 
    0.049, 0.046, 0.049, 0.05, 0.061, 0.052, 0.054, 0.067, 0.059, 0.067, 
    0.08, 0.07, 0.08, 0.095, 0.104, 0.11 };
  double p7011_d3x1y4_yerr[] = { 1.616, 1.136, 1.048, 0.807, 0.574, 0.356, 0.278, 0.224, 0.199, 
    0.152, 0.134, 0.051, 0.037, 0.031, 0.026, 0.023, 0.019, 0.015, 0.013, 
    0.011, 0.011, 0.011, 0.009, 0.01, 0.012, 0.012, 0.013, 0.014, 0.015, 
    0.015, 0.015, 0.014, 0.015, 0.014, 0.014, 0.012, 0.012, 0.012, 0.012, 
    0.012, 0.011, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.008, 0.008, 
    0.007, 0.007, 0.007, 0.006, 0.006, 0.006, 0.005, 0.005, 0.005, 0.005, 
    0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 
    0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 0.007, 0.006, 0.005, 0.005, 
    0.005, 0.005, 0.005, 0.005, 0.006, 0.005, 0.006, 0.007, 0.006, 0.006, 
    0.009, 0.008, 0.009, 0.009, 0.008, 0.008 };
  for(int it=0;it<95;it++){ // add systematic 4%
    double system = 0.04*p7011_d3x1y4_yval[it];
    p7011_d3x1y4_yerr[it] = sqrt(p7011_d3x1y4_yerr[it]*p7011_d3x1y4_yerr[it]+system*system);
  }

  //transform data in t and dsig/dt --------
  //step 1: cos(theta_cm) -> t
  for(int it=0;it<95;it++){
    double cosCm = p7011_d3x1y4_xval[it];
    double t_val = t_cosCM(cosCm, plab);
    p7011_d3x1y4_xval[it] = fabs(t_val);
  }
  
  //step 2: dsig/dOmega -> dsig/dt
 for(int it=0;it<95;it++){
   double ds_do = p7011_d3x1y4_yval[it];
   double ds_dt_val = ds_dt(ds_do,plab);
   p7011_d3x1y4_yval[it] = ds_dt_val;

   double err_ds_do = p7011_d3x1y4_yerr[it];
   double err_ds_dt_val = ds_dt(err_ds_do,plab);
   p7011_d3x1y4_yerr[it] = err_ds_dt_val;
 }
  //----------------------------------------

  int p7011_d3x1y4_numpoints = 95;
  expdata = TGraphErrors(p7011_d3x1y4_numpoints, p7011_d3x1y4_xval, p7011_d3x1y4_yval,0, p7011_d3x1y4_yerr);
  return 0;
  }

  if(fabs(plab-1.71)<1e-3){
    //http://durpdg.dur.ac.uk/view/ins109426
    cout<<"Data from EISENHANDLER 1976 — Differential Cross-Sections for anti-Proton-Proton Elastic Scattering Between 0.69-GeV/c and 2.43-GeV/c, added 4% system.error!"<<endl;
    //COS(THETA)	D(SIG)/DOMEGA IN MB/SR
 // Plot: p7011_d4x1y4
  double p7011_d4x1y1_xval[] = { 0.93, 0.91, 0.89, 0.87, 0.85, 0.83, 0.81, 0.79, 0.77, 
    0.75, 0.73, 0.71, 0.69, 0.67, 0.65, 0.63, 0.61, 0.59, 0.57, 
    0.55, 0.53, 0.51, 0.49, 0.47, 0.45, 0.43, 0.41, 0.39, 0.37, 
    0.35, 0.33, 0.31, 0.29, 0.27, 0.25, 0.23, 0.21, 0.19, 0.17, 
    0.15, 0.13, 0.11, 0.09, 0.07, 0.05, 0.03, 0.01, -0.01, -0.03, 
    -0.05, -0.07, -0.09, -0.11, -0.13, -0.15, -0.17, -0.19, -0.21, -0.23, 
    -0.25, -0.27, -0.29, -0.31, -0.33, -0.35, -0.37, -0.39, -0.41, -0.43, 
    -0.45, -0.47, -0.49, -0.51, -0.53, -0.55, -0.57, -0.59, -0.61, -0.63, 
    -0.65, -0.67, -0.69, -0.71, -0.73, -0.75, -0.77, -0.79, -0.81, -0.83, 
    -0.85, -0.87, -0.89, -0.91, -0.93, -0.95 };
 
  double p7011_d4x1y1_yval[] = { 30.287, 24.091, 17.651, 13.896, 10.373, 8.365, 5.964, 4.515, 3.081, 
    2.177, 1.413, 1.107, 0.749, 0.503, 0.301, 0.28, 0.231, 0.195, 0.195, 
    0.175, 0.193, 0.194, 0.238, 0.258, 0.249, 0.283, 0.299, 0.297, 0.339, 
    0.342, 0.303, 0.349, 0.308, 0.317, 0.31, 0.286, 0.279, 0.301, 0.271, 
    0.254, 0.239, 0.224, 0.221, 0.186, 0.178, 0.177, 0.153, 0.135, 0.134, 
    0.099, 0.096, 0.086, 0.082, 0.067, 0.074, 0.056, 0.058, 0.055, 0.05, 
    0.042, 0.049, 0.043, 0.052, 0.047, 0.043, 0.048, 0.049, 0.05, 0.043, 
    0.053, 0.045, 0.052, 0.053, 0.055, 0.056, 0.053, 0.052, 0.041, 0.044, 
    0.055, 0.051, 0.051, 0.051, 0.055, 0.046, 0.049, 0.061, 0.054, 0.058, 
    0.069, 0.056, 0.071, 0.092, 0.092, 0.083 };
  double p7011_d4x1y1_yerr[] = { 1.583, 1.128, 0.923, 0.738, 0.536, 0.373, 0.251, 0.203, 0.163, 
    0.133, 0.105, 0.092, 0.074, 0.06, 0.044, 0.042, 0.04, 0.028, 0.023, 
    0.011, 0.012, 0.013, 0.011, 0.011, 0.012, 0.013, 0.013, 0.013, 0.015, 
    0.015, 0.015, 0.017, 0.016, 0.016, 0.012, 0.011, 0.011, 0.011, 0.01, 
    0.01, 0.01, 0.009, 0.009, 0.008, 0.008, 0.008, 0.008, 0.007, 0.007, 
    0.006, 0.006, 0.005, 0.005, 0.004, 0.005, 0.004, 0.004, 0.004, 0.005, 
    0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 
    0.004, 0.004, 0.005, 0.004, 0.005, 0.005, 0.005, 0.006, 0.004, 0.004, 
    0.004, 0.004, 0.005, 0.005, 0.005, 0.005, 0.005, 0.006, 0.007, 0.007, 
    0.008, 0.007, 0.007, 0.008, 0.007, 0.007 };
  for(int it=0;it<95;it++){ // add systematic 4%
    double system = 0.04*p7011_d4x1y1_yval[it];
    p7011_d4x1y1_yerr[it] = sqrt(p7011_d4x1y1_yerr[it]*p7011_d4x1y1_yerr[it]+system*system);
  }

  //transform data in t and dsig/dt --------
  //step 1: cos(theta_cm) -> t
  for(int it=0;it<95;it++){
    double cosCm = p7011_d4x1y1_xval[it];
    double t_val = t_cosCM(cosCm, plab);
    p7011_d4x1y1_xval[it] = fabs(t_val);
  }
  
  //step 2: dsig/dOmega -> dsig/dt
 for(int it=0;it<95;it++){
   double ds_do = p7011_d4x1y1_yval[it];
   double ds_dt_val = ds_dt(ds_do,plab);
   p7011_d4x1y1_yval[it] = ds_dt_val;

   double err_ds_do = p7011_d4x1y1_yerr[it];
   double err_ds_dt_val = ds_dt(err_ds_do,plab);
   p7011_d4x1y1_yerr[it] = err_ds_dt_val;
 }
  //----------------------------------------

  int p7011_d4x1y1_numpoints = 95;
  expdata =  TGraphErrors(p7011_d4x1y1_numpoints, p7011_d4x1y1_xval, p7011_d4x1y1_yval, 0, p7011_d4x1y1_yerr);
   
    return 0;
  }
  if(fabs(plab-1.81)<1e-4){
    //http://durpdg.dur.ac.uk/view/ins109426
    cout<<"Data from EISENHANDLER 1976 — Differential Cross-Sections for anti-Proton-Proton Elastic Scattering Between 0.69-GeV/c and 2.43-GeV/c, added 4% system.error!"<<endl;
    //COS(THETA)	D(SIG)/DOMEGA IN MB/SR
double p7011_d4x1y2_xval[] = { 0.93, 0.91, 0.89, 0.87, 0.85, 0.83, 0.81, 0.79, 0.77, 
    0.75, 0.73, 0.71, 0.69, 0.67, 0.65, 0.63, 0.61, 0.59, 0.57, 
    0.55, 0.53, 0.51, 0.49, 0.47, 0.45, 0.43, 0.41, 0.39, 0.37, 
    0.35, 0.33, 0.31, 0.29, 0.27, 0.25, 0.23, 0.21, 0.19, 0.17, 
    0.15, 0.13, 0.11, 0.09, 0.07, 0.05, 0.03, 0.01, -0.01, -0.03, 
    -0.05, -0.07, -0.09, -0.11, -0.13, -0.15, -0.17, -0.19, -0.21, -0.23, 
    -0.25, -0.27, -0.29, -0.31, -0.33, -0.35, -0.37, -0.39, -0.41, -0.43, 
    -0.45, -0.47, -0.49, -0.51, -0.53, -0.55, -0.57, -0.59, -0.61, -0.63, 
    -0.65, -0.67, -0.69, -0.71, -0.73, -0.75, -0.77, -0.79, -0.81, -0.83, 
    -0.85, -0.87, -0.89, -0.91, -0.93, -0.95 };
 
  double p7011_d4x1y2_yval[] = { 30.067, 22.912, 17.334, 12.95, 8.843, 6.805, 4.602, 3.36, 2.375, 
    1.63, 1.069, 0.887, 0.473, 0.459, 0.24, 0.207, 0.226, 0.145, 0.25, 
    0.219, 0.24, 0.269, 0.276, 0.298, 0.334, 0.323, 0.312, 0.324, 0.337, 
    0.33, 0.333, 0.315, 0.331, 0.294, 0.292, 0.284, 0.282, 0.248, 0.247, 
    0.218, 0.208, 0.208, 0.193, 0.169, 0.149, 0.142, 0.137, 0.122, 0.113, 
    0.097, 0.085, 0.075, 0.07, 0.065, 0.054, 0.055, 0.049, 0.047, 0.041, 
    0.047, 0.04, 0.041, 0.051, 0.039, 0.046, 0.043, 0.04, 0.049, 0.051, 
    0.045, 0.049, 0.05, 0.047, 0.047, 0.052, 0.052, 0.051, 0.044, 0.045, 
    0.048, 0.046, 0.049, 0.052, 0.039, 0.038, 0.042, 0.035, 0.049, 0.06, 
    0.062, 0.088, 0.068, 0.062, 0.073, 0.075 };
  double p7011_d4x1y2_yerr[] = { 1.594, 1.104, 0.919, 0.731, 0.496, 0.328, 0.228, 0.17, 0.143, 
    0.116, 0.092, 0.084, 0.059, 0.061, 0.044, 0.038, 0.038, 0.023, 0.032, 
    0.013, 0.011, 0.013, 0.012, 0.013, 0.015, 0.015, 0.011, 0.011, 0.011, 
    0.011, 0.011, 0.011, 0.012, 0.011, 0.011, 0.011, 0.011, 0.01, 0.01, 
    0.009, 0.009, 0.009, 0.009, 0.007, 0.006, 0.006, 0.006, 0.006, 0.005, 
    0.005, 0.005, 0.004, 0.004, 0.004, 0.004, 0.004, 0.003, 0.004, 0.003, 
    0.004, 0.004, 0.004, 0.004, 0.003, 0.004, 0.004, 0.004, 0.004, 0.004, 
    0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.003, 0.003, 0.003, 
    0.003, 0.003, 0.004, 0.004, 0.004, 0.004, 0.005, 0.004, 0.007, 0.008, 
    0.008, 0.01, 0.008, 0.007, 0.007, 0.007 };
  for(int it=0;it<95;it++){ // add systematic 4%
    double system = 0.04*p7011_d4x1y2_yval[it];
    p7011_d4x1y2_yerr[it] = sqrt(p7011_d4x1y2_yerr[it]*p7011_d4x1y2_yerr[it]+system*system);
  }

  //transform data in t and dsig/dt --------
  //step 1: cos(theta_cm) -> t
  for(int it=0;it<95;it++){
    double cosCm = p7011_d4x1y2_xval[it];
    double t_val = t_cosCM(cosCm, plab);
    p7011_d4x1y2_xval[it] = fabs(t_val);
  }
  
  //step 2: dsig/dOmega -> dsig/dt
 for(int it=0;it<95;it++){
   double ds_do = p7011_d4x1y2_yval[it];
   double ds_dt_val = ds_dt(ds_do,plab);
   p7011_d4x1y2_yval[it] = ds_dt_val;

   double err_ds_do = p7011_d4x1y2_yerr[it];
   double err_ds_dt_val = ds_dt(err_ds_do,plab);
   p7011_d4x1y2_yerr[it] = err_ds_dt_val;
 }
  //----------------------------------------
  int p7011_d4x1y2_numpoints = 95;
  expdata = TGraphErrors(p7011_d4x1y2_numpoints, p7011_d4x1y2_xval, p7011_d4x1y2_yval, 0, p7011_d4x1y2_yerr);
   
    return 0;
  }
  if(fabs(plab-1.86)<1e-4){
    //http://durpdg.dur.ac.uk/view/ins109426
    cout<<"Data from EISENHANDLER 1976 — Differential Cross-Sections for anti-Proton-Proton Elastic Scattering Between 0.69-GeV/c and 2.43-GeV/c, added 4% system.error!"<<endl;
    //COS(THETA)	D(SIG)/DOMEGA IN MB/SR
    double p7011_d4x1y3_xval[] = { 0.93, 0.91, 0.89, 0.87, 0.85, 0.83, 0.81, 0.79, 0.77, 
    0.75, 0.73, 0.71, 0.69, 0.67, 0.65, 0.63, 0.61, 0.59, 0.57, 
    0.55, 0.53, 0.51, 0.49, 0.47, 0.45, 0.43, 0.41, 0.39, 0.37, 
    0.35, 0.33, 0.31, 0.29, 0.27, 0.25, 0.23, 0.21, 0.19, 0.17, 
    0.15, 0.13, 0.11, 0.09, 0.07, 0.05, 0.03, 0.01, -0.01, -0.03, 
    -0.05, -0.07, -0.09, -0.11, -0.13, -0.15, -0.17, -0.19, -0.21, -0.23, 
    -0.25, -0.27, -0.29, -0.31, -0.33, -0.35, -0.37, -0.39, -0.41, -0.43, 
    -0.45, -0.47, -0.49, -0.51, -0.53, -0.55, -0.57, -0.59, -0.61, -0.63, 
    -0.65, -0.67, -0.69, -0.71, -0.73, -0.75, -0.77, -0.79, -0.81, -0.83, 
    -0.85, -0.87, -0.89, -0.91, -0.93, -0.95 };
  
  double p7011_d4x1y3_yval[] = { 28.613, 21.835, 16.511, 11.651, 9.085, 6.407, 4.317, 3.13, 2.268, 
    1.491, 0.869, 0.685, 0.391, 0.354, 0.24, 0.188, 0.2, 0.277, 0.26, 
    0.322, 0.282, 0.325, 0.296, 0.317, 0.361, 0.332, 0.356, 0.325, 0.342, 
    0.332, 0.347, 0.328, 0.312, 0.296, 0.271, 0.273, 0.261, 0.243, 0.23, 
    0.211, 0.21, 0.182, 0.159, 0.158, 0.147, 0.133, 0.121, 0.115, 0.098, 
    0.086, 0.074, 0.085, 0.076, 0.064, 0.06, 0.056, 0.05, 0.047, 0.047, 
    0.049, 0.044, 0.047, 0.044, 0.039, 0.047, 0.038, 0.042, 0.044, 0.052, 
    0.047, 0.058, 0.05, 0.048, 0.054, 0.044, 0.05, 0.05, 0.052, 0.046, 
    0.044, 0.04, 0.043, 0.041, 0.038, 0.044, 0.036, 0.034, 0.04, 0.056, 
    0.048, 0.059, 0.052, 0.064, 0.058, 0.069 };
  double p7011_d4x1y3_yerr[] = { 1.433, 0.971, 0.815, 0.596, 0.446, 0.277, 0.202, 0.145, 0.12, 
    0.097, 0.071, 0.061, 0.047, 0.043, 0.035, 0.034, 0.033, 0.034, 0.026, 
    0.026, 0.012, 0.012, 0.012, 0.013, 0.015, 0.014, 0.015, 0.011, 0.012, 
    0.011, 0.012, 0.011, 0.011, 0.011, 0.01, 0.01, 0.01, 0.01, 0.009, 
    0.009, 0.009, 0.009, 0.008, 0.006, 0.006, 0.006, 0.006, 0.005, 0.005, 
    0.005, 0.004, 0.005, 0.005, 0.004, 0.004, 0.004, 0.003, 0.004, 0.004, 
    0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 
    0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.003, 0.003, 
    0.003, 0.003, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.007, 
    0.007, 0.007, 0.006, 0.006, 0.006, 0.005 };
for(int it=0;it<95;it++){ // add systematic 4%
    double system = 0.04*p7011_d4x1y3_yval[it];
    p7011_d4x1y3_yerr[it] = sqrt(p7011_d4x1y3_yerr[it]*p7011_d4x1y3_yerr[it]+system*system);
  }

  //transform data in t and dsig/dt --------
  //step 1: cos(theta_cm) -> t
  for(int it=0;it<95;it++){
    double cosCm = p7011_d4x1y3_xval[it];
    double t_val = t_cosCM(cosCm, plab);
    p7011_d4x1y3_xval[it] = fabs(t_val);
  }
  
  //step 2: dsig/dOmega -> dsig/dt
 for(int it=0;it<95;it++){
   double ds_do = p7011_d4x1y3_yval[it];
   double ds_dt_val = ds_dt(ds_do,plab);
   p7011_d4x1y3_yval[it] = ds_dt_val;

   double err_ds_do = p7011_d4x1y3_yerr[it];
   double err_ds_dt_val = ds_dt(err_ds_do,plab);
   p7011_d4x1y3_yerr[it] = err_ds_dt_val;
 }
  //----------------------------------------
  int p7011_d4x1y3_numpoints = 95;
  expdata = TGraphErrors(p7011_d4x1y3_numpoints, p7011_d4x1y3_xval, p7011_d4x1y3_yval,0,p7011_d4x1y3_yerr);
  return 0;
  }
 if(fabs(plab-1.91)<1e-4){
   //http://durpdg.dur.ac.uk/view/ins109426
   cout<<"Data from EISENHANDLER 1976 — Differential Cross-Sections for anti-Proton-Proton Elastic Scattering Between 0.69-GeV/c and 2.43-GeV/c, added 4% system.error!"<<endl;
    //COS(THETA)	D(SIG)/DOMEGA IN MB/SR
 double p7011_d4x1y4_xval[] = { 0.93, 0.91, 0.89, 0.87, 0.85, 0.83, 0.81, 0.79, 0.77, 
    0.75, 0.73, 0.71, 0.69, 0.67, 0.65, 0.63, 0.61, 0.59, 0.57, 
    0.55, 0.53, 0.51, 0.49, 0.47, 0.45, 0.43, 0.41, 0.39, 0.37, 
    0.35, 0.33, 0.31, 0.29, 0.27, 0.25, 0.23, 0.21, 0.19, 0.17, 
    0.15, 0.13, 0.11, 0.09, 0.07, 0.05, 0.03, 0.01, -0.01, -0.03, 
    -0.05, -0.07, -0.09, -0.11, -0.13, -0.15, -0.17, -0.19, -0.21, -0.23, 
    -0.25, -0.27, -0.29, -0.31, -0.33, -0.35, -0.37, -0.39, -0.41, -0.43, 
    -0.45, -0.47, -0.49, -0.51, -0.53, -0.55, -0.57, -0.59, -0.61, -0.63, 
    -0.65, -0.67, -0.69, -0.71, -0.73, -0.75, -0.77, -0.79, -0.81, -0.83, 
    -0.85, -0.87, -0.89, -0.91, -0.93, -0.95 };
 double p7011_d4x1y4_yval[] = { 29.232, 21.118, 14.56, 11.209, 7.035, 5.169, 3.81, 2.821, 1.888, 
    1.233, 0.68, 0.49, 0.294, 0.284, 0.187, 0.15, 0.224, 0.285, 0.245, 
    0.354, 0.298, 0.324, 0.329, 0.353, 0.345, 0.384, 0.359, 0.368, 0.348, 
    0.332, 0.342, 0.325, 0.299, 0.285, 0.274, 0.251, 0.235, 0.226, 0.209, 
    0.196, 0.193, 0.179, 0.17, 0.134, 0.135, 0.122, 0.114, 0.114, 0.099, 
    0.081, 0.076, 0.078, 0.066, 0.062, 0.054, 0.058, 0.054, 0.053, 0.05, 
    0.045, 0.046, 0.05, 0.038, 0.042, 0.048, 0.045, 0.041, 0.05, 0.044, 
    0.054, 0.051, 0.052, 0.053, 0.046, 0.051, 0.052, 0.043, 0.048, 0.044, 
    0.046, 0.045, 0.046, 0.039, 0.046, 0.043, 0.038, 0.041, 0.03, 0.049, 
    0.022, 0.069, 0.04, 0.042, 0.062, 0.055 };
  double p7011_d4x1y4_yerr[] = { 1.594, 1.052, 0.839, 0.668, 0.455, 0.307, 0.217, 0.168, 0.132, 
    0.106, 0.082, 0.065, 0.052, 0.047, 0.039, 0.039, 0.051, 0.043, 0.034, 
    0.037, 0.015, 0.015, 0.013, 0.015, 0.015, 0.016, 0.016, 0.017, 0.012, 
    0.012, 0.012, 0.012, 0.011, 0.011, 0.011, 0.01, 0.01, 0.01, 0.009, 
    0.009, 0.009, 0.009, 0.008, 0.006, 0.006, 0.006, 0.006, 0.006, 0.005, 
    0.005, 0.005, 0.005, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 
    0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 
    0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.003, 0.003, 0.003, 
    0.003, 0.003, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.007, 0.008, 
    0.007, 0.009, 0.007, 0.006, 0.007, 0.006 };
 for(int it=0;it<95;it++){ // add systematic 4%
    double system = 0.04*p7011_d4x1y4_yval[it];
    p7011_d4x1y4_yerr[it] = sqrt(p7011_d4x1y4_yerr[it]*p7011_d4x1y4_yerr[it]+system*system);
  }

  //transform data in t and dsig/dt --------
  //step 1: cos(theta_cm) -> t
  for(int it=0;it<95;it++){
    double cosCm = p7011_d4x1y4_xval[it];
    double t_val = t_cosCM(cosCm, plab);
    p7011_d4x1y4_xval[it] = fabs(t_val);
  }
  
  //step 2: dsig/dOmega -> dsig/dt
 for(int it=0;it<95;it++){
   double ds_do = p7011_d4x1y4_yval[it];
   double ds_dt_val = ds_dt(ds_do,plab);
   p7011_d4x1y4_yval[it] = ds_dt_val;

   double err_ds_do = p7011_d4x1y4_yerr[it];
   double err_ds_dt_val = ds_dt(err_ds_do,plab);
   p7011_d4x1y4_yerr[it] = err_ds_dt_val;
 }
  //----------------------------------------
  int p7011_d4x1y4_numpoints = 95;
  expdata = TGraphErrors(p7011_d4x1y4_numpoints, p7011_d4x1y4_xval, p7011_d4x1y4_yval,0,p7011_d4x1y4_yerr);
  return 0;
  }
 if(fabs(plab-2.01)<1e-4){
 //http://durpdg.dur.ac.uk/view/ins109426
   cout<<"Data from EISENHANDLER 1976 — Differential Cross-Sections for anti-Proton-Proton Elastic Scattering Between 0.69-GeV/c and 2.43-GeV/c, added 4% system.error!"<<endl;
    //COS(THETA)	D(SIG)/DOMEGA IN MB/SR
 double p7011_d5x1y1_xval[] = { 0.93, 0.91, 0.89, 0.87, 0.85, 0.83, 0.81, 0.79, 0.77, 
    0.75, 0.73, 0.71, 0.69, 0.67, 0.65, 0.63, 0.61, 0.59, 0.57, 
    0.55, 0.53, 0.51, 0.49, 0.47, 0.45, 0.43, 0.41, 0.39, 0.31, 
    0.35, 0.33, 0.31, 0.29, 0.27, 0.25, 0.23, 0.21, 0.19, 0.17, 
    0.15, 0.13, 0.11, 0.09, 0.07, 0.05, 0.03, 0.01, -0.01, -0.03, 
    -0.05, -0.07, -0.09, -0.11, -0.13, -0.15, -0.17, -0.19, -0.21, -0.23, 
    -0.25, -0.27, -0.29, -0.31, -0.33, -0.35, -0.37, -0.39, -0.41, -0.43, 
    -0.45, -0.47, -0.49, -0.51, -0.53, -0.55, -0.57, -0.59, -0.61, -0.63, 
    -0.65, -0.67, -0.69, -0.71, -0.73, -0.75, -0.77, -0.79, -0.81, -0.83, 
    -0.85, -0.87, -0.89, -0.91, -0.93, -0.95 };
  double p7011_d5x1y1_yval[] = { 24.984, 19.125, 13.546, 10.311, 7.07, 4.783, 3.276, 2.413, 1.432, 
    0.906, 0.55, 0.481, 0.227, 0.189, 0.213, 0.241, 0.273, 0.271, 0.268, 
    0.285, 0.314, 0.365, 0.368, 0.366, 0.388, 0.383, 0.364, 0.354, 0.324, 
    0.352, 0.316, 0.311, 0.301, 0.27, 0.251, 0.213, 0.219, 0.199, 0.18, 
    0.173, 0.157, 0.13, 0.127, 0.127, 0.102, 0.105, 0.104, 0.085, 0.09, 
    0.069, 0.07, 0.061, 0.058, 0.06, 0.054, 0.054, 0.045, 0.04, 0.056, 
    0.052, 0.047, 0.057, 0.045, 0.04, 0.04, 0.044, 0.048, 0.048, 0.045, 
    0.036, 0.048, 0.047, 0.052, 0.042, 0.045, 0.046, 0.034, 0.039, 0.044, 
    0.039, 0.04, 0.037, 0.036, 0.039, 0.035, 0.027, 0.028, 0.024, 0.032, 
    0.023, 0.025, 0.039, 0.032, 0.05, 0.042 };
  double p7011_d5x1y1_yerr[] = { 1.061, 0.658, 0.507, 0.372, 0.249, 0.172, 0.131, 0.109, 0.084, 
    0.064, 0.047, 0.046, 0.031, 0.027, 0.03, 0.031, 0.033, 0.035, 0.032, 
    0.017, 0.016, 0.014, 0.014, 0.014, 0.016, 0.016, 0.016, 0.014, 0.013, 
    0.015, 0.014, 0.014, 0.014, 0.013, 0.012, 0.011, 0.011, 0.011, 0.01, 
    0.01, 0.01, 0.009, 0.008, 0.008, 0.007, 0.007, 0.007, 0.006, 0.007, 
    0.006, 0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 
    0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 
    0.004, 0.005, 0.005, 0.005, 0.004, 0.005, 0.004, 0.003, 0.004, 0.004, 
    0.003, 0.003, 0.003, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 
    0.005, 0.006, 0.006, 0.005, 0.006, 0.006 };
  for(int it=0;it<95;it++){ // add systematic 4%
    double system = 0.04*p7011_d5x1y1_yval[it];
    p7011_d5x1y1_yerr[it] = sqrt(p7011_d5x1y1_yerr[it]*p7011_d5x1y1_yerr[it]+system*system);
  }

  //transform data in t and dsig/dt --------
  //step 1: cos(theta_cm) -> t
  for(int it=0;it<95;it++){
    double cosCm = p7011_d5x1y1_xval[it];
    double t_val = t_cosCM(cosCm, plab);
    p7011_d5x1y1_xval[it] = fabs(t_val);
  }
  
  //step 2: dsig/dOmega -> dsig/dt
 for(int it=0;it<95;it++){
   double ds_do = p7011_d5x1y1_yval[it];
   double ds_dt_val = ds_dt(ds_do,plab);
   p7011_d5x1y1_yval[it] = ds_dt_val;

   double err_ds_do = p7011_d5x1y1_yerr[it];
   double err_ds_dt_val = ds_dt(err_ds_do,plab);
   p7011_d5x1y1_yerr[it] = err_ds_dt_val;
 }
  //----------------------------------------
  int p7011_d5x1y1_numpoints = 95;
  expdata = TGraphErrors(p7011_d5x1y1_numpoints, p7011_d5x1y1_xval, p7011_d5x1y1_yval,0,p7011_d5x1y1_yerr);
    
    return 0;
  }
 if(fabs(plab-2.12)<1e-4){
 //http://durpdg.dur.ac.uk/view/ins109426
   cout<<"Data from EISENHANDLER 1976 — Differential Cross-Sections for anti-Proton-Proton Elastic Scattering Between 0.69-GeV/c and 2.43-GeV/c, added 4% system.error!"<<endl;
    //COS(THETA)	D(SIG)/DOMEGA IN MB/SR
   double p7011_d5x1y2_xval[] = { 0.93, 0.91, 0.89, 0.87, 0.85, 0.83, 0.81, 0.79, 0.77, 
    0.75, 0.73, 0.71, 0.69, 0.67, 0.65, 0.63, 0.61, 0.59, 0.57, 
    0.55, 0.53, 0.51, 0.49, 0.47, 0.45, 0.43, 0.41, 0.39, 0.31, 
    0.35, 0.33, 0.31, 0.29, 0.27, 0.25, 0.23, 0.21, 0.19, 0.17, 
    0.15, 0.13, 0.11, 0.09, 0.07, 0.05, 0.03, 0.01, -0.01, -0.03, 
    -0.05, -0.07, -0.09, -0.11, -0.13, -0.15, -0.17, -0.19, -0.21, -0.23, 
    -0.25, -0.27, -0.29, -0.31, -0.33, -0.35, -0.37, -0.39, -0.41, -0.43, 
    -0.45, -0.47, -0.49, -0.51, -0.53, -0.55, -0.57, -0.59, -0.61, -0.63, 
    -0.65, -0.67, -0.69, -0.71, -0.73, -0.75, -0.77, -0.79, -0.81, -0.83, 
    -0.85, -0.87, -0.89, -0.91, -0.93, -0.95 };
  
  double p7011_d5x1y2_yval[] = { 25.224, 18.915, 12.743, 8.447, 5.994, 4.233, 2.56, 1.287, 0.967, 
    0.626, 0.383, 0.309, 0.274, 0.22, 0.19, 0.203, 0.355, 0.323, 0.414, 
    0.464, 0.338, 0.373, 0.373, 0.384, 0.403, 0.369, 0.356, 0.356, 0.34, 
    0.307, 0.275, 0.287, 0.279, 0.236, 0.201, 0.193, 0.183, 0.164, 0.156, 
    0.156, 0.13, 0.119, 0.11, 0.097, 0.09, 0.083, 0.084, 0.076, 0.07, 
    0.075, 0.065, 0.065, 0.07, 0.07, 0.057, 0.052, 0.068, 0.056, 0.056, 
    0.055, 0.049, 0.058, 0.05, 0.055, 0.05, 0.049, 0.058, 0.053, 0.045, 
    0.046, 0.046, 0.038, 0.044, 0.041, 0.048, 0.03, 0.033, 0.035, 0.035, 
    0.036, 0.03, 0.031, 0.027, 0.028, 0.031, 0.025, 0.026, 0.037, 0.024, 
    0.035, 0.027, 0.013, 0.018, 0.03, 0.027 };
  double p7011_d5x1y2_yerr[] = { 1.43, 0.967, 0.748, 0.549, 0.408, 0.291, 0.201, 0.172, 0.102, 
    0.077, 0.061, 0.052, 0.047, 0.04, 0.042, 0.055, 0.055, 0.05, 0.053, 
    0.054, 0.033, 0.013, 0.012, 0.013, 0.014, 0.014, 0.014, 0.015, 0.014, 
    0.012, 0.011, 0.011, 0.011, 0.01, 0.009, 0.009, 0.009, 0.008, 0.008, 
    0.008, 0.007, 0.007, 0.007, 0.006, 0.005, 0.005, 0.005, 0.005, 0.005, 
    0.005, 0.005, 0.004, 0.005, 0.005, 0.004, 0.005, 0.005, 0.004, 0.004, 
    0.004, 0.005, 0.005, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 
    0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.003, 0.003, 0.003, 0.003, 
    0.003, 0.003, 0.004, 0.003, 0.004, 0.004, 0.003, 0.005, 0.006, 0.006, 
    0.006, 0.006, 0.006, 0.006, 0.005, 0.006 };
   for(int it=0;it<95;it++){ // add systematic 4%
    double system = 0.04*p7011_d5x1y2_yval[it];
    p7011_d5x1y2_yerr[it] = sqrt(p7011_d5x1y2_yerr[it]*p7011_d5x1y2_yerr[it]+system*system);
  }

  //transform data in t and dsig/dt --------
  //step 1: cos(theta_cm) -> t
  for(int it=0;it<95;it++){
    double cosCm = p7011_d5x1y2_xval[it];
    double t_val = t_cosCM(cosCm, plab);
    p7011_d5x1y2_xval[it] = fabs(t_val);
  }
  
  //step 2: dsig/dOmega -> dsig/dt
 for(int it=0;it<95;it++){
   double ds_do = p7011_d5x1y2_yval[it];
   double ds_dt_val = ds_dt(ds_do,plab);
   p7011_d5x1y2_yval[it] = ds_dt_val;

   double err_ds_do = p7011_d5x1y2_yerr[it];
   double err_ds_dt_val = ds_dt(err_ds_do,plab);
   p7011_d5x1y2_yerr[it] = err_ds_dt_val;
 }
  //----------------------------------------
  int p7011_d5x1y2_numpoints = 95;
  expdata = TGraphErrors(p7011_d5x1y2_numpoints, p7011_d5x1y2_xval, p7011_d5x1y2_yval,0, p7011_d5x1y2_yerr);
   
    return 0;
  }
 if(fabs(plab-2.23)<1e-4){
   //http://durpdg.dur.ac.uk/view/ins109426
   cout<<"Data from EISENHANDLER 1976 — Differential Cross-Sections for anti-Proton-Proton Elastic Scattering Between 0.69-GeV/c and 2.43-GeV/c, added 4% system.error!"<<endl;
    //COS(THETA)	D(SIG)/DOMEGA IN MB/SR
double p7011_d5x1y3_xval[] = { 0.93, 0.91, 0.89, 0.87, 0.85, 0.83, 0.81, 0.79, 0.77, 
    0.75, 0.73, 0.71, 0.69, 0.67, 0.65, 0.63, 0.61, 0.59, 0.57, 
    0.55, 0.53, 0.51, 0.49, 0.47, 0.45, 0.43, 0.41, 0.39, 0.31, 
    0.35, 0.33, 0.31, 0.29, 0.27, 0.25, 0.23, 0.21, 0.19, 0.17, 
    0.15, 0.13, 0.11, 0.09, 0.07, 0.05, 0.03, 0.01, -0.01, -0.03, 
    -0.05, -0.07, -0.09, -0.11, -0.13, -0.15, -0.17, -0.19, -0.21, -0.23, 
    -0.25, -0.27, -0.29, -0.31, -0.33, -0.35, -0.37, -0.39, -0.41, -0.43, 
    -0.45, -0.47, -0.49, -0.51, -0.53, -0.55, -0.57, -0.59, -0.61, -0.63, 
    -0.65, -0.67, -0.69, -0.71, -0.73, -0.75, -0.77 };
 
  double p7011_d5x1y3_yval[] = { 23.799, 16.618, 11.649, 7.065, 5.093, 3.017, 1.726, 1.132, 0.703, 
    0.436, 0.361, 0.22, 0.28, 0.266, 0.272, 0.316, 0.389, 0.389, 0.471, 
    0.443, 0.387, 0.42, 0.393, 0.423, 0.334, 0.389, 0.324, 0.353, 0.314, 
    0.264, 0.262, 0.245, 0.227, 0.215, 0.196, 0.175, 0.144, 0.142, 0.115, 
    0.104, 0.114, 0.096, 0.095, 0.087, 0.077, 0.073, 0.073, 0.073, 0.063, 
    0.059, 0.062, 0.062, 0.06, 0.052, 0.052, 0.053, 0.058, 0.057, 0.056, 
    0.052, 0.058, 0.059, 0.048, 0.054, 0.057, 0.05, 0.048, 0.048, 0.045, 
    0.055, 0.042, 0.035, 0.034, 0.041, 0.028, 0.031, 0.031, 0.028, 0.032, 
    0.026, 0.023, 0.028, 0.037, 0.035, 0.03, 0.036 };
  double p7011_d5x1y3_yerr[] = { 1.213, 0.741, 0.586, 0.386, 0.284, 0.186, 0.121, 0.09, 0.057, 
    0.044, 0.042, 0.032, 0.033, 0.033, 0.033, 0.035, 0.04, 0.041, 0.044, 
    0.037, 0.026, 0.026, 0.021, 0.023, 0.022, 0.029, 0.028, 0.041, 0.046, 
    0.013, 0.012, 0.012, 0.011, 0.011, 0.01, 0.01, 0.009, 0.009, 0.008, 
    0.007, 0.008, 0.005, 0.005, 0.005, 0.005, 0.005, 0.004, 0.004, 0.004, 
    0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.005, 0.004, 
    0.004, 0.005, 0.005, 0.004, 0.005, 0.005, 0.005, 0.004, 0.004, 0.004, 
    0.005, 0.004, 0.004, 0.004, 0.004, 0.003, 0.003, 0.003, 0.002, 0.003, 
    0.002, 0.002, 0.004, 0.005, 0.005, 0.005, 0.005 };
 for(int it=0;it<95;it++){ // add systematic 4%
    double system = 0.04*p7011_d5x1y3_yval[it];
    p7011_d5x1y3_yerr[it] = sqrt(p7011_d5x1y3_yerr[it]*p7011_d5x1y3_yerr[it]+system*system);
  }

  //transform data in t and dsig/dt --------
  //step 1: cos(theta_cm) -> t
  for(int it=0;it<95;it++){
    double cosCm = p7011_d5x1y3_xval[it];
    double t_val = t_cosCM(cosCm, plab);
    p7011_d5x1y3_xval[it] = fabs(t_val);
  }
  
  //step 2: dsig/dOmega -> dsig/dt
 for(int it=0;it<95;it++){
   double ds_do = p7011_d5x1y3_yval[it];
   double ds_dt_val = ds_dt(ds_do,plab);
   p7011_d5x1y3_yval[it] = ds_dt_val;

   double err_ds_do = p7011_d5x1y3_yerr[it];
   double err_ds_dt_val = ds_dt(err_ds_do,plab);
   p7011_d5x1y3_yerr[it] = err_ds_dt_val;
 }
  //----------------------------------------
 //  int p7011_d5x1y3_numpoints = 95;
  int p7011_d5x1y3_numpoints = 86;
  expdata = TGraphErrors(p7011_d5x1y3_numpoints, p7011_d5x1y3_xval, p7011_d5x1y3_yval, 0, p7011_d5x1y3_yerr);
   
    return 0;
  }

 if(fabs(plab-2.43)<1e-4){
 //http://durpdg.dur.ac.uk/view/ins109426
   cout<<"Data from EISENHANDLER 1976 — Differential Cross-Sections for anti-Proton-Proton Elastic Scattering Between 0.69-GeV/c and 2.43-GeV/c, added 4% system.error!"<<endl;
    //COS(THETA)	D(SIG)/DOMEGA IN MB/SR
  double p7011_d6x1y1_xval[] = { 0.93, 0.91, 0.89, 0.87, 0.85, 0.83, 0.81, 0.79, 0.77, 
    0.75, 0.73, 0.71, 0.69, 0.67, 0.65, 0.63, 0.61, 0.59, 0.57, 
    0.55, 0.53, 0.51, 0.49, 0.47, 0.45, 0.43, 0.41, 0.39, 0.37, 
    0.35, 0.33, 0.31, 0.29, 0.27, 0.25, 0.23, 0.21, 0.19, 0.17, 
    0.15, 0.13, 0.11, 0.09, 0.07, 0.05, 0.03, 0.01, -0.01, -0.03, 
    -0.05, -0.07, -0.09, -0.11, -0.13, -0.15, -0.17, -0.19, -0.21, -0.23, 
    -0.25, -0.27, -0.29, -0.31, -0.33, -0.35, -0.37, -0.39, -0.41, -0.43, 
    -0.45, -0.47, -0.49, -0.51, -0.53, -0.55, -0.57, -0.59, -0.61, -0.63, 
    -0.65, -0.67 };

  double p7011_d6x1y1_yval[] = { 27.443, 14.831, 8.968, 5.274, 3.837, 2.074, 1.324, 0.764, 0.432, 
    0.284, 0.166, 0.257, 0.286, 0.306, 0.33, 0.397, 0.265, 0.415, 0.37, 
    0.361, 0.37, 0.408, 0.371, 0.329, 0.347, 0.352, 0.315, 0.24, 0.272, 
    0.201, 0.178, 0.164, 0.155, 0.143, 0.133, 0.123, 0.116, 0.105, 0.09, 
    0.082, 0.086, 0.084, 0.072, 0.074, 0.067, 0.064, 0.061, 0.057, 0.06, 
    0.061, 0.055, 0.058, 0.055, 0.058, 0.048, 0.054, 0.046, 0.054, 0.048, 
    0.045, 0.046, 0.049, 0.049, 0.049, 0.048, 0.042, 0.044, 0.041, 0.036, 
    0.035, 0.031, 0.031, 0.024, 0.025, 0.024, 0.022, 0.02, 0.012, 0.017, 
    0.017, 0.015 };
  double p7011_d6x1y1_yerr[] = { 1.128, 0.667, 0.476, 0.304, 0.239, 0.133, 0.087, 0.058, 0.046, 
    0.033, 0.033, 0.033, 0.032, 0.034, 0.037, 0.039, 0.033, 0.042, 0.035, 
    0.029, 0.024, 0.02, 0.021, 0.021, 0.027, 0.03, 0.035, 0.04, 0.059, 
    0.01, 0.008, 0.007, 0.007, 0.007, 0.006, 0.006, 0.006, 0.006, 0.005, 
    0.005, 0.005, 0.004, 0.004, 0.004, 0.003, 0.003, 0.003, 0.003, 0.003, 
    0.003, 0.003, 0.003, 0.003, 0.003, 0.003, 0.003, 0.003, 0.003, 0.004, 
    0.003, 0.003, 0.004, 0.004, 0.004, 0.004, 0.003, 0.003, 0.003, 0.003, 
    0.003, 0.003, 0.003, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 
    0.002, 0.003 };
  for(int it=0;it<94;it++){ // add systematic 4%
    double system = 0.04*p7011_d6x1y1_yval[it];
    p7011_d6x1y1_yerr[it] = sqrt(p7011_d6x1y1_yerr[it]*p7011_d6x1y1_yerr[it]+system*system);
  }

  //transform data in t and dsig/dt --------
  //step 1: cos(theta_cm) -> t
  for(int it=0;it<94;it++){
    double cosCm = p7011_d6x1y1_xval[it];
    double t_val = t_cosCM(cosCm, plab);
    p7011_d6x1y1_xval[it] = fabs(t_val);
  }
  
  //step 2: dsig/dOmega -> dsig/dt
 for(int it=0;it<94;it++){
   double ds_do = p7011_d6x1y1_yval[it];
   double ds_dt_val = ds_dt(ds_do,plab);
   p7011_d6x1y1_yval[it] = ds_dt_val;

   double err_ds_do = p7011_d6x1y1_yerr[it];
   double err_ds_dt_val = ds_dt(err_ds_do,plab);
   p7011_d6x1y1_yerr[it] = err_ds_dt_val;
 }
  //----------------------------------------
 //  int p7011_d6x1y1_numpoints = 94;
  int p7011_d6x1y1_numpoints = 81;
   expdata = TGraphErrors(p7011_d6x1y1_numpoints, p7011_d6x1y1_xval, p7011_d6x1y1_yval, 0, p7011_d6x1y1_yerr);
   
    return 0;
  }

if(fabs(plab-2.33)<1e-4 && sampl==0){
//http://durpdg.dur.ac.uk/view/ins109426
   cout<<"Data from EISENHANDLER 1976 — Differential Cross-Sections for anti-Proton-Proton Elastic Scattering Between 0.69-GeV/c and 2.43-GeV/c, added 4% system.error!"<<endl;
    //COS(THETA)	D(SIG)/DOMEGA IN MB/SR
double p7011_d5x1y4_xval[] = { 0.93, 0.91, 0.89, 0.87, 0.85, 0.83, 0.81, 0.79, 0.77, 
    0.75, 0.73, 0.71, 0.69, 0.67, 0.65, 0.63, 0.61, 0.59, 0.57, 
    0.55, 0.53, 0.51, 0.49, 0.47, 0.45, 0.43, 0.41, 0.39, 0.31, 
    0.35, 0.33, 0.31, 0.29, 0.27, 0.25, 0.23, 0.21, 0.19, 0.17, 
    0.15, 0.13, 0.11, 0.09, 0.07, 0.05, 0.03, 0.01, -0.01, -0.03, 
    -0.05, -0.07, -0.09, -0.11, -0.13, -0.15, -0.17, -0.19, -0.21, -0.23, 
    -0.25, -0.27, -0.29, -0.31, -0.33, -0.35, -0.37, -0.39, -0.41, -0.43, 
    -0.45, -0.47, -0.49, -0.51, -0.53, -0.55, -0.57, -0.59, -0.61, -0.63, 
    -0.65, -0.67, -0.69, -0.71, -0.73 };
  
  double p7011_d5x1y4_yval[] = { 23.619, 16.178, 10.371, 6.625, 4.216, 2.708, 1.488, 0.877, 0.509, 
    0.267, 0.295, 0.226, 0.214, 0.302, 0.302, 0.294, 0.38, 0.329, 0.375, 
    0.406, 0.425, 0.427, 0.359, 0.385, 0.366, 0.291, 0.261, 0.28, 0.261, 
    0.232, 0.232, 0.208, 0.193, 0.174, 0.153, 0.13, 0.116, 0.122, 0.111, 
    0.102, 0.102, 0.083, 0.078, 0.075, 0.073, 0.067, 0.06, 0.061, 0.053, 
    0.056, 0.051, 0.054, 0.056, 0.064, 0.054, 0.05, 0.058, 0.054, 0.056, 
    0.055, 0.052, 0.047, 0.054, 0.057, 0.044, 0.052, 0.053, 0.042, 0.041, 
    0.043, 0.036, 0.034, 0.038, 0.029, 0.024, 0.026, 0.021, 0.02, 0.024, 
    0.023, 0.021, 0.02, 0.027, 0.017 };
  double p7011_d5x1y4_yerr[] = { 1.152, 0.69, 0.506, 0.338, 0.232, 0.145, 0.09, 0.065, 0.049, 
    0.035, 0.033, 0.029, 0.026, 0.03, 0.033, 0.032, 0.036, 0.033, 0.032, 
    0.028, 0.022, 0.019, 0.018, 0.021, 0.024, 0.023, 0.02, 0.018, 0.016, 
    0.013, 0.012, 0.011, 0.01, 0.01, 0.009, 0.008, 0.008, 0.008, 0.008, 
    0.007, 0.007, 0.007, 0.005, 0.005, 0.005, 0.004, 0.004, 0.004, 0.004, 
    0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.004, 0.005, 0.005, 
    0.005, 0.005, 0.005, 0.005, 0.005, 0.004, 0.005, 0.004, 0.004, 0.004, 
    0.004, 0.004, 0.003, 0.004, 0.003, 0.003, 0.003, 0.003, 0.003, 0.002, 
    0.002, 0.002, 0.003, 0.004, 0.003 };
  for(int it=0;it<94;it++){ // add systematic 4%
    double system = 0.04*p7011_d5x1y4_yval[it];
    p7011_d5x1y4_yerr[it] = sqrt(p7011_d5x1y4_yerr[it]*p7011_d5x1y4_yerr[it]+system*system);
  }

  //transform data in t and dsig/dt --------
  //step 1: cos(theta_cm) -> t
  for(int it=0;it<94;it++){
    double cosCm = p7011_d5x1y4_xval[it];
    double t_val = t_cosCM(cosCm, plab);
    p7011_d5x1y4_xval[it] = fabs(t_val);
  }
  
  //step 2: dsig/dOmega -> dsig/dt
 for(int it=0;it<94;it++){
   double ds_do = p7011_d5x1y4_yval[it];
   double ds_dt_val = ds_dt(ds_do,plab);
   p7011_d5x1y4_yval[it] = ds_dt_val;

   double err_ds_do = p7011_d5x1y4_yerr[it];
   double err_ds_dt_val = ds_dt(err_ds_do,plab);
   p7011_d5x1y4_yerr[it] = err_ds_dt_val;
 }
  //----------------------------------------
 //  int p7011_d5x1y4_numpoints = 95;
  int p7011_d5x1y4_numpoints = 84;
  expdata = TGraphErrors(p7011_d5x1y4_numpoints, p7011_d5x1y4_xval, p7011_d5x1y4_yval,0,p7011_d5x1y4_yerr);
    
    return 0;
  }

 if(fabs(plab-2.33)<1e-4 && sampl==1){
//http://durpdg.dur.ac.uk/view/ins93330
   cout<<"Data from CRAWLEY 1973 — Anti-p p elastic scattering at 2.33 gev/c, added system.error 2.7%"<<endl;
     //-T IN GEV**2	D(SIG)/DT IN MB/GEV**2
 double p5140_d1x1y1_xval[] = { 0.042499999999999996, 0.0475, 0.052500000000000005, 0.057499999999999996, 0.0625, 0.0675, 0.07250000000000001, 0.0775, 0.0825, 0.0875, 0.0925, 0.0975, 0.10250000000000001, 0.1075, 0.1125, 0.1175, 0.1225, 0.1275, 0.1325, 
    0.1375, 0.14250000000000002, 0.1475, 0.1525, 0.1575, 0.1625, 0.1675, 0.1725, 0.1775, 0.185, 
    0.195, 0.20500000000000002, 0.215, 0.225, 0.235, 0.245, 0.255, 0.265, 0.275, 0.28500000000000003, 
    0.295, 0.3125, 0.3375, 0.3625, 0.3875, 0.4125, 0.4375, 0.4625, 0.4875, 0.525, 
    0.575, 0.625, 0.675, 0.75, 0.8500000000000001, 0.95, 1.1, 1.2999999999999998, 1.5, 1.7000000000000002, 
    1.9, 2.1, 2.3, 2.5, 2.7, 2.9, 3.1 };
  double p5140_d1x1y1_xerr[] = { 0.0024999999999999953, 0.0025000000000000022, 0.0025000000000000022, 0.0024999999999999953, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 
    0.0024999999999999883, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0024999999999999883, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 
    0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0024999999999999745, 0.0025000000000000022, 0.0050000000000000044, 
    0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.004999999999999977, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 
    0.0050000000000000044, 0.012500000000000011, 0.012500000000000011, 0.012500000000000011, 0.012500000000000011, 0.012499999999999956, 0.012500000000000011, 0.012500000000000011, 0.012500000000000011, 0.025000000000000022, 
    0.02499999999999991, 0.025000000000000022, 0.025000000000000022, 0.050000000000000044, 0.050000000000000044, 0.04999999999999993, 0.10000000000000009, 0.09999999999999987, 0.10000000000000009, 0.10000000000000009, 
    0.09999999999999987, 0.10000000000000009, 0.09999999999999964, 0.10000000000000009, 0.10000000000000009, 0.10000000000000009, 0.10000000000000009 };

  double p5140_d1x1y1_yval[] = { 242.6, 205.9, 171.8, 172.5, 170.5, 164.7, 152.5, 139.9, 127.8, 
    122.3, 112.0, 106.8, 98.7, 87.5, 82.4, 82.4, 67.8, 68.6, 68.2, 
    61.8, 56.6, 51.9, 44.2, 47.2, 42.5, 31.7, 34.3, 32.2, 36.7, 
    31.1, 25.1, 14.8, 16.7, 13.7, 12.9, 8.58, 6.22, 6.44, 5.79, 
    3.65, 2.66, 2.23, 1.29, 1.03, 1.03, 0.43, 0.86, 1.29, 1.42, 
    1.37, 1.76, 1.89, 1.65, 1.42, 1.11, 0.761, 0.375, 0.3, 0.247, 
    0.193, 0.161, 0.15, 0.107, 0.086, 0.075, 0.011 };
  double p5140_d1x1y1_yerr[] = { 10.2, 9.4, 8.6, 8.6, 8.6, 8.4, 8.1, 7.7, 7.4, 
    7.2, 6.9, 6.8, 6.5, 6.1, 5.9, 5.9, 5.4, 5.4, 5.4, 
    5.1, 4.9, 4.7, 4.4, 4.5, 4.3, 3.7, 3.8, 3.7, 2.8, 
    2.6, 2.3, 1.8, 1.9, 1.7, 1.7, 1.36, 1.16, 1.17, 1.11, 
    0.88, 0.48, 0.44, 0.33, 0.3, 0.3, 0.19, 0.27, 0.33, 0.25, 
    0.24, 0.27, 0.29, 0.19, 0.17, 0.15, 0.09, 0.063, 0.057, 0.051, 
    0.045, 0.042, 0.04, 0.034, 0.03, 0.028, 0.011 };

 for(int it=0;it<66;it++){ // add systematic 2.7%
    double system = 0.027*p5140_d1x1y1_yval[it];
    p5140_d1x1y1_yerr[it] = sqrt(p5140_d1x1y1_yerr[it]*p5140_d1x1y1_yerr[it]+system*system);
  }

 // int p5140_d1x1y1_numpoints = 66;
 int p5140_d1x1y1_numpoints = 65;
  expdata = TGraphErrors(p5140_d1x1y1_numpoints, p5140_d1x1y1_xval, p5140_d1x1y1_yval, p5140_d1x1y1_xerr, 
				   p5140_d1x1y1_yerr);
     
    return 0;
  }
 if(fabs(plab-1.776)<1e-4){
   //http://durpdg.dur.ac.uk/view/ins99005
   cout<<"Data from  JENNI 1975 — Measurement of the Real Part of the Forward Amplitude in anti-p p Elastic Scattering, added systematic error 2%"<<endl;
   //-T IN GEV**2	D(SIG)/DT IN MB/GEV**2
 double p3043_d3x1y1_xval[] = { 5.0E-4, 7.5E-4, 0.00105, 0.0014, 0.0018, 0.00225, 0.00274, 0.00329, 0.00389, 
    0.00453, 0.00523, 0.00597, 0.00677, 0.00761, 0.00851, 0.00945, 0.01044, 0.01148, 0.01257, 
    0.01371, 0.0149, 0.01614, 0.01742, 0.01876, 0.02014, 0.02157, 0.02306, 0.02459, 0.02616, 
    0.02779, 0.02946, 0.03119, 0.03296, 0.03478, 0.03665, 0.03856, 0.04052, 0.04253, 0.04459, 
    0.0467, 0.04885, 0.05105, 0.05329, 0.05559, 0.05793, 0.06032, 0.06275, 0.06523, 0.06776, 
    0.07033, 0.07295, 0.07561, 0.07832, 0.08108, 0.08388, 0.08673, 0.08962, 0.09256, 0.09554, 
    0.09857 };
   double p3043_d3x1y1_yval[] = { 2290.3, 1136.4, 882.1, 722.9, 545.8, 558.1, 482.1, 492.1, 401.7, 
    441.9, 402.5, 436.4, 397.8, 401.0, 382.1, 410.1, 411.9, 391.1, 377.6, 
    411.0, 382.4, 370.6, 366.3, 379.5, 349.1, 334.2, 326.1, 315.4, 311.0, 
    326.0, 314.8, 278.4, 265.0, 276.0, 272.6, 281.5, 260.5, 250.0, 252.8, 
    247.9, 234.5, 222.9, 213.0, 212.3, 188.1, 199.7, 184.1, 192.8, 182.3, 
    176.3, 171.8, 162.4, 154.2, 161.5, 149.6, 144.2, 142.5, 130.5, 112.9, 
    119.3 };
  double p3043_d3x1y1_yerr[] = { 164.9, 96.7, 70.0, 51.9, 43.9, 37.1, 35.3, 31.9, 28.9, 
    29.2, 27.0, 25.5, 24.2, 23.1, 22.6, 21.5, 20.9, 20.0, 18.7, 
    18.2, 17.7, 17.0, 16.1, 16.1, 15.3, 14.6, 14.2, 13.7, 13.4, 
    13.3, 13.0, 12.0, 11.5, 11.6, 11.5, 11.4, 10.7, 10.5, 10.3, 
    10.0, 9.7, 9.4, 9.0, 9.1, 8.4, 8.5, 8.1, 8.2, 7.9, 
    7.9, 7.6, 7.3, 7.0, 7.2, 7.0, 6.7, 6.7, 6.3, 5.9, 
    6.0 };
  for(int it=0;it<60;it++){ // add systematic 2%
    double system = 0.02*p3043_d3x1y1_yval[it];
    p3043_d3x1y1_yerr[it] = sqrt(p3043_d3x1y1_yerr[it]*p3043_d3x1y1_yerr[it]+system*system);
  }
  int p3043_d3x1y1_numpoints = 60;
  expdata = TGraphErrors(p3043_d3x1y1_numpoints, p3043_d3x1y1_xval, p3043_d3x1y1_yval,0,p3043_d3x1y1_yerr);
  return 0;
  }
 if(fabs(plab-2.607)<1e-4){
  //http://durpdg.dur.ac.uk/view/ins99005
   cout<<"Data from  JENNI 1975 — Measurement of the Real Part of the Forward Amplitude in anti-p p Elastic Scattering, added systematic error 2%"<<endl;
   //-T IN GEV**2	D(SIG)/DT IN MB/GEV**2
 double p3043_d4x1y1_xval[] = { 5.1E-4, 7.6E-4, 0.00106, 0.00141, 0.00182, 0.00227, 0.00277, 0.00332, 0.00393, 
    0.00458, 0.00528, 0.00603, 0.00684, 0.00769, 0.00859, 0.00954, 0.01055, 0.0116, 0.0127, 
    0.01385, 0.01505, 0.01631, 0.01761, 0.01896, 0.02036, 0.02181, 0.0233, 0.02485, 0.02645, 
    0.02809, 0.02979, 0.03153, 0.03333, 0.03517, 0.03706, 0.039, 0.04098, 0.04302, 0.04511, 
    0.04724, 0.04942, 0.05165, 0.05393, 0.05625, 0.05863, 0.06105, 0.06352, 0.06603, 0.0686, 
    0.07121, 0.07386, 0.07657, 0.07932, 0.08212, 0.08497, 0.08786, 0.0908, 0.09379, 0.09682, 
    0.0999 };
 double p3043_d4x1y1_yval[] = { 1623.8, 967.9, 538.1, 603.4, 458.2, 358.2, 328.3, 262.1, 309.9, 
    358.7, 283.4, 297.7, 262.9, 340.6, 374.9, 288.0, 226.8, 313.6, 330.6, 
    246.2, 291.3, 250.9, 260.8, 260.8, 237.8, 267.2, 257.1, 260.5, 233.0, 
    268.0, 224.3, 214.7, 200.2, 209.6, 218.5, 213.0, 201.1, 188.8, 189.1, 
    183.0, 179.9, 179.5, 167.2, 153.1, 153.2, 161.2, 132.5, 123.1, 139.6, 
    127.3, 114.5, 113.8, 117.4, 136.7, 127.1, 95.2, 96.2, 80.6, 83.1, 
    61.8 };
  double p3043_d4x1y1_yerr[] = { 192.4, 110.8, 83.1, 67.8, 52.3, 50.2, 47.1, 39.4, 40.7, 
    39.6, 35.7, 34.9, 30.9, 30.6, 30.9, 27.2, 24.7, 25.7, 24.6, 
    23.7, 23.4, 21.0, 21.9, 19.3, 17.7, 19.3, 18.6, 19.2, 16.8, 
    17.2, 16.4, 15.2, 14.3, 14.4, 14.9, 14.5, 13.6, 13.0, 12.9, 
    12.7, 12.5, 12.3, 11.5, 11.2, 11.1, 10.8, 9.8, 9.4, 9.9, 
    9.2, 9.1, 9.2, 8.6, 9.2, 9.0, 8.2, 7.6, 7.2, 7.1, 
    5.9 };
for(int it=0;it<60;it++){ // add systematic 2%
    double system = 0.02*p3043_d4x1y1_yval[it];
    p3043_d4x1y1_yerr[it] = sqrt(p3043_d4x1y1_yerr[it]*p3043_d4x1y1_yerr[it]+system*system);
  }
  int p3043_d4x1y1_numpoints = 60;
  expdata = TGraphErrors(p3043_d4x1y1_numpoints, p3043_d4x1y1_xval, p3043_d4x1y1_yval,0,p3043_d4x1y1_yerr);
     
    return 0;
  }
 if(fabs(plab-2.85)<1e-4){
   //http://durpdg.dur.ac.uk/view/ins93404
   cout<<"Data from CRAWLEY 1973 — Anti-p p elastic scattering at 2.85 gev/c, added system 2.2%"<<endl;
   //-T IN GEV**2	D(SIG)/DT IN MB/GEV**2
 double p5136_d1x1y1_xval[] = {0.042499999999999996, 0.0475, 0.052500000000000005, 0.057499999999999996, 0.0625, 0.0675, 0.07250000000000001, 0.0775, 0.0825, 
    0.0875, 0.0925, 0.0975, 0.10250000000000001, 0.1075, 0.1125, 0.1175, 0.1225, 0.1275, 0.1325, 
    0.1375, 0.14250000000000002, 0.1475, 0.1525, 0.1575, 0.1625, 0.1675, 0.1725, 0.1775, 0.1825, 
    0.1875, 0.1925, 0.1975, 0.2025, 0.2075, 0.2125, 0.2175, 0.2225, 0.2275, 0.23249999999999998, 
    0.2375, 0.245, 0.255, 0.265, 0.275, 0.28500000000000003, 0.295, 0.305, 0.315, 0.325, 
    0.335, 0.345, 0.3625, 0.3875, 0.42500000000000004, 0.475, 0.525, 0.575, 0.625, 0.675, 
    0.725, 0.775, 0.825, 0.875, 0.925, 0.975, 1.025, 1.0750000000000002, 1.125, 1.1749999999999998, 
    1.25, 1.35, 1.45, 1.55, 1.65, 1.75, 1.85, 1.95, 2.05, 2.1500000000000004, 
    2.3, 2.5, 2.7, 2.9, 3.1, 3.3, 3.5, 3.7, 3.9 };
  double p5136_d1x1y1_xerr[] = {0.0024999999999999953, 0.0025000000000000022, 0.0025000000000000022, 0.0024999999999999953, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 
    0.0024999999999999883, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0024999999999999883, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 
    0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0024999999999999745, 0.0025000000000000022, 0.0025000000000000022, 
    0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0025000000000000022, 0.0024999999999999745, 
    0.0025000000000000022, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 
    0.0050000000000000044, 0.004999999999999949, 0.012500000000000011, 0.012500000000000011, 0.025000000000000022, 0.024999999999999967, 0.025000000000000022, 0.02499999999999991, 0.025000000000000022, 0.025000000000000022, 
    0.025000000000000022, 0.025000000000000022, 0.02499999999999991, 0.025000000000000022, 0.025000000000000022, 0.025000000000000022, 0.02499999999999991, 0.025000000000000133, 0.02499999999999991, 0.02499999999999991, 
    0.050000000000000044, 0.050000000000000044, 0.050000000000000044, 0.050000000000000044, 0.04999999999999982, 0.050000000000000044, 0.050000000000000044, 0.050000000000000044, 0.04999999999999982, 0.050000000000000266, 
    0.09999999999999964, 0.10000000000000009, 0.10000000000000009, 0.10000000000000009, 0.10000000000000009, 0.09999999999999964, 0.10000000000000009, 0.10000000000000009, 0.10000000000000009 };
  double p5136_d1x1y1_yval[] = { 171.8, 167.2, 144.0, 140.4, 138.6, 126.8, 126.3, 116.3, 112.2, 
    101.6, 97.9, 87.7, 89.7, 82.5, 82.3, 71.1, 66.1, 61.8, 60.4, 
    53.2, 49.1, 50.7, 42.7, 42.9, 37.3, 35.9, 31.4, 29.8, 26.4, 
    30.7, 25.7, 25.5, 20.2, 18.9, 16.8, 18.9, 14.8, 14.3, 11.8, 
    13.0, 9.66, 9.66, 8.52, 6.93, 6.82, 2.95, 4.66, 3.07, 2.95, 
    2.61, 1.48, 1.14, 0.55, 0.75, 0.68, 0.82, 0.82, 1.07, 1.64, 
    1.2, 1.52, 1.32, 1.18, 0.95, 0.7, 0.82, 0.52, 0.66, 0.39, 
    0.364, 0.352, 0.239, 0.216, 0.148, 0.114, 0.181, 0.161, 0.121, 0.141, 
    0.051, 0.081, 0.071, 0.055, 0.02, 0.01, 0.01, 0.015, 0.005 };
  double p5136_d1x1y1_yerr[] = { 6.2, 6.2, 5.7, 5.6, 5.6, 5.4, 5.4, 5.1, 5.1, 
    4.8, 4.7, 4.5, 4.5, 4.3, 4.3, 4.0, 3.9, 3.7, 3.7, 
    3.5, 3.3, 3.4, 3.1, 3.1, 2.9, 2.9, 2.7, 2.6, 2.4, 
    2.6, 2.4, 2.4, 2.1, 2.1, 2.0, 2.1, 1.8, 1.8, 1.6, 
    1.7, 1.05, 1.05, 0.98, 0.89, 0.88, 0.58, 0.73, 0.59, 0.58, 
    0.54, 0.41, 0.23, 0.16, 0.13, 0.12, 0.14, 0.14, 0.16, 0.19, 
    0.17, 0.19, 0.17, 0.16, 0.15, 0.13, 0.14, 0.11, 0.12, 0.09, 
    0.064, 0.063, 0.052, 0.05, 0.041, 0.036, 0.043, 0.04, 0.035, 0.038, 
    0.016, 0.02, 0.019, 0.017, 0.01, 0.007, 0.007, 0.009, 0.005 };
 for(int it=0;it<88;it++){ // add systematic 2.2%
    double system = 0.022*p5136_d1x1y1_yval[it];
    p5136_d1x1y1_yerr[it] = sqrt(p5136_d1x1y1_yerr[it]*p5136_d1x1y1_yerr[it]+system*system);
  }
  int p5136_d1x1y1_numpoints = 88;
  expdata = TGraphErrors(p5136_d1x1y1_numpoints, p5136_d1x1y1_xval, p5136_d1x1y1_yval, p5136_d1x1y1_xerr, 
			 p5136_d1x1y1_yerr);
   
    return 0;
  }
 if(fabs(plab-3.0)<1e-4){
   //http://durpdg.dur.ac.uk/view/ins92992
   cout<<"Data from AMBATS 1974 — Systematic study of pi+- p, k+- p, p p, and anti-p p forward elastic scattering from 3 to 6 gev/c, added system.err 4%"<<endl;
   //-T IN GEV**2	D(SIG)/DT IN MB/GEV**2
double p7224_d3x1y2_xval[] = {0.025, 0.035, 0.045, 0.055, 0.065, 0.075, 0.085, 0.095, 
    0.11, 0.13, 0.15, 0.17, 0.19, 0.21, 0.23, 0.25, 0.27, 0.29, 
    0.31, 0.33, 0.35, 0.37, 0.39, 0.42, 0.46, 0.5, 0.54, 0.58, 
    0.65, 0.75, 0.85, 0.95, 1.05, 1.15, 1.25, 1.35 };
 double p7224_d3x1y2_yval[] = { 213.9, 208.5, 154.0, 147.8, 131.7, 114.0, 104.8, 92.8, 
    68.9, 53.2, 43.4, 33.4, 26.0, 19.5, 10.9, 8.5, 6.02, 3.74, 
    4.31, 3.29, 1.3, 1.19, 2.16, 0.53, 0.6, 0.8, 0.57, 1.15, 
    1.31, 1.24, 0.62, 0.73, 0.52, 0.12, 0.41, 0.3 };
  double p7224_d3x1y2_yerr[] = { 11.8, 12.6, 7.0, 6.4, 5.9, 5.5, 5.3, 4.9, 
    3.0, 2.6, 2.4, 2.1, 1.8, 1.6, 1.2, 1.1, 0.89, 0.7, 
    0.77, 0.67, 0.41, 0.39, 0.54, 0.19, 0.21, 0.24, 0.21, 0.31, 
    0.22, 0.23, 0.17, 0.19, 0.2, 0.09, 0.22, 0.19 };
 
 for(int it=0;it<37;it++){ // add systematic 4%
    double system = 0.04*p7224_d3x1y2_yval[it];
    p7224_d3x1y2_yerr[it] = sqrt(p7224_d3x1y2_yerr[it]*p7224_d3x1y2_yerr[it]+system*system);
  }
  int p7224_d3x1y2_numpoints = 36;
  expdata = TGraphErrors(p7224_d3x1y2_numpoints, p7224_d3x1y2_xval, p7224_d3x1y2_yval,0, p7224_d3x1y2_yerr);
  
     return 0;
  }
 if(fabs(plab-3.65)<1e-4){
  //http://durpdg.dur.ac.uk/view/ins92992
   cout<<"Data from AMBATS 1974 — Systematic study of pi+- p, k+- p, p p, and anti-p p forward elastic scattering from 3 to 6 gev/c, added system.err 4%"<<endl;
   //-T IN GEV**2	D(SIG)/DT IN MB/GEV**2
double p7224_d6x1y2_xval[] = {0.035, 0.045, 0.055, 0.065, 0.075, 0.085, 0.095, 0.11, 
    0.13, 0.15, 0.17, 0.19, 0.21, 0.23, 0.25, 0.27, 0.29, 0.31, 
    0.33, 0.35, 0.37, 0.39, 0.42, 0.46, 0.5, 0.54, 0.58, 0.65, 
    0.75, 0.85, 0.95, 1.05, 1.15, 1.25, 1.35, 1.45 };
  double p7224_d6x1y2_yval[] = {160.1, 143.0, 137.4, 112.0, 98.4, 89.0, 77.4, 66.7, 
    51.5, 33.9, 24.5, 20.2, 15.6, 12.0, 8.3, 4.84, 5.13, 2.68, 
    1.55, 1.13, 1.0, 1.01, 0.49, 0.64, 0.73, 0.51, 0.36, 0.52, 
    0.78, 0.49, 0.73, 0.48, 0.42, 0.24, 0.13, 0.09 };
  double p7224_d6x1y2_yerr[] = {10.9, 7.4, 6.8, 6.0, 5.6, 5.4, 5.0, 3.2, 
    2.8, 2.3, 1.9, 1.7, 1.5, 1.3, 1.1, 0.87, 0.9, 0.65, 
    0.49, 0.41, 0.39, 0.39, 0.19, 0.23, 0.25, 0.2, 0.17, 0.14, 
    0.17, 0.14, 0.18, 0.15, 0.15, 0.11, 0.08, 0.06 };
for(int it=0;it<39;it++){ // add systematic 4%
    double system = 0.04*p7224_d6x1y2_yval[it];
    p7224_d6x1y2_yerr[it] = sqrt(p7224_d6x1y2_yerr[it]*p7224_d6x1y2_yerr[it]+system*system);
  }
  int p7224_d6x1y2_numpoints = 36;
  expdata = TGraphErrors(p7224_d6x1y2_numpoints, p7224_d6x1y2_xval, p7224_d6x1y2_yval,0,p7224_d6x1y2_yerr);
   
    return 0;
  }
 if(fabs(plab-5.0)<1e-4 && sampl==0){
 //http://durpdg.dur.ac.uk/view/ins92992
   cout<<"Data from AMBATS 1974 — Systematic study of pi+- p, k+- p, p p, and anti-p p forward elastic scattering from 3 to 6 gev/c, added system.err 4%"<<endl;
   //-T IN GEV**2	D(SIG)/DT IN MB/GEV**2
double p7224_d9x1y2_xval[] = {0.035, 0.045, 0.055, 0.065, 0.075, 0.085, 0.095, 0.11, 
    0.13, 0.15, 0.17, 0.19, 0.21, 0.23, 0.25, 0.27, 0.29, 0.31, 
    0.33, 0.35, 0.37, 0.39, 0.42, 0.46, 0.5, 0.54, 0.58, 0.65, 
    0.75, 0.85, 0.95, 1.05, 1.15, 1.25 };
   double p7224_d9x1y2_yval[] = {135.6, 122.4, 107.1, 93.2, 79.9, 67.9, 60.6, 46.0, 
    40.0, 31.9, 25.0, 19.1, 12.6, 10.1, 7.14, 6.51, 5.52, 3.53, 
    2.46, 1.29, 0.82, 0.94, 0.4, 0.87, 0.35, 0.17, 0.3, 0.47, 
    0.44, 0.39, 0.203, 0.213, 0.31, 0.032 };
  double p7224_d9x1y2_yerr[] = {8.0, 6.2, 5.4, 5.0, 4.6, 4.2, 4.0, 2.4, 
    2.3, 2.0, 1.8, 1.5, 1.3, 1.1, 0.98, 0.93, 0.85, 0.69, 
    0.57, 0.41, 0.32, 0.34, 0.16, 0.24, 0.15, 0.1, 0.14, 0.12, 
    0.11, 0.1, 0.076, 0.08, 0.099, 0.024 };
 for(int it=0;it<41;it++){ // add systematic 4%
    double system = 0.04*p7224_d9x1y2_yval[it];
    p7224_d9x1y2_yerr[it] = sqrt(p7224_d9x1y2_yerr[it]*p7224_d9x1y2_yerr[it]+system*system);
  }
  int p7224_d9x1y2_numpoints = 34;
  expdata = TGraphErrors(p7224_d9x1y2_numpoints, p7224_d9x1y2_xval, p7224_d9x1y2_yval,0,p7224_d9x1y2_yerr);
   
    return 0;
  }
 if(fabs(plab-5.0)<1e-4 && sampl==1){
   //http://durpdg.dur.ac.uk/view/ins83926
   cout<<"Data from EIDE 1973 — Elastic scattering and two-body annihilations at 5 gev/c, added system. err 15%"<<endl;
   //-T IN GEV**2	D(SIG)/D(T) IN MUB/GEV**2
 double p7064_d5x1y1_xval[] = { 0.16, 0.18, 0.2, 0.22, 0.24, 0.26, 0.28, 0.3, 0.32, 
    0.34, 0.36, 0.38, 0.4, 0.42, 0.44, 0.46, 0.48, 0.5, 0.52, 
    0.54, 0.56, 0.58, 0.6, 0.62, 0.64, 0.66, 0.68, 0.7, 0.72, 
    0.74, 0.76, 0.78, 0.8, 0.82, 0.84, 0.86, 0.88, 0.9, 0.92, 
    0.94, 0.96, 0.98, 1.0, 1.035, 1.085, 1.135, 1.185, 1.235, 1.285, 
    1.335, 1.385, 1.435, 1.485, 1.535, 1.585, 1.635, 1.685, 1.735, 1.785, 
    1.835, 1.885, 1.935, 1.985, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 
    2.7, 2.8, 2.9, 3.0, 3.15, 3.35, 3.55, 3.75, 3.95, 4.35, 
    4.95, 5.55, 6.15, 6.75, 7.15, 7.35, 7.55 };

  double p7064_d5x1y1_yval[] = { 15600.0, 10500.0, 9300.0, 6400.0, 5500.0, 5900.0, 2800.0, 3000.0, 1790.0, 
    1400.0, 1010.0, 749.0, 634.0, 475.0, 380.0, 281.0, 248.0, 198.0, 182.0, 
    219.0, 211.0, 229.0, 242.0, 243.0, 277.0, 272.0, 264.0, 304.0, 287.0, 
    290.0, 302.0, 278.0, 294.0, 264.0, 279.0, 231.0, 244.0, 238.0, 228.0, 
    215.0, 220.0, 199.0, 176.0, 167.0, 140.0, 118.0, 97.5, 81.6, 70.8, 
    57.1, 39.3, 36.5, 34.4, 26.9, 17.4, 18.6, 15.8, 10.4, 13.9, 
    10.2, 10.3, 8.3, 5.2, 8.4, 8.0, 6.2, 7.6, 8.6, 6.5, 
    5.9, 4.5, 2.1, 3.7, 1.7, 0.6, 0.36, 0.1, 0.04, 0.05, 
    0.026, 0.016, 0.01, 0.018, 0.023, 0.05, 0.37 };
  double p7064_d5x1y1_yerr[] = { 1800.0, 1200.0, 1200.0, 900.0, 800.0, 900.0, 600.0, 300.0, 120.0, 
    70.0, 40.0, 25.0, 20.0, 15.0, 13.0, 10.0, 9.0, 8.0, 8.0, 
    9.0, 9.0, 9.0, 9.0, 9.0, 10.0, 10.0, 10.0, 11.0, 10.0, 
    10.0, 11.0, 10.0, 11.0, 10.0, 10.0, 9.0, 10.0, 10.0, 9.0, 
    9.0, 9.0, 9.0, 8.0, 8.0, 5.0, 4.0, 4.0, 3.6, 3.4, 
    3.0, 2.5, 2.5, 2.5, 2.3, 1.9, 1.9, 1.8, 1.4, 1.8, 
    1.4, 1.4, 1.4, 0.8, 1.0, 1.1, 1.0, 1.3, 1.4, 1.2, 
    1.1, 0.9, 0.6, 0.8, 0.4, 0.21, 0.15, 0.07, 0.04, 0.029, 
    0.026, 0.016, 0.01, 0.013, 0.023, 0.034, 0.12 };
  for(int it=0;it<86;it++){ // add systematic 15%
    double system = 0.15*p7064_d5x1y1_yval[it];
    p7064_d5x1y1_yerr[it] = sqrt(p7064_d5x1y1_yerr[it]*p7064_d5x1y1_yerr[it]+system*system);
    p7064_d5x1y1_yerr[it] *=1e-3;//mub -> mb
    p7064_d5x1y1_yval[it] *=1e-3;//mub -> mb
  }
  int p7064_d5x1y1_numpoints = 86;
  expdata = TGraphErrors(p7064_d5x1y1_numpoints, p7064_d5x1y1_xval, p7064_d5x1y1_yval,0,p7064_d5x1y1_yerr);
  return 0;
  }
 if(fabs(plab-3.55)<1e-4){
   //http://durpdg.dur.ac.uk/view/ins56106
   cout<<"Data from BAKER 1969 — Anti-proton-proton elastic scattering at 3.55 gev/c , added system.error 20%"<<endl;
   //-T IN GEV**2	D(SIG)/DT IN MB/GEV**2
 double p2486_d1x1y1_xval[] = { 0.18, 0.22, 0.28, 0.36, 0.44, 0.54, 0.7, 0.9, 1.1, 
    1.4, 1.8, 4.0 };
 
  double p2486_d1x1y1_yval[] = { 23.7, 13.9, 9.4, 3.04, 0.25, 0.68, 0.74, 0.75, 0.39, 
    0.21, 0.07, 0.005 };
  double p2486_d1x1y1_yerr[] = { 2.4, 1.9, 1.3, 0.7, 0.13, 0.17, 0.13, 0.14, 0.12, 
    0.07, 0.07, 0.004 };
 for(int it=0;it<12;it++){ // add systematic 20%
    double system = 0.20*p2486_d1x1y1_yval[it];
    p2486_d1x1y1_yerr[it] = sqrt(p2486_d1x1y1_yerr[it]*p2486_d1x1y1_yerr[it]+system*system);
  }
  int p2486_d1x1y1_numpoints = 12;
  expdata = TGraphErrors(p2486_d1x1y1_numpoints, p2486_d1x1y1_xval, p2486_d1x1y1_yval,0,p2486_d1x1y1_yerr);
  return 0;
  }

 if(fabs(plab-3.70)<1e-4){
   //http://durpdg.dur.ac.uk/view/ins431921
   cout<<"Data from ARMSTRONG 1996 — Precision measurements of anti-proton proton forward elastic scattering parameters in the 3.7-GeV/c to 6.2-GeV/c region, added 2% system.err"<<endl;
  double p7839_d4x1y1_xval[] = { 8.701E-4, 8.701E-4, 9.218E-4, 0.001122, 0.001162, 0.00145, 0.001566, 0.001671, 0.001924, 
    0.002204, 0.002525, 0.002886, 0.003126, 0.003607, 0.004003, 0.004409, 0.004884, 0.005337, 0.005743, 
    0.006292, 0.006787, 0.007261, 0.007831, 0.008411, 0.008991, 0.00963, 0.0103, 0.0109, 0.01154, 
    0.01241, 0.01306, 0.01373, 0.01449, 0.01537, 0.01613 };
  double p7839_d4x1y1_yval[] = { 546.9, 524.0, 518.0, 469.4, 454.7, 389.7, 353.1, 349.0, 323.8, 
    307.3, 289.0, 275.3, 270.9, 263.1, 260.0, 258.5, 256.9, 252.4, 247.8, 
    243.2, 240.6, 240.2, 236.4, 234.1, 232.5, 229.5, 228.1, 224.9, 223.4, 
    223.4, 217.6, 217.3, 214.2, 212.7, 209.6 };
  double p7839_d4x1y1_yerr[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  for(int it=0;it<35;it++){ // add systematic 2%
    double system = 0.02*p7839_d4x1y1_yval[it];
    p7839_d4x1y1_yerr[it] = sqrt(p7839_d4x1y1_yerr[it]*p7839_d4x1y1_yerr[it]+system*system);
  }
  int p7839_d4x1y1_numpoints = 35;
  expdata = TGraphErrors(p7839_d4x1y1_numpoints, p7839_d4x1y1_xval, p7839_d4x1y1_yval,0,p7839_d4x1y1_yerr);
    
    return 0;
  }
if(fabs(plab-4.07)<1e-4){
 //http://durpdg.dur.ac.uk/view/ins431921
   cout<<"Data from ARMSTRONG 1996 — Precision measurements of anti-proton proton forward elastic scattering parameters in the 3.7-GeV/c to 6.2-GeV/c region, added 2% system.err"<<endl;
  double p7839_d5x1y1_xval[] = { 0.001015, 0.001183, 0.001454, 0.001729, 0.002007, 0.002372, 0.002661, 0.003008, 0.003382, 
    0.003803, 0.004182, 0.004651, 0.005116, 0.005666, 0.006016, 0.006537, 0.007061, 0.007606, 0.008177, 
    0.008793, 0.00943, 0.01008, 0.01074, 0.0114, 0.01198, 0.01283, 0.0136, 0.01435, 0.01512, 
    0.01607, 0.01683 };
 
  double p7839_d5x1y1_yval[] = { 489.8, 404.6, 354.2, 319.1, 293.2, 275.1, 261.9, 248.8, 242.7, 
    241.2, 239.8, 235.4, 232.0, 229.9, 224.8, 224.4, 222.2, 222.1, 219.6, 
    215.2, 214.4, 211.8, 209.1, 206.1, 207.6, 203.0, 200.0, 201.2, 196.9, 
    193.9, 192.3 };
  double p7839_d5x1y1_yerr[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0 };
  for(int it=0;it<31;it++){ // add systematic 2%
    double system = 0.02*p7839_d5x1y1_yval[it];
    p7839_d5x1y1_yerr[it] = sqrt(p7839_d5x1y1_yerr[it]*p7839_d5x1y1_yerr[it]+system*system);
  }
  int p7839_d5x1y1_numpoints = 31;
  expdata =  TGraphErrors(p7839_d5x1y1_numpoints, p7839_d5x1y1_xval, p7839_d5x1y1_yval, 0, p7839_d5x1y1_yerr);
   
    return 0;
  }
 if(fabs(plab-5.6)<1e-4){
//http://durpdg.dur.ac.uk/view/ins431921
   cout<<"Data from ARMSTRONG 1996 — Precision measurements of anti-proton proton forward elastic scattering parameters in the 3.7-GeV/c to 6.2-GeV/c region, added 2% system.err"<<endl;
 double p7839_d6x1y1_xval[] = { 9.125E-4, 0.001179, 0.001369, 0.001597, 0.001901, 0.002205, 0.002927, 0.003118, 0.003422, 
    0.00384, 0.004182, 0.0046, 0.005133, 0.005665, 0.006235, 0.006729, 0.007072, 0.007414, 0.00806, 
    0.00863, 0.009239, 0.009961, 0.01065, 0.01144, 0.01171, 0.01217, 0.01262, 0.013, 0.01384, 
    0.01456, 0.01536, 0.01627, 0.01715, 0.0181, 0.01909 };
 double p7839_d6x1y1_yval[] = { 471.1, 371.7, 318.1, 276.4, 249.5, 233.6, 208.8, 202.8, 201.8, 
    196.9, 191.9, 190.9, 187.9, 186.9, 182.9, 182.0, 181.0, 179.0, 177.0, 
    175.0, 172.0, 171.0, 169.0, 167.0, 170.0, 167.0, 168.0, 165.1, 164.1, 
    163.1, 160.1, 159.1, 157.1, 156.1, 154.1 };
  double p7839_d6x1y1_yerr[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  for(int it=0;it<35;it++){ // add systematic 2%
    double system = 0.02*p7839_d6x1y1_yval[it];
    p7839_d6x1y1_yerr[it] = sqrt(p7839_d6x1y1_yerr[it]*p7839_d6x1y1_yerr[it]+system*system);
  }
  int p7839_d6x1y1_numpoints = 35;
  expdata = TGraphErrors(p7839_d6x1y1_numpoints, p7839_d6x1y1_xval, p7839_d6x1y1_yval,0,p7839_d6x1y1_yerr);
   
    return 0;
  }
 if(fabs(plab-5.72)<1e-4){
//http://durpdg.dur.ac.uk/view/ins431921
   cout<<"Data from ARMSTRONG 1996 — Precision measurements of anti-proton proton forward elastic scattering parameters in the 3.7-GeV/c to 6.2-GeV/c region, added 2% system.err"<<endl;

 double p7839_d7x1y1_xval[] = { 8.38E-4, 0.001028, 0.001067, 0.001257, 0.001371, 0.001447, 0.001562, 0.001752, 0.001828, 
    0.002171, 0.0024, 0.002514, 0.002895, 0.003124, 0.003314, 0.003733, 0.004114, 0.004419, 0.004609, 
    0.004952, 0.005142, 0.005485, 0.005752, 0.006057, 0.006361, 0.006666, 0.006971, 0.007466, 0.007885, 
    0.008075, 0.008533, 0.009066, 0.01063, 0.01112, 0.01154, 0.01208, 0.01276, 0.0131, 0.01383, 
    0.01467, 0.01543, 0.01718, 0.01783, 0.01813, 0.01901, 0.0195 };
 double p7839_d7x1y1_yval[] = { 481.3, 396.0, 374.2, 328.5, 313.7, 288.9, 276.0, 256.2, 250.2, 
    232.4, 217.5, 214.5, 206.6, 201.6, 200.6, 193.7, 189.7, 186.7, 190.7, 
    182.8, 184.8, 182.8, 182.8, 181.8, 178.8, 178.8, 175.8, 174.8, 175.8, 
    170.9, 171.9, 169.9, 166.9, 163.9, 166.9, 163.9, 163.9, 160.0, 161.0, 
    157.0, 159.0, 155.0, 153.0, 155.0, 151.0, 150.1 };
  double p7839_d7x1y1_yerr[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
 for(int it=0;it<46;it++){ // add systematic 2%
    double system = 0.02*p7839_d7x1y1_yval[it];
    p7839_d7x1y1_yerr[it] = sqrt(p7839_d7x1y1_yerr[it]*p7839_d7x1y1_yerr[it]+system*system);
  }
  int p7839_d7x1y1_numpoints = 46;
  expdata = TGraphErrors(p7839_d7x1y1_numpoints, p7839_d7x1y1_xval, p7839_d7x1y1_yval,0,p7839_d7x1y1_yerr);

    return 0;
  }
 if(fabs(plab-5.94)<1e-4){
   //http://durpdg.dur.ac.uk/view/ins431921
   cout<<"Data from ARMSTRONG 1996 — Precision measurements of anti-proton proton forward elastic scattering parameters in the 3.7-GeV/c to 6.2-GeV/c region, added 2% system.err"<<endl;
 double p7839_d8x1y1_xval[] = { 8.702E-4, 9.862E-4, 0.001044, 0.001102, 0.00116, 0.001334, 0.001392, 0.001633, 0.001972, 
    0.002265, 0.00261, 0.003017, 0.00323, 0.003423, 0.003887, 0.004319, 0.004815, 0.005263, 0.005801, 
    0.006381, 0.006933, 0.00755, 0.008121, 0.008711, 0.009398, 0.009978, 0.01079, 0.01154, 0.01236, 
    0.01311, 0.01398, 0.01479, 0.01572, 0.01653, 0.01758, 0.01856, 0.01943, 0.01978 };
 double p7839_d8x1y1_yval[] = { 451.1, 440.4, 374.8, 354.9, 344.3, 304.6, 293.9, 261.5, 235.9, 
    213.7, 203.8, 194.6, 190.6, 190.6, 184.9, 183.1, 179.4, 178.4, 174.8, 
    173.3, 171.8, 168.9, 167.2, 165.2, 164.1, 161.1, 158.0, 156.5, 158.0, 
    155.0, 153.4, 151.9, 148.8, 147.3, 145.8, 145.8, 142.7, 141.2 };
  double p7839_d8x1y1_yerr[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
 for(int it=0;it<38;it++){ // add systematic 2%
    double system = 0.02*p7839_d8x1y1_yval[it];
    p7839_d8x1y1_yerr[it] = sqrt(p7839_d8x1y1_yerr[it]*p7839_d8x1y1_yerr[it]+system*system);
  }
  int p7839_d8x1y1_numpoints = 38;
  expdata = TGraphErrors(p7839_d8x1y1_numpoints, p7839_d8x1y1_xval, p7839_d8x1y1_yval,0, p7839_d8x1y1_yerr);
   
    return 0;
  }
 if(fabs(plab-6.23)<1e-4){
 //http://durpdg.dur.ac.uk/view/ins431921
   cout<<"Data from ARMSTRONG 1996 — Precision measurements of anti-proton proton forward elastic scattering parameters in the 3.7-GeV/c to 6.2-GeV/c region, added 2% system.err"<<endl;
 double p7839_d9x1y1_xval[] = { 0.001006, 0.001242, 0.001603, 0.001834, 0.00216, 0.002544, 0.002898, 0.003079, 0.003287, 
    0.003727, 0.0042, 0.004706, 0.005087, 0.00562, 0.006211, 0.006803, 0.007128, 0.00737, 0.007986, 
    0.00865, 0.009286, 0.009938, 0.01065, 0.01142, 0.01301, 0.01381, 0.01474, 0.01564, 0.0164, 
    0.01738, 0.01842, 0.01931, 0.02 };
   double p7839_d9x1y1_yval[] = { 393.4, 318.9, 272.6, 247.6, 228.3, 211.5, 203.1, 200.3, 194.3, 
    190.4, 184.9, 182.2, 178.6, 177.6, 175.1, 175.0, 176.8, 172.4, 170.8, 
    169.6, 164.5, 164.2, 161.9, 160.6, 157.0, 154.0, 155.2, 152.5, 150.7, 
    148.9, 145.9, 145.3, 145.3 };
  double p7839_d9x1y1_yerr[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0 };
  for(int it=0;it<33;it++){ // add systematic 2%
    double system = 0.02*p7839_d9x1y1_yval[it];
    p7839_d9x1y1_yerr[it] = sqrt(p7839_d9x1y1_yerr[it]*p7839_d9x1y1_yerr[it]+system*system);
  }
  int p7839_d9x1y1_numpoints = 33;
  expdata = TGraphErrors(p7839_d9x1y1_numpoints, p7839_d9x1y1_xval, p7839_d9x1y1_yval,0,p7839_d9x1y1_yerr);
    
    return 0;
  }
 if(fabs(plab-4.2)<1e-4){
   //http://durpdg.dur.ac.uk/view/ins120467
   cout<<"Data from JENNI 1977 — Anti-p p and p p Forward Elastic Scattering Between 4-GeV/c and 10-GeV/c, added system err 1%"<<endl;
   //-T IN GEV**2	D(SIG)/DT IN MB/GEV**2
 double p2530_d2x1y1_xval[] = { 0.00106, 0.00141, 0.00181, 0.00226, 0.00276, 0.00331, 0.0039, 0.00455, 0.00525, 
    0.006, 0.0068, 0.00765, 0.00855, 0.0095, 0.0105, 0.01154, 0.01264, 0.01379, 0.01498, 
    0.01623, 0.01753, 0.01887 };
 
  double p2530_d2x1y1_yval[] = { 535.5, 353.4, 326.1, 279.3, 283.7, 260.4, 210.4, 232.2, 192.6, 
    198.1, 228.4, 215.2, 211.0, 208.9, 205.5, 212.3, 195.2, 198.0, 190.4, 
    183.9, 177.7, 181.5 };
  double p2530_d2x1y1_yerr[] = { 40.5, 31.6, 27.1, 24.0, 20.6, 19.0, 17.5, 16.0, 14.7, 
    13.9, 12.5, 12.2, 11.1, 10.6, 9.8, 9.5, 8.7, 8.3, 8.0, 
    7.8, 7.4, 7.4 };
  for(int it=0;it<22;it++){ // add systematic 1%
    double system = 0.01*p2530_d2x1y1_yval[it];
    p2530_d2x1y1_yerr[it] = sqrt(p2530_d2x1y1_yerr[it]*p2530_d2x1y1_yerr[it]+system*system);
  }
  int p2530_d2x1y1_numpoints = 22;
  expdata = TGraphErrors(p2530_d2x1y1_numpoints, p2530_d2x1y1_xval, p2530_d2x1y1_yval, 0, p2530_d2x1y1_yerr);
    
    return 0;
  }
 if(fabs(plab-6.0)<1e-4){
 //http://durpdg.dur.ac.uk/view/ins120467
   cout<<"Data from JENNI 1977 — Anti-p p and p p Forward Elastic Scattering Between 4-GeV/c and 10-GeV/c, added system err 1%"<<endl;
   //-T IN GEV**2	D(SIG)/DT IN MB/GEV**2
double p2530_d2x1y2_xval[] = { 0.00141, 0.00181, 0.00226, 0.00276, 0.00331, 0.0039, 0.00455, 0.00525, 
    0.006, 0.0068, 0.00765, 0.00855, 0.0095, 0.0105, 0.01154, 0.01264, 0.01379, 0.01498, 
    0.01623, 0.01753, 0.01887, 0.02028, 0.02173, 0.02322, 0.02477, 0.02636, 0.028, 0.0297, 
    0.03144, 0.03323, 0.03507 };
 
  double p2530_d2x1y2_yval[] = { 342.1, 291.7, 195.9, 260.9, 208.4, 167.7, 227.5, 175.0, 
    172.8, 184.6, 158.8, 169.0, 184.8, 169.4, 146.5, 167.1, 172.1, 163.2, 
    167.2, 145.7, 147.8, 168.2, 157.1, 146.8, 138.2, 145.5, 133.0, 132.5, 
    113.3, 116.2, 115.5 };
  double p2530_d2x1y2_yerr[] = { 43.2, 37.5, 30.1, 29.0, 26.1, 23.1, 21.4, 19.3, 
    17.7, 17.1, 16.0, 15.0, 14.3, 13.7, 12.4, 12.2, 11.6, 11.2, 
    10.6, 10.3, 9.3, 9.7, 9.2, 8.7, 8.4, 8.3, 7.9, 7.6, 
    7.0, 7.0, 6.7 };
   for(int it=0;it<31;it++){ // add systematic 1%
    double system = 0.01*p2530_d2x1y2_yval[it];
    p2530_d2x1y2_yerr[it] = sqrt(p2530_d2x1y2_yerr[it]*p2530_d2x1y2_yerr[it]+system*system);
  }
  int p2530_d2x1y2_numpoints = 31;
  expdata = TGraphErrors(p2530_d2x1y2_numpoints, p2530_d2x1y2_xval, p2530_d2x1y2_yval,0,p2530_d2x1y2_yerr);
    
    return 0;
  }
if(fabs(plab-8.0)<1e-4 && sampl==1){
 //http://durpdg.dur.ac.uk/view/ins120467
   cout<<"Data from JENNI 1977 — Anti-p p and p p Forward Elastic Scattering Between 4-GeV/c and 10-GeV/c, added system err 1%"<<endl;
   //-T IN GEV**2	D(SIG)/DT IN MB/GEV**2
double p2530_d2x1y3_xval[] = { 0.00181, 0.00226, 0.00276, 0.00331, 0.0039, 0.00455, 0.00525, 
    0.006, 0.0068, 0.00765, 0.00855, 0.0095, 0.0105, 0.01154, 0.01264, 0.01379, 0.01498, 
    0.01623, 0.01753, 0.01887, 0.02028, 0.02173, 0.02322, 0.02477, 0.02636, 0.028, 0.0297, 
    0.03144, 0.03323, 0.03507, 0.03697, 0.03891, 0.0409, 0.04293, 0.04502, 0.04716, 0.04934, 
    0.05158, 0.05386, 0.0562, 0.05858, 0.06108, 0.06356, 0.06609, 0.06868, 0.07131, 0.07399, 
    0.07672, 0.0795 };
  
  double p2530_d2x1y3_yval[] = { 235.6, 176.3, 134.6, 144.9, 167.5, 152.1, 147.3, 
    140.2, 157.7, 141.4, 142.5, 148.5, 151.4, 125.8, 144.1, 146.1, 131.9, 
    125.4, 125.4, 131.0, 114.4, 119.5, 119.1, 120.6, 109.7, 118.0, 107.6, 
    98.4, 104.5, 98.8, 107.1, 86.2, 96.6, 94.7, 88.8, 90.3, 95.3, 
    83.8, 78.7, 77.7, 71.3, 79.3, 77.4, 71.4, 71.0, 68.9, 61.5, 
    61.1, 51.4 };
  double p2530_d2x1y3_yerr[] = { 28.4, 25.2, 22.4, 19.9, 18.5, 16.6, 15.7, 
    14.3, 13.6, 13.0, 11.9, 10.8, 11.2, 9.7, 9.5, 9.1, 8.4, 
    7.9, 7.6, 7.7, 6.9, 6.8, 6.7, 6.6, 6.0, 6.0, 5.9, 
    7.6, 7.6, 7.3, 7.5, 6.7, 6.9, 6.8, 6.4, 6.5, 6.6, 
    5.9, 5.8, 5.8, 5.5, 5.6, 5.5, 5.2, 5.1, 5.0, 4.7, 
    4.7, 4.6 };
  for(int it=0;it<49;it++){ // add systematic 1%
    double system = 0.01*p2530_d2x1y3_yval[it];
    p2530_d2x1y3_yerr[it] = sqrt(p2530_d2x1y3_yerr[it]*p2530_d2x1y3_yerr[it]+system*system);
  }
  int p2530_d2x1y3_numpoints = 49;
  expdata = TGraphErrors(p2530_d2x1y3_numpoints, p2530_d2x1y3_xval, p2530_d2x1y3_yval,0, p2530_d2x1y3_yerr);
    
    return 0;
  }
 if(fabs(plab-10.0)<1e-4){
 //http://durpdg.dur.ac.uk/view/ins120467
   cout<<"Data from JENNI 1977 — Anti-p p and p p Forward Elastic Scattering Between 4-GeV/c and 10-GeV/c, added system err 1%"<<endl;
   //-T IN GEV**2	D(SIG)/DT IN MB/GEV**2
 double p2530_d2x1y4_xval[] = { 0.00181, 0.00226, 0.00276, 0.00331, 0.0039, 0.00455, 0.00525, 
    0.006, 0.0068, 0.00765, 0.00855, 0.0095, 0.0105, 0.01154, 0.01264, 0.01379, 0.01498, 
    0.01623, 0.01753, 0.01887, 0.02028, 0.02173, 0.02322, 0.02477, 0.02636, 0.028, 0.0297, 
    0.03144, 0.03323, 0.03507, 0.03697, 0.03891, 0.0409, 0.04293, 0.04502, 0.04716, 0.04934, 
    0.05158, 0.05386, 0.0562, 0.05858, 0.06108, 0.06356, 0.06609, 0.06868, 0.07131, 0.07399, 
    0.07672, 0.0795, 0.08233, 0.0852, 0.08813, 0.0911, 0.09412, 0.0972 };
  double p2530_d2x1y4_yval[] = { 199.4, 204.8, 163.0, 153.7, 131.6, 151.6, 146.8, 
    129.3, 141.8, 142.5, 130.8, 131.9, 117.3, 127.8, 125.3, 132.4, 118.1, 
    112.2, 125.1, 131.0, 119.6, 111.3, 122.5, 100.9, 111.8, 103.6, 97.4, 
    100.3, 106.9, 101.2, 93.5, 96.5, 92.5, 90.2, 85.0, 85.9, 85.6, 
    80.9, 77.5, 72.9, 71.3, 70.6, 67.8, 67.6, 60.8, 62.2, 63.4, 
    57.3, 54.2, 53.5, 54.7, 56.9, 47.8, 47.7, 45.8 };
  double p2530_d2x1y4_yerr[] = { 24.0, 22.2, 18.7, 17.3, 15.6, 14.6, 13.3, 
    12.0, 11.3, 10.3, 9.7, 9.0, 8.7, 8.1, 7.5, 7.3, 6.9, 
    6.6, 6.3, 6.0, 5.5, 5.5, 5.2, 4.7, 4.8, 4.5, 4.2, 
    4.2, 4.1, 4.0, 3.8, 3.7, 3.6, 3.5, 3.4, 3.3, 3.3, 
    3.1, 3.0, 3.0, 2.8, 2.8, 2.7, 2.8, 2.6, 2.5, 2.6, 
    2.5, 2.3, 2.3, 2.2, 2.3, 2.1, 2.1, 2.0 };
  for(int it=0;it<55;it++){ // add systematic 1%
    double system = 0.01*p2530_d2x1y4_yval[it];
    p2530_d2x1y4_yerr[it] = sqrt(p2530_d2x1y4_yerr[it]*p2530_d2x1y4_yerr[it]+system*system);
  }
  int p2530_d2x1y4_numpoints = 55;
  expdata = TGraphErrors(p2530_d2x1y4_numpoints, p2530_d2x1y4_xval, p2530_d2x1y4_yval,0,p2530_d2x1y4_yerr);
    
    return 0;
  }
if(fabs(plab-16.0)<1e-4 && sampl==0){
//http://durpdg.dur.ac.uk/view/ins124704
    cout<<"Data from RUSS 1977 — Elastic Scattering of pi-, K-, and anti-p from Hydrogen at 8-GeV/c and 16-GeV/c, added syst. 5%"<<endl;
    //-T IN GEV**2	D(SIG)/DT IN MB/GEV**2
  double p4722_d5x1y1_xval[] = { 0.085, 0.095, 0.105, 0.115, 0.125, 0.135, 0.145, 0.155, 0.165, 
    0.175, 0.185, 0.195, 0.21, 0.23, 0.25, 0.27, 0.29, 0.31, 0.33, 
    0.35, 0.38, 0.42, 0.46, 0.5, 0.54, 0.58, 0.62, 0.66, 0.7, 
    0.74, 0.85, 0.95, 1.05, 1.2 };
   double p4722_d5x1y1_yval[] = { 40.12, 35.18, 32.45, 28.54, 25.49, 24.06, 19.63, 17.51, 16.05, 
    14.87, 13.0, 11.14, 9.95, 7.79, 6.39, 4.91, 3.97, 3.19, 2.39, 
    1.891, 1.37, 0.843, 0.527, 0.318, 0.18, 0.16, 0.13, 0.08, 0.08, 
    0.07, 0.05, 0.07, 0.04, 0.04 };
  double p4722_d5x1y1_yerr[] = { 0.67, 0.64, 0.62, 0.6, 0.58, 0.56, 0.52, 0.5, 0.48, 
    0.47, 0.45, 0.42, 0.2, 0.18, 0.17, 0.15, 0.14, 0.13, 0.11, 
    0.096, 0.059, 0.048, 0.039, 0.032, 0.033, 0.09, 0.08, 0.07, 0.07, 
    0.07, 0.04, 0.04, 0.02, 0.02 };
  for(int it=0;it<34;it++){ // add systematic 5%
    double system = 0.05*p4722_d5x1y1_yval[it];
    p4722_d5x1y1_yerr[it] = sqrt(p4722_d5x1y1_yerr[it]*p4722_d5x1y1_yerr[it]+system*system);
  }
  int p4722_d5x1y1_numpoints = 34;
  expdata = TGraphErrors(p4722_d5x1y1_numpoints, p4722_d5x1y1_xval, p4722_d5x1y1_yval,0,p4722_d5x1y1_yerr);
 return 0;
 }
if(fabs(plab-6.2)<1e-4){
//http://durpdg.dur.ac.uk/view/ins99557
  cout<<"Data from BURAN 1976 — Anti-Proton-Proton Elastic Scattering at 6.2-GeV/c, added system.err 20%"<<endl;
  double p2447_d1x1y1_xval[] = { 0.31, 0.33, 0.35, 0.37, 0.39, 0.41, 0.43, 0.45, 0.47, 
    0.49, 0.51, 0.53, 0.55, 0.57, 0.59, 0.61, 0.63, 0.65, 0.67, 
    0.69, 0.71, 0.73, 0.75, 0.77, 0.79, 0.81, 0.83, 0.85, 0.87, 
    0.89, 0.91, 0.93, 0.95, 0.97, 0.99, 1.02, 1.06, 1.1, 1.14, 
    1.18, 1.22, 1.26, 1.3, 1.34, 1.38, 1.42, 1.46, 1.49, 1.55, 
    1.65, 1.75, 1.85, 1.95, 2.05, 2.15, 2.25, 2.35, 2.45, 2.55, 
    2.65, 2.75, 2.85, 2.95, 3.05, 3.15, 3.35, 4.0, 5.5, 7.75, 
    9.5 };
  double p2447_d1x1y1_xerr[] = { 0.020000000000000018, 0.020000000000000018, 0.019999999999999962, 0.020000000000000018, 0.020000000000000018, 0.019999999999999962, 0.020000000000000018, 0.020000000000000018, 0.019999999999999962, 
    0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.019999999999999907, 0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 
    0.019999999999999907, 0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.019999999999999907, 0.020000000000000018, 0.020000000000000018, 
    0.020000000000000018, 0.020000000000000018, 0.020000000000000018, 0.019999999999999907, 0.020000000000000018, 0.020000000000000018, 0.040000000000000036, 0.040000000000000036, 0.040000000000000036, 0.039999999999999813, 
    0.040000000000000036, 0.040000000000000036, 0.040000000000000036, 0.040000000000000036, 0.040000000000000036, 0.039999999999999813, 0.040000000000000036, 0.040000000000000036, 0.020000000000000018, 0.10000000000000009, 
    0.010000000000000009, 0.010000000000000009, 0.010000000000000009, 0.010000000000000009, 0.009999999999999787, 0.009999999999999787, 0.009999999999999787, 0.010000000000000231, 0.010000000000000231, 0.009999999999999787, 
    0.009999999999999787, 0.009999999999999787, 0.010000000000000231, 0.010000000000000231, 0.009999999999999787, 0.009999999999999787, 0.30000000000000027, 1.0, 2.0, 2.5, 
    1.0 };
 
  double p2447_d1x1y1_yval[] = { 2377.0, 1771.0, 1253.0, 918.0, 647.0, 466.0, 342.0, 261.0, 193.0, 
    160.0, 147.0, 132.0, 134.0, 136.0, 141.0, 145.0, 159.0, 169.0, 177.0, 
    183.0, 167.0, 184.0, 181.0, 172.0, 166.0, 176.0, 165.0, 170.0, 151.0, 
    150.0, 153.0, 136.0, 133.0, 115.0, 123.0, 112.0, 100.0, 83.4, 68.4, 
    60.6, 50.8, 44.6, 41.4, 29.8, 24.9, 21.1, 18.7, 15.7, 12.0, 
    10.4, 7.94, 7.7, 5.47, 5.15, 5.32, 5.16, 4.6, 3.78, 3.28, 
    2.68, 2.59, 1.69, 1.44, 1.04, 0.93, 0.33, 0.077, 0.026, 0.0074, 
    0.03 };
  double p2447_d1x1y1_yerr[] = { 64.0, 49.0, 36.0, 16.0, 12.0, 11.0, 10.0, 8.0, 7.0, 
    6.0, 6.0, 5.0, 5.0, 5.0, 5.0, 5.0, 6.0, 6.0, 7.0, 
    7.0, 6.0, 7.0, 7.0, 7.0, 6.0, 7.0, 7.0, 7.0, 6.0, 
    6.0, 7.0, 6.0, 6.0, 6.0, 6.0, 4.0, 4.0, 2.8, 2.8, 
    2.7, 2.5, 2.4, 2.2, 1.9, 1.7, 1.6, 1.5, 2.0, 0.73, 
    0.7, 0.6, 0.6, 0.48, 0.43, 0.47, 0.47, 0.46, 0.42, 0.42, 
    0.4, 0.4, 0.4, 0.34, 0.33, 0.35, 0.15, 0.046, 0.021, 0.0059, 
    0.015 };
  for(int it=0;it<70;it++){ // add systematic 20%
    double system = 0.2*p2447_d1x1y1_yval[it];
    p2447_d1x1y1_yerr[it] = sqrt(p2447_d1x1y1_yerr[it]*p2447_d1x1y1_yerr[it]+system*system);
  }
  int p2447_d1x1y1_numpoints = 70;
  expdata =TGraphErrors(p2447_d1x1y1_numpoints, p2447_d1x1y1_xval, p2447_d1x1y1_yval, p2447_d1x1y1_xerr, p2447_d1x1y1_yerr);
    
    return 0;
  }
 if(fabs(plab-5.7)<1e-4){
   //http://durpdg.dur.ac.uk/view/ins2106
   cout<<"Data from BRAUN 1975 — Analysis of Two Prong Events in anti-Proton-Proton Interactions at 5.7-GeV/c"<<endl;
  double p3048_d2x1y1_xval[] = { 0.035, 0.045, 0.055, 0.065, 0.07500000000000001, 0.08499999999999999, 0.095, 0.10500000000000001, 0.11499999999999999, 
    0.125, 0.135, 0.14500000000000002, 0.155, 0.165, 0.175, 0.185, 0.195, 0.20500000000000002, 0.215, 
    0.225, 0.235, 0.245, 0.255, 0.265, 0.275, 0.28500000000000003, 0.295, 0.305, 0.315, 
    0.325, 0.335, 0.345, 0.355, 0.38, 0.42000000000000004, 0.48, 0.56, 0.64, 0.74, 
    0.8600000000000001, 0.98, 1.1400000000000001, 1.3399999999999999, 1.74, 1.94, 2.14, 2.54 };
  double p3048_d2x1y1_xerr[] = { 0.0050000000000000044, 0.0049999999999999975, 0.0049999999999999975, 0.0050000000000000044, 0.0050000000000000044, 0.0049999999999999906, 0.0050000000000000044, 0.0050000000000000044, 0.0049999999999999906, 
    0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.004999999999999977, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 
    0.0050000000000000044, 0.004999999999999977, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 0.0050000000000000044, 
    0.0050000000000000044, 0.0050000000000000044, 0.004999999999999949, 0.0050000000000000044, 0.020000000000000018, 0.020000000000000018, 0.03999999999999998, 0.040000000000000036, 0.040000000000000036, 0.05999999999999994, 
    0.06000000000000005, 0.05999999999999994, 0.10000000000000009, 0.09999999999999987, 0.10000000000000009, 0.09999999999999987, 0.10000000000000009, 0.10000000000000009 };
 
  double p3048_d2x1y1_yval[] = { 132.8, 105.1, 92.4, 87.6, 82.2, 67.1, 61.2, 56.5, 47.7, 
    41.8, 36.3, 33.2, 26.1, 26.0, 24.0, 19.0, 18.4, 16.1, 12.7, 
    10.7, 10.9, 9.0, 7.94, 6.77, 6.7, 4.32, 4.96, 4.37, 3.12, 
    2.41, 2.41, 2.75, 1.04, 0.97, 0.97, 0.26, 0.247, 0.355, 0.195, 
    0.226, 0.134, 0.16, 0.081, 0.025, 0.031, 0.013, 0.019 };
  double p3048_d2x1y1_yerr[] = { 4.8, 4.1, 3.7, 3.6, 3.4, 3.0, 2.9, 2.9, 2.6, 
    2.4, 2.3, 2.1, 1.9, 2.0, 1.8, 1.6, 1.6, 1.5, 1.3, 
    1.2, 1.2, 1.1, 1.0, 0.94, 0.96, 0.76, 0.84, 0.77, 0.65, 
    0.56, 0.56, 0.61, 0.37, 0.17, 0.15, 0.063, 0.061, 0.073, 0.045, 
    0.048, 0.037, 0.031, 0.022, 0.013, 0.014, 0.009, 0.011 };
  
  int p3048_d2x1y1_numpoints = 47;
  expdata = TGraphErrors(p3048_d2x1y1_numpoints, p3048_d2x1y1_xval, p3048_d2x1y1_yval, p3048_d2x1y1_xerr, p3048_d2x1y1_yerr);
  return 0;
  }
  if(fabs(plab-10.4)<1e-4){
 //from http://durpdg.dur.ac.uk/view/ins100639
    cout<<"Data from BRANDENBURG 1975 — Measurement of Particle and anti-Particle Elastic Scattering on Protons Between 6-GeV and 14-GeV, added system.err 2%"<<endl;
double p7130_d8x1y2_xval[] = { 0.0225, 0.0275, 0.0325, 0.0375, 0.0425, 0.0475, 0.0525, 0.0575, 0.0625, 
    0.0675, 0.0725, 0.0775, 0.0825, 0.0875, 0.0925, 0.0975, 0.1025, 0.1075, 0.1125, 
    0.1175, 0.1225, 0.1275, 0.1325, 0.1375, 0.1425, 0.1475, 0.1525, 0.1575, 0.1625, 
    0.1675, 0.1725, 0.1775, 0.1825, 0.1875, 0.1925, 0.1975 };
 double p7130_d8x1y2_yval[] = { 111.9, 105.2, 104.7, 93.04, 90.16, 85.21, 80.83, 76.1, 71.69, 
    67.69, 63.34, 58.99, 55.4, 51.97, 50.26, 46.51, 44.59, 40.47, 39.09, 
    36.53, 34.33, 33.16, 30.52, 28.11, 27.24, 26.06, 23.66, 22.92, 21.0, 
    20.61, 18.72, 17.29, 17.15, 15.71, 14.46, 14.17 };
  double p7130_d8x1y2_yerr[] = { 0.9805, 0.9345, 0.8988, 0.8534, 0.8222, 0.7964, 0.7716, 0.7534, 0.714, 
    0.6994, 0.6664, 0.6443, 0.618, 0.5982, 0.584, 0.5752, 0.5512, 0.5325, 0.5169, 
    0.4919, 0.4789, 0.4692, 0.4554, 0.4312, 0.4217, 0.4154, 0.3925, 0.3882, 0.367, 
    0.3645, 0.349, 0.3286, 0.332, 0.323, 0.3062, 0.2975 };
 for(int it=0;it<36;it++){ // add systematic 2%
    double system = 0.02*p7130_d8x1y2_yval[it];
    p7130_d8x1y2_yerr[it] = sqrt(p7130_d8x1y2_yerr[it]*p7130_d8x1y2_yerr[it]+system*system);
  }
  int p7130_d8x1y2_numpoints = 36;
  expdata = TGraphErrors(p7130_d8x1y2_numpoints, p7130_d8x1y2_xval, p7130_d8x1y2_yval,0,p7130_d8x1y2_yerr);
    
      return 0;
  }
  if(fabs(plab-8.0)<1e-4 && sampl==2){
    //from http://durpdg.dur.ac.uk/view/ins56380
    cout<<"Data from BIRNBAUM 1969 — Anti-p-p elastic scattering at 8 and 16 gev/c, added system.err 10%"<<endl;
 double p7230_d3x1y1_xval[] = { 0.046, 0.0528, 0.0602, 0.068, 0.0763, 0.0851, 0.0943, 0.104, 0.1143, 
    0.1249, 0.1361, 0.1477, 0.1598, 0.1724, 0.1855, 0.199, 0.2131, 0.2218, 0.2374, 
    0.2535, 0.2702, 0.2874, 0.3051, 0.3234, 0.3422, 0.3615, 0.3813, 0.4017, 0.4226, 
    0.4441, 0.5553, 0.5815, 0.622, 0.678, 0.736, 0.797, 0.86 };
 
  double p7230_d3x1y1_yval[] = { 93.3, 84.42, 76.99, 70.8, 63.58, 56.9, 50.95, 44.84, 39.19, 
    33.9, 29.15, 25.92, 21.77, 18.3, 15.71, 12.88, 10.8, 11.09, 9.244, 
    7.222, 5.944, 4.711, 3.544, 2.809, 2.153, 1.71, 1.425, 0.9811, 0.7602, 
    0.5716, 0.2021, 0.1538, 0.09534, 0.08978, 0.09536, 0.0985, 0.09622 };
  double p7230_d3x1y1_yerr[] = { 0.4275, 0.4071, 0.3891, 0.3735, 0.3543, 0.3355, 0.3179, 0.2984, 0.2795, 
    0.2602, 0.3292, 0.3108, 0.2853, 0.2621, 0.2433, 0.2209, 0.2027, 0.2891, 0.2645, 
    0.2351, 0.2139, 0.1913, 0.1667, 0.1493, 0.1317, 0.1186, 0.1092, 0.09165, 0.08132, 
    0.072, 0.03753, 0.03207, 0.02194, 0.02134, 0.02086, 0.0205, 0.01962 };
 for(int it=0;it<37;it++){ // add systematic 10%
    double system = 0.1*p7230_d3x1y1_yval[it];
    p7230_d3x1y1_yerr[it] = sqrt(p7230_d3x1y1_yerr[it]*p7230_d3x1y1_yerr[it]+system*system);
  }
  int p7230_d3x1y1_numpoints = 37;
  expdata = TGraphErrors(p7230_d3x1y1_numpoints, p7230_d3x1y1_xval, p7230_d3x1y1_yval,0,p7230_d3x1y1_yerr);
    
    return 0;
  }
  if(fabs(plab-16.0)<1e-4 && sampl==2){
  //from http://durpdg.dur.ac.uk/view/ins56380
    cout<<"Data from BIRNBAUM 1969 — Anti-p-p elastic scattering at 8 and 16 gev/c, added system.err 10%"<<endl;
  double p7230_d4x1y1_xval[] = { 0.111, 0.1328, 0.1568, 0.1823, 0.2101, 0.2397, 0.2713, 0.3049, 0.3405, 
    0.378, 0.4174, 0.4588, 0.5022, 0.5476, 0.5948, 0.6441, 0.6953, 0.7485, 0.797, 
    0.918, 1.047, 1.185, 1.331 };
  double p7230_d4x1y1_yval[] = { 28.51, 22.28, 16.89, 12.41, 8.86, 6.407, 4.503, 2.979, 1.975, 
    1.267, 0.8033, 0.4747, 0.2441, 0.2007, 0.1316, 0.1092, 0.06666, 0.05836, 0.03413, 
    0.05253, 0.03808, 0.01859, 0.001358 };
  double p7230_d4x1y1_yerr[] = { 0.3018, 0.2674, 0.2334, 0.2008, 0.1702, 0.145, 0.1221, 0.09988, 0.08172, 
    0.059, 0.04759, 0.03669, 0.02786, 0.02505, 0.02008, 0.01885, 0.01414, 0.01513, 0.0137, 
    0.0162, 0.01378, 0.009744, 0.004358 };
 for(int it=0;it<23;it++){ // add systematic 10%
    double system = 0.1*p7230_d4x1y1_yval[it];
    p7230_d4x1y1_yerr[it] = sqrt(p7230_d4x1y1_yerr[it]*p7230_d4x1y1_yerr[it]+system*system);
  }
  int p7230_d4x1y1_numpoints = 23;
  expdata = TGraphErrors(p7230_d4x1y1_numpoints, p7230_d4x1y1_xval, p7230_d4x1y1_yval,0,p7230_d4x1y1_yerr);
  return 0;
  }
  return 1;
 // if(fabs(plab-)<1e-4){
  //   expdata = 
  //   return 0;
  // }
}

int main(){
  gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/tests/ModelUncert/rootlogon.C");
  double zoomLim=0.26;
  // double zoomLim=0.0062;
  //double zoomLim=0.5;
  //double zoomLim=1.;
  //  double plab = 6.2;
  // double plab = 8.0;

  //Compares DPM and E760 models with Data. 
  //Models uncertanties are calculated from fit of energy dependence of model parameters

  //following data is included:

  //from http://durpdg.dur.ac.uk/view/ins109426
  //double plab = 1.5;
  // double plab = 1.6;
  //double plab = 1.71;
  //double plab = 1.81;
  //double plab = 1.86;
  // double plab = 1.91;
  // double plab = 2.01;
  //double plab = 2.12;
  // double plab = 2.23;
  // double plab = 2.43;
  // double plab = 2.33;

  //from http://durpdg.dur.ac.uk/view/ins93330
  // double plab = 2.33;

  //from http://durpdg.dur.ac.uk/view/ins99005
  //double plab = 1.776;
  //double plab = 2.607;

  //from http://durpdg.dur.ac.uk/view/ins93404
  //double plab = 2.85;

  //from http://durpdg.dur.ac.uk/view/ins92992
  //double plab = 3.0;
  // double plab = 3.65;
  //double plab = 5.0;

  //from http://durpdg.dur.ac.uk/view/ins83926
  //double plab = 5.0;

  //from http://durpdg.dur.ac.uk/view/ins56106
  // double plab = 3.55;

  //from http://durpdg.dur.ac.uk/view/ins431921
  //double plab = 3.7;
 //  double plab = 4.07;
 //double plab = 5.6;
 //double plab = 5.72;
 //double plab = 5.94;
 //double plab = 6.23;

 //from http://durpdg.dur.ac.uk/view/ins120467
 // double plab = 4.2;
 //double plab = 6.0;
 //double plab = 8.0;
 // double plab = 10.0;

  //from http://durpdg.dur.ac.uk/view/ins124704
  //double plab = 8.0;
  //double plab = 16.0;

  //from http://durpdg.dur.ac.uk/view/ins99557
  //double plab = 6.2;

  //from http://durpdg.dur.ac.uk/view/ins2106
  // double plab = 5.7;

  //from http://durpdg.dur.ac.uk/view/ins100639
  //double plab = 10.4;

  //from http://durpdg.dur.ac.uk/view/ins133174
  //double plab = 10.1;

  //from http://durpdg.dur.ac.uk/view/ins56380
  //plab given in range. middle value is taken!
  //double plab = 8.0;
  double plab = 16.0;
  // //one data sample
  TGraphErrors expdata;
  //int resData = ExpDataBase(plab,expdata);
  // expdata.Print();
  // // //2nd data sample
  // TGraphErrors expdata;
  int resData = ExpDataBase(plab,expdata,2);
  // if(resData!=0) return 1;
  // resData = ExpDataBase(plab,expdata1,0);
  

  //E760-like model with sigmaTot, B and rho parameters; 
  double parsigT,parb,parrho;
  double errparsigT,errparb,errparrho;
  E760fit(plab,parsigT,parb,parrho, 
	  errparsigT,errparb, errparrho);
  double parE760[4],errparE760[4];
  //par[3]={sigT,b,rho,dsig/dt}
  parE760[0] = parsigT; parE760[1] =  parb; parE760[2] = parrho;
  errparE760[0] = errparsigT; errparE760[1] =  errparb; errparE760[2] = errparrho;
  // cout<<parsigT<<" +/- "<<errparsigT<<" "<<parb<<" +/- "<<errparb<<" "<<parrho<<" +/- "<<errparrho<<endl;
  LumiFit::PndLmdFitModelOptions model_optionsE760;
  model_optionsE760.dpm_elastic_parts = LumiFit::ALL_RHO_B_SIGTOT;
  //PndLmdDPMAngModel1D modelE760("e760_angular_1d", model_optionsE760.dpm_elastic_parts);
  PndLmdDPMMTModel1D modelE760("e760_mt_1d", model_optionsE760.dpm_elastic_parts);
  shared_ptr<Parametrization> paraE760(new PndLmdE760LikeModelParametrization(modelE760.getModelParameterSet()));
  //  shared_ptr<Parametrization> para(new PndLmdE760ModelParametrization(modelE760.getModelParameterSet()));
  modelE760.getModelParameterHandler().registerParametrizations(
  							    modelE760.getModelParameterSet(), paraE760);
  modelE760.getModelParameterSet().setModelParameterValue("p_lab", plab);
  modelE760.getModelParameterSet().setModelParameterValue("luminosity", 1);
  ((Model1D*) &modelE760)->init();
  cout<<modelE760.getSigmaTotal()<<" "<<modelE760.getB()<<" "<<modelE760.getRho()<<endl;

  //DPM model with A1,A2,A3,t1 and t2 parameters; 
  double para1, para2, para3, part1, part2;
  double errpara1, errpara2, errpara3, errpart1, errpart2;
  DPMfit(plab, para1, para2, para3, part1, part2,
	 errpara1, errpara2, errpara3, errpart1, errpart2);
//par[5]={A1,A2,A3,t1,t2}
  double parDPM[5],errparDPM[5];
  parDPM[0] =  para1;   parDPM[1] =  para2;   parDPM[2] =  para3;   parDPM[3] =  part1;   parDPM[4] =  part2;   
  errparDPM[0] =  errpara1;   errparDPM[1] =  errpara2;   errparDPM[2] =  errpara3;   errparDPM[3] =  errpart1;   errparDPM[4] =  errpart2;   
 // cout<<para1<<" "<<para2<<" "<<para3<<" "<<part1<<" "<<part2<<endl;
  // cout<<errpara1<<" "<<errpara2<<" "<<errpara3<<" "<<errpart1<<" "<<errpart2<<endl;
 LumiFit::PndLmdFitModelOptions model_options;
 //PndLmdDPMAngModel1D modelDPM("dpm_angular_1d", LumiFit::ALL);
 PndLmdDPMMTModel1D modelDPM("dpm_mt_1d", LumiFit::ALL);
 shared_ptr<Parametrization> para(
				  new PndLmdDPMModelParametrization(modelDPM.getModelParameterSet()));
 modelDPM.getModelParameterHandler().registerParametrizations(
							      modelDPM.getModelParameterSet(), para);
 modelDPM.getModelParameterSet().setModelParameterValue("p_lab", plab);
 modelDPM.getModelParameterSet().setModelParameterValue("luminosity", 1);
 ((Model1D*) &modelDPM)->init();
 Double_t* trange_exp = expdata.GetX();
 int Nexppoints = expdata.GetN();
 //if data in t -------------
 double t_dw = trange_exp[0];
 double t_up = trange_exp[Nexppoints-1];
 //-------------------------

 // //if data for cos(theta) ---
 //PndLmdLumiHelper *lmd_help  = new PndLmdLumiHelper();
 // //radians = degrees x PI / 180 
 // double th_dw = TMath::Pi()*TMath::ACos(trange_exp[Nexppoints-1])/180.;
 // double t_dw = (-1)*(lmd_help->getMomentumTransferFromTheta(plab, th_dw));
 // double th_up = TMath::Pi()*TMath::ACos(trange_exp[0])/180.;
 // double t_up = (-1)*(lmd_help->getMomentumTransferFromTheta(plab, th_up));
 // //--------------------------
 const int nst = 7*Nexppoints;
 //const int nst = 10;
 double csDPM_val[nst], csDPM_uncert[nst];//dpm
 double csE760_val[nst], csE760_uncert[nst];//dpm
 double th_val[nst]; 
 double t_val[nst];
 for(int i=0;i<nst;i++){
    double dt = (t_up-t_dw)/nst;
    double t_cur =  t_dw +i*dt;
    t_val[i] = t_cur;
    th_val[i] = thLAB_t(plab, t_cur);
    // double t_test = (lmd_help->getMomentumTransferFromTheta(plab, th_val[i]));
    // cout<<" t_test "<<t_test<<" "<<t_val[i]<<endl;
    //    cs_val[i] = modelDPM.getRawFullElastic(&t_val[i]);
    csDPM_val[i] = modelDPM.getRawFullElastic(&t_cur);
    //  cout<<"HADRONIC PART  = "<<modelDPM.getRawHadronicPart(&t_cur)<<endl;
    csDPM_uncert[i] = DfDPM(t_cur,parDPM,errparDPM);
    // csDPM_uncert[i] = 0; //TEST
    csE760_val[i] = modelE760.getRawRhoBSigtotFullElastic(&t_cur);
    parE760[3] = csE760_val[i];
    csE760_uncert[i] = DfE760(t_cur,parE760,errparE760);
    //  cout<<"DPM: "<<csDPM_val[i]<<" +/- "<<csDPM_uncert[i]<<" E760: "<<csE760_val[i]<<" +/- "<<csE760_uncert[i]<<endl;
 }
 double diffDPM[Nexppoints], diffE760[Nexppoints];
 double undiffDPM[Nexppoints], undiffE760[Nexppoints];
 Double_t* cs_exp = expdata.GetY();
 Double_t* errcs_exp = expdata.GetEY();
 for(int i=0;i<Nexppoints;i++){
   //if data in t -------------
   double t_cur = trange_exp[i];
   //--------------------------
   // //if data for cos(theta) ---
   // double th_cur = TMath::Pi()*TMath::ACos(trange_exp[i])/180.;
   // double t_cur = (-1)*(lmd_help->getMomentumTransferFromTheta(plab, th_cur));
   // //--------------------------
   double dpm_val = modelDPM.getRawFullElastic(&t_cur);
   double dpm_un = DfDPM(t_cur,parDPM,errparDPM);
   //   double dpm_un = 0;
   diffDPM[i] = 100.*(cs_exp[i]-dpm_val)/cs_exp[i];
   double un1 = 100.*dpm_val*errcs_exp[i]/(cs_exp[i]*cs_exp[i]);
   double un2 = 100.*dpm_un/cs_exp[i];
   undiffDPM[i] = sqrt(un1*un1+un2*un2);
   double e760_val = modelE760.getRawRhoBSigtotFullElastic(&t_cur);
   double e760_un = DfE760(t_cur,parE760,errparE760);
   diffE760[i] = 100.*(cs_exp[i]-e760_val)/cs_exp[i];
   un1 = 100.*e760_val*errcs_exp[i]/(cs_exp[i]*cs_exp[i]);
   un2 = 100.*e760_un/cs_exp[i];
   undiffE760[i] = sqrt(un1*un1+un2*un2);
   cout<<t_cur<<" diffE760[i] = "<<diffE760[i]<<" "<<cs_exp[i]<<"- "<<e760_val<<" undiffE760[i] = "<<undiffE760[i]<<endl;
 }


 TGraphErrors *gr_csDPM_t = new TGraphErrors(nst,t_val,csDPM_val,0,csDPM_uncert);
 gr_csDPM_t->SetMarkerColor(kBlue+1);
 gr_csDPM_t->SetLineColor(kBlue+1);
 gr_csDPM_t->SetMarkerStyle(21);
 gr_csDPM_t->SetMarkerSize(0.4);
 TGraphErrors *gr_csE760_t = new TGraphErrors(nst,t_val,csE760_val,0,csE760_uncert);
 gr_csE760_t->SetMarkerColor(kAzure+1);
 gr_csE760_t->SetLineColor(kAzure+1);
 gr_csE760_t->SetMarkerStyle(21);
 gr_csE760_t->SetMarkerSize(0.4);
 TMultiGraph *mg_cs_t = new TMultiGraph();
 mg_cs_t->Add(gr_csDPM_t);
 mg_cs_t->Add(gr_csE760_t);
 TLegend *leg = new TLegend(0.7,0.6,0.95,0.95);
 TString head= "P_{lab} = ";
 TString plabStr;
 plabStr.Form("%2.2f\n",plab);
 head +=plabStr;
 leg->SetHeader(head);
 leg->SetFillColor(0);
 leg->SetTextFont(42);
 leg->SetTextSize(0.05);
 leg->AddEntry(gr_csDPM_t,"DPM","lpe");
 leg->AddEntry(gr_csE760_t,"E760-like","lpe");


 //Add exp data
 if(resData==0){
   //   expdata.SetMarkerColor(kSpring-1);
   //   expdata.SetMarkerColor(kOrange+8);
   expdata.SetMarkerStyle(20);
   expdata.SetMarkerSize(0.9);
   mg_cs_t->Add(&expdata);
   leg->AddEntry(&expdata,"exp. data","pe");
 }

 
 TGraphErrors *gr_uncsDPM_t = new TGraphErrors(Nexppoints,trange_exp,diffDPM,0,undiffDPM);
 gr_uncsDPM_t->SetMarkerColor(kBlue+1);
 gr_uncsDPM_t->SetLineColor(kBlue+1);
 gr_uncsDPM_t->SetMarkerStyle(21);
 gr_uncsDPM_t->SetMarkerSize(0.4);
TGraphErrors *gr_uncsE760_t = new TGraphErrors(Nexppoints,trange_exp,diffE760,0,undiffE760);
 gr_uncsE760_t->SetMarkerColor(kAzure+1);
 gr_uncsE760_t->SetLineColor(kAzure+1);
 gr_uncsE760_t->SetMarkerStyle(21);
 gr_uncsE760_t->SetMarkerSize(0.4);
 TMultiGraph *mg_uncs_t = new TMultiGraph();
 mg_uncs_t->Add(gr_uncsDPM_t);
 mg_uncs_t->Add(gr_uncsE760_t);

 TLegend *leg2 = new TLegend(0.73,0.8,0.95,0.95);
 // leg2->SetHeader(head);
 leg2->SetFillColor(0);
 leg2->SetTextFont(42);
 leg2->SetTextSize(0.05);
 leg2->AddEntry(gr_uncsDPM_t,"DPM","lpe");
 // leg2->AddEntry(gr_uncsE760_t,"E760-like","lpe");

 TLegend *leg3 = new TLegend(0.73,0.8,0.95,0.95);
 // leg3->SetHeader(head);
 leg3->SetFillColor(0);
 leg3->SetTextFont(42);
 leg3->SetTextSize(0.05);
 leg3->AddEntry(gr_uncsE760_t,"E760-like","lpe");

 TGraph *gr_t_th = new TGraph(nst,t_val,th_val);

 TCanvas cRES("cRES","canvas",600,800);
 cRES.Divide(2,3);
 cRES.cd(2);
 mg_cs_t->Draw("AP");
 mg_cs_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
 //mg_cs_t->GetXaxis()->SetLimits(-0.001,0.021);
 mg_cs_t->GetYaxis()->SetTitle("d#sigma/dt, mb/(GeV/c)^{2}");
 // mg_cs_t->GetXaxis()->SetLimits(-1e-6,0.25);
 leg->Draw();
 cRES.cd(1);
 gr_t_th->SetTitle("");
 gr_t_th->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
 gr_t_th->GetYaxis()->SetTitle("#theta^{#bar{p}}_{lab}, rad");
 gr_t_th->Draw("AL");
 // gr_t_th->GetXaxis()->SetLimits(-1e-6,0.25);
 cRES.cd(5);
 TMultiGraph *mg_cs_t2 = new TMultiGraph();
 mg_cs_t2->Add(gr_csDPM_t);
 mg_cs_t2->Add(gr_csE760_t);
 mg_cs_t2->Add(&expdata);
 mg_cs_t2->Draw("AP");
 mg_cs_t2->GetXaxis()->SetRangeUser(0,zoomLim);
 mg_cs_t2->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
 mg_cs_t2->GetYaxis()->SetTitle("d#sigma/dt, mb/(GeV/c)^{2}");
 leg->Draw();
 cRES.cd(3);
 gr_uncsDPM_t->SetTitle("");
 gr_uncsDPM_t->Draw("AP");
 gr_uncsDPM_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
 gr_uncsDPM_t->GetYaxis()->SetTitle("((d#sigma/dt)_{exp} - (d#sigma/dt)_{model})/(d#sigma/dt)_{exp}, %");
 // gr_uncsDPM_t->GetXaxis()->SetLimits(-1e-6,0.25);
//mg_uncs_t->Draw("AP");
 leg2->Draw();
 // mg_uncs_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
 // mg_uncs_t->GetYaxis()->SetTitle("((d#sigma/dt)_{exp} - (d#sigma/dt)_{model})/(d#sigma/dt)_{exp}, %");
 cRES.cd(4);
 gr_uncsE760_t->SetTitle("");
 gr_uncsE760_t->Draw("AP");
 gr_uncsE760_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
 gr_uncsE760_t->GetYaxis()->SetTitle("((d#sigma/dt)_{exp} - (d#sigma/dt)_{model})/(d#sigma/dt)_{exp}, %");
 // gr_uncsE760_t->GetXaxis()->SetLimits(-1e-6,0.25);
 leg3->Draw();
 cRES.cd(6);
 mg_uncs_t->Draw("AP");
 mg_uncs_t->GetXaxis()->SetRangeUser(0,zoomLim);
 mg_uncs_t->GetYaxis()->SetRangeUser(-100,100);
 mg_uncs_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
 mg_uncs_t->GetYaxis()->SetTitle("((d#sigma/dt)_{exp} - (d#sigma/dt)_{model})/(d#sigma/dt)_{exp}, %");
 TLegend *leg4 = new TLegend(0.75,0.7,0.99,0.99);
 // leg4->SetHeader(head);
 leg4->SetFillColor(0);
 leg4->SetTextFont(42);
 leg4->SetTextSize(0.05);
 leg4->AddEntry(gr_uncsDPM_t,"DPM","lpe");
 leg4->AddEntry(gr_uncsE760_t,"E760-like","lpe");
 leg4->Draw();
 TString fname = "cs_and_uncert_DPM_E760like_vs_exp_";
 fname += plab;
 TString fnamepdf = fname + ".pdf";
 TString fnameroot = fname + ".root";
 cRES.SaveAs(fnamepdf);
 cRES.SaveAs(fnameroot);
 return 0;
}
