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
#include <TFile.h>
#include <TH2D.h>
using namespace std;


int main(){
  gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/tests/ModelUncert/rootlogon.C");
 TLegend *leg_sigt = new TLegend(0.64,0.45,0.87,0.7);
  leg_sigt->SetFillColor(0);
  leg_sigt->SetTextFont(42);
  leg_sigt->SetTextSize(0.04);
  leg_sigt->SetHeader("P_{lab} = 10.0 GeV/c");
  double plab = 10.0;
  //part 1: draw hadronic contribution ----------------------------------
  double t_min = 1e-7;
  double t_max = 1e0;
  //  double t_max = 1e1;
  //all
  LumiFit::PndLmdFitModelOptions model_options;
 PndLmdDPMAngModel1D modelDPM("dpm_angular_1d", LumiFit::ALL);
  shared_ptr<Parametrization> para(
  				   new PndLmdDPMModelParametrization(modelDPM.getModelParameterSet()));
  modelDPM.getModelParameterHandler().registerParametrizations(
  							    modelDPM.getModelParameterSet(), para);
  modelDPM.getModelParameterSet().setModelParameterValue("p_lab", plab);
  modelDPM.getModelParameterSet().setModelParameterValue("luminosity", 1);
  // modelDPM->init();
  ((Model1D*) &modelDPM)->init();
//all
  PndLmdDPMAngModel1D modelE760("e760_angular_1d", LumiFit::ALL);
  // shared_ptr<Model1D> modelE760 = model_factory.generate1DModel(model_options, mom);
  shared_ptr<Parametrization> parae760(
  				   new PndLmdE760LikeModelParametrization(modelE760.getModelParameterSet()));
  modelE760.getModelParameterHandler().registerParametrizations(
  							    modelE760.getModelParameterSet(), parae760);
  modelE760.getModelParameterSet().setModelParameterValue("p_lab", plab);
  modelE760.getModelParameterSet().setModelParameterValue("luminosity", 1);
  ((Model1D*) &modelE760)->init();

  PndLmdLumiHelper *lmd_help  = new PndLmdLumiHelper();
  double th1 = 2e-3;
  double th2 = 1e-2;
  double t_1 = fabs(lmd_help->getMomentumTransferFromTheta(plab, th1));
  double t_2 = fabs(lmd_help->getMomentumTransferFromTheta(plab, th2));
  TBox *lmd_range_box = new TBox(t_1,-15,t_2,110);
  lmd_range_box->SetFillColor(12);
  lmd_range_box->SetFillStyle(3005);
 
  //cout<<modelDPM.getRho()<<" "<<modelDPM.getB()<<" "<<modelDPM.getSigmaTotal()<<endl;

 const int Nstep = 1e4;
 double cs_ratio[Nstep];
 double cs_ratio_e760[Nstep];
 double cs_had_dpm_y[Nstep];
 double cs_had_e760_y[Nstep];
 double t_axis[Nstep];
 double cs_had_rat_DPM,  cs_had_rat_E760;
  for(int i=0;i<Nstep;i++){
    double t_cur = (-1)*(t_min+i*(t_max-t_min)/Nstep);
    t_axis[i] = fabs(t_cur);

    double cs_el = modelDPM.getRawFullElastic(&t_cur);
    double cs_had = modelDPM.getRawHadronicPart(&t_cur)+modelDPM.getRawInterferencePart(&t_cur);
    //cs_had_dpm_y[i] =  cs_had;
    cs_had_dpm_y[i] =  modelDPM.getRawHadronicPart(&t_cur);
    cs_ratio[i] = 100.*cs_had/cs_el;
    double cs_el_e760 = modelE760.getRawRhoBSigtotFullElastic(&t_cur);
    double cs_had_e760 = modelE760.getRawRhoBSigtotHadronicPart(&t_cur)+modelDPM.getRawInterferencePart(&t_cur);
    //cs_had_e760_y[i] = cs_had_e760;
    cs_had_e760_y[i] = modelE760.getRawRhoBSigtotHadronicPart(&t_cur);
    cs_ratio_e760[i] = 100.*cs_had_e760/cs_el_e760;
    // cout<<"t_axis[i]-t_2 = "<<t_axis[i]-t_2<<endl;
    // if(fabs(t_axis[i]-t_2)<5e-5){
    //   cs_had_rat_DPM = cs_ratio[i];
    //   cs_had_rat_E760 =  cs_ratio_e760[i];
    // }
    //   cout<<"t = "<<t_cur<<"  cs_ratio = "<<cs_ratio[i]<<" cs_el = "<<cs_el<<"  cs_had = "<<cs_had<<endl;
  }
  cs_had_rat_DPM = 100.*(modelDPM.getRawHadronicPart(&t_2)+modelDPM.getRawInterferencePart(&t_2))/modelDPM.getRawFullElastic(&t_2);
  cs_had_rat_E760 = 100.*(modelE760.getRawRhoBSigtotHadronicPart(&t_2)+modelDPM.getRawInterferencePart(&t_2))/modelE760.getRawRhoBSigtotFullElastic(&t_2);
  cout<<"DPM part = "<<cs_had_rat_DPM<<" E760 part = "<<cs_had_rat_E760<<endl;

  TCanvas c1("c1","canvas",800,600);
  c1.Divide(2,2);
  c1.cd(1);
  gPad->SetLogx();
  TMultiGraph *mgr = new TMultiGraph();
 
  TGraph *gr_had_ratio = new TGraph(Nstep,t_axis,cs_ratio); 
  gr_had_ratio->SetLineColor(kBlue+1);
  gr_had_ratio->SetTitle("DPM");
  gr_had_ratio->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  gr_had_ratio->GetYaxis()->SetTitle("(#sigma_{had}+#sigma_{iterf})/#sigma_{el}, %");
  gr_had_ratio->GetYaxis()->SetRangeUser(-15,110);
  // gr_had_ratio->Draw("AL");  
  mgr->Add(gr_had_ratio);
  TGraph *gr_had_ratio_e760 = new TGraph(Nstep,t_axis,cs_ratio_e760);
  gr_had_ratio_e760->SetLineColor(kAzure+1);
  gr_had_ratio_e760->SetTitle("E760 like");
  gr_had_ratio_e760->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  gr_had_ratio_e760->GetYaxis()->SetTitle("(#sigma_{had}+#sigma_{iterf})/#sigma_{el}, %");
  gr_had_ratio_e760->GetYaxis()->SetRangeUser(-15,110);
  // gr_had_ratio_e760->Draw("AL");
  mgr->Add(gr_had_ratio_e760);
  mgr->Draw("AL");
  mgr->GetYaxis()->SetRangeUser(-15,110);
  mgr->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  mgr->GetYaxis()->SetTitle("(#sigma_{had}+#sigma_{iterf})/#sigma_{el}, %");
  leg_sigt->AddEntry(gr_had_ratio,"DPM","l");
  leg_sigt->AddEntry(gr_had_ratio_e760,"E760","l");
  lmd_range_box->Draw("same");
  leg_sigt->Draw();
  //  c1.SetLogx();
  //  c1.cd(3);
  gPad->SetLogx();
  // TGraph *gr_had_ratio_e760 = new TGraph(Nstep,t_axis,cs_ratio_e760);
  // gr_had_ratio_e760->SetLineColor(kAzure+1);
  // gr_had_ratio_e760->SetTitle("E760 like");
  // gr_had_ratio_e760->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  // gr_had_ratio_e760->GetYaxis()->SetTitle("(#sigma_{had}+#sigma_{iterf})/#sigma_{el}, %");
  // gr_had_ratio_e760->GetYaxis()->SetRangeUser(-15,110);
  // gr_had_ratio_e760->Draw("AL");
  // lmd_range_box->Draw("same");
  //[end] part 1: draw hadronic contribution ---------------------------
  c1.cd(3);
  gPad->SetLogx();
  gPad->SetLogy();
  TMultiGraph *mg_had = new TMultiGraph();
  TGraph *gr_had_dpm = new TGraph(Nstep,t_axis,cs_had_dpm_y); 
  gr_had_dpm->SetLineColor(kBlue+1);
  TGraph *gr_had_e760 = new TGraph(Nstep,t_axis,cs_had_e760_y); 
  gr_had_e760->SetLineColor(kAzure+1);
  mg_had->Add(gr_had_dpm);
  mg_had->Add(gr_had_e760);
  mg_had->Draw("AL");
  mg_had->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  mg_had->GetYaxis()->SetTitle("(d#sigma/dt)^{had}, mb/(GeV/c)^{2}");
  //  mg_had->GetXaxis()->SetRangeUser(1e-7,2e-1);
  //part2 draw data ---------------------------------------------------------
   c1.cd(2);
 gPad->SetLogx();
 TFile *fin_dpm = new TFile("/home/karavdina/Disser/reports/pbarp_prev_exp/DPMfitUncert/DPM_vs_E760/dS_rel/dS_DPM_10GeV.root","READ");
  TGraphErrors *gr_dpm = (TGraphErrors *)fin_dpm->Get("Graph");
  double x_dpm,y_dpm;
  gr_dpm->GetPoint(0,x_dpm,y_dpm);
 
  //gr_dpm->GetYaxis()->SetRangeUser(-100,100);
  //  gr_dpm->GetXaxis()->SetRangeUser(1e-7,2e-1);
  //gr_dpm->GetXaxis()->SetRangeUser(1e-7,1);
  // gr_dpm->Draw("AP");
  // TF1 *fa1 = new TF1("fa1","[0]+[1]*x",x_dpm,5e-1);
 //gr_dpm->Fit(fa1,"","",x_dpm*0.95,2.5e-1);
 // gr_dpm->Fit(fa1,"","",1e-2,1e-1);
  // TF1 *fa1 = new TF1("fa1","[0]+[1]*x",x_dpm,1);
 TF1 *fa1 = new TF1("fa1","[0]",x_dpm,1);
 //gr_dpm->Fit(fa1,"","",x_dpm*0.95,2.6e-1);
 gr_dpm->Fit(fa1,"","",1e-2,1.3e-1);
// gr_dpm->Fit(fa1,"","",8e-2,2e-1);
  double p0_dpm =  fa1->GetParameter(0);
  double errp0_dpm =  fa1->GetParError(0);
  cout<<"DPM: p0 = "<<p0_dpm<<" +/- "<<errp0_dpm<<endl;
  TMultiGraph *mg_sys = new TMultiGraph();
  mg_sys->Add(gr_dpm);

  TFile *fin_e760 = new TFile("/home/karavdina/Disser/reports/pbarp_prev_exp/DPMfitUncert/DPM_vs_E760/dS_rel/dS_e760_10GeV.root","READ");
  TGraphErrors *gr_e760 = (TGraphErrors *)fin_e760->Get("Graph");

  
  gr_e760->GetYaxis()->SetRangeUser(-100,100);
  gr_e760->GetXaxis()->SetRangeUser(1e-7,2e-1);
  // gr_e760->Draw("AP");
  mg_sys->Add(gr_e760);
  mg_sys->Draw("AP");
  mg_sys->GetYaxis()->SetRangeUser(-100,100);
  //  mg_sys->GetXaxis()->SetRangeUser(1e-7,5e-1);
  // mg_sys->GetXaxis()->SetRangeUser(1e-7,1);
  mg_sys->GetYaxis()->SetTitle("((d#sigma/dt)_{exp}-(d#sigma/dt)_{model})/(d#sigma/dt)_{exp}, %");
  mg_sys->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");

  // TF1 *fa2 = new TF1("fa2","[0]+[1]*x",x_dpm,5e-1);
 // gr_e760->Fit(fa2,"","",x_dpm*0.95,2.5e-1);
 // gr_e760->Fit(fa2,"","",1e-2,1e-1);
  // TF1 *fa2 = new TF1("fa2","[0]+[1]*x",x_dpm,1);
 TF1 *fa2 = new TF1("fa2","[0]",x_dpm,1);
 //gr_e760->Fit(fa2,"","",x_dpm*0.95,2.6e-1);
 gr_e760->Fit(fa2,"","",1e-2,1.3e-1);
 // gr_e760->Fit(fa2,"","",8e-2,2e-1);
  double p0_e760 =  fa2->GetParameter(0);
  double errp0_e760 =  fa2->GetParError(0);
  cout<<"E760: p0 = "<<p0_e760<<" +/- "<<errp0_e760<<endl;
 cout<<"Pbeam = "<<plab<<" "<<"DPM = "<<fabs(p0_dpm*1e-2*cs_had_rat_DPM)<<" +/- "<<fabs(errp0_dpm*1e-2*cs_had_rat_DPM)
      <<"; E760 = "<<fabs(p0_e760*1e-2*cs_had_rat_E760)<<" +/- "<<fabs(errp0_e760*1e-2*cs_had_rat_E760)<<" %"<<endl;
 cout<<" LINE: "<<"DPM = "<<fabs(p0_dpm)<<" +/- "<<fabs(errp0_dpm)
      <<"; E760 = "<<fabs(p0_e760)<<" +/- "<<fabs(errp0_e760)<<" %"<<endl;
  c1.cd(4);

  double plab_x[1]={plab};
  double dpm_sys[1]={fabs(p0_dpm*1e-2*cs_had_rat_DPM)};
  double errdpm_sys[1]={fabs(errp0_dpm*1e-2*cs_had_rat_DPM)};
  TGraphErrors *dpm_err = new TGraphErrors(1,plab_x,dpm_sys,0,errdpm_sys);
  dpm_err->SetMarkerColor(kBlue+1);
  dpm_err->SetLineColor(kBlue+1);
  dpm_err->SetMarkerStyle(21);
  double e760_sys[1]={fabs(p0_e760*1e-2*cs_had_rat_E760)};
  double erre760_sys[1]={fabs(errp0_e760*1e-2*cs_had_rat_E760)};
 TGraphErrors *e760_err = new TGraphErrors(1,plab_x,e760_sys,0,erre760_sys);
  e760_err->SetMarkerColor(kAzure+1);
  e760_err->SetLineColor(kAzure+1);
  e760_err->SetMarkerStyle(20);
  TMultiGraph *mg_err = new TMultiGraph();
  mg_err->Add(dpm_err);
  mg_err->Add(e760_err);
  mg_err->Draw("AP");
  mg_err->GetYaxis()->SetTitle("model system.err, %");
  mg_err->GetXaxis()->SetTitle("P_{lab}, GeV/c");
  //[end] part2 draw data --------------------------------------------------
  c1.SaveAs("syserr_estim_10GeV.pdf");
  c1.SaveAs("syserr_estim_10GeV.root");
  
  return 0;
}
