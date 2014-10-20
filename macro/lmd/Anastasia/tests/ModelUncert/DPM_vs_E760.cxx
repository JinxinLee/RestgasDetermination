// macro for comparision DPM model and E760 model with their uncertainty in LMD range
#include <model/PndLmdDPMAngModel1D.h>
#include <model/PndLmdDPMMTModel1D.h>
#include <model/PndLmdDPMModelParametrization.h>
#include <model/PndLmdE760ModelParametrization.h>
#include <model/PndLmdE760LikeModelParametrization.h>
//#include <PndLmdLumiFitOptions.h>
#include <model/PndLmdModelFactory.h>
//#include <Model1D.h>
#include <PndLmdLumiHelper.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TGraphAsymmErrors.h>
#include <TAxis.h>
#include <iostream>
#include <TMath.h>
#include <utility>
#include <vector>
#include <TROOT.h>
#include <TLegend.h>
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
  gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/test_macros/rootlogon.C");
  // Data from E760 ----------
 
// // // Plot: p7839_d4x1y1
//   double mom = 3.7;
//   double p7839_d4x1y1_xval[] = { 8.701E-4, 8.701E-4, 9.218E-4, 0.001122, 0.001162, 0.00145, 0.001566, 0.001671, 0.001924, 
//     0.002204, 0.002525, 0.002886, 0.003126, 0.003607, 0.004003, 0.004409, 0.004884, 0.005337, 0.005743, 
//     0.006292, 0.006787, 0.007261, 0.007831, 0.008411, 0.008991, 0.00963, 0.0103, 0.0109, 0.01154, 
//     0.01241, 0.01306, 0.01373, 0.01449, 0.01537, 0.01613 };
 
//   double p7839_d4x1y1_yval[] = { 546.9, 524.0, 518.0, 469.4, 454.7, 389.7, 353.1, 349.0, 323.8, 
//     307.3, 289.0, 275.3, 270.9, 263.1, 260.0, 258.5, 256.9, 252.4, 247.8, 
//     243.2, 240.6, 240.2, 236.4, 234.1, 232.5, 229.5, 228.1, 224.9, 223.4, 
//     223.4, 217.6, 217.3, 214.2, 212.7, 209.6 };

//   int p7839_d4x1y1_numpoints = 35;
//   TGraph* p7839_d4x1y1 = new TGraph(p7839_d4x1y1_numpoints, p7839_d4x1y1_xval, p7839_d4x1y1_yval);
//   p7839_d4x1y1->SetName("exp_data_e760_3_7GeV");
//   p7839_d4x1y1->SetTitle("E760 data, P$_{lab}$ = 3.7");
//   p7839_d4x1y1->SetMarkerStyle(29);
//   p7839_d4x1y1->SetMarkerSize(0.7);
//   p7839_d4x1y1->SetMarkerColor(8);

// // Plot: p7839_d5x1y1
// double mom = 4.07;
// double p7839_d5x1y1_xval[] = { 0.001015, 0.001183, 0.001454, 0.001729, 0.002007, 0.002372, 0.002661, 0.003008, 0.003382, 
//     0.003803, 0.004182, 0.004651, 0.005116, 0.005666, 0.006016, 0.006537, 0.007061, 0.007606, 0.008177, 
//     0.008793, 0.00943, 0.01008, 0.01074, 0.0114, 0.01198, 0.01283, 0.0136, 0.01435, 0.01512, 
//     0.01607, 0.01683 };
//  double p7839_d5x1y1_yval[] = { 489.8, 404.6, 354.2, 319.1, 293.2, 275.1, 261.9, 248.8, 242.7, 
//     241.2, 239.8, 235.4, 232.0, 229.9, 224.8, 224.4, 222.2, 222.1, 219.6, 
//     215.2, 214.4, 211.8, 209.1, 206.1, 207.6, 203.0, 200.0, 201.2, 196.9, 
//     193.9, 192.3 };
  
//   int p7839_d5x1y1_numpoints = 31;
//    TGraph* p7839_d5x1y1 = new TGraph(p7839_d5x1y1_numpoints, p7839_d5x1y1_xval, p7839_d5x1y1_yval);
//    p7839_d5x1y1->SetName("exp_data_e760_4_07GeV");
//   p7839_d5x1y1->SetTitle("E760 data, P$_{lab}$ = 4.07");
//   p7839_d5x1y1->SetMarkerStyle(29);
//   p7839_d5x1y1->SetMarkerSize(0.7);
//   p7839_d5x1y1->SetMarkerColor(8);

