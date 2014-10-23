// macro for calculation DPM model in uncertainty in LMD range
//include <PndLmdDPMAngModel1D.h>
#include <PndLmdLumiFitOptions.h>
#include <PndLmdModelFactory.h>
#include <Model1D.h>
#include <PndLmdLumiHelper.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TAxis.h>
#include <iostream>
#include <TMath.h>
#include <utility>
#include <vector>
using namespace std;
//par[5]={A1,A2,A3,t1,t2}

double df_dA1(double &t, double par[]){
  double A1 = par[0];
  double A2 = par[1];
  double A3 = par[2];
  double t1 = par[3];
  double t2 = par[4];
  double int_res = TMath::Exp(t/(2*t1))-A2*TMath::Exp(t/(2*t2));
  double res = TMath::Power(int_res,2);
  return res;
}
double df_dA2(double &t, double par[]){
  double A1 = par[0];
  double A2 = par[1];
  double A3 = par[2];
  double t1 = par[3];
  double t2 = par[4];
  double res = -2*A1*TMath::Exp(t/(2*t1)+t/(2*t2))+2*A1*A2*TMath::Exp(t/t2);
  return res;
}

double df_dA3(double &t, double par[]){
  double A1 = par[0];
  double A2 = par[1];
  double A3 = par[2];
  double t1 = par[3];
  double t2 = par[4];
  double res = TMath::Exp(t/t2);
  return res;
}

double df_dt1(double &t, double par[]){
  double A1 = par[0];
  double A2 = par[1];
  double A3 = par[2];
  double t1 = par[3];
  double t2 = par[4];
  double res = A1*(-TMath::Exp(t/t1)+A2*TMath::Exp(t/(2*t1)+t/(2*t2)))*(t/(t1*t1));
  return res;
}

