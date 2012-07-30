#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TCanvas.h>

#include <sstream>
using namespace std;
void GenAlignEffect(TString path="/data/FAIRsorf/pandaroot/trunk/macro/lmd/tmpOutputAlign/results/01062012/",double misalsc=0, int crGraph=0, int poNum=10, TString resname="/data/FAIRsorf/pandaroot/trunk/macro/lmd/tmpOutputAlign/results/Sens.root")
{
  /// Theta resolution--------------------------------------------------------------------------------------  
  TString name1 = path + "Lumi_out_MC_and_REC_trks_matches_with_IDs0_before.root";
  TString name2 = path + "Lumi_out_MC_and_REC_trks_matches_with_IDs0_after.root";

  //  TH1F *hbeforetmp;   TH1F *haftertmp;
  TH1F *hbefore;   
  TH1F *hafter;
  TFile *f1 = new TFile(name1,"READ");
  hbefore = (TH1F*)f1->Get("hResTheta");
  hbefore->SetTitle("ResThetaBEFORE");
  //  hbefore->Print();
  TF1 *funrth_b = new TF1("fitrth_b","gaus",-0.01,0.01);
  funrth_b->SetParameters(100,0,3e-3);
  funrth_b->SetParNames("Constant","Mean","Sigma");
  hbefore->Fit(funrth_b);
  double thMean_b = funrth_b->GetParameter("Mean");
  double thSigma_b = funrth_b->GetParameter("Sigma");
  cout<<""<<endl;
  std::cout<<"Before: mean="<<thMean_b<<", sigma="<<thSigma_b<<std::endl;
  // std::cout<<"Read all from 1st file!"<<std::endl;
 
 TFile *f2 = new TFile(name2,"READ");
 hafter = (TH1F*)f2->Get("hResTheta");
 hafter->SetTitle("ResThetaAFTER");
 // hafter->Print();
 TF1 *funrth_a = new TF1("fitrth_a","gaus",-0.01,0.01);
 funrth_a->SetParameters(100,0,3e-3);
 funrth_a->SetParNames("Constant","Mean","Sigma");
 hafter->Fit(funrth_a);
 double thMean_a = funrth_a->GetParameter("Mean");
 double thSigma_a = funrth_a->GetParameter("Sigma");
 cout<<""<<endl;
 // f2->Close();
 std::cout<<"After: mean="<<thMean_a<<", sigma="<<thSigma_a<<std::endl;
 
 // ---- Output file ----------------------------------------------------------------
 TString out = path + "ResThetaBeforeAndAfter.root";
 TFile *f = new TFile(out,"RECREATE");
 // ---------------------------------------------------------------------------------
 
 TCanvas *cdiff = new TCanvas("difftheta");
 cdiff->Divide(2,2);
 cdiff->cd(1);
 hbefore->Draw();
 cdiff->cd(2);
 hafter->SetLineColor(3);
 hafter->Draw();
 cdiff->cd(3);
 hbefore->Draw();
 hafter->SetLineColor(3);
 hafter->Draw("same");
 cdiff->Write();
 cdiff->Close();
 f->Write();
 f->Close();


 /// END (Theta resolution) ---------------------------------------------------------------

 ///Residuals ------------------------------------------------------------------------------
 TString nameresb = path + "ResidualsHist_before.root";
 TString nameresa = path + "ResidualsHist_after.root";
 TFile *fresb = new TFile(nameresb,"READ");
 TFile *fresa = new TFile(nameresa,"READ");
 TH2F *hresxb = (TH2F*)fresb->Get("resx");
 double resxb = hresxb->ProjectionY()->GetRMS();
 TH2F *hresxa = (TH2F*)fresa->Get("resx");
 double resxa = hresxa->ProjectionY()->GetRMS();
 TH2F *hresyb = (TH2F*)fresb->Get("resy");
 double resyb = hresyb->ProjectionY()->GetRMS();
 TH2F *hresya = (TH2F*)fresa->Get("resy");
 double resya = hresya->ProjectionY()->GetRMS();
 /// END (Residuals) -----------------------------------------------------------------------

 ///Misalignment constants -----------------------------------------------------------------
 TString namemisc = path + "Results.root";
 TFile *fmisc = new TFile(namemisc,"READ");
 TH2F *hmis_b_0 = (TH2F*)fmisc->Get("mis_before_0");
 double mis_b_0 = hmis_b_0->ProjectionY()->GetRMS();
 TH2F *hmis_b_1 = (TH2F*)fmisc->Get("mis_before_1");
 double mis_b_1 = hmis_b_1->ProjectionY()->GetRMS();
 TH2F *hmis_b_2 = (TH2F*)fmisc->Get("mis_before_2");
 double mis_b_2 = hmis_b_2->ProjectionY()->GetRMS();
 TH2F *hmis_b_3 = (TH2F*)fmisc->Get("mis_before_3");
 double mis_b_3 = hmis_b_3->ProjectionY()->GetRMS();
 TH2F *hmis_b_4 = (TH2F*)fmisc->Get("mis_before_4");
 double mis_b_4 = hmis_b_4->ProjectionY()->GetRMS();
 TH2F *hmis_b_5 = (TH2F*)fmisc->Get("mis_before_5");
 double mis_b_5 = hmis_b_5->ProjectionY()->GetRMS();

 TH2F *hmis_a_0 = (TH2F*)fmisc->Get("mis_after_0");
 double mis_a_0 = hmis_a_0->ProjectionY()->GetRMS();
 TH2F *hmis_a_1 = (TH2F*)fmisc->Get("mis_after_1");
 double mis_a_1 = hmis_a_1->ProjectionY()->GetRMS();
 TH2F *hmis_a_2 = (TH2F*)fmisc->Get("mis_after_2");
 double mis_a_2 = hmis_a_2->ProjectionY()->GetRMS();
 TH2F *hmis_a_3 = (TH2F*)fmisc->Get("mis_after_3");
 double mis_a_3 = hmis_a_3->ProjectionY()->GetRMS();
 TH2F *hmis_a_4 = (TH2F*)fmisc->Get("mis_after_4");
 double mis_a_4 = hmis_a_4->ProjectionY()->GetRMS();
 TH2F *hmis_a_5 = (TH2F*)fmisc->Get("mis_after_5");
 double mis_a_5 = hmis_a_5->ProjectionY()->GetRMS();
 /// END (Misalignment constants) -----------------------------------------------------------

 TFile *fres;
  TGraph *resx_b;
  TGraph *resy_b;
  TGraph *resx_a;
  TGraph *resy_a;
  TGraph *th_mean;
  TGraph *th_sigma;

  TGraph *misal_b_0;
  TGraph *misal_a_0;
  TGraph *misal_b_1;
  TGraph *misal_a_1;
  TGraph *misal_b_2;
  TGraph *misal_a_2;
  TGraph *misal_b_3;
  TGraph *misal_a_3;
  TGraph *misal_b_4;
  TGraph *misal_a_4;
  TGraph *misal_b_5;
  TGraph *misal_a_5;

  if(crGraph<1){
    fres = new TFile(resname,"RECREATE");
    resx_b = new TGraph(poNum);
    resx_b->SetName("resx_b");
    resy_b = new TGraph(poNum);
    resy_b->SetName("resy_b");
    resx_a = new TGraph(poNum);
    resx_a->SetName("resx_a");
    resy_a = new TGraph(poNum);
    resy_a->SetName("resy_a");
    th_mean = new TGraph(poNum);
    th_mean->SetName("th_mean");
    th_sigma = new TGraph(poNum);
    th_sigma->SetName("th_sigma");

    misal_b_0 = new TGraph(poNum);
    misal_b_0->SetName("misal_b_0");
    misal_a_0 = new TGraph(poNum);
    misal_a_0->SetName("misal_a_0");
    misal_b_1 = new TGraph(poNum);
    misal_b_1->SetName("misal_b_1");
    misal_a_1 = new TGraph(poNum);
    misal_a_1->SetName("misal_a_1");
    misal_b_2 = new TGraph(poNum);
    misal_b_2->SetName("misal_b_2");
    misal_a_2 = new TGraph(poNum);
    misal_a_2->SetName("misal_a_2");
    misal_b_3 = new TGraph(poNum);
    misal_b_3->SetName("misal_b_3");
    misal_a_3 = new TGraph(poNum);
    misal_a_3->SetName("misal_a_3");
    misal_b_4 = new TGraph(poNum);
    misal_b_4->SetName("misal_b_4");
    misal_a_4 = new TGraph(poNum);
    misal_a_4->SetName("misal_a_4");
    misal_b_5 = new TGraph(poNum);
    misal_b_5->SetName("misal_b_5");
    misal_a_5 = new TGraph(poNum);
    misal_a_5->SetName("misal_a_5");

  }
  else{
    fres = new TFile(resname,"UPDATE");
    resx_b = (TGraph*)fres->Get("resx_b");
    resy_b = (TGraph*)fres->Get("resy_b");
    resx_a = (TGraph*)fres->Get("resx_a");
    resy_a = (TGraph*)fres->Get("resy_a");
    th_mean = (TGraph*)fres->Get("th_mean");
    th_sigma = (TGraph*)fres->Get("th_sigma");
    misal_b_0 = (TGraph*)fres->Get("misal_b_0");
    misal_a_0 = (TGraph*)fres->Get("misal_a_0");
    misal_b_1 = (TGraph*)fres->Get("misal_b_1");
    misal_a_1 = (TGraph*)fres->Get("misal_a_1");
    misal_b_2 = (TGraph*)fres->Get("misal_b_2");
    misal_a_2 = (TGraph*)fres->Get("misal_a_2");
    misal_b_3 = (TGraph*)fres->Get("misal_b_3");
    misal_a_3 = (TGraph*)fres->Get("misal_a_3");
    misal_b_4 = (TGraph*)fres->Get("misal_b_4");
    misal_a_4 = (TGraph*)fres->Get("misal_a_4");
    misal_b_5 = (TGraph*)fres->Get("misal_b_5");
    misal_a_5 = (TGraph*)fres->Get("misal_a_5");
  }
  th_mean->SetPoint(crGraph,misalsc,thMean_a);
  th_sigma->SetPoint(crGraph,misalsc,thSigma_a);
  th_mean->Write("th_mean",2);//option 2 = kOverwrite
  th_sigma->Write("th_sigma",2);

  resx_b->SetPoint(crGraph,misalsc,resxb);
  resx_a->SetPoint(crGraph,misalsc,resxa);
  resy_b->SetPoint(crGraph,misalsc,resyb);
  resy_a->SetPoint(crGraph,misalsc,resya);
  resx_b->Write("resx_b",2);
  resx_a->Write("resx_a",2);
  resy_b->Write("resy_b",2);
  resy_a->Write("resy_a",2);

 
  misal_b_0->SetPoint(crGraph,misalsc,mis_b_0);
  misal_a_0->SetPoint(crGraph,misalsc,mis_a_0); 
  misal_b_1->SetPoint(crGraph,misalsc,mis_b_1);
  misal_a_1->SetPoint(crGraph,misalsc,mis_a_1);
  misal_b_2->SetPoint(crGraph,misalsc,mis_b_2);
  misal_a_2->SetPoint(crGraph,misalsc,mis_a_2);
  misal_b_3->SetPoint(crGraph,misalsc,mis_b_3);
  misal_a_3->SetPoint(crGraph,misalsc,mis_a_3);
  misal_b_4->SetPoint(crGraph,misalsc,mis_b_4);
  misal_a_4->SetPoint(crGraph,misalsc,mis_a_4);
  misal_b_5->SetPoint(crGraph,misalsc,mis_b_5);
  misal_a_5->SetPoint(crGraph,misalsc,mis_a_5);
  misal_b_0->Write("misal_b_0",2);
  misal_a_0->Write("misal_a_0",2); 
  misal_b_1->Write("misal_b_1",2);
  misal_a_1->Write("misal_a_1",2);
  misal_b_2->Write("misal_b_2",2);
  misal_a_2->Write("misal_a_2",2);
  misal_b_3->Write("misal_b_3",2);
  misal_a_3->Write("misal_a_3",2);
  misal_b_4->Write("misal_b_4",2);
  misal_a_4->Write("misal_a_4",2);
  misal_b_5->Write("misal_b_5",2);
  misal_a_5->Write("misal_a_5",2);
  fres->Write();
}