// double mom = 5.6;
// // Plot: p7839_d6x1y1
//   double p7839_d6x1y1_xval[] = { 9.125E-4, 0.001179, 0.001369, 0.001597, 0.001901, 0.002205, 0.002927, 0.003118, 0.003422, 
//     0.00384, 0.004182, 0.0046, 0.005133, 0.005665, 0.006235, 0.006729, 0.007072, 0.007414, 0.00806, 
//     0.00863, 0.009239, 0.009961, 0.01065, 0.01144, 0.01171, 0.01217, 0.01262, 0.013, 0.01384, 
//     0.01456, 0.01536, 0.01627, 0.01715, 0.0181, 0.01909 };
//   double p7839_d6x1y1_yval[] = { 471.1, 371.7, 318.1, 276.4, 249.5, 233.6, 208.8, 202.8, 201.8, 
//     196.9, 191.9, 190.9, 187.9, 186.9, 182.9, 182.0, 181.0, 179.0, 177.0, 
//     175.0, 172.0, 171.0, 169.0, 167.0, 170.0, 167.0, 168.0, 165.1, 164.1, 
//     163.1, 160.1, 159.1, 157.1, 156.1, 154.1 };
//    int p7839_d6x1y1_numpoints = 35;
//   TGraph* p7839_d6x1y1 = new TGraph(p7839_d6x1y1_numpoints, p7839_d6x1y1_xval, p7839_d6x1y1_yval);

//   p7839_d6x1y1->SetName("exp_data_e760_5_6GeV");
//   p7839_d6x1y1->SetTitle("E760 data, P$_{lab}$ = 5.6");
//   p7839_d6x1y1->SetMarkerStyle(29);
//   p7839_d6x1y1->SetMarkerSize(0.7);
//   p7839_d6x1y1->SetMarkerColor(8);

// double mom = 5.72;
//  double p7839_d7x1y1_xval[] = { 8.38E-4, 0.001028, 0.001067, 0.001257, 0.001371, 0.001447, 0.001562, 0.001752, 0.001828, 
//     0.002171, 0.0024, 0.002514, 0.002895, 0.003124, 0.003314, 0.003733, 0.004114, 0.004419, 0.004609, 
//     0.004952, 0.005142, 0.005485, 0.005752, 0.006057, 0.006361, 0.006666, 0.006971, 0.007466, 0.007885, 
//     0.008075, 0.008533, 0.009066, 0.01063, 0.01112, 0.01154, 0.01208, 0.01276, 0.0131, 0.01383, 
//     0.01467, 0.01543, 0.01718, 0.01783, 0.01813, 0.01901, 0.0195 };
//    double p7839_d7x1y1_yval[] = { 481.3, 396.0, 374.2, 328.5, 313.7, 288.9, 276.0, 256.2, 250.2, 
//     232.4, 217.5, 214.5, 206.6, 201.6, 200.6, 193.7, 189.7, 186.7, 190.7, 
//     182.8, 184.8, 182.8, 182.8, 181.8, 178.8, 178.8, 175.8, 174.8, 175.8, 
//     170.9, 171.9, 169.9, 166.9, 163.9, 166.9, 163.9, 163.9, 160.0, 161.0, 
//     157.0, 159.0, 155.0, 153.0, 155.0, 151.0, 150.1 };
//    int p7839_d7x1y1_numpoints = 46;
//   TGraph* p7839_d7x1y1 = new TGraph(p7839_d7x1y1_numpoints, p7839_d7x1y1_xval, p7839_d7x1y1_yval);
//  p7839_d7x1y1->SetName("exp_data_e760_5_72GeV");
//   p7839_d7x1y1->SetTitle("E760 data, P$_{lab}$ = 5.72");
//   p7839_d7x1y1->SetMarkerStyle(29);
//   p7839_d7x1y1->SetMarkerSize(0.7);
//   p7839_d7x1y1->SetMarkerColor(8);