double df_dt2(double &t, double par[]){
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

int main(){
  //only final DPM fit is considered  (p.7)
  double mom = 2.33;
  double par[5] = {582,0.196,2.72,0.0899,0.322};
  double errpar[5] = {17,0.008,0.38,0.006,0.012}; //warning: t1 is fixed param, the largest uncertainty is taken from paper

  // double mom = 2.85;
  // double par[5] = {426,0.153,1.78,0.0899,0.394};
  // double errpar[5] = {8.5,0.005,0.19,0.006,0.012}; //warning: t1 is fixed param, the largest uncertainty is taken from paper
  
 // double mom = 3.55;
 //  double par[5] = {382,0.137,1.33,0.0899,0.392};
 //  double errpar[5] = {50.2,0.021,0.42,0.006,0.06}; //warning: t1 is fixed param, the largest uncertainty is taken from paper

  // double mom = 5.7;
  // double par[5] = {232,0.11,0.77,0.0899,0.351};
  // double errpar[5] = {7.4,0.01,0.2,0.006,0.028}; //warning: t1 is fixed param, the largest uncertainty is taken from paper

  // double mom = 10.40;
  // double par[5] = {171,0.074,0.89,0.0899,0.293};
  // double errpar[5] = {1.9,0.004,0.12,0.006,0.015}; //warning: t1 is fixed param, the largest uncertainty is taken from paper


  // double mom = 15.95;
  // double par[5] = {113,0.049,0.69,0.0899,0.049};
  // double errpar[5] = {4.2,0.013,0.32,0.006,0.049}; //warning: t1 is fixed param, the largest uncertainty is taken from paper

  /// set up LmdFit for calculation t out of theta --------

  LumiFit::PndLmdFitModelOptions model_options;
  // model_options.momentum_transfer_active = true; // will calc cross-section as a func from (t)
  model_options.momentum_transfer_active = false; // will calc cross-section as a func from (theta), theta in rad

  PndLmdModelFactory model_factory;
  shared_ptr<Model1D> model1d = model_factory.generate1DModel(model_options, mom);


  // double th_dw = 1.04719994E-03;//rad -> 0.06 DPM
  double th_dw = 0.002;//
 //double th_up = TMath::Pi();
  double th_up = 0.02;
  const int nst = 2e3;
  double cs_loc = 0;
  double cs_val[nst], cs_uncert[nst];
  double rel_err[nst];
  double th_val[nst],  t_val[nst];
  for(int i=0;i<nst;i++){
    double dth = (th_up-th_dw)/nst;
    double theta =  th_dw +i*dth;
    double th1 = theta;
    double th2 = theta + dth;
    PndLmdLumiHelper *lmd_help  = new PndLmdLumiHelper();
    double t_cur = (-1)*(lmd_help->getMomentumTransferFromTheta(mom, th1));
    t_val[i] = (-1)*t_cur;
    //  cout<<"t_cur = "<<t_cur<<endl;
    th_val[i] = 1e3*th1;
    cs_val[i] = model1d->eval(&th1);
    cs_uncert[i] = Df(t_cur,par,errpar);
    rel_err[i] = 100*(cs_uncert[i]/cs_val[i]);
    // //TEST: calculate total cross-section
    // vector<pair<double, double> > range;
    // range.push_back(make_pair(th1,th2));
    // double dcs =  model1d->Integral(range,1e-6);
    // cs_loc += dcs;
    // cout<<"cs_val = "<<cs_val[i]<<" +/- "<<cs_uncert[i]<<endl;
  }

  TGraphErrors *gr_cs_th = new TGraphErrors(nst,th_val,cs_val,0,cs_uncert);
  gr_cs_th->SetTitle("P_{beam} = 2.33 GeV/c");
  gr_cs_th->GetXaxis()->SetTitle("#theta, mrad");
  gr_cs_th->GetYaxis()->SetTitle("d#sigma/dt, mb/(GeV/c)^{2}");
  gr_cs_th->SetFillColor(4);
  gr_cs_th->SetFillStyle(3001);

  TGraphErrors *gr_cs_t = new TGraphErrors(nst,t_val,cs_val,0,cs_uncert);
  gr_cs_t->SetTitle("");
  gr_cs_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  gr_cs_t->GetYaxis()->SetTitle("d#sigma/dt, mb/(GeV/c)^{2}");
  gr_cs_t->SetFillColor(4);
  gr_cs_t->SetFillStyle(3001);

  TGraph *gr_cs_un_th = new TGraph(nst,th_val,cs_uncert);
  gr_cs_un_th->SetTitle("");
  gr_cs_un_th->GetXaxis()->SetTitle("#theta, mrad");
  gr_cs_un_th->GetYaxis()->SetTitle("#Delta(d#sigma/dt, mb/(GeV/c)^{2})");

  TGraph *gr_cs_un_t = new TGraph(nst,t_val,cs_uncert);
  gr_cs_un_t->SetTitle("");
  gr_cs_un_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  gr_cs_un_t->GetYaxis()->SetTitle("#Delta(d#sigma/dt, mb/(GeV/c)^{2})");

 TGraph *gr_cs_relun_th = new TGraph(nst,th_val,rel_err);
 gr_cs_relun_th->SetTitle("");
  gr_cs_relun_th->GetXaxis()->SetTitle("#theta, mrad");
  gr_cs_relun_th->GetYaxis()->SetTitle("#Delta(d#sigma/dt)/(d#sigma/dt), %");

  TGraph *gr_cs_relun_t = new TGraph(nst,t_val,rel_err);
  gr_cs_relun_t->SetTitle("");
  gr_cs_relun_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  gr_cs_relun_t->GetYaxis()->SetTitle("#Delta(d#sigma/dt)/(d#sigma/dt), %");

  TCanvas c1;
  c1.Divide(2,3);
  c1.cd(1);
  gr_cs_th->Draw("AP4");
  c1.cd(2);
  gr_cs_t->Draw("AP4");
  c1.cd(3);
  gr_cs_un_th->Draw("AL");
  c1.cd(4);
  gr_cs_un_t->Draw("AL");
  c1.cd(5);
  gr_cs_relun_th->Draw("AL");
  c1.cd(6);
  gr_cs_relun_t->Draw("AL");
  c1.SaveAs("cs_and_uncert_2_33.pdf");
  c1.SaveAs("cs_and_uncert_2_33.root");
  return 0;
}
