#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TCanvas.h>

#include <sstream>
using namespace std;
void SummaryAlign(TString path="/panda/pandaroot/macro/lmd/tmpOutputAlignTMP/", double tr_sc=200, double rt_sc=3, int nTrks)
{
  //start a PROOF-Lite session
  //  TProof::Open("");
  TString pathfull = path+"/results_";
  pathfull+=tr_sc;
  pathfull+="mkm_";
  pathfull+=rt_sc;
  pathfull+="mrad_";
  pathfull+=nTrks;
  pathfull+="events";
  TString resname= pathfull+"/ResultsSummary_";
  resname+=tr_sc;
  resname+="um_";
  resname+=rt_sc;
  resname+="mrad_";
  resname+=nTrks;
  resname+="simevents";

  // TString resthname= path+"/ResTheta_";
  // resthname+=tr_sc;
  // resthname+="um_";
  // resthname+=rt_sc;
  // resthname+="mrad.root";

  /// Theta resolution--------------------------------------------------------------------------------------  
  TString name1 = pathfull + "/Lumi_out_MC_and_REC_trks_matches_with_IDs0_dr_";
  name1+=tr_sc;
  name1+="_da_";
  name1+=rt_sc;
  name1+="_before.root";

  TString name2 = pathfull + "/Lumi_out_MC_and_REC_trks_matches_with_IDs0_dr_";
  name2+=tr_sc;
  name2+="_da_";
  name2+=rt_sc;
  name2+="_after.root";

  TString name3 = path + "/results_0mkm_0mrad_";
  name3+=nTrks;
  name3+="events/Lumi_out_MC_and_REC_trks_matches_with_IDs0_dr_0_da_0_before.root";

  //  TH1F *hbeforetmp;   TH1F *haftertmp;
  TH1F *hbefore;   
  TH1F *hafter;
  TH1F *hrefer;

  TFile *f1 = new TFile(name1,"READ");
  if (f1->IsZombie()) {
    std::cout << "!!! Error opening file " <<name1<< std::endl;
    //   return;
  }

  hbefore = (TH1F*)f1->Get("hResTheta");
  hbefore->SetName("hResThetaBefore");
  hbefore->SetTitle("#theta resolution");
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
  if (f2->IsZombie()) {
    std::cout << "!!! Error opening file " <<name2<< std::endl;
    // return;
  }
  // TFile *fresth = new TFile(resthname,"RECREATE");

  hafter = (TH1F*)f2->Get("hResTheta");
  hafter->SetName("hResThetaAfter");
  hafter->SetTitle("#theta resolution");
  ///hafter->Write();
  //fresth->Close();
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
 
  TFile *f3 = new TFile(name3,"READ");
  if (f3->IsZombie()) {
    std::cout << "!!! Error opening file " <<name2<< std::endl;
    // return;
  }

  hrefer = (TH1F*)f3->Get("hResTheta");
  hrefer->SetName("hResThetaRef");
  hrefer->SetTitle("#theta resolution");
  // hafter->Print();
  TF1 *funrth_r = new TF1("fitrth_r","gaus",-0.01,0.01);
  funrth_r->SetParameters(100,0,3e-3);
  funrth_r->SetParNames("Constant","Mean","Sigma");
  hrefer->Fit(funrth_r);
  double thMean_r = funrth_r->GetParameter("Mean");
  double thSigma_r = funrth_r->GetParameter("Sigma");
  cout<<""<<endl;
  // f2->Close();
  std::cout<<"Reference: mean="<<thMean_r<<", sigma="<<thSigma_r<<std::endl;
  /// END (Theta resolution) ---------------------------------------------------------------

  // ///Residuals ------------------------------------------------------------------------------
  TString nameresb0 = pathfull + "/hitsRes_before_alignment_sector0.root";
  TString nameresa0 = pathfull + "/hitsRes_after_alignment_sector0.root";
  TString nameresb1 = pathfull + "/hitsRes_before_alignment_sector1.root";
  TString nameresa1 = pathfull + "/hitsRes_after_alignment_sector1.root";
  TString nameresb2 = pathfull + "/hitsRes_before_alignment_sector2.root";
  TString nameresa2 = pathfull + "/hitsRes_after_alignment_sector2.root";
  TString nameresb3 = pathfull + "/hitsRes_before_alignment_sector3.root";
  TString nameresa3 = pathfull + "/hitsRes_after_alignment_sector3.root";
  TString nameresb4 = pathfull + "/hitsRes_before_alignment_sector4.root";
  TString nameresa4 = pathfull + "/hitsRes_after_alignment_sector4.root";

  TFile *fresb0 = new TFile(nameresb0,"READ");
  if (fresb0->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb0<< std::endl;
    // return;
  }
  TFile *fresa0 = new TFile(nameresa0,"READ");
  if (fresa0->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresa0<< std::endl;
    //return;
  }
  TFile *fresb1 = new TFile(nameresb1,"READ");
  if (fresb1->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb1<< std::endl;
    // return;
  }
  TFile *fresa1 = new TFile(nameresa1,"READ");
  if (fresa1->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresa1<< std::endl;
    //return;
  }
  TFile *fresb2 = new TFile(nameresb2,"READ");
  if (fresb2->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb2<< std::endl;
    // return;
  }
  TFile *fresa2 = new TFile(nameresa2,"READ");
  if (fresa2->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresa2<< std::endl;
    //return;
  }
TFile *fresb3 = new TFile(nameresb3,"READ");
  if (fresb3->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb3<< std::endl;
    // return;
  }
  TFile *fresa3 = new TFile(nameresa3,"READ");
  if (fresa3->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresa3<< std::endl;
    //return;
  }
  TFile *fresb4 = new TFile(nameresb4,"READ");
  if (fresb4->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb4<< std::endl;
    // return;
  }
  TFile *fresa4 = new TFile(nameresa4,"READ");
  if (fresa4->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresa4<< std::endl;
    //return;
  }

  TH2F *hresxb0 = (TH2F*)fresb0->Get("resx");
  TH2F *hresxa0 = (TH2F*)fresa0->Get("resx");
  TH2F *hresyb0 = (TH2F*)fresb0->Get("resy");
  TH2F *hresya0 = (TH2F*)fresa0->Get("resy");

  TH2F *hresxb1 = (TH2F*)fresb1->Get("resx");
  TH2F *hresxa1 = (TH2F*)fresa1->Get("resx");
  TH2F *hresyb1 = (TH2F*)fresb1->Get("resy");
  TH2F *hresya1 = (TH2F*)fresa1->Get("resy");
  TH2F *hresxb2 = (TH2F*)fresb2->Get("resx");
  TH2F *hresxa2 = (TH2F*)fresa2->Get("resx");
  TH2F *hresyb2 = (TH2F*)fresb2->Get("resy");
  TH2F *hresya2 = (TH2F*)fresa2->Get("resy");
  TH2F *hresxb3 = (TH2F*)fresb3->Get("resx");
  TH2F *hresxa3 = (TH2F*)fresa3->Get("resx");
  TH2F *hresyb3 = (TH2F*)fresb3->Get("resy");
  TH2F *hresya3 = (TH2F*)fresa3->Get("resy");
  TH2F *hresxb4 = (TH2F*)fresb4->Get("resx");
  TH2F *hresxa4 = (TH2F*)fresa4->Get("resx");
  TH2F *hresyb4 = (TH2F*)fresb4->Get("resy");
  TH2F *hresya4 = (TH2F*)fresa4->Get("resy");

  // /// END (Residuals) -----------------------------------------------------------------------

  ///Misalignment constants -----------------------------------------------------------------
  TString namemisc = pathfull + "/KnossosResults.root";
  TFile *fmisc = new TFile(namemisc,"READ");
  if (fmisc->IsZombie()) {
    std::cout << "!!! Error opening file " <<namemisc<< std::endl;
    // return;
  }
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

  TH2F *hmis_a_0 = (TH2F*)fmisc->Get("mis_diff_0");
  double mis_a_0 = hmis_a_0->ProjectionY()->GetRMS();
  TH2F *hmis_a_1 = (TH2F*)fmisc->Get("mis_diff_1");
  double mis_a_1 = hmis_a_1->ProjectionY()->GetRMS();
  TH2F *hmis_a_2 = (TH2F*)fmisc->Get("mis_diff_2");
  double mis_a_2 = hmis_a_2->ProjectionY()->GetRMS();
  TH2F *hmis_a_3 = (TH2F*)fmisc->Get("mis_diff_3");
  double mis_a_3 = hmis_a_3->ProjectionY()->GetRMS();
  TH2F *hmis_a_4 = (TH2F*)fmisc->Get("mis_diff_4");
  double mis_a_4 = hmis_a_4->ProjectionY()->GetRMS();
  TH2F *hmis_a_5 = (TH2F*)fmisc->Get("mis_diff_5");
  double mis_a_5 = hmis_a_5->ProjectionY()->GetRMS();
  /// END (Misalignment constants) -----------------------------------------------------------


  TString resname_pdf = resname+".pdf";
  TString resname_pdf_o = resname_pdf+"(";
  TCanvas c1("c1");
  hrefer->SetLineStyle(2);
  hrefer->SetLineColor(13);
  hrefer->SetFillColor(kGray);
  hrefer->SetFillStyle(3003);
  hrefer->Draw();
  hbefore->SetLineColor(kGreen-3);
  hbefore->SetFillColor(kGreen-3);
  hbefore->SetFillStyle(3335);
  hbefore->Draw("same");
  hafter->SetLineColor(kOrange+7);
  hafter->SetFillColor(kOrange+7);
  hafter->SetFillStyle(3353);
  hafter->Draw("same");
  
  c1.Print(resname_pdf_o); //write canvas and keep the ps file open


 TCanvas c3("c3");
  hmis_b_0->SetTitle("translation x");
  hmis_b_0->GetXaxis()->SetTitle("module#");
  hmis_b_0->GetYaxis()->SetTitle("#Delta_{x}, cm");
  hmis_b_0->SetMarkerStyle(21);
  hmis_b_0->SetMarkerColor(kGreen-3);
  hmis_b_0->SetLineColor(kGreen-3);
  hmis_b_0->SetMarkerSize(2.);
  //hmis_b_0->SetMaximum(tr_sc*1.1*1e-4);
  // hmis_b_0->SetMinimum(0.);
  hmis_b_0->Draw("BOX");
  //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
  hmis_a_0->SetTitle("translation x");
  hmis_a_0->GetXaxis()->SetTitle("module#");
  hmis_a_0->GetYaxis()->SetTitle("#Delta_{x}, cm");
  hmis_a_0->SetMarkerStyle(22);
  hmis_a_0->SetMarkerColor(kOrange+7);
  hmis_a_0->SetLineColor(kOrange+7);
  hmis_a_0->SetMarkerSize(2.);
  // hmis_a_0->SetMaximum(tr_sc*1.1*1e-4);
  // hmis_a_0->SetMinimum(0.);
  hmis_a_0->Draw("BOX;same");
  c3.Print(resname_pdf_o); //write canvas and keep the ps file open

  TH1D* hconst_b_0 = hmis_b_0->ProjectionY();
  TH1D* hconst_a_0 = hmis_a_0->ProjectionY();
  hconst_b_0->SetLineColor(kGreen-3);
  hconst_a_0->SetLineColor(kOrange+7);
  hconst_a_0->Draw();
  hconst_b_0->Draw("same");
  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
  hmis_b_1->SetTitle("translation y");
  hmis_b_1->GetXaxis()->SetTitle("module#");
  hmis_b_1->GetYaxis()->SetTitle("#Delta_{y}, cm");
  hmis_b_1->SetMarkerStyle(21);
  hmis_b_1->SetMarkerColor(kGreen-3);
  hmis_b_1->SetLineColor(kGreen-3);
  hmis_b_1->SetMarkerSize(2.);
  // hmis_b_1->SetMaximum(tr_sc*1.1*1e-4);
  // hmis_b_1->SetMinimum(0.);
  hmis_b_1->Draw("BOX");
  //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
  hmis_a_1->SetTitle("translation y");
  hmis_a_1->GetXaxis()->SetTitle("module#");
  hmis_a_1->GetYaxis()->SetTitle("#Delta_{y}, cm");
  hmis_a_1->SetMarkerStyle(22);
  hmis_a_1->SetMarkerColor(kOrange+7);
  hmis_a_1->SetLineColor(kOrange+7);
  hmis_a_1->SetMarkerSize(2.);
  // hmis_a_1->SetMaximum(tr_sc*1.1*1e-4);
  // hmis_a_1->SetMinimum(0.);
  hmis_a_1->Draw("BOX;same");
  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
  TH1D* hconst_b_1 = hmis_b_1->ProjectionY();
  TH1D* hconst_a_1 = hmis_a_1->ProjectionY();
  hconst_b_1->SetLineColor(kGreen-3);
  hconst_a_1->SetLineColor(kOrange+7);
  hconst_a_1->Draw();
  hconst_b_1->Draw("same");
  c3.Print(resname_pdf_o); //write canvas and keep the ps file open

  hmis_b_2->SetTitle("translation z");
  hmis_b_2->GetXaxis()->SetTitle("module#");
  hmis_b_2->GetYaxis()->SetTitle("#Delta_{z}, cm");
  hmis_b_2->SetMarkerStyle(21);
  hmis_b_2->SetMarkerColor(kGreen-3);
  hmis_b_2->SetLineColor(kGreen-3);
  hmis_b_2->SetMarkerSize(2.);
  // hmis_b_2->SetMaximum(tr_sc*1.1*1e-4);
  // hmis_b_2->SetMinimum(0.);
  hmis_b_2->Draw("BOX");
  //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
  hmis_a_2->SetTitle("translation z");
  hmis_a_2->GetXaxis()->SetTitle("module#");
  hmis_a_2->GetYaxis()->SetTitle("#Delta_{z}, cm");
  hmis_a_2->SetMarkerStyle(22);
  hmis_a_2->SetMarkerColor(kOrange+7);
  hmis_a_2->SetLineColor(kOrange+7);
  hmis_a_2->SetMarkerSize(2.);
  // hmis_a_2->SetMaximum(tr_sc*1.1*1e-4);
  // hmis_a_2->SetMinimum(0.);
  hmis_a_2->Draw("BOX;same");
  //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
  hmis_b_3->SetTitle("rotation x");
  hmis_b_3->GetXaxis()->SetTitle("module#");
  hmis_b_3->GetYaxis()->SetTitle("#Delta_{#alpha}, rad");
  hmis_b_3->SetMarkerStyle(21);
  hmis_b_3->SetMarkerColor(kGreen-3);
  hmis_b_3->SetLineColor(kGreen-3);
  hmis_b_3->SetMarkerSize(2.);
  // hmis_b_3->SetMaximum(rt_sc*10.1*1e-3);
  // hmis_b_3->SetMinimum(0.);
  hmis_b_3->Draw("BOX");
  //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
  hmis_a_3->SetTitle("rotation x");
  hmis_a_3->GetXaxis()->SetTitle("module#");
  hmis_a_3->GetYaxis()->SetTitle("#Delta_{#alpha}, rad");
  hmis_a_3->SetMarkerStyle(22);
  hmis_a_3->SetMarkerColor(kOrange+7);
  hmis_a_3->SetLineColor(kOrange+7);
  hmis_a_3->SetMarkerSize(2.);
  // hmis_a_3->SetMaximum(rt_sc*10.1*1e-3);
  // hmis_a_3->SetMinimum(0.);
  hmis_a_3->Draw("BOX;same");
  // c3.Print(resname_pdf_o); //write canvas and keep the ps file open
  hmis_b_4->SetTitle("rotation y");
  hmis_b_4->GetXaxis()->SetTitle("module#");
  hmis_b_4->GetYaxis()->SetTitle("#Delta_{#beta}, rad");
  hmis_b_4->SetMarkerStyle(21);
  hmis_b_4->SetMarkerColor(kGreen-3);
  hmis_b_4->SetLineColor(kGreen-3);
  hmis_b_4->SetMarkerSize(2.);
  // hmis_b_4->SetMaximum(rt_sc*10.1*1e-3);
  // hmis_b_4->SetMinimum(0.);
  hmis_b_4->Draw("BOX");
  //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
  hmis_a_4->SetTitle("rotation y");
  hmis_a_4->GetXaxis()->SetTitle("module#");
  hmis_a_4->GetYaxis()->SetTitle("#Delta_{#beta}, rad");
  hmis_a_4->SetMarkerStyle(22);
  hmis_a_4->SetMarkerColor(kOrange+7);
  hmis_a_4->SetLineColor(kOrange+7);
  hmis_a_4->SetMarkerSize(2.);
  // hmis_a_4->SetMaximum(rt_sc*10.1*1e-3);
  // hmis_a_4->SetMinimum(0.);
  hmis_a_4->Draw("BOX;same");
  // c3.Print(resname_pdf_o); //write canvas and keep the ps file open
  hmis_b_5->SetTitle("rotation z");
  hmis_b_5->GetXaxis()->SetTitle("module#");
  hmis_b_5->GetYaxis()->SetTitle("#Delta_{#gamma}, rad");
  hmis_b_5->SetMarkerStyle(21);
  hmis_b_5->SetMarkerColor(kGreen-3);
  hmis_b_5->SetLineColor(kGreen-3);
  hmis_b_5->SetMarkerSize(2.);
  // hmis_b_5->SetMaximum(rt_sc*1.1*1e-3);
  // hmis_b_5->SetMinimum(0.);
  hmis_b_5->Draw("BOX");
  //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
  hmis_a_5->SetTitle("rotation z");
  hmis_a_5->GetXaxis()->SetTitle("module#");
  hmis_a_5->GetYaxis()->SetTitle("#Delta_{#gamma}, rad");
  hmis_a_5->SetMarkerStyle(22);
  hmis_a_5->SetMarkerColor(kOrange+7);
  hmis_a_5->SetLineColor(kOrange+7);
  hmis_a_5->SetMarkerSize(2.);
  // hmis_a_5->SetMaximum(rt_sc*1.1*1e-3);
  // hmis_a_5->SetMinimum(0.);
  hmis_a_5->Draw("BOX;same");

  c3.Print(resname_pdf_o); //write canvas and keep the ps file open

  TH1D* hconst_b_5 = hmis_b_5->ProjectionY();
  TH1D* hconst_a_5 = hmis_a_5->ProjectionY();
  hconst_b_5->SetLineColor(kGreen-3);
  hconst_a_5->SetLineColor(kOrange+7);
  hconst_a_5->Draw();
  hconst_b_5->Draw("same");
  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
 
  TCanvas c2("residualsx");
  // c2.Divide(3,2);
  hresxb0->SetMarkerStyle(6);
  hresxb0->SetTitle("sector 0");
  hresxb0->SetMarkerColor(kGreen-3);
  hresxb0->SetLineColor(kGreen-3);
  hresyb0->SetMarkerStyle(6);
  hresyb0->SetTitle("sector 0");
  hresyb0->SetMarkerColor(kGreen-3);
  hresyb0->SetLineColor(kGreen-3);
  hresxa0->SetMarkerStyle(6);
  hresxa0->SetMarkerColor(kOrange+7);
  hresxa0->SetLineColor(kOrange+7);
  hresya0->SetMarkerStyle(6);
  hresya0->SetMarkerColor(kOrange+7);
  hresya0->SetLineColor(kOrange+7);

  hresxb1->SetMarkerStyle(6);
  hresxb1->SetMarkerColor(kGreen-3);
  hresxb1->SetLineColor(kGreen-3);
  hresyb1->SetMarkerStyle(6);
  hresyb1->SetMarkerColor(kGreen-3);
  hresyb1->SetLineColor(kGreen-3);
  hresxa1->SetMarkerStyle(6);
  hresxa1->SetMarkerColor(kOrange+7);
  hresxa1->SetLineColor(kOrange+7);
  hresya1->SetMarkerStyle(6);
  hresya1->SetMarkerColor(kOrange+7);
  hresya1->SetLineColor(kOrange+7);
  hresxb1->SetTitle("sector 1");
  hresyb1->SetTitle("sector 1");

  hresxb2->SetMarkerStyle(6);
  hresxb2->SetMarkerColor(kGreen-3);
  hresxb2->SetLineColor(kGreen-3);
  hresyb2->SetMarkerStyle(6);
  hresyb2->SetMarkerColor(kGreen-3);
  hresyb2->SetLineColor(kGreen-3);
  hresxa2->SetMarkerStyle(6);
  hresxa2->SetMarkerColor(kOrange+7);
  hresxa2->SetLineColor(kOrange+7);
  hresya2->SetMarkerStyle(6);
  hresya2->SetMarkerColor(kOrange+7);
  hresya2->SetLineColor(kOrange+7);
  hresxb2->SetTitle("sector 2");
  hresyb2->SetTitle("sector 2");

  hresxb3->SetMarkerStyle(6);
  hresxb3->SetMarkerColor(kGreen-3);
  hresxb3->SetLineColor(kGreen-3);
  hresyb3->SetMarkerStyle(6);
  hresyb3->SetMarkerColor(kGreen-3);
  hresyb3->SetLineColor(kGreen-3);
  hresxa3->SetMarkerStyle(6);
  hresxa3->SetMarkerColor(kOrange+7);
  hresxa3->SetLineColor(kOrange+7);
  hresya3->SetMarkerStyle(6);
  hresya3->SetMarkerColor(kOrange+7);
  hresya3->SetLineColor(kOrange+7);
  hresxb3->SetTitle("sector 3");
  hresyb3->SetTitle("sector 3");

  hresxb4->SetMarkerStyle(6);
  hresxb4->SetMarkerColor(kGreen-3);
  hresxb4->SetLineColor(kGreen-3);
  hresyb4->SetMarkerStyle(6);
  hresyb4->SetMarkerColor(kGreen-3);
  hresyb4->SetLineColor(kGreen-3);
  hresxa4->SetMarkerStyle(6);
  hresxa4->SetMarkerColor(kOrange+7);
  hresxa4->SetLineColor(kOrange+7);
  hresya4->SetMarkerStyle(6);
  hresya4->SetMarkerColor(kOrange+7);
  hresya4->SetLineColor(kOrange+7);
  hresxb4->SetTitle("sector 4");
  hresyb4->SetTitle("sector 4");

  // c2.cd(1);
  hresxb0->Draw("BOX");
  hresxa0->Draw("BOX;same");
  c2.Print(resname_pdf_o); //write canvas and keep the ps file open
  // c2.cd(2);
  hresxb1->Draw("BOX");
  hresxa1->Draw("BOX;same");
  c2.Print(resname_pdf_o); //write canvas and keep the ps file open
  // c2.cd(3);
  hresxb2->Draw("BOX");
  hresxa2->Draw("BOX;same");
  c2.Print(resname_pdf_o); //write canvas and keep the ps file open
  // c2.cd(4);
  hresxb3->Draw("BOX");
  hresxa3->Draw("BOX;same");
  c2.Print(resname_pdf_o); //write canvas and keep the ps file open
  // c2.cd(5);
  hresxb4->Draw("BOX");
  hresxa4->Draw("BOX;same");
  c2.Print(resname_pdf_o); //write canvas and keep the ps file open

  TCanvas c4("residualsy");
  // c4.Divide(3,2);
  // c4.cd(1);
  hresyb0->Draw("BOX");
  hresya0->Draw("BOX;same");
  c4.Print(resname_pdf_o); //write canvas and keep the ps file open
  //  c4.cd(2);
  hresyb1->Draw("BOX");
  hresya1->Draw("BOX;same");
  c4.Print(resname_pdf_o); //write canvas and keep the ps file open
  // c4.cd(3);
  hresyb2->Draw("BOX");
  hresya2->Draw("BOX;same");
  c4.Print(resname_pdf_o); //write canvas and keep the ps file open
  /// c4.cd(4);
  hresyb3->Draw("BOX");
  hresya3->Draw("BOX;same");
  c4.Print(resname_pdf_o); //write canvas and keep the ps file open
  // c4.cd(5);
  hresyb4->Draw("BOX");
  hresya4->Draw("BOX;same");
  //c4.Print(resname_pdf_o); //write canvas and keep the ps file open
  TString resname_pdf_c = resname_pdf+")";
  c4.Print(resname_pdf_c); //canvas is added to "c3.ps" and ps file is closed
 
  // ---- Output file ----------------------------------------------------------------
  TString out = resname+".root";
  TFile *f = new TFile(out,"RECREATE");
  hrefer->Write();
  hbefore->Write();
  hafter->Write();
  hmis_b_0->Write();
  hmis_a_0->Write();
  hmis_b_1->Write();
  hmis_a_1->Write();
  hmis_b_2->Write();
  hmis_a_2->Write();
  hmis_b_3->Write();
  hmis_a_3->Write();
  hmis_b_4->Write();
  hmis_a_4->Write();
  hmis_b_5->Write();
  hmis_a_5->Write();

  f->Write();
  f->Close();
  // // ---------------------------------------------------------------------------------
}