// double mom = 5.94;
//  // Plot: p7839_d8x1y1
//   double p7839_d8x1y1_xval[] = { 8.702E-4, 9.862E-4, 0.001044, 0.001102, 0.00116, 0.001334, 0.001392, 0.001633, 0.001972, 
//     0.002265, 0.00261, 0.003017, 0.00323, 0.003423, 0.003887, 0.004319, 0.004815, 0.005263, 0.005801, 
//     0.006381, 0.006933, 0.00755, 0.008121, 0.008711, 0.009398, 0.009978, 0.01079, 0.01154, 0.01236, 
//     0.01311, 0.01398, 0.01479, 0.01572, 0.01653, 0.01758, 0.01856, 0.01943, 0.01978 };
 
//   double p7839_d8x1y1_yval[] = { 451.1, 440.4, 374.8, 354.9, 344.3, 304.6, 293.9, 261.5, 235.9, 
//     213.7, 203.8, 194.6, 190.6, 190.6, 184.9, 183.1, 179.4, 178.4, 174.8, 
//     173.3, 171.8, 168.9, 167.2, 165.2, 164.1, 161.1, 158.0, 156.5, 158.0, 
//     155.0, 153.4, 151.9, 148.8, 147.3, 145.8, 145.8, 142.7, 141.2 };
 
//   int p7839_d8x1y1_numpoints = 38;
//   TGraph* p7839_d8x1y1 = new TGraph(p7839_d8x1y1_numpoints, p7839_d8x1y1_xval, p7839_d8x1y1_yval);
//  p7839_d8x1y1->SetName("exp_data_e760_5_94GeV");
//   p7839_d8x1y1->SetTitle("E760 data, P$_{lab}$ = 5.94");
//   p7839_d8x1y1->SetMarkerStyle(29);
//   p7839_d8x1y1->SetMarkerSize(0.7);
//   p7839_d8x1y1->SetMarkerColor(8);

  double mom = 6.23;
