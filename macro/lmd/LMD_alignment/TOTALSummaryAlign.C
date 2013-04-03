#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TCanvas.h>

#include <sstream>
using namespace std;
//void TOTALSummaryAlign(TString pathG="/panda/pandaroot/macro/lmd/tmpOutputAlignTMP/", double tr_sc=200, double rt_sc=9)
void TOTALSummaryAlign(TString pathG="/panda/pandaroot/macro/lmd/tmpOutputAlignDPM/", double tr_sc=200, double rt_sc=3)
{
  const int nS=8;
    double TrksSim[nS]={10000, 100000, 200000, 300000, 400000, 500000, 700000, 1000000};
  //  double TrksSim[nS]={10, 100, 200, 300, 400, 500, 700, 1000};
  double Trks[nS];
  //How to save data
  TString resname= pathG+"/ResultsTOTALSummary_";
  resname+=tr_sc;
  resname+="um_";
  resname+=rt_sc;
  resname+="mrad";

  TString resname_pdf = resname+".pdf";
  TString resname_pdf_o = resname_pdf+"(";
  TString resname_pdf_c = resname_pdf+")";

  double mis_b[6][nS];
  double mis_a[6][nS];

  double theta_mean_b[nS], theta_mean_a[nS], theta_mean_r[nS];
  double theta_rms_b[nS], theta_rms_a[nS], theta_rms_r[nS];


  for(int i=0;i<nS;i++){
    nTrks =  TrksSim[i];
    //Where search for files
    TString path = pathG+"/results_";
    path+=tr_sc;
    path+="mkm_";
    path+=rt_sc;
    path+="mrad_";
    path+=nTrks;
    path+="events/";

  //   // ///Residuals files for estimate of trks/sector ------------------------------------------------------------------------------
  TString nameresb0 = path + "/hitsRes_before_alignment_sector0.root";
  TString nameresb1 = path + "/hitsRes_before_alignment_sector1.root";
  TString nameresb2 = path + "/hitsRes_before_alignment_sector2.root";
  TString nameresb3 = path + "/hitsRes_before_alignment_sector3.root";
  TString nameresb4 = path + "/hitsRes_before_alignment_sector4.root";
  TString nameresb5 = path + "/hitsRes_before_alignment_sector5.root";
  TString nameresb6 = path + "/hitsRes_before_alignment_sector6.root";
  TString nameresb7 = path + "/hitsRes_before_alignment_sector7.root";
  TString nameresb8 = path + "/hitsRes_before_alignment_sector8.root";
  TString nameresb9 = path + "/hitsRes_before_alignment_sector9.root";
  TFile *fresb0 = new TFile(nameresb0,"READ");
  if (fresb0->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb0<< std::endl;
    // return;
  }
  TFile *fresb1 = new TFile(nameresb1,"READ");
  if (fresb1->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb1<< std::endl;
    // return;
  }
 TFile *fresb2 = new TFile(nameresb2,"READ");
  if (fresb2->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb2<< std::endl;
    // return;
  }
 TFile *fresb3 = new TFile(nameresb3,"READ");
  if (fresb3->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb3<< std::endl;
    // return;
  }
  TFile *fresb4 = new TFile(nameresb4,"READ");
  if (fresb4->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb4<< std::endl;
    // return;
  }
 TFile *fresb5 = new TFile(nameresb5,"READ");
  if (fresb5->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb5<< std::endl;
    // return;
  }
 TFile *fresb6 = new TFile(nameresb5,"READ");
  if (fresb6->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb6<< std::endl;
    // return;
  }
  TFile *fresb7 = new TFile(nameresb7,"READ");
  if (fresb7->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb7<< std::endl;
    // return;
  }
 TFile *fresb8 = new TFile(nameresb8,"READ");
  if (fresb8->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb8<< std::endl;
    // return;
  }
  TFile *fresb9 = new TFile(nameresb9,"READ");
  if (fresb9->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb9<< std::endl;
    // return;
  }
  int trksCur = 0;
  TNtuple* hntuple0 = (TNtuple*)fresb0->Get("nhits");
   trksCur+= 0.25*(hntuple0->GetEntries());
  TNtuple* hntuple1 = (TNtuple*)fresb1->Get("nhits");
  trksCur+= 0.25*(hntuple1->GetEntries());
  TNtuple* hntuple2 = (TNtuple*)fresb2->Get("nhits");
  trksCur+= 0.25*(hntuple2->GetEntries());
  TNtuple* hntuple3 = (TNtuple*)fresb3->Get("nhits");
  trksCur+= 0.25*(hntuple3->GetEntries());
  TNtuple* hntuple4 = (TNtuple*)fresb4->Get("nhits");
  trksCur+= 0.25*(hntuple4->GetEntries());
  TNtuple* hntuple5 = (TNtuple*)fresb5->Get("nhits");
  trksCur+= 0.25*(hntuple5->GetEntries());
  TNtuple* hntuple6 = (TNtuple*)fresb6->Get("nhits");
  trksCur+= 0.25*(hntuple6->GetEntries());
  TNtuple* hntuple7 = (TNtuple*)fresb7->Get("nhits");
  trksCur+= 0.25*(hntuple7->GetEntries());
  TNtuple* hntuple8 = (TNtuple*)fresb8->Get("nhits");
  trksCur+= 0.25*(hntuple8->GetEntries());
  TNtuple* hntuple9 = (TNtuple*)fresb9->Get("nhits");
  trksCur+= 0.25*(hntuple9->GetEntries());
  Trks[i] = int(trksCur*0.1);
  
    ///Misalignment constants -----------------------------------------------------------------
    TString namemisc = path + "/KnossosResults.root";
    TFile *fmisc = new TFile(namemisc,"READ");
    if (fmisc->IsZombie()) {
      std::cout << "!!! Error opening file " <<namemisc<< std::endl;
      // return;
    }
    TH2F *hmis_b_0 = (TH2F*)fmisc->Get("mis_before_0");
    mis_b[0][i] = 1e4*(hmis_b_0->ProjectionY()->GetRMS());
    TH2F *hmis_b_1 = (TH2F*)fmisc->Get("mis_before_1");
    mis_b[1][i] = 1e4*(hmis_b_1->ProjectionY()->GetRMS());
    TH2F *hmis_b_2 = (TH2F*)fmisc->Get("mis_before_2");
    mis_b[2][i] = 1e4*(hmis_b_2->ProjectionY()->GetRMS());
    TH2F *hmis_b_3 = (TH2F*)fmisc->Get("mis_before_3");
    mis_b[3][i] = 1e3*(hmis_b_3->ProjectionY()->GetRMS());
    TH2F *hmis_b_4 = (TH2F*)fmisc->Get("mis_before_4");
    mis_b[4][i] = 1e3*(hmis_b_4->ProjectionY()->GetRMS());
    TH2F *hmis_b_5 = (TH2F*)fmisc->Get("mis_before_5");
    mis_b[5][i] = 1e3*(hmis_b_5->ProjectionY()->GetRMS());
    
    TH2F *hmis_a_0 = (TH2F*)fmisc->Get("mis_diff_0");
    mis_a[0][i] = 1e4*(hmis_a_0->ProjectionY()->GetRMS());
    TH2F *hmis_a_1 = (TH2F*)fmisc->Get("mis_diff_1");
    mis_a[1][i] = 1e4*(hmis_a_1->ProjectionY()->GetRMS());
    TH2F *hmis_a_2 = (TH2F*)fmisc->Get("mis_diff_2");
    mis_a[2][i] = 1e4*(hmis_a_2->ProjectionY()->GetRMS());
    TH2F *hmis_a_3 = (TH2F*)fmisc->Get("mis_diff_3");
    mis_a[3][i] = 1e3*(hmis_a_3->ProjectionY()->GetRMS());
    TH2F *hmis_a_4 = (TH2F*)fmisc->Get("mis_diff_4");
    mis_a[4][i] = 1e3*(hmis_a_4->ProjectionY()->GetRMS());
    TH2F *hmis_a_5 = (TH2F*)fmisc->Get("mis_diff_5");
    mis_a[5][i] = 1e3*(hmis_a_5->ProjectionY()->GetRMS());
    /// END (Misalignment constants) -----------------------------------------------------------


/// Theta resolution--------------------------------------------------------------------------------------  
  TString name1 = path + "/Lumi_out_MC_and_REC_trks_matches_with_IDs0_dr_";
  name1+=tr_sc;
  name1+="_da_";
  name1+=rt_sc;
  name1+="_before.root";

  TString name2 = path + "/Lumi_out_MC_and_REC_trks_matches_with_IDs0_dr_";
  name2+=tr_sc;
  name2+="_da_";
  name2+=rt_sc;
  name2+="_after.root";

  TString name3 = pathG+"/results_0mkm_0mrad_";
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
  theta_mean_b[i]  = 1e6*(funrth_b->GetParameter("Mean"));
  theta_rms_b[i]  = 1e6*(funrth_b->GetParameter("Sigma"));

 
  TFile *f2 = new TFile(name2,"READ");
  if (f2->IsZombie()) {
    std::cout << "!!! Error opening file " <<name2<< std::endl;
    // return;
  }

  hafter = (TH1F*)f2->Get("hResTheta");
  hafter->SetName("hResThetaAfter");
  hafter->SetTitle("#theta resolution");
  
  TF1 *funrth_a = new TF1("fitrth_a","gaus",-0.01,0.01);
  funrth_a->SetParameters(100,0,3e-3);
  funrth_a->SetParNames("Constant","Mean","Sigma");
  hafter->Fit(funrth_a);

  theta_mean_a[i] = 1e6*(funrth_a->GetParameter("Mean"));
  theta_rms_a[i] = 1e6*(funrth_a->GetParameter("Sigma"));
   
  TFile *f3 = new TFile(name3,"READ");
  if (f3->IsZombie()) {
    std::cout << "!!! Error opening file " <<name2<< std::endl;
    // return;
  }

  hrefer = (TH1F*)f3->Get("hResTheta");
  hrefer->SetName("hResThetaRef");
  hrefer->SetTitle("#theta resolution");

  TF1 *funrth_r = new TF1("fitrth_r","gaus",-0.01,0.01);
  funrth_r->SetParameters(100,0,3e-3);
  funrth_r->SetParNames("Constant","Mean","Sigma");
  hrefer->Fit(funrth_r);
  theta_mean_r[i] = 1e6*(funrth_r->GetParameter("Mean"));
  theta_rms_r[i] = 1e6*(funrth_r->GetParameter("Sigma"));
  /// END (Theta resolution) ---------------------------------------------------------------
  }
  TCanvas c1;
  TGraph *grstat = new TGraph(nS,TrksSim,Trks);
  grstat->SetMarkerStyle(22);
  grstat->SetMarkerSize(2.5);
  grstat->Draw("APL");
  //  grstat->GetYaxis()->SetTitle("Average number of rec. trks per sector");
  grstat->GetXaxis()->SetTitle("Number of sim. trks");
  grstat->SetTitle("Average number of rec. trks per sector (used in Knossos)");
  c1.Print(resname_pdf_o); //write canvas and keep the ps file open
  c1.Clear();
  //put together missalignment consts results
  TGraph *gr_mis_b[6];
  TGraph *gr_mis_a[6];
  TMultiGraph *mgr_mis[6];
 
  for (int g=0;g<6;g++){
    gr_mis_b[g] = new TGraph(nS,Trks,mis_b[g]);
    gr_mis_b[g]->SetMarkerStyle(20);
    gr_mis_b[g]->SetMarkerColor(kGreen-3);
    gr_mis_b[g]->SetMarkerSize(2.5);
    gr_mis_a[g] = new TGraph(nS,Trks,mis_a[g]);
    gr_mis_a[g]->SetMarkerStyle(29);
    gr_mis_a[g]->SetMarkerColor(kOrange+7);
    gr_mis_a[g]->SetMarkerSize(2.5);
    mgr_mis[g] = new TMultiGraph();
    mgr_mis[g]->Add(gr_mis_b[g]);
    mgr_mis[g]->Add(gr_mis_a[g]);
    mgr_mis[g]->Draw("AP");
    mgr_mis[g]->GetXaxis()->SetTitle("Average number of rec. trks per sector");
    if(g==0) mgr_mis[g]->GetYaxis()->SetTitle("#Delta_{x}, #mum");
    if(g==1) mgr_mis[g]->GetYaxis()->SetTitle("#Delta_{y}, #mum");
    if(g==2) mgr_mis[g]->GetYaxis()->SetTitle("#Delta_{z}, #mum");
    if(g==3) mgr_mis[g]->GetYaxis()->SetTitle("#Delta_{#alpha}, mrad");
    if(g==4) mgr_mis[g]->GetYaxis()->SetTitle("#Delta_{#beta}, mrad");
    if(g==5) mgr_mis[g]->GetYaxis()->SetTitle("#Delta_{#gamma}, mrad");
    //    char histoName[100];
    //    sprintf(histoName, "misalignemt_param_%d",g);
    //    mgr_mis[g]->GetYaxis()->SetTitle(histoName);
    if(g<2 || g==5) c1.Print(resname_pdf_o); //write canvas and keep the ps file open
    c1.Clear();
  }
  
    //put together theta resolution results
  TGraph *gr_theta_b = new TGraph(nS,Trks, theta_mean_b);
  gr_theta_b->SetMarkerStyle(20); 
  gr_theta_b->SetMarkerColor(kGreen-3); 
  gr_theta_b->SetMarkerSize(2.5); 
  TGraph *gr_theta_a = new TGraph(nS,Trks, theta_mean_a);
  gr_theta_a->SetMarkerStyle(29); 
  gr_theta_a->SetMarkerColor(kOrange+7); 
  gr_theta_a->SetMarkerSize(2.5); 
  TGraph *gr_theta_r = new TGraph(nS,Trks, theta_mean_r);
  gr_theta_r->SetMarkerStyle(21); 
  gr_theta_r->SetMarkerColor(15); 
  gr_theta_r->SetMarkerSize(2.5); 


  TMultiGraph *mgr_theta = new TMultiGraph();
  //  mgr_theta->Add(gr_theta_b);
  mgr_theta->Add(gr_theta_r);
  mgr_theta->Add(gr_theta_a);

  mgr_theta->Draw("AP");
  mgr_theta->GetXaxis()->SetTitle("Average number of rec. trks per sector");
  mgr_theta->GetYaxis()->SetTitle("#theta_{mean}, #murad");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();

 TGraphErrors *gr_theta_rms_b = new TGraphErrors(nS,Trks,theta_rms_b);
  gr_theta_rms_b->SetMarkerStyle(20); 
  gr_theta_rms_b->SetMarkerColor(kGreen-3); 
  gr_theta_rms_b->SetMarkerSize(2.5); 
  TGraphErrors *gr_theta_rms_a = new TGraphErrors(nS,Trks,theta_rms_a);
  gr_theta_rms_a->SetMarkerStyle(29); 
  gr_theta_rms_a->SetMarkerColor(kOrange+7); 
  gr_theta_rms_a->SetMarkerSize(2.5); 
  TGraphErrors *gr_theta_rms_r = new TGraphErrors(nS,Trks,theta_rms_r);
  gr_theta_rms_r->SetMarkerStyle(21); 
  gr_theta_rms_r->SetMarkerColor(15); 
  gr_theta_rms_r->SetMarkerSize(2.5); 
  TMultiGraph *mgr_theta_rms = new TMultiGraph();
  //  mgr_theta_rms->Add(gr_theta_rms_b);
  mgr_theta_rms->Add(gr_theta_rms_r);
  mgr_theta_rms->Add(gr_theta_rms_a);
  mgr_theta_rms->Draw("AP");
  mgr_theta_rms->GetXaxis()->SetTitle("Average number of rec. trks per sector");
  mgr_theta_rms->GetYaxis()->SetTitle("#theta_{#sigma}, #murad");
  c1.Print(resname_pdf_c); //write canvas and close ps file

  TString out = resname+".root";
  TFile *f = new TFile(out,"RECREATE");
  for (int g=0;g<6;g++){
    gr_mis_b[g]->Write();
    gr_mis_a[g]->Write();
    mgr_mis[g]->Write();
  }
  gr_theta_b->Write();
  gr_theta_r->Write();
  gr_theta_a->Write();
  mgr_theta->Write();
  gr_theta_rms_b->Write();
  gr_theta_rms_r->Write();
  gr_theta_rms_a->Write();
  mgr_theta_rms->Write();
  f->Write();
  f->Close();

//   // ///Residuals ------------------------------------------------------------------------------
//   TString nameresb0 = path + "/hitsRes_before_alignment_sector0.root";
//   TString nameresa0 = path + "/hitsRes_after_alignment_sector0.root";
//   TString nameresb1 = path + "/hitsRes_before_alignment_sector1.root";
//   TString nameresa1 = path + "/hitsRes_after_alignment_sector1.root";
//   TString nameresb2 = path + "/hitsRes_before_alignment_sector2.root";
//   TString nameresa2 = path + "/hitsRes_after_alignment_sector2.root";
//   TString nameresb3 = path + "/hitsRes_before_alignment_sector3.root";
//   TString nameresa3 = path + "/hitsRes_after_alignment_sector3.root";
//   TString nameresb4 = path + "/hitsRes_before_alignment_sector4.root";
//   TString nameresa4 = path + "/hitsRes_after_alignment_sector4.root";

//   TFile *fresb0 = new TFile(nameresb0,"READ");
//   if (fresb0->IsZombie()) {
//     std::cout << "!!! Error opening file " <<nameresb0<< std::endl;
//     // return;
//   }
//   TFile *fresa0 = new TFile(nameresa0,"READ");
//   if (fresa0->IsZombie()) {
//     std::cout << "!!! Error opening file " <<nameresa0<< std::endl;
//     //return;
//   }
//   TFile *fresb1 = new TFile(nameresb1,"READ");
//   if (fresb1->IsZombie()) {
//     std::cout << "!!! Error opening file " <<nameresb1<< std::endl;
//     // return;
//   }
//   TFile *fresa1 = new TFile(nameresa1,"READ");
//   if (fresa1->IsZombie()) {
//     std::cout << "!!! Error opening file " <<nameresa1<< std::endl;
//     //return;
//   }
//   TFile *fresb2 = new TFile(nameresb2,"READ");
//   if (fresb2->IsZombie()) {
//     std::cout << "!!! Error opening file " <<nameresb2<< std::endl;
//     // return;
//   }
//   TFile *fresa2 = new TFile(nameresa2,"READ");
//   if (fresa2->IsZombie()) {
//     std::cout << "!!! Error opening file " <<nameresa2<< std::endl;
//     //return;
//   }
// TFile *fresb3 = new TFile(nameresb3,"READ");
//   if (fresb3->IsZombie()) {
//     std::cout << "!!! Error opening file " <<nameresb3<< std::endl;
//     // return;
//   }
//   TFile *fresa3 = new TFile(nameresa3,"READ");
//   if (fresa3->IsZombie()) {
//     std::cout << "!!! Error opening file " <<nameresa3<< std::endl;
//     //return;
//   }
//   TFile *fresb4 = new TFile(nameresb4,"READ");
//   if (fresb4->IsZombie()) {
//     std::cout << "!!! Error opening file " <<nameresb4<< std::endl;
//     // return;
//   }
//   TFile *fresa4 = new TFile(nameresa4,"READ");
//   if (fresa4->IsZombie()) {
//     std::cout << "!!! Error opening file " <<nameresa4<< std::endl;
//     //return;
//   }

//   TH2F *hresxb0 = (TH2F*)fresb0->Get("resx");
//   TH2F *hresxa0 = (TH2F*)fresa0->Get("resx");
//   TH2F *hresyb0 = (TH2F*)fresb0->Get("resy");
//   TH2F *hresya0 = (TH2F*)fresa0->Get("resy");

//   TH2F *hresxb1 = (TH2F*)fresb1->Get("resx");
//   TH2F *hresxa1 = (TH2F*)fresa1->Get("resx");
//   TH2F *hresyb1 = (TH2F*)fresb1->Get("resy");
//   TH2F *hresya1 = (TH2F*)fresa1->Get("resy");
//   TH2F *hresxb2 = (TH2F*)fresb2->Get("resx");
//   TH2F *hresxa2 = (TH2F*)fresa2->Get("resx");
//   TH2F *hresyb2 = (TH2F*)fresb2->Get("resy");
//   TH2F *hresya2 = (TH2F*)fresa2->Get("resy");
//   TH2F *hresxb3 = (TH2F*)fresb3->Get("resx");
//   TH2F *hresxa3 = (TH2F*)fresa3->Get("resx");
//   TH2F *hresyb3 = (TH2F*)fresb3->Get("resy");
//   TH2F *hresya3 = (TH2F*)fresa3->Get("resy");
//   TH2F *hresxb4 = (TH2F*)fresb4->Get("resx");
//   TH2F *hresxa4 = (TH2F*)fresa4->Get("resx");
//   TH2F *hresyb4 = (TH2F*)fresb4->Get("resy");
//   TH2F *hresya4 = (TH2F*)fresa4->Get("resy");

//   // /// END (Residuals) -----------------------------------------------------------------------



 
 //  TCanvas c1("c1");
 //  hrefer->SetLineStyle(2);
 //  hrefer->SetLineColor(13);
 //  hrefer->SetFillColor(kGray);
 //  hrefer->SetFillStyle(3003);
 //  hrefer->Draw();
 //  hbefore->SetLineColor(kGreen-3);
 //  hbefore->SetFillColor(kGreen-3);
 //  hbefore->SetFillStyle(3335);
 //  hbefore->Draw("same");
 //  hafter->SetLineColor(kOrange+7);
 //  hafter->SetFillColor(kOrange+7);
 //  hafter->SetFillStyle(3353);
 //  hafter->Draw("same");
  
 //  c1.Print(resname_pdf_o); //write canvas and keep the ps file open


 // TCanvas c3("c3");
 //  hmis_b_0->SetTitle("translation x");
 //  hmis_b_0->GetXaxis()->SetTitle("module#");
 //  hmis_b_0->GetYaxis()->SetTitle("#Delta_{x}, cm");
 //  hmis_b_0->SetMarkerStyle(21);
 //  hmis_b_0->SetMarkerColor(kGreen-3);
 //  hmis_b_0->SetLineColor(kGreen-3);
 //  hmis_b_0->SetMarkerSize(2.);
 //  //hmis_b_0->SetMaximum(tr_sc*1.1*1e-4);
 //  // hmis_b_0->SetMinimum(0.);
 //  hmis_b_0->Draw("BOX");
 //  //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  hmis_a_0->SetTitle("translation x");
 //  hmis_a_0->GetXaxis()->SetTitle("module#");
 //  hmis_a_0->GetYaxis()->SetTitle("#Delta_{x}, cm");
 //  hmis_a_0->SetMarkerStyle(22);
 //  hmis_a_0->SetMarkerColor(kOrange+7);
 //  hmis_a_0->SetLineColor(kOrange+7);
 //  hmis_a_0->SetMarkerSize(2.);
 //  // hmis_a_0->SetMaximum(tr_sc*1.1*1e-4);
 //  // hmis_a_0->SetMinimum(0.);
 //  hmis_a_0->Draw("BOX;same");
 //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open

 //  TH1D* hconst_b_0 = hmis_b_0->ProjectionY();
 //  TH1D* hconst_a_0 = hmis_a_0->ProjectionY();
 //  hconst_b_0->SetLineColor(kGreen-3);
 //  hconst_a_0->SetLineColor(kOrange+7);
 //  hconst_a_0->Draw();
 //  hconst_b_0->Draw("same");
 //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  hmis_b_1->SetTitle("translation y");
 //  hmis_b_1->GetXaxis()->SetTitle("module#");
 //  hmis_b_1->GetYaxis()->SetTitle("#Delta_{y}, cm");
 //  hmis_b_1->SetMarkerStyle(21);
 //  hmis_b_1->SetMarkerColor(kGreen-3);
 //  hmis_b_1->SetLineColor(kGreen-3);
 //  hmis_b_1->SetMarkerSize(2.);
 //  // hmis_b_1->SetMaximum(tr_sc*1.1*1e-4);
 //  // hmis_b_1->SetMinimum(0.);
 //  hmis_b_1->Draw("BOX");
 //  //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  hmis_a_1->SetTitle("translation y");
 //  hmis_a_1->GetXaxis()->SetTitle("module#");
 //  hmis_a_1->GetYaxis()->SetTitle("#Delta_{y}, cm");
 //  hmis_a_1->SetMarkerStyle(22);
 //  hmis_a_1->SetMarkerColor(kOrange+7);
 //  hmis_a_1->SetLineColor(kOrange+7);
 //  hmis_a_1->SetMarkerSize(2.);
 //  // hmis_a_1->SetMaximum(tr_sc*1.1*1e-4);
 //  // hmis_a_1->SetMinimum(0.);
 //  hmis_a_1->Draw("BOX;same");
 //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  TH1D* hconst_b_1 = hmis_b_1->ProjectionY();
 //  TH1D* hconst_a_1 = hmis_a_1->ProjectionY();
 //  hconst_b_1->SetLineColor(kGreen-3);
 //  hconst_a_1->SetLineColor(kOrange+7);
 //  hconst_a_1->Draw();
 //  hconst_b_1->Draw("same");
 //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open

 //  hmis_b_2->SetTitle("translation z");
 //  hmis_b_2->GetXaxis()->SetTitle("module#");
 //  hmis_b_2->GetYaxis()->SetTitle("#Delta_{z}, cm");
 //  hmis_b_2->SetMarkerStyle(21);
 //  hmis_b_2->SetMarkerColor(kGreen-3);
 //  hmis_b_2->SetLineColor(kGreen-3);
 //  hmis_b_2->SetMarkerSize(2.);
 //  // hmis_b_2->SetMaximum(tr_sc*1.1*1e-4);
 //  // hmis_b_2->SetMinimum(0.);
 //  hmis_b_2->Draw("BOX");
 //  //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  hmis_a_2->SetTitle("translation z");
 //  hmis_a_2->GetXaxis()->SetTitle("module#");
 //  hmis_a_2->GetYaxis()->SetTitle("#Delta_{z}, cm");
 //  hmis_a_2->SetMarkerStyle(22);
 //  hmis_a_2->SetMarkerColor(kOrange+7);
 //  hmis_a_2->SetLineColor(kOrange+7);
 //  hmis_a_2->SetMarkerSize(2.);
 //  // hmis_a_2->SetMaximum(tr_sc*1.1*1e-4);
 //  // hmis_a_2->SetMinimum(0.);
 //  hmis_a_2->Draw("BOX;same");
 //  //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  hmis_b_3->SetTitle("rotation x");
 //  hmis_b_3->GetXaxis()->SetTitle("module#");
 //  hmis_b_3->GetYaxis()->SetTitle("#Delta_{#alpha}, rad");
 //  hmis_b_3->SetMarkerStyle(21);
 //  hmis_b_3->SetMarkerColor(kGreen-3);
 //  hmis_b_3->SetLineColor(kGreen-3);
 //  hmis_b_3->SetMarkerSize(2.);
 //  // hmis_b_3->SetMaximum(rt_sc*10.1*1e-3);
 //  // hmis_b_3->SetMinimum(0.);
 //  hmis_b_3->Draw("BOX");
 //  //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  hmis_a_3->SetTitle("rotation x");
 //  hmis_a_3->GetXaxis()->SetTitle("module#");
 //  hmis_a_3->GetYaxis()->SetTitle("#Delta_{#alpha}, rad");
 //  hmis_a_3->SetMarkerStyle(22);
 //  hmis_a_3->SetMarkerColor(kOrange+7);
 //  hmis_a_3->SetLineColor(kOrange+7);
 //  hmis_a_3->SetMarkerSize(2.);
 //  // hmis_a_3->SetMaximum(rt_sc*10.1*1e-3);
 //  // hmis_a_3->SetMinimum(0.);
 //  hmis_a_3->Draw("BOX;same");
 //  // c3.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  hmis_b_4->SetTitle("rotation y");
 //  hmis_b_4->GetXaxis()->SetTitle("module#");
 //  hmis_b_4->GetYaxis()->SetTitle("#Delta_{#beta}, rad");
 //  hmis_b_4->SetMarkerStyle(21);
 //  hmis_b_4->SetMarkerColor(kGreen-3);
 //  hmis_b_4->SetLineColor(kGreen-3);
 //  hmis_b_4->SetMarkerSize(2.);
 //  // hmis_b_4->SetMaximum(rt_sc*10.1*1e-3);
 //  // hmis_b_4->SetMinimum(0.);
 //  hmis_b_4->Draw("BOX");
 //  //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  hmis_a_4->SetTitle("rotation y");
 //  hmis_a_4->GetXaxis()->SetTitle("module#");
 //  hmis_a_4->GetYaxis()->SetTitle("#Delta_{#beta}, rad");
 //  hmis_a_4->SetMarkerStyle(22);
 //  hmis_a_4->SetMarkerColor(kOrange+7);
 //  hmis_a_4->SetLineColor(kOrange+7);
 //  hmis_a_4->SetMarkerSize(2.);
 //  // hmis_a_4->SetMaximum(rt_sc*10.1*1e-3);
 //  // hmis_a_4->SetMinimum(0.);
 //  hmis_a_4->Draw("BOX;same");
 //  // c3.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  hmis_b_5->SetTitle("rotation z");
 //  hmis_b_5->GetXaxis()->SetTitle("module#");
 //  hmis_b_5->GetYaxis()->SetTitle("#Delta_{#gamma}, rad");
 //  hmis_b_5->SetMarkerStyle(21);
 //  hmis_b_5->SetMarkerColor(kGreen-3);
 //  hmis_b_5->SetLineColor(kGreen-3);
 //  hmis_b_5->SetMarkerSize(2.);
 //  // hmis_b_5->SetMaximum(rt_sc*1.1*1e-3);
 //  // hmis_b_5->SetMinimum(0.);
 //  hmis_b_5->Draw("BOX");
 //  //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  hmis_a_5->SetTitle("rotation z");
 //  hmis_a_5->GetXaxis()->SetTitle("module#");
 //  hmis_a_5->GetYaxis()->SetTitle("#Delta_{#gamma}, rad");
 //  hmis_a_5->SetMarkerStyle(22);
 //  hmis_a_5->SetMarkerColor(kOrange+7);
 //  hmis_a_5->SetLineColor(kOrange+7);
 //  hmis_a_5->SetMarkerSize(2.);
 //  // hmis_a_5->SetMaximum(rt_sc*1.1*1e-3);
 //  // hmis_a_5->SetMinimum(0.);
 //  hmis_a_5->Draw("BOX;same");

 //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open

 //  TH1D* hconst_b_5 = hmis_b_5->ProjectionY();
 //  TH1D* hconst_a_5 = hmis_a_5->ProjectionY();
 //  hconst_b_5->SetLineColor(kGreen-3);
 //  hconst_a_5->SetLineColor(kOrange+7);
 //  hconst_a_5->Draw();
 //  hconst_b_5->Draw("same");
 //  c3.Print(resname_pdf_o); //write canvas and keep the ps file open
 
 //  TCanvas c2("residualsx");
 //  // c2.Divide(3,2);
 //  hresxb0->SetMarkerStyle(6);
 //  hresxb0->SetTitle("sector 0");
 //  hresxb0->SetMarkerColor(kGreen-3);
 //  hresxb0->SetLineColor(kGreen-3);
 //  hresyb0->SetMarkerStyle(6);
 //  hresyb0->SetTitle("sector 0");
 //  hresyb0->SetMarkerColor(kGreen-3);
 //  hresyb0->SetLineColor(kGreen-3);
 //  hresxa0->SetMarkerStyle(6);
 //  hresxa0->SetMarkerColor(kOrange+7);
 //  hresxa0->SetLineColor(kOrange+7);
 //  hresya0->SetMarkerStyle(6);
 //  hresya0->SetMarkerColor(kOrange+7);
 //  hresya0->SetLineColor(kOrange+7);

 //  hresxb1->SetMarkerStyle(6);
 //  hresxb1->SetMarkerColor(kGreen-3);
 //  hresxb1->SetLineColor(kGreen-3);
 //  hresyb1->SetMarkerStyle(6);
 //  hresyb1->SetMarkerColor(kGreen-3);
 //  hresyb1->SetLineColor(kGreen-3);
 //  hresxa1->SetMarkerStyle(6);
 //  hresxa1->SetMarkerColor(kOrange+7);
 //  hresxa1->SetLineColor(kOrange+7);
 //  hresya1->SetMarkerStyle(6);
 //  hresya1->SetMarkerColor(kOrange+7);
 //  hresya1->SetLineColor(kOrange+7);
 //  hresxb1->SetTitle("sector 1");
 //  hresyb1->SetTitle("sector 1");

 //  hresxb2->SetMarkerStyle(6);
 //  hresxb2->SetMarkerColor(kGreen-3);
 //  hresxb2->SetLineColor(kGreen-3);
 //  hresyb2->SetMarkerStyle(6);
 //  hresyb2->SetMarkerColor(kGreen-3);
 //  hresyb2->SetLineColor(kGreen-3);
 //  hresxa2->SetMarkerStyle(6);
 //  hresxa2->SetMarkerColor(kOrange+7);
 //  hresxa2->SetLineColor(kOrange+7);
 //  hresya2->SetMarkerStyle(6);
 //  hresya2->SetMarkerColor(kOrange+7);
 //  hresya2->SetLineColor(kOrange+7);
 //  hresxb2->SetTitle("sector 2");
 //  hresyb2->SetTitle("sector 2");

 //  hresxb3->SetMarkerStyle(6);
 //  hresxb3->SetMarkerColor(kGreen-3);
 //  hresxb3->SetLineColor(kGreen-3);
 //  hresyb3->SetMarkerStyle(6);
 //  hresyb3->SetMarkerColor(kGreen-3);
 //  hresyb3->SetLineColor(kGreen-3);
 //  hresxa3->SetMarkerStyle(6);
 //  hresxa3->SetMarkerColor(kOrange+7);
 //  hresxa3->SetLineColor(kOrange+7);
 //  hresya3->SetMarkerStyle(6);
 //  hresya3->SetMarkerColor(kOrange+7);
 //  hresya3->SetLineColor(kOrange+7);
 //  hresxb3->SetTitle("sector 3");
 //  hresyb3->SetTitle("sector 3");

 //  hresxb4->SetMarkerStyle(6);
 //  hresxb4->SetMarkerColor(kGreen-3);
 //  hresxb4->SetLineColor(kGreen-3);
 //  hresyb4->SetMarkerStyle(6);
 //  hresyb4->SetMarkerColor(kGreen-3);
 //  hresyb4->SetLineColor(kGreen-3);
 //  hresxa4->SetMarkerStyle(6);
 //  hresxa4->SetMarkerColor(kOrange+7);
 //  hresxa4->SetLineColor(kOrange+7);
 //  hresya4->SetMarkerStyle(6);
 //  hresya4->SetMarkerColor(kOrange+7);
 //  hresya4->SetLineColor(kOrange+7);
 //  hresxb4->SetTitle("sector 4");
 //  hresyb4->SetTitle("sector 4");

 //  // c2.cd(1);
 //  hresxb0->Draw("BOX");
 //  hresxa0->Draw("BOX;same");
 //  c2.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  // c2.cd(2);
 //  hresxb1->Draw("BOX");
 //  hresxa1->Draw("BOX;same");
 //  c2.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  // c2.cd(3);
 //  hresxb2->Draw("BOX");
 //  hresxa2->Draw("BOX;same");
 //  c2.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  // c2.cd(4);
 //  hresxb3->Draw("BOX");
 //  hresxa3->Draw("BOX;same");
 //  c2.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  // c2.cd(5);
 //  hresxb4->Draw("BOX");
 //  hresxa4->Draw("BOX;same");
 //  c2.Print(resname_pdf_o); //write canvas and keep the ps file open

 //  TCanvas c4("residualsy");
 //  // c4.Divide(3,2);
 //  // c4.cd(1);
 //  hresyb0->Draw("BOX");
 //  hresya0->Draw("BOX;same");
 //  c4.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  //  c4.cd(2);
 //  hresyb1->Draw("BOX");
 //  hresya1->Draw("BOX;same");
 //  c4.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  // c4.cd(3);
 //  hresyb2->Draw("BOX");
 //  hresya2->Draw("BOX;same");
 //  c4.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  /// c4.cd(4);
 //  hresyb3->Draw("BOX");
 //  hresya3->Draw("BOX;same");
 //  c4.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  // c4.cd(5);
 //  hresyb4->Draw("BOX");
 //  hresya4->Draw("BOX;same");
 //  //c4.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  TString resname_pdf_c = resname_pdf+")";
 //  c4.Print(resname_pdf_c); //canvas is added to "c3.ps" and ps file is closed
 
 //  // ---- Output file ----------------------------------------------------------------
 //  TString out = resname+".root";
 //  TFile *f = new TFile(out,"RECREATE");
 //  hrefer->Write();
 //  hbefore->Write();
 //  hafter->Write();
 //  hmis_b_0->Write();
 //  hmis_a_0->Write();
 //  hmis_b_1->Write();
 //  hmis_a_1->Write();
 //  hmis_b_2->Write();
 //  hmis_a_2->Write();
 //  hmis_b_3->Write();
 //  hmis_a_3->Write();
 //  hmis_b_4->Write();
 //  hmis_a_4->Write();
 //  hmis_b_5->Write();
 //  hmis_a_5->Write();

 //  f->Write();
 //  f->Close();
 //  // // ---------------------------------------------------------------------------------
}
