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

int main(){
  gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/test_macros/rootlogon.C");

  // part1: fit energy dependence of DPM parameters ---------------
  double mom[6]={3.70,4.07,5.60,5.72,5.94,6.23};
  //values for fixed sigma_total!
  double sigT[6]={70.3,68.0,61.3,60.9,60.2,59.4};
  double errsigT[6]={1e-2,1e-2,1e-2,1e-2,1e-2,1e-2};
  double b[6]={12.9,12.8,12.5,12.2,12.6,12.2};
  double errb[6]={0.4,0.7,0.3,0.4,0.3,0.6};
  double rho[6]={0.006,-0.007,-0.030,-0.018,-0.035,-0.029};
  double errrho[6]={0.008,0.012,0.007,0.008,0.008,0.010};
  // //values for free sigma_total!
  // double rho[6]={0.018,-0.015,-0.047,-0.051,-0.063,-0.006};
  // double errrho[6]={0.014,0.024,0.007,0.011,0.008,0.020};

  TCanvas c1("c1","canvas",1200,800);
  c1.Divide(3,2);
  // TCanvas c1("c1","canvas",600,800);
  // c1.Divide(1,2);
  TGraphErrors *grA1 = new TGraphErrors(6,mom,sigT,0,errsigT); 
  grA1->SetMarkerStyle(21);
  grA1->SetMarkerSize(0.9); 
  TF1 *fa1 = new TF1("fa1","[0]+[1]*TMath::Power(x,[2])",mom[0]-0.5,mom[5]+0.5);
  fa1->SetParNames("par0","par1","par2");
  fa1->SetParameter(0,34);
  fa1->SetParameter(1,89.7);
  fa1->SetParameter(2,-0.7);
  grA1->Fit(fa1,"R");

  TF1 *fa1_dpm = new TF1("fa1_dpm","[0]+[1]*TMath::Power(x,[2])",mom[0]-0.5,mom[5]+0.5);
  fa1_dpm->SetParNames("par0","par1","par2");
  fa1_dpm->SetParameter(0,34.48);
  fa1_dpm->SetParameter(1,89.7);
  fa1_dpm->SetParameter(2,-0.7);
  fa1_dpm->SetLineColor(4);
  //Create a TGraphErrors to hold the confidence intervals
  int np=1000;
  TGraphErrors *grint_a1 = new TGraphErrors(np);
  TGraphErrors *grdiff_a1 = new TGraphErrors(np);
  // grint->SetTitle("Fitted line with .95 conf. band");
  grint_a1->SetTitle("");
  grdiff_a1->SetTitle("");
  for (int i=0; i<np; i++){
    //    double pcur= mom[0]-1.0+((mom[5]+1.0)-(mom[0]-1.0))*i/np;
    double pcur= mom[0]-2.3+((mom[5]+8.9)-(mom[0]-2.3))*i/np;
    grint_a1->SetPoint(i, pcur, 0);
    double myfit = fa1->Eval(pcur);
    double dpmfit = fa1_dpm->Eval(pcur);
    double difffit = 100.*(dpmfit-myfit)/dpmfit;
    grdiff_a1->SetPoint(i, pcur, difffit);
  }
  //Compute the confidence intervals at the x points of the created graph
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(grint_a1,0.68);
  grint_a1->SetLineColor(2);
  grint_a1->SetLineWidth(3);
  grint_a1->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grint_a1->GetYaxis()->SetTitle("#sigma_{T}");
  grint_a1->SetFillColor(2);
  grint_a1->SetFillStyle(3005);

  //   TLegend *leg_a1 = new TLegend(0.62,0.35,0.85,0.55);
  TLegend *leg_a1 = new TLegend(0.72,0.55,0.68,0.75);
  //  TLegend *leg = new TLegend(0.22,0.15,0.37,0.35);
  leg_a1->SetFillColor(0);
  leg_a1->SetTextFont(42);
  leg_a1->SetTextSize(0.05);
  leg_a1->AddEntry(grA1,"data","ep");
  leg_a1->AddEntry(fa1_dpm,"E760 fit","l");
  leg_a1->AddEntry(fa1,"our fit","l");

  c1.cd(1);
  grint_a1->Draw("AP4");
  grA1->Draw("psame");
  fa1_dpm->Draw("same");
  leg_a1->Draw();
  c1.cd(4);
  //c1.cd(2);
  grdiff_a1->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grdiff_a1->GetYaxis()->SetTitle("(E760-fit)/E760, %");
  grdiff_a1->SetMarkerStyle(20);
  grdiff_a1->SetMarkerSize(0.5);
  grdiff_a1->Draw("AP");

  //A2
 TGraphErrors *grA2 = new TGraphErrors(6,mom,b,0,errb); 
  grA2->SetMarkerStyle(21);
  grA2->SetMarkerSize(0.9); 
  TF1 *fa2 = new TF1("fa2","[0]+[1]*x",mom[0]-0.5,mom[5]+0.5);
  fa2->SetParNames("par0","par1");
  fa2->SetParameter(0,13.64);
  fa2->SetParameter(1,-0.2);
  grA2->Fit(fa2,"R");

  TF1 *fa2_dpm = new TF1("fa2_dpm","[0]+[1]*x",mom[0]-0.5,mom[5]+0.5);
  fa2_dpm->SetParNames("par0","par1");
  fa2_dpm->SetParameter(0,13.64);
  fa2_dpm->SetParameter(1,-0.2);
  fa2_dpm->SetLineColor(4);
  //Create a TGraphErrors to hold the confidence intervals
  TGraphErrors *grint_a2 = new TGraphErrors(np);
  TGraphErrors *grdiff_a2 = new TGraphErrors(np);
  // grint->SetTitle("Fitted line with .95 conf. band");
  grint_a2->SetTitle("");
  grdiff_a2->SetTitle("");
  for (int i=0; i<np; i++){
    //    double pcur= mom[0]-1.0+((mom[5]+1.0)-(mom[0]-1.0))*i/np;
    double pcur= mom[0]-2.3+((mom[5]+8.9)-(mom[0]-2.3))*i/np;
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
  grint_a2->GetYaxis()->SetTitle("b");
  grint_a2->SetFillColor(2);
  grint_a2->SetFillStyle(3005);


  c1.cd(2);
  // c1.cd(1);
  grint_a2->Draw("AP4");
  grA2->Draw("psame");
  fa2_dpm->Draw("same");
  leg_a1->Draw();
  c1.cd(5);
  //c1.cd(2);
  grdiff_a2->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grdiff_a2->GetYaxis()->SetTitle("(E760-fit)/E760, %");
  grdiff_a2->SetMarkerStyle(20);
  grdiff_a2->SetMarkerSize(0.5);
  grdiff_a2->Draw("AP");

 //t2
 TGraphErrors *grT2 = new TGraphErrors(6,mom,rho,0,errrho); 
  grT2->SetMarkerStyle(21);
  grT2->SetMarkerSize(0.9); 
  TF1 *ft2 = new TF1("ft2","[0]+[1]*x",mom[0]-0.5,mom[5]+0.5);
  ft2->SetParNames("par0","par1");
  ft2->SetParameter(0,-0.12);
  ft2->SetParameter(1,0.03);
  grT2->Fit(ft2,"R");

  TF1 *ft2_dpm = new TF1("ft2_dpm","[0]+[1]*x",mom[0]-0.5,mom[5]+0.5);
  ft2_dpm->SetParNames("par0","par1");
  ft2_dpm->SetParameter(0,-0.12);
  ft2_dpm->SetParameter(1,0.03);
  ft2_dpm->SetLineColor(4);
  //Create a TGraphErrors to hold the confidence intervals
  TGraphErrors *grint_t2 = new TGraphErrors(np);
  TGraphErrors *grdiff_t2 = new TGraphErrors(np);
  // grint->SetTitle("Fitted line with .95 conf. band");
  grint_t2->SetTitle("");
  grdiff_t2->SetTitle("");
 
  //Compute the confidence intervals at the x points of the created graph
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(grint_t2,0.68);
  grint_t2->SetLineColor(kRed);
  grint_t2->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grint_t2->GetYaxis()->SetTitle("#rho");
  grint_t2->SetFillColor(2);
  grint_t2->SetFillStyle(3005);
 for (int i=0; i<np; i++){
    // double pcur= mom[0]-1.0+((mom[5]+1.0)-(mom[0]-1.0))*i/np;
    double pcur= mom[0]-2.3+((mom[5]+8.9)-(mom[0]-2.3))*i/np;
    grint_t2->SetPoint(i, pcur, 0);
    double myfit = ft2->Eval(pcur);
    double dpmfit = ft2_dpm->Eval(pcur);
    double difffit = 100.*(dpmfit-myfit)/dpmfit;
    grdiff_t2->SetPoint(i, pcur, difffit);
    // double y = grint_t2->GetErrorY(i);
    // double un = TMath::Sqrt(TMath::Power(dpmfit-myfit,2)+TMath::Power(y,2));
    // cout<<"OLD: "<<y<<" NEW: "<<un<<endl;
    // grint_t2->SetPointError(i, 0, un);//!TEST
  }

  c1.cd(3);
  // c1.cd(1);
  grint_t2->Draw("AP4");
  grT2->Draw("psame");
  ft2_dpm->Draw("same");
  leg_a1->Draw();
  c1.cd(6);
  // c1.cd(2);
  grdiff_t2->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  grdiff_t2->GetYaxis()->SetTitle("(E760-fit)/E760, %");
  grdiff_t2->SetMarkerStyle(20);
  grdiff_t2->SetMarkerSize(0.5);
  grdiff_t2->SetMinimum(-9e2);
  grdiff_t2->SetMaximum(9e2);
  grdiff_t2->Draw("AP");


  c1.SaveAs("E760_fit.pdf");
  c1.SaveAs("E760_fit.root");

  //[end] part1: fit energy dependence of E760 parameters ---------------------

  // part2: calculate differencial cross-section and its uncertanty ----------


  double Plab_min=1.5;
   double Plab_max=15;
   int Plabstep= 30;
  // double Plab_min=4;
  // double Plab_max=5;
  // int Plabstep= 2;
  TGraph *resUn2mrad = new TGraph(Plabstep);
  resUn2mrad->SetMarkerStyle(20);
  resUn2mrad->SetMarkerColor(32);
  resUn2mrad->SetMarkerSize(2.0);
  TGraph *resUn5mrad = new TGraph(Plabstep);
  resUn5mrad->SetMarkerStyle(21);
  resUn5mrad->SetMarkerColor(46); resUn5mrad->SetMarkerSize(1.7);
  TGraph *resUn10mrad = new TGraph(Plabstep);
  resUn10mrad->SetMarkerStyle(22);
  resUn10mrad->SetMarkerColor(38);  resUn10mrad->SetMarkerSize(1.5);
  for(int pstep=0;pstep<=Plabstep;pstep++){
    double plab = Plab_min+pstep*(Plab_max-Plab_min)/Plabstep;
  //get patameters for DPM model from the fit
  // double plab = 10.40;
//par[3]={sigT,b,rho,dsig/dt}
  double par[4],errpar[4];
  par[0] = fa1->Eval(plab);
  par[1] = fa2->Eval(plab);
  par[2] = ft2->Eval(plab);
  // grint_a1->Print();
  for(int i=0;i<grint_a1->GetN();i++){
  //  for(int i=0;i<10;i++){
    double xval,yval;
    grint_a1->GetPoint(i,xval,yval);
    //  cout<<par[0]<<" "<<yval<<endl;
    double errpar_0 = grint_a1->GetErrorY(i);
   
    //  if(fabs(xval-plab)<1e-1 && fabs(par[0]-yval)<1e-1){
    //    cout<<par[0]<<" "<<yval<<endl;
    //    cout<<"xval="<<xval<<" fabs(par[0]-yval)="<<fabs(par[0]-yval)<<" 0.2*errpar_0="<<0.2*errpar_0<<endl;
    if(fabs(xval-plab)<0.1 && fabs(par[0]-yval)<5*errpar_0){
    //  if(fabs(xval-plab)<1){
    //  cout<<" errpar_0 = "<<errpar_0<<endl;
      errpar[0] = grint_a1->GetErrorY(i);
      errpar[1] = grint_a2->GetErrorY(i);
      errpar[2] = grint_t2->GetErrorY(i);
    }
    //    grint_a1->Print();
  }
  cout<<"E760-like Params for Plab="<<plab<<endl;
  for(int jdpm=0;jdpm<3;jdpm++){
    cout<<par[jdpm]<<" +- "<<errpar[jdpm]<<endl;
  }
  LumiFit::PndLmdFitModelOptions model_options;
  model_options.momentum_transfer_active = true; // will calc cross-section as a func from (t)
  //model_options.momentum_transfer_active = false; // will calc cross-section as a func from (theta), theta in rad
  model_options.acceptance_correction_active = false;
  model_options.momentum_transfer_active = false;
  model_options.resolution_smearing_active = false;
  PndLmdModelFactory model_factory;
  shared_ptr<Model1D> model1d = model_factory.generate1DModel(model_options, plab);


  PndLmdDPMAngModel1D modelDPM("dpm_angular_1d", LumiFit::ALL);
  shared_ptr<Parametrization> para(new PndLmdE760LikeModelParametrization(modelDPM.getModelParameterSet()));
  //  shared_ptr<Parametrization> para(new PndLmdE760ModelParametrization(modelDPM.getModelParameterSet()));
  modelDPM.getModelParameterHandler().registerParametrizations(
  							    modelDPM.getModelParameterSet(), para);
  modelDPM.getModelParameterSet().setModelParameterValue("p_lab", plab);
  modelDPM.getModelParameterSet().setModelParameterValue("luminosity", 1);
  // modelDPM->init();
  ((Model1D*) &modelDPM)->init();
// double th_dw = 1.04719994E-03;//rad -> 0.06 DPM
  double th_dw = 0.0015;//
  //  double th_dw = 0.003;//
 //double th_up = TMath::Pi();
  // double th_up = 0.02;
  double th_up = 0.015;
  const int nst = 2e3;
  //    const int nst = 2e0;
  double cs_loc = 0;
  double cs_val[nst], cs_uncert[nst];//dpm
  double csE760_val[nst], csE760_uncert[nst];//dpm
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
    cs_val[i] = modelDPM.getRawRhoBSigtotFullElastic(&t_cur);
    par[3] = cs_val[i];
    //  cout<<"F = "<<par[3]<<" rho = "<<par[2]<<" +/- "<<errpar[2]<<endl;
    cs_uncert[i] = Df(t_cur,par,errpar);
    rel_err[i] = 100*(cs_uncert[i]/cs_val[i]);
   
    if(fabs(th_val[i]-2.)<1e-3){ 
      resUn2mrad->SetPoint(pstep, plab, rel_err[i]);
      //    cout<<th_val[i]<<endl;
    }
    if(fabs(th_val[i]-5.)<1e-2){
      resUn5mrad->SetPoint(pstep, plab, rel_err[i]);
      //   cout<<th_val[i]<<endl;
    }
    if(fabs(th_val[i]-10.)<1e-2){
      resUn10mrad->SetPoint(pstep, plab, rel_err[i]);
      //   cout<<th_val[i]<<endl;
    }
    // rel_err[i] = 100*(cs_uncert[i]/cs_val[i]);
    // //TEST: calculate total cross-section
    // vector<pair<double, double> > range;
    // range.push_back(make_pair(th1,th2));
    // double dcs =  model1d->Integral(range,1e-6);
    // cs_loc += dcs;
    // cout<<"cs_val = "<<cs_val[i]<<" +/- "<<cs_uncert[i]<<endl;
  }

  TGraphErrors *gr_cs_th = new TGraphErrors(nst,th_val,cs_val,0,cs_uncert);
  // gr_cs_th->SetTitle("P_{beam} = 2.33 GeV/c");
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
  gr_cs_un_th->GetYaxis()->SetTitle("#Delta(d#sigma/dt), mb/(GeV/c)^{2}");

  TGraph *gr_cs_un_t = new TGraph(nst,t_val,cs_uncert);
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
  gr_cs_un_t->Draw("AL");
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
  }
  TCanvas c3("c3","canvas",800,600);
  TMultiGraph *mg_res = new TMultiGraph();
  mg_res->Add(resUn2mrad);
  mg_res->Add(resUn5mrad);
  mg_res->Add(resUn10mrad);

  TLegend *leg = new TLegend(0.27,0.65,0.53,0.85);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.05);
  leg->AddEntry(resUn2mrad,"#theta = 2 mrad","p");
  leg->AddEntry(resUn5mrad,"#theta = 5 mrad","p");
  leg->AddEntry(resUn10mrad,"#theta = 10 mrad","p");
  mg_res->Draw("AP");
  mg_res->SetMinimum(-1);
  //  mg_res->SetMaximum(10);
  mg_res->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  mg_res->GetYaxis()->SetTitle("#Delta(d#sigma/dt)/(d#sigma/dt), %");
  leg->Draw();
  c3.SaveAs("uncert_vs_Plab.pdf");
  c3.SaveAs("uncert_vs_Plab.root");
  return 0;
}