// Plot: p7839_d9x1y1
  double p7839_d9x1y1_xval[] = { 0.001006, 0.001242, 0.001603, 0.001834, 0.00216, 0.002544, 0.002898, 0.003079, 0.003287, 
    0.003727, 0.0042, 0.004706, 0.005087, 0.00562, 0.006211, 0.006803, 0.007128, 0.00737, 0.007986, 
    0.00865, 0.009286, 0.009938, 0.01065, 0.01142, 0.01301, 0.01381, 0.01474, 0.01564, 0.0164, 
    0.01738, 0.01842, 0.01931, 0.02 };
 
  double p7839_d9x1y1_yval[] = { 393.4, 318.9, 272.6, 247.6, 228.3, 211.5, 203.1, 200.3, 194.3, 
    190.4, 184.9, 182.2, 178.6, 177.6, 175.1, 175.0, 176.8, 172.4, 170.8, 
    169.6, 164.5, 164.2, 161.9, 160.6, 157.0, 154.0, 155.2, 152.5, 150.7, 
    148.9, 145.9, 145.3, 145.3 };
 
  int p7839_d9x1y1_numpoints = 33;
  TGraph* p7839_d9x1y1 = new TGraph(p7839_d9x1y1_numpoints, p7839_d9x1y1_xval, p7839_d9x1y1_yval);
  p7839_d9x1y1->SetName("exp_data_e760_6_23GeV");
  p7839_d9x1y1->SetTitle("E760 data, P$_{lab}$ = 6.23");
  p7839_d9x1y1->SetMarkerStyle(29);
  p7839_d9x1y1->SetMarkerSize(0.7);
  p7839_d9x1y1->SetMarkerColor(8);
  // END Data from E760 ------



  //only final DPM fit is considered  (p.7)
  // double mom = 2.33;
  // double par[5] = {582,0.196,2.72,0.0899,0.322};
  // double errpar[5] = {17,0.008,0.38,0.006,0.012}; //warning: t1 is fixed param, the largest uncertainty is taken from paper

  // double mom = 2.85;
  // double par[5] = {426,0.153,1.78,0.0899,0.394};
  // double errpar[5] = {8.5,0.005,0.19,0.006,0.012}; //warning: t1 is fixed param, the largest uncertainty is taken from paper
  
 // double mom = 3.55;
 //  double par[5] = {382,0.137,1.33,0.0899,0.392};
 //  double errpar[5] = {50.2,0.021,0.42,0.006,0.06}; //warning: t1 is fixed param, the largest uncertainty is taken from paper

  //double mom = 5.7;
  double par[5] = {232,0.11,0.77,0.0899,0.351};
  double errpar[5] = {7.4,0.01,0.2,0.006,0.028}; //warning: t1 is fixed param, the largest uncertainty is taken from paper

  // double mom = 10.40;
  // double par[5] = {171,0.074,0.89,0.0899,0.293};
  // double errpar[5] = {1.9,0.004,0.12,0.006,0.015}; //warning: t1 is fixed param, the largest uncertainty is taken from paper


  // double mom = 15.95;
  // double par[5] = {113,0.049,0.69,0.0899,0.049};
  // double errpar[5] = {4.2,0.013,0.32,0.006,0.049}; //warning: t1 is fixed param, the largest uncertainty is taken from paper
  LumiFit::PndLmdFitModelOptions model_options;
  model_options.momentum_transfer_active = true; // will calc cross-section as a func from (t)
  //model_options.momentum_transfer_active = false; // will calc cross-section as a func from (theta), theta in rad
  model_options.acceptance_correction_active = false;
  model_options.momentum_transfer_active = false;
  model_options.resolution_smearing_active = false;
  PndLmdModelFactory model_factory;
  shared_ptr<Model1D> model1d = model_factory.generate1DModel(model_options, mom);

  /// create Model with DPM params ------------
  //shared_ptr<PndLmdDPMAngModel1D> modelDPM = model1d;
  // PndLmdDPMAngModel1D modelDPM(model1d);
  PndLmdDPMAngModel1D modelDPM("dpm_angular_1d", LumiFit::ALL);
  shared_ptr<Parametrization> para(
  				   new PndLmdDPMModelParametrization(modelDPM.getModelParameterSet()));
  modelDPM.getModelParameterHandler().registerParametrizations(
  							    modelDPM.getModelParameterSet(), para);
  modelDPM.getModelParameterSet().setModelParameterValue("p_lab", mom);
  modelDPM.getModelParameterSet().setModelParameterValue("luminosity", 1);
  // modelDPM->init();
  ((Model1D*) &modelDPM)->init();
  cout<<modelDPM.getRho()<<" "<<modelDPM.getB()<<" "<<modelDPM.getSigmaTotal()<<endl;
  // modelDPM.getModelParameterSet().initParameters();
  // modelDPM.getModelParameterSet().adjustStrictParameters();
  // modelDPM.getModelParameterSet().adjustNonStrictParameters();
  /// [end] create Model with DPM params ------
 /// create Model with E760 params ------------
  PndLmdDPMAngModel1D modelE760("e760_angular_1d", LumiFit::ALL);
  // shared_ptr<Model1D> modelE760 = model_factory.generate1DModel(model_options, mom);
  shared_ptr<Parametrization> parae760(
  				   new PndLmdE760ModelParametrization(modelE760.getModelParameterSet()));
  modelE760.getModelParameterHandler().registerParametrizations(
  							    modelE760.getModelParameterSet(), parae760);
  modelE760.getModelParameterSet().setModelParameterValue("p_lab", mom);
  modelE760.getModelParameterSet().setModelParameterValue("luminosity", 1);
  ((Model1D*) &modelE760)->init();
  cout<<modelE760.getRho()<<" "<<modelE760.getB()<<" "<<modelE760.getSigmaTotal()<<endl;
   /// [end] create Model with E760 params ------
  /// create Model with E760-like params ------------
  PndLmdDPMAngModel1D modelE760like("e760like_angular_1d", LumiFit::ALL);
  shared_ptr<Parametrization> parae760like(
  				   new PndLmdE760LikeModelParametrization(modelE760like.getModelParameterSet()));
  modelE760like.getModelParameterHandler().registerParametrizations(
  							    modelE760like.getModelParameterSet(), parae760like);
  modelE760like.getModelParameterSet().setModelParameterValue("p_lab", mom);
  modelE760like.getModelParameterSet().setModelParameterValue("luminosity", 1);
  ((Model1D*) &modelE760like)->init();
  cout<<modelE760like.getRho()<<" "<<modelE760like.getB()<<" "<<modelE760like.getSigmaTotal()<<endl;
   /// [end] create Model with E760-like params ------


  /// set up LmdFit for calculation t out of theta --------

 


  // double th_dw = 1.04719994E-03;//rad -> 0.06 DPM
  //double th_dw = 0.002;//
  double th_dw = 0.0045;//
 //double th_up = TMath::Pi();
  //   double th_up = 0.02;
   double th_up = 0.025;
  // double th_up = 0.015;
   const int nst = 2e3;
   // const int nst = 2e1;
  double cs_loc = 0;
  double cs_val[nst], cs_uncert[nst];//dpm
  double csE760_val[nst], csE760_uncert[nst];//e760
  double csE760like_val[nst], csE760like_uncert[nst];//e760like
  double diff_cs_val[nst], diff_cs_uncert[nst];// dpm - e760like 
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
    // cout<<"t_cur = "<<t_cur<<endl;
    th_val[i] = 1e3*th1;
    //  cs_val[i] = model1d->eval(&th1);
    // cout<<model1d->eval(&t_cur)<<endl;
    // double cs_e760 = modelE760.eval(&th1);
    csE760_val[i] = modelE760.getRawRhoBSigtotFullElastic(&t_val[i]);
    csE760like_val[i] = modelE760like.getRawRhoBSigtotFullElastic(&t_val[i]);
    // csE760_val[i] = modelE760.getRawFullElastic(&t_val[i]);
    csE760_uncert[i] = 0;
    csE760like_uncert[i] = 0;
    cs_val[i] = modelDPM.getRawFullElastic(&t_val[i]);
    //  cs_val[i] = modelDPM.getRawRhoBSigtotFullElastic(&t_val[i]);
    //    cout<<" DPM "<<cs_val[i]<<" E760 "<<csE760_val[i]<<endl;
    cs_uncert[i] = 0;
    // cs_uncert[i] = Df(t_cur,par,errpar);
    diff_cs_val[i] =100.*(cs_val[i]-csE760like_val[i])/cs_val[i];
    diff_cs_uncert[nst] = sqrt(csE760like_uncert[i]* csE760like_uncert[i] +cs_uncert[i]*cs_uncert[i]);

    
    // rel_err[i] = 100*(cs_uncert[i]/cs_val[i]);
    // //TEST: calculate total cross-section
    // vector<pair<double, double> > range;
    // range.push_back(make_pair(th1,th2));
    // double dcs =  model1d->Integral(range,1e-6);
    // cs_loc += dcs;
    // cout<<"cs_val = "<<cs_val[i]<<" +/- "<<cs_uncert[i]<<endl;
  }

  TGraph *gr_t_th = new TGraph(nst,t_val,th_val);

  // TGraphErrors *gr_cs_th = new TGraphErrors(nst,th_val,cs_val,0,cs_uncert);
  // gr_cs_th->SetFillColor(4);
  // gr_cs_th->SetMarkerColor(4);
  // gr_cs_th->SetFillStyle(3001);

  TGraphErrors *gr_cs_t = new TGraphErrors(nst,t_val,cs_val,0,cs_uncert);
  gr_cs_t->SetFillColor(4);
  gr_cs_t->SetMarkerColor(4);
  gr_cs_t->SetMarkerStyle(21);
  gr_cs_t->SetMarkerSize(0.3);
  gr_cs_t->SetFillStyle(3001);

  // TGraphErrors *gr_csE760_th = new TGraphErrors(nst,th_val,csE760_val,0,csE760_uncert);
  // gr_csE760_th->SetMarkerColor(2);
  // gr_csE760_th->SetFillColor(2);
  // gr_csE760_th->SetFillStyle(3001);

  TGraphErrors *gr_csE760_t = new TGraphErrors(nst,t_val,csE760_val,0,csE760_uncert);
  gr_csE760_t->SetMarkerColor(2);
  gr_csE760_t->SetFillColor(2);
  gr_csE760_t->SetMarkerStyle(21);
  gr_csE760_t->SetMarkerSize(0.3);
  gr_csE760_t->SetFillStyle(3001);

  TGraphErrors *gr_csE760like_t = new TGraphErrors(nst,t_val,csE760like_val,0,csE760like_uncert);
  gr_csE760like_t->SetMarkerColor(1);
  gr_csE760like_t->SetMarkerStyle(21);
  gr_csE760like_t->SetMarkerSize(0.3);
  gr_csE760like_t->SetFillColor(1);
  gr_csE760like_t->SetFillStyle(3001);

  // TMultiGraph *mg_cs_th = new TMultiGraph();
  // mg_cs_th->Add(gr_cs_th);
  // mg_cs_th->Add(gr_csE760_th);
 
  TLegend *leg = new TLegend(0.55,0.55,0.85,0.85);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.05);

  leg->AddEntry(p7839_d9x1y1,"Exp.Data","ep");
  leg->AddEntry(gr_cs_t,"DPM","p");
  leg->AddEntry(gr_csE760_t,"E760","p");
  leg->AddEntry(gr_csE760like_t,"E760-like","p");

  TMultiGraph *mg_cs_t = new TMultiGraph();
  mg_cs_t->Add(gr_cs_t);
  mg_cs_t->Add(gr_csE760like_t);
  mg_cs_t->Add(gr_csE760_t);
  //  mg_cs_t->Add(p7839_d9x1y1); // data for 5.72!!!
  mg_cs_t->Add(p7839_d9x1y1); // data for 3.7!!!
  // TGraphErrors *gr_diff_cs_th = new TGraphErrors(nst,th_val,diff_cs_val,0,diff_cs_uncert);
  // gr_diff_cs_th->SetFillColor(4);
  // gr_diff_cs_th->SetMarkerColor(4);
  // gr_diff_cs_th->SetFillStyle(3001);
  // gr_diff_cs_th->GetXaxis()->SetTitle("#theta, mrad");
  // gr_diff_cs_th->GetYaxis()->SetTitle("((d#sigma/dt)_{DPM}-(d#sigma/dt)_{E760})/(d#sigma/dt)_{DPM}, %");

  TGraphErrors *gr_diff_cs_t = new TGraphErrors(nst,t_val,diff_cs_val,0,diff_cs_uncert);
  gr_diff_cs_t->SetFillColor(4);
  gr_diff_cs_t->SetMarkerColor(4);
  gr_diff_cs_t->SetFillStyle(3001);
  gr_diff_cs_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  gr_diff_cs_t->GetYaxis()->SetTitle("((d#sigma/dt)_{DPM}-(d#sigma/dt)_{E760-like})/(d#sigma/dt)_{DPM}, %");

  TGraphErrors *gr_diff_csE760_data_t = new TGraphErrors(p7839_d9x1y1->GetN());
  gr_diff_csE760_data_t->SetMarkerColor(2);
  gr_diff_csE760_data_t->SetMarkerStyle(21);
  TGraphErrors *gr_diff_csE760like_data_t = new TGraphErrors(p7839_d9x1y1->GetN());
  gr_diff_csE760like_data_t->SetMarkerColor(1);
  gr_diff_csE760like_data_t->SetMarkerStyle(21);
  TGraphErrors *gr_diff_csDPM_data_t = new TGraphErrors(p7839_d9x1y1->GetN());
  gr_diff_csDPM_data_t->SetMarkerColor(4);
  gr_diff_csDPM_data_t->SetMarkerStyle(21);
  for(int ik=0;ik<p7839_d9x1y1->GetN();ik++){
    double t_cur = p7839_d9x1y1_xval[ik];
    double csE760_cur = 100.*(modelE760.getRawRhoBSigtotFullElastic(&t_cur)-p7839_d9x1y1_yval[ik])/p7839_d9x1y1_yval[ik];
    double csE760like_cur = 100.*(modelE760like.getRawRhoBSigtotFullElastic(&t_cur)-p7839_d9x1y1_yval[ik])/p7839_d9x1y1_yval[ik];
    double csDPM_cur = 100.*(modelDPM.getRawFullElastic(&t_cur)-p7839_d9x1y1_yval[ik])/p7839_d9x1y1_yval[ik];
    gr_diff_csE760_data_t->SetPoint(ik,t_cur,csE760_cur);
    gr_diff_csE760like_data_t->SetPoint(ik,t_cur,csE760like_cur);
    gr_diff_csDPM_data_t->SetPoint(ik,t_cur,csDPM_cur);
  }

 
  TCanvas c1("c1","canvas",600,800);
  c1.Divide(2,3);
  //c1.Divide(2,2);
  c1.cd(1);
  gr_t_th->SetTitle("P_{beam} = 6.23 GeV/c");
  gr_t_th->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  gr_t_th->GetXaxis()->SetLimits(-0.001,0.021);
  gr_t_th->GetYaxis()->SetTitle("#theta, mrad");
  gr_t_th->Draw("AP4");
  //  gr_cs_th->Draw("AP4");
  // mg_cs_th->Draw("AP");
  // mg_cs_th->SetTitle("P_{beam} = 5.72 GeV/c");
  // mg_cs_th->GetXaxis()->SetTitle("#theta, mrad");
  // mg_cs_th->GetYaxis()->SetTitle("d#sigma/dt, mb/(GeV/c)^{2}");
  c1.cd(3);
  //gr_cs_t->Draw("AP4");
  mg_cs_t->Draw("AP");
  mg_cs_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  mg_cs_t->GetXaxis()->SetLimits(-0.001,0.021);
  mg_cs_t->GetYaxis()->SetTitle("d#sigma/dt, mb/(GeV/c)^{2}");
  leg->Draw();
  // c1.cd(2);
  // gr_diff_cs_th->Draw("AP4");
  c1.cd(4);
  gr_diff_cs_t->SetTitle("");
  gr_diff_cs_t->GetXaxis()->SetLimits(-0.001,0.021);
  gr_diff_cs_t->Draw("AP4");
  c1.cd(5);
  TMultiGraph *mg_e760_data = new TMultiGraph();
  mg_e760_data->Add(gr_diff_csE760_data_t);
  mg_e760_data->Add(gr_diff_csE760like_data_t);
  mg_e760_data->Draw("AP");
  mg_e760_data->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  mg_e760_data->GetXaxis()->SetLimits(-0.001,0.021);
  mg_e760_data->GetYaxis()->SetTitle("((d#sigma/dt)_{E760}-Exp.Data)/Exp.Data, %");
  //gr_diff_csE760_data_t->Draw("AP");
  // gr_diff_csE760like_data_t->Draw("psame");
  c1.cd(6);
  gr_diff_csDPM_data_t->SetTitle("");
  gr_diff_csDPM_data_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  gr_diff_csDPM_data_t->GetXaxis()->SetLimits(-0.001,0.021);
  gr_diff_csDPM_data_t->GetYaxis()->SetTitle("((d#sigma/dt)_{DPM}-Exp.Data)/Exp.Data, %");
  gr_diff_csDPM_data_t->Draw("AP");
  // gr_cs_un_th->Draw("AL");
  // c1.cd(4);
  // gr_cs_un_t->Draw("AL");
  // c1.cd(5);
  // gr_cs_relun_th->Draw("AL");
  // c1.cd(6);
  // gr_cs_relun_t->Draw("AL");
  c1.SaveAs("cs_and_uncert_DPM_E760_E760like_exp_6_23.pdf");
  c1.SaveAs("cs_and_uncert_DPM_E760_E760like_exp_6_23.root");
  return 0;

