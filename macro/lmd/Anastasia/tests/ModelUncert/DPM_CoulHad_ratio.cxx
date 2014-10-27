// macro for comparision DPM model and E760 model with their uncertainty in LMD range
#include <model/PndLmdDPMAngModel1D.h>
#include <model/PndLmdDPMMTModel1D.h>
#include <model/PndLmdDPMModelParametrization.h>
#include <model/PndLmdE760LikeModelParametrization.h>
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


int main(){
  gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/test_macros/rootlogon.C");
  double Plab_min=1.5;
  double Plab_max=15;
  int Plabstep= 100;
  TGraphErrors *resUn2mrad_c = new TGraphErrors(Plabstep);
  resUn2mrad_c->SetLineWidth(2);
  resUn2mrad_c->SetLineColor(2);

  TGraphErrors *resUn2mrad_i = new TGraphErrors(Plabstep);
  resUn2mrad_i->SetLineWidth(2);
  resUn2mrad_i->SetLineColor(8);


  TGraphErrors *resUn2mrad_h = new TGraphErrors(Plabstep);
  resUn2mrad_h->SetLineWidth(2);
  resUn2mrad_h->SetLineColor(4);

  //double coul[Plabstep],interf[Plabstep],had[Plabstep];
  //  double Pbeam[Plabstep];
 for(int pstep=0;pstep<=Plabstep;pstep++){
    double plab = Plab_min+pstep*(Plab_max-Plab_min)/Plabstep;
  double lower_bound = 4.0;
  double upper_bound = 8.0 ;
  std::vector<std::pair<double, double> > integral_ranges;
  integral_ranges.push_back(std::make_pair(lower_bound / 1000.0, upper_bound / 1000.0));
 PndLmdLumiHelper *lmd_help  = new PndLmdLumiHelper();
  double t_1 = (lmd_help->getMomentumTransferFromTheta(plab, lower_bound/1000.0));
  double t_2 = (lmd_help->getMomentumTransferFromTheta(plab, upper_bound/1000.0));
  std::vector<std::pair<double, double> > integral_ranges_t;
  integral_ranges_t.push_back(std::make_pair(t_1, t_2));

  //all
 LumiFit::PndLmdFitModelOptions model_options;
  model_options.dpm_elastic_parts = LumiFit::ALL_RHO_B_SIGTOT;
  PndLmdDPMAngModel1D modelE760("e760_angular_1d", model_options.dpm_elastic_parts);
  shared_ptr<Parametrization> para(new PndLmdE760LikeModelParametrization(modelE760.getModelParameterSet()));
  modelE760.getModelParameterHandler().registerParametrizations(
  							    modelE760.getModelParameterSet(), para);
  modelE760.getModelParameterSet().setModelParameterValue("p_lab", plab);
  modelE760.getModelParameterSet().setModelParameterValue("luminosity", 1);
  ((Model1D*) &modelE760)->init();


//coul only
  model_options.dpm_elastic_parts = LumiFit::COUL;
  PndLmdDPMMTModel1D modelDPM_c("dpm_angular_1d_c", model_options.dpm_elastic_parts);
   shared_ptr<Parametrization> para_c(new PndLmdE760LikeModelParametrization(modelDPM_c.getModelParameterSet()));
   modelDPM_c.getModelParameterHandler().registerParametrizations(modelDPM_c.getModelParameterSet(), para_c);
  modelDPM_c.getModelParameterSet().setModelParameterValue("p_lab", plab);
  modelDPM_c.getModelParameterSet().setModelParameterValue("luminosity", 1);
  ((Model1D*) &modelDPM_c)->init();

  //iterfer only
  model_options.dpm_elastic_parts = LumiFit::INT;
  PndLmdDPMMTModel1D modelDPM_i("dpm_angular_1d_i", model_options.dpm_elastic_parts);
   shared_ptr<Parametrization> para_i(new PndLmdE760LikeModelParametrization(modelDPM_i.getModelParameterSet()));
   modelDPM_i.getModelParameterHandler().registerParametrizations(modelDPM_i.getModelParameterSet(), para_i);
  modelDPM_i.getModelParameterSet().setModelParameterValue("p_lab", plab);
  modelDPM_i.getModelParameterSet().setModelParameterValue("luminosity", 1);
  ((Model1D*) &modelDPM_i)->init();

  //had only 
   model_options.dpm_elastic_parts = LumiFit::HAD_RHO_B_SIGTOT;
  PndLmdDPMMTModel1D modelDPM_h("dpm_angular_1d_h", model_options.dpm_elastic_parts);
  shared_ptr<Parametrization> para_h(new PndLmdE760LikeModelParametrization(modelDPM_h.getModelParameterSet()));
   modelDPM_h.getModelParameterHandler().registerParametrizations(modelDPM_h.getModelParameterSet(), para_h);
  modelDPM_h.getModelParameterSet().setModelParameterValue("p_lab", plab);
  modelDPM_h.getModelParameterSet().setModelParameterValue("luminosity", 1);
  ((Model1D*) &modelDPM_h)->init();
  double integral_2_10 = modelE760.Integral(integral_ranges, 0.001);
  double integral_2_10_c = modelDPM_c.Integral(integral_ranges_t, 0.001);
  double integral_2_10_i = modelDPM_i.Integral(integral_ranges_t, 0.001);
  double integral_2_10_h = modelDPM_h.Integral(integral_ranges_t, 0.001);


  double coul = 100.*integral_2_10_c/integral_2_10;
  double interf = 100.*integral_2_10_i/integral_2_10;
  double had = 100.*integral_2_10_h/integral_2_10;
  resUn2mrad_c->SetPoint(pstep, plab,coul);
  resUn2mrad_i->SetPoint(pstep, plab,interf);
  resUn2mrad_h->SetPoint(pstep, plab,had);
  cout<<"TOT: "<<integral_2_10<<" Coul: "<<integral_2_10_c<<" Interfer: "<<integral_2_10_i<<" Had: "<<integral_2_10_h<<" SUM="<<integral_2_10_c+integral_2_10_i+integral_2_10_h<<endl;
 }

 TCanvas c3("c3","canvas",800,600);
 TMultiGraph *mg_res = new TMultiGraph();
 mg_res->Add(resUn2mrad_c);
 mg_res->Add(resUn2mrad_i);
 mg_res->Add(resUn2mrad_h);
 mg_res->Draw("AL");
 mg_res->GetXaxis()->SetTitle("P_{lab}, GeV/c");
 mg_res->GetYaxis()->SetTitle("#sigma_{contrib}/#sigma, %");
 TLegend *leg = new TLegend(0.75,0.5,0.87,0.7);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.05);
  leg->AddEntry(resUn2mrad_c,"coul","l");
  leg->AddEntry(resUn2mrad_i,"interf","l");
  leg->AddEntry(resUn2mrad_h,"had","l");
  leg->Draw();
  c3.SaveAs("E760_contrib_vs_Plab_integral_4_8.pdf");
  c3.SaveAs("E760_contrib_vs_Plab_integral_4_8.root");
  // lower_bound = 3.0;
  // upper_bound = 9.0 ;
  // std::vector<std::pair<double, double> > integral_ranges2;
  // integral_ranges2.push_back(std::make_pair(lower_bound / 1000.0, upper_bound / 1000.0));
  // double integral_3_9 = modelDPM.Integral(integral_ranges2, 0.0001);
  // lower_bound = 4.0;
  // upper_bound = 8.0 ;
  // std::vector<std::pair<double, double> > integral_ranges3;
  // integral_ranges3.push_back(std::make_pair(lower_bound / 1000.0, upper_bound / 1000.0));
  // double integral_4_8 = modelDPM.Integral(integral_ranges3, 0.0001);
  // cout<<" integral (2-10) = "<<integral_2_10<<endl;

  
  // double lower_bound = 2.0;
  // double upper_bound = 10.0 ;
  // std::vector<std::pair<double, double> > integral_ranges;
  // integral_ranges.push_back(std::make_pair(lower_bound / 1000.0, upper_bound / 1000.0));
  // double integral_2_10 = modelDPM.Integral(integral_ranges, 0.0001);
  // lower_bound = 3.0;
  // upper_bound = 9.0 ;
  // std::vector<std::pair<double, double> > integral_ranges2;
  // integral_ranges2.push_back(std::make_pair(lower_bound / 1000.0, upper_bound / 1000.0));
  // double integral_3_9 = modelDPM.Integral(integral_ranges2, 0.0001);
  // lower_bound = 4.0;
  // upper_bound = 8.0 ;
  // std::vector<std::pair<double, double> > integral_ranges3;
  // integral_ranges3.push_back(std::make_pair(lower_bound / 1000.0, upper_bound / 1000.0));
  // double integral_4_8 = modelDPM.Integral(integral_ranges3, 0.0001);
  // cout<<" integral (2-10) = "<<integral_2_10<<endl;



  return 0;
}