//  TGraph *gr_cs_un_th = new TGraph(nst,th_val,cs_uncert);
 //  gr_cs_un_th->SetTitle("");
 //  gr_cs_un_th->GetXaxis()->SetTitle("#theta, mrad");
 //  gr_cs_un_th->GetYaxis()->SetTitle("#Delta(d#sigma/dt, mb/(GeV/c)^{2})");

 //  TGraph *gr_cs_un_t = new TGraph(nst,t_val,cs_uncert);
 //  gr_cs_un_t->SetTitle("");
 //  gr_cs_un_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
 //  gr_cs_un_t->GetYaxis()->SetTitle("#Delta(d#sigma/dt, mb/(GeV/c)^{2})");

 // TGraph *gr_cs_relun_th = new TGraph(nst,th_val,rel_err);
 // gr_cs_relun_th->SetTitle("");
 //  gr_cs_relun_th->GetXaxis()->SetTitle("#theta, mrad");
 //  gr_cs_relun_th->GetYaxis()->SetTitle("#Delta(d#sigma/dt)/(d#sigma/dt), %");

 //  TGraph *gr_cs_relun_t = new TGraph(nst,t_val,rel_err);
 //  gr_cs_relun_t->SetTitle("");
 //  gr_cs_relun_t->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
 //  gr_cs_relun_t->GetYaxis()->SetTitle("#Delta(d#sigma/dt)/(d#sigma/dt), %");
}
