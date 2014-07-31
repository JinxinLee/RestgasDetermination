#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TCanvas.h>

#include <sstream>
using namespace std;
void trickySummaryAlign(TString pathGi="/panda/pandaroot/macro/lmd/testPixelAlignSIM_", double tr_sc=0, double rt_sc=0)
//void SummaryAlign(TString pathG="/panda/pandaroot/macro/lmd/testPixelAlignSIM_10000/mom_15//", double tr_sc=0, double rt_sc=0)
{

  TString pathG1=pathGi+"1000000/mom_15/";
  //  TString pathG2=pathGi+"400000/mom_15/";
  TString pathG2=pathGi+"1000000/mom_15/";
  const int colors[4] = {28,kAzure+2,46,8};
  const int colors_a[4] = {kViolet-6,kOrange+7,kGreen+3,kRed+1};
  const int ntrksSample = 1e4;
  const double i_TrksSimi = 100./ntrksSample; //relative to simulated in %
  const int nParDt=6;
  double v_Dt[nParDt]={0, 50, 100, 200, 300, 400};
  
 const int nParDa=4;
 double v_Da[nParDa]={0,1,3,5};
 const int nS=1;
 double TrksSim[nParDa][nParDt];
 
 double TrksBef[nParDa][nParDt];
 double TrksAft[nParDa][nParDt];
  double TrksDiff[nParDa][nParDt];
  double TrksKnossos[nParDa][nParDt], errTrksKnossos[nParDa][nParDt];
  double TrksKnossos0[nParDa][nParDt], errTrksKnossos0[nParDa][nParDt];

  //How to save data
  TString resname= pathG1+"/Results1SampleSummary_BOX";
  TString resname_pdf = resname+".pdf";
  TString resname_pdf_o = resname_pdf+"(";
  TString resname_pdf_c = resname_pdf+")";

  double mis_b[nParDa][6][nParDt];
  double mis_a[nParDa][6][nParDt];
  double mis_v_b[nParDa][6][nParDt];
  double mis_v_a[nParDa][6][nParDt];

  double theta_mean_b[nParDa][nParDt], theta_mean_a[nParDa][nParDt], theta_mean_r[nParDa][nParDt];
  double theta_rms_b[nParDa][nParDt], theta_rms_a[nParDa][nParDt], theta_rms_r[nParDa][nParDt];

  double err_theta_mean_b[nParDa][nParDt], err_theta_mean_a[nParDa][nParDt], err_theta_mean_r[nParDa][nParDt];
  double err_theta_rms_b[nParDa][nParDt], err_theta_rms_a[nParDa][nParDt], err_theta_rms_r[nParDa][nParDt];
  // TH1D *hthetabefore = new TH1D("hthetabefore","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e2,-1e-3,1e-3);
  // TH1D *hthetaafter = new TH1D("hthetaafter","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e2,-1e-3,1e-3);
  // TH1D *hthetaref = new TH1D("hthetaref","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e2,-1e-3,1e-3);
  for(int ipart=0;ipart<nParDa;ipart++){
    for(int i=0;i<nParDt;i++){
      TrksSim[ipart][i]=ntrksSample ; //ntrksSample events with 1trk/event
      theta_mean_b[ipart][i] = 0;
      theta_mean_a[ipart][i] = 0;
      theta_mean_r[ipart][i] = 0;
      theta_rms_b[ipart][i] = 0;
      theta_rms_a[ipart][i] = 0;
      theta_rms_r[ipart][i] = 0;
    }
  }

 for(int ipart=0;ipart<nParDa;ipart++){
    for(int i=0;i<nParDt;i++){
      tr_sc = v_Dt[i];
      rt_sc= v_Da[ipart];
      int nTrks =  TrksSim[ipart][i];
      //Where search for files
      TString pathG = pathG1;
      if(rt_sc<3) pathG = pathG2;
      TString path = pathG+"/";
      path+=tr_sc;
      path+="mkm_";
      path+=rt_sc;
      path+="mrad/";

      /// Theta resolution--------------------------------------------------------------------------------------  
      TString name1 = path + "/Lumi_QA_before.root";
      TString name2 = path + "/Lumi_QA_after.root";
      // TString name2 = path + "/Lumi_QA_after_";
      // name2 +=i;
      // name2 +="SumAlign.root";
      
      
      TString name3 = pathG1+"/0mkm_0mrad";
      name3+="/Lumi_QA_after.root";
      
      //  TH1F *hbeforetmp;   TH1F *haftertmp;
      TH1 *hbefore;   
      TH1 *hafter;
      TH1 *hrefer;
      // TNtuple *nsectors_b;
      // TNtuple *nsectors_a;
      // TNtuple *nsectors_r;
      
      TFile *f1 = new TFile(name1,"READ");
      if (f1->IsZombie()) {
	std::cout << "!!! Error opening file " <<name1<< std::endl;
	continue;
	//   return;
      }
      cout<<name1<<endl;


      hbefore = (TH1*)f1->Get("NearIP/hResTheta");
      //hbefore->Print();
      hbefore->SetName("hResThetaBefore");
      hbefore->SetTitle("#theta resolution");
      //  hbefore->Print();
      TrksBef[ipart][i] = hbefore->GetEntries();
     
  theta_mean_b[ipart][i]  = 1e3*(hbefore->GetMean());
  theta_rms_b[ipart][i]  = 1e3*(hbefore->GetRMS());
  err_theta_mean_b[ipart][i]  = 1e3*(hbefore->GetMeanError());
  err_theta_rms_b[ipart][i]  = 1e3*(hbefore->GetRMSError());
  // nsectors_b = (TNtuple*)f1->Get("nsectors");
  TFile *f2 = new TFile(name2,"READ");
  if (f2->IsZombie()) {
    std::cout << "!!! Error opening file " <<name2<< std::endl;
    continue;
    // return;
  }
  hafter = (TH1F*)f2->Get("NearIP/hResTheta");
  hafter->SetName("hResThetaAfter");
  hafter->SetTitle("#theta resolution");
 
  TrksAft[ipart][i] = hafter->GetEntries();
  TrksDiff[ipart][i] = (TrksAft[ipart][i] - TrksBef[ipart][i]);

  theta_mean_a[ipart][i]  = 1e3*(hafter->GetMean());
  theta_rms_a[ipart][i]  = 1e3*(hafter->GetRMS());
  err_theta_mean_a[ipart][i]  = 1e3*(hafter->GetMeanError());
  err_theta_rms_a[ipart][i]  = 3*1e3*(hafter->GetRMSError());
  //cout<<"err_theta_rms_a[ipart][i] = "<<err_theta_rms_a[ipart][i]<<endl;
  TFile *f3 = new TFile(name3,"READ");
  if (f3->IsZombie()) {
    std::cout << "!!! Error opening file " <<name2<< std::endl;
    continue;
    // return;
  }
  hrefer = (TH1F*)f3->Get("NearIP/hResTheta");
  hrefer->SetName("hResThetaRef");
  hrefer->SetTitle("#theta resolution");

  theta_mean_r[ipart][i]  = 1e3*(hrefer->GetMean());
  theta_rms_r[ipart][i]  = 1e3*(hrefer->GetRMS());
  err_theta_mean_r[ipart][i]  = 1e3*(hrefer->GetMeanError());
  err_theta_rms_r[ipart][i]  = 1e3*(hrefer->GetRMSError());
 
  /// END (Theta resolution) ---------------------------------------------------------------
  f1->Close();
  f2->Close();
  f3->Close();
 ///Misalignment constants -----------------------------------------------------------------
  TString namemisc = pathG+"/";
  namemisc+=tr_sc;
  namemisc+="mkm_";
  namemisc+=rt_sc;
  namemisc+="mrad";
  namemisc+="/KnossosResults.root";
  TFile *fmisc = new TFile(namemisc,"READ");
    if (fmisc->IsZombie()) {
      std::cout << "!!! Error opening file " <<namemisc<< std::endl;
      // return;
    }
    TH2F *hStatKnossos = (TH2F*)fmisc->Get("haStat_out");
    TrksKnossos[ipart][i] = hStatKnossos->ProjectionY()->GetMean();
    errTrksKnossos[ipart][i] = hStatKnossos->ProjectionY()->GetRMS();
    TH2F *hStatKnossos0 = (TH2F*)fmisc->Get("haStat_in");
    TrksKnossos0[ipart][i] = hStatKnossos0->ProjectionY()->GetMean();
    errTrksKnossos0[ipart][i] = hStatKnossos0->ProjectionY()->GetRMS();
    TH2F *hmis_b_0 = (TH2F*)fmisc->Get("mis_before_0");
    mis_b[ipart][0][i] = 1e4*(hmis_b_0->ProjectionY()->GetRMS());
    TH2F *hmis_b_1 = (TH2F*)fmisc->Get("mis_before_1");
    mis_b[ipart][1][i] = 1e4*(hmis_b_1->ProjectionY()->GetRMS());
    TH2F *hmis_b_2 = (TH2F*)fmisc->Get("mis_before_2");
    mis_b[ipart][2][i] = 1e4*(hmis_b_2->ProjectionY()->GetRMS());
    TH2F *hmis_b_3 = (TH2F*)fmisc->Get("mis_before_3");
    mis_b[ipart][3][i] = 1e3*(hmis_b_3->ProjectionY()->GetRMS());
    TH2F *hmis_b_4 = (TH2F*)fmisc->Get("mis_before_4");
    mis_b[ipart][4][i] = 1e3*(hmis_b_4->ProjectionY()->GetRMS());
    TH2F *hmis_b_5 = (TH2F*)fmisc->Get("mis_before_5");
    mis_b[ipart][5][i] = 1e3*(hmis_b_5->ProjectionY()->GetRMS());
    
    TH2F *hmis_a_0 = (TH2F*)fmisc->Get("mis_diff_0");
    mis_a[ipart][0][i] = 1e4*(hmis_a_0->ProjectionY()->GetRMS());
    TH2F *hmis_a_1 = (TH2F*)fmisc->Get("mis_diff_1");
    mis_a[ipart][1][i] = 1e4*(hmis_a_1->ProjectionY()->GetRMS());
    TH2F *hmis_a_2 = (TH2F*)fmisc->Get("mis_diff_2");
    mis_a[ipart][2][i] = 1e4*(hmis_a_2->ProjectionY()->GetRMS());
    TH2F *hmis_a_3 = (TH2F*)fmisc->Get("mis_diff_3");
    mis_a[ipart][3][i] = 1e3*(hmis_a_3->ProjectionY()->GetRMS());
    TH2F *hmis_a_4 = (TH2F*)fmisc->Get("mis_diff_4");
    mis_a[ipart][4][i] = 1e3*(hmis_a_4->ProjectionY()->GetRMS());
    TH2F *hmis_a_5 = (TH2F*)fmisc->Get("mis_diff_5");
    mis_a[ipart][5][i] = 1e3*(hmis_a_5->ProjectionY()->GetRMS());
    

    mis_v_b[ipart][0][i] = 1e4*(hmis_b_0->ProjectionY()->GetMean());
    mis_v_b[ipart][1][i] = 1e4*(hmis_b_1->ProjectionY()->GetMean());
    mis_v_b[ipart][2][i] = 1e4*(hmis_b_2->ProjectionY()->GetMean());
    mis_v_b[ipart][3][i] = 1e3*(hmis_b_3->ProjectionY()->GetMean());
    mis_v_b[ipart][4][i] = 1e3*(hmis_b_4->ProjectionY()->GetMean());
    mis_v_b[ipart][5][i] = 1e3*(hmis_b_5->ProjectionY()->GetMean());
    
    mis_v_a[ipart][0][i] = 1e4*(hmis_a_0->ProjectionY()->GetMean());
    mis_v_a[ipart][1][i] = 1e4*(hmis_a_1->ProjectionY()->GetMean());
    mis_v_a[ipart][2][i] = 1e4*(hmis_a_2->ProjectionY()->GetMean());
    mis_v_a[ipart][3][i] = 1e3*(hmis_a_3->ProjectionY()->GetMean());
    mis_v_a[ipart][4][i] = 1e3*(hmis_a_4->ProjectionY()->GetMean());
    mis_v_a[ipart][5][i] = 1e3*(hmis_a_5->ProjectionY()->GetMean());
    // cout<<"mis_v_a["<<ipart<<"][5]["<<i<<"] = "<<mis_v_a[ipart][5][i]<<endl;
    // cout<<"mis_a["<<ipart<<"][5]["<<i<<"] = "<<mis_a[ipart][5][i]<<endl;
    /// END (Misalignment constants) -----------------------------------------------------------
    fmisc->Close();
    }// trans
 }// rotation
 
 // Draw results --------------------------------
 double min_theta_mean_b = theta_mean_b[0][0], min_theta_rms_b = theta_rms_b[0][0];
 double min_theta_mean_a = theta_mean_a[0][0], min_theta_rms_a = theta_rms_a[0][0];
 double max_theta_mean_b = theta_mean_b[0][0], max_theta_rms_b = theta_rms_b[0][0];
 double max_theta_mean_a = theta_mean_a[0][0], max_theta_rms_a = theta_rms_a[0][0];
 for(int ipart=0;ipart<nParDa;ipart++){
   for(int i=0;i<nParDt;i++){
     if(min_theta_mean_b>theta_mean_b[ipart][i]) min_theta_mean_b = theta_mean_b[ipart][i];
     if(max_theta_mean_b<theta_mean_b[ipart][i]) max_theta_mean_b = theta_mean_b[ipart][i];
     if(min_theta_mean_a>theta_mean_a[ipart][i]) min_theta_mean_a = theta_mean_a[ipart][i];
     if(max_theta_mean_a<theta_mean_a[ipart][i]) max_theta_mean_a = theta_mean_a[ipart][i];

     if(min_theta_rms_b>theta_rms_b[ipart][i]) min_theta_rms_b = theta_rms_b[ipart][i];
     if(max_theta_rms_b<theta_rms_b[ipart][i]) max_theta_rms_b = theta_rms_b[ipart][i];
     if(min_theta_rms_a>theta_rms_a[ipart][i]) min_theta_rms_a = theta_rms_a[ipart][i];
     if(max_theta_rms_a<theta_rms_a[ipart][i]) max_theta_rms_a = theta_rms_a[ipart][i];
   }
 }

 for(int ipart=0;ipart<nParDa;ipart++){
   for(int i=0;i<nParDt;i++){

     if(min_theta_mean_b>theta_mean_r[ipart][i]) min_theta_mean_b = theta_mean_r[ipart][i];
     if(max_theta_mean_b<theta_mean_r[ipart][i]) max_theta_mean_b = theta_mean_r[ipart][i];
     if(min_theta_mean_a>theta_mean_r[ipart][i]) min_theta_mean_a = theta_mean_r[ipart][i];
     if(max_theta_mean_a<theta_mean_r[ipart][i]) max_theta_mean_a = theta_mean_r[ipart][i];

     if(min_theta_rms_b>theta_rms_r[ipart][i]) min_theta_rms_b = theta_rms_r[ipart][i];
     if(max_theta_rms_b<theta_rms_r[ipart][i]) max_theta_rms_b = theta_rms_r[ipart][i];
     if(min_theta_rms_a>theta_rms_r[ipart][i]) min_theta_rms_a = theta_rms_r[ipart][i];
     if(max_theta_rms_a<theta_rms_r[ipart][i]) max_theta_rms_a = theta_rms_r[ipart][i];
   }
 }

 TCanvas c1;
 TLegend *leg_b = new TLegend(0.83,0.65,0.97,0.97);
 leg_b->SetFillColor(0);
 TLegend *leg_a = new TLegend(0.83,0.65,0.97,0.97);
 leg_a->SetFillColor(0);
 // TLegend *leg_b = new TLegend(0.78,0.51,0.98,0.98);
 // leg_b->SetFillColor(0);

 TMultiGraph *mgr_theta = new TMultiGraph();  
 TMultiGraph *mgr_theta_b = new TMultiGraph();  
 TGraphErrors *gr_theta_b[nParDa],*gr_theta_a[nParDa],*gr_theta_r[nParDa];
 for(int ipart=0;ipart<nParDa;ipart++){// loop over rotaion scale
 // double [nParDt], theta_mean_a[nParDa][nParDt], theta_mean_r[nParDa][nParDt];
 //  double [nParDt], theta_rms_a[nParDa][nParDt], theta_rms_r[nParDa][nParDt];
  gr_theta_b[ipart] = new TGraphErrors(nParDt,v_Dt,theta_mean_b[ipart],0,err_theta_mean_b[ipart]);
  gr_theta_b[ipart]->SetMarkerStyle(20+ipart); 
  //  gr_theta_b[ipart]->SetMarkerColor(kGreen-3); 
  //  gr_theta_b[ipart]->SetMarkerColor(30*ipart+8);
  gr_theta_b[ipart]->SetMarkerColor(colors[ipart]);
  gr_theta_b[ipart]->SetLineColor(colors[ipart]);
  // gr_theta_b[ipart]->SetLineColor(kGreen-3+ipart);
  gr_theta_b[ipart]->SetMarkerSize(1.7+ipart*0.3); 
  gr_theta_a[ipart] = new TGraphErrors(nParDt,v_Dt,theta_mean_a[ipart],0,err_theta_mean_a[ipart]);
  gr_theta_a[ipart]->SetMarkerStyle(20+ipart); 
  //gr_theta_a[ipart]->SetMarkerColor(kOrange+7); 
  gr_theta_a[ipart]->SetMarkerColor(colors_a[ipart]); 
  gr_theta_a[ipart]->SetLineColor(colors_a[ipart]);
  gr_theta_a[ipart]->SetMarkerSize(1.7+ipart*0.3); 
  mgr_theta_b->Add(gr_theta_b[ipart]);
  mgr_theta->Add(gr_theta_a[ipart]);
  if(ipart==0){
  gr_theta_r[ipart] = new TGraphErrors(nParDt,v_Dt,theta_mean_r[ipart],0,err_theta_mean_r[ipart]);
  gr_theta_r[ipart]->SetMarkerStyle(20+ipart); 
  gr_theta_r[ipart]->SetMarkerColor(kGray+3); 
  gr_theta_r[ipart]->SetMarkerSize(2.7); 
  mgr_theta_b->Add(gr_theta_r[ipart]);
  mgr_theta->Add(gr_theta_r[ipart]);
  // leg->AddEntry(gr_theta_r[ipart],"ideal","lep");
  leg_b->AddEntry(gr_theta_r[ipart],"ideal","lep");
  leg_a->AddEntry(gr_theta_r[ipart],"ideal","lep");
  }
  TString dRName = "#Delta_{r} = ";
  dRName +=v_Da[ipart];
  //  TString dRName_b = dRName+ " (BEFORE)";  
  // TString dRName_a = dRName+ " (AFTER)";  
  // leg->AddEntry(gr_theta_b[ipart],dRName_b,"lep");
  // leg->AddEntry(gr_theta_a[ipart],dRName_a,"lep");
  TString dRName_b = dRName;  
  leg_b->AddEntry(gr_theta_b[ipart],dRName_b,"lep");
  leg_a->AddEntry(gr_theta_a[ipart],dRName_b,"lep");
 }// END loop over rotaion scale
  
 mgr_theta_b->Draw("AP");
 leg_b->Draw();
 mgr_theta_b->SetTitle("Before");
 mgr_theta_b->GetXaxis()->SetTitle("#Delta_{t}, #mum");
 mgr_theta_b->GetYaxis()->SetTitle("#theta_{mean}, #murad");
 mgr_theta_b->GetHistogram()->SetMaximum(1.5*max_theta_mean_b);
 mgr_theta_b->GetHistogram()->SetMinimum(0.5*min_theta_mean_b);
 if(min_theta_mean_b<0) mgr_theta_b->GetHistogram()->SetMinimum(1.5*min_theta_mean_b);
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 
 
 mgr_theta->Draw("AP");
 leg_a->Draw();
 mgr_theta->SetTitle("After");
 mgr_theta->GetXaxis()->SetTitle("#Delta_{t}, #mum");
 mgr_theta->GetYaxis()->SetTitle("#theta_{mean}, #murad");
 mgr_theta->GetHistogram()->SetMaximum(1.5*max_theta_mean_a);
 mgr_theta->GetHistogram()->SetMinimum(0.5*min_theta_mean_a);
 if(min_theta_mean_a<0) mgr_theta->GetHistogram()->SetMinimum(1.5*min_theta_mean_a);
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 ///// theta rms
 TMultiGraph *mgr_theta_rms = new TMultiGraph();  
 TMultiGraph *mgr_theta_b_rms = new TMultiGraph();  
 for(int ipart=0;ipart<nParDa;ipart++){// loop over rotaion scale
   gr_theta_b[ipart] = new TGraphErrors(nParDt,v_Dt,theta_rms_b[ipart],0,err_theta_rms_b[ipart]);
   gr_theta_b[ipart]->SetMarkerStyle(20+ipart); 
   //  gr_theta_b[ipart]->SetMarkerColor(kGreen-3); 
   gr_theta_b[ipart]->SetMarkerColor(colors[ipart]); 
   gr_theta_b[ipart]->SetLineColor(colors[ipart]);
   gr_theta_b[ipart]->SetMarkerSize(1.7+ipart*0.3); 
   gr_theta_a[ipart] = new TGraphErrors(nParDt,v_Dt,theta_rms_a[ipart],0,err_theta_rms_a[ipart]);
  gr_theta_a[ipart]->SetMarkerStyle(20+ipart); 
  //  gr_theta_a[ipart]->SetMarkerColor(kOrange+7); 
  gr_theta_a[ipart]->SetMarkerColor(colors_a[ipart]); 
  gr_theta_a[ipart]->SetLineColor(colors_a[ipart]);
  gr_theta_a[ipart]->SetMarkerSize(1.7+ipart*0.3); 
  mgr_theta_b_rms->Add(gr_theta_b[ipart]);
  mgr_theta_rms->Add(gr_theta_a[ipart]);
  if(ipart==0){
    gr_theta_r[ipart] = new TGraphErrors(nParDt,v_Dt,theta_rms_r[ipart],0,err_theta_rms_r[ipart]);
    gr_theta_r[ipart]->SetMarkerStyle(20+ipart); 
    gr_theta_r[ipart]->SetMarkerColor(kGray+3); 
    gr_theta_r[ipart]->SetMarkerSize(2.7);
    mgr_theta_b_rms->Add(gr_theta_r[ipart]);
    mgr_theta_rms->Add(gr_theta_r[ipart]);
  }
 }// END loop over rotaion scale
  
 mgr_theta_b_rms->Draw("AP");
 leg_b->Draw();
 mgr_theta_b_rms->GetXaxis()->SetTitle("#Delta_{t}, #mum");
 mgr_theta_b_rms->GetYaxis()->SetTitle("#theta_{rms}, #murad");
 mgr_theta_b_rms->GetHistogram()->SetMaximum(1.5*max_theta_rms_b);
 mgr_theta_b_rms->GetHistogram()->SetMinimum(0.5*min_theta_rms_b);
 mgr_theta_b_rms->SetTitle("Before");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 
 
 mgr_theta_rms->Draw("AP");
 leg_a->Draw();
 mgr_theta_rms->GetXaxis()->SetTitle("#Delta_{t}, #mum");
 mgr_theta_rms->GetYaxis()->SetTitle("#theta_{rms}, #murad");
 mgr_theta_rms->GetHistogram()->SetMaximum(1.5*max_theta_rms_a);
 mgr_theta_rms->GetHistogram()->SetMinimum(0.5*min_theta_rms_a);
 mgr_theta_rms->SetTitle("After");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 
 //put together missalignment consts results
  TGraphErrors *gr_mis_b[6][nParDa];
  TGraphErrors *gr_mis_a[6][nParDa];
  TMultiGraph *mgr_mis[6]; 
  TMultiGraph *mgr_mis_b[6];
  for(int ialc=0;ialc<6;ialc++){
    mgr_mis[ialc] = new TMultiGraph();
    mgr_mis_b[ialc] = new TMultiGraph();
  }
  for(int ipart=0;ipart<nParDa;ipart++){// loop over rotaion scale
    for(int ialc=0;ialc<6;ialc++){
      gr_mis_b[ialc][ipart] = new TGraphErrors(nParDt,v_Dt,mis_v_b[ipart][ialc],0,mis_b[ipart][ialc]);
      gr_mis_b[ialc][ipart]->SetMarkerStyle(20+ipart);
      gr_mis_b[ialc][ipart]->SetMarkerColor(colors[ipart]);
      gr_mis_b[ialc][ipart]->SetLineColor(colors[ipart]);
      gr_mis_b[ialc][ipart]->SetMarkerSize(1.7+ipart*0.3);
      gr_mis_a[ialc][ipart] = new TGraphErrors(nParDt,v_Dt,mis_v_a[ipart][ialc],0,mis_a[ipart][ialc]);
      gr_mis_a[ialc][ipart]->SetMarkerStyle(20+ipart);
      gr_mis_a[ialc][ipart]->SetMarkerColor(colors_a[ipart]);
      gr_mis_a[ialc][ipart]->SetMarkerSize(1.7+ipart*0.3);
      gr_mis_a[ialc][ipart]->SetLineColor(colors_a[ipart]);
      mgr_mis_b[ialc]->Add(gr_mis_b[ialc][ipart]);
      mgr_mis[ialc]->Add(gr_mis_a[ialc][ipart]);
    }
  }// END loop over rotaion scale
 for(int ialc=0;ialc<6;ialc++){
   if(ialc<2 || ialc==5){
     mgr_mis_b[ialc]->Draw("AP");
     leg_b->Draw();
     mgr_mis_b[ialc]->GetXaxis()->SetTitle("#Delta_{t}, #mum");
     mgr_mis_b[ialc]->GetHistogram()->SetMaximum(1200.);
     mgr_mis_b[ialc]->GetHistogram()->SetMinimum(-1200.);
     if(ialc==5){
       mgr_mis_b[ialc]->GetHistogram()->SetMaximum(10.);
       mgr_mis_b[ialc]->GetHistogram()->SetMinimum(-10.);
     }
     mgr_mis_b[ialc]->SetTitle("Before");
     if(ialc==0) mgr_mis_b[ialc]->GetYaxis()->SetTitle("#Delta_{x}, #mum");
     if(ialc==1) mgr_mis_b[ialc]->GetYaxis()->SetTitle("#Delta_{y}, #mum");
     if(ialc==2) mgr_mis_b[ialc]->GetYaxis()->SetTitle("#Delta_{z}, #mum");
     if(ialc==3) mgr_mis_b[ialc]->GetYaxis()->SetTitle("#Delta_{#alpha}, mrad");
     if(ialc==4) mgr_mis_b[ialc]->GetYaxis()->SetTitle("#Delta_{#beta}, mrad");
     if(ialc==5) mgr_mis_b[ialc]->GetYaxis()->SetTitle("#Delta_{#gamma}, mrad");
     c1.Print(resname_pdf_o); //write canvas and keep the file open
     c1.Clear();

     mgr_mis[ialc]->Draw("AP");
     leg_a->Draw();
     mgr_mis[ialc]->GetXaxis()->SetTitle("#Delta_{t}, #mum");
     mgr_mis[ialc]->GetHistogram()->SetMaximum(10.);
     mgr_mis[ialc]->GetHistogram()->SetMinimum(-10.);
     if(ialc==5){
       mgr_mis[ialc]->GetHistogram()->SetMaximum(5.);
       mgr_mis[ialc]->GetHistogram()->SetMinimum(-5.);
     }
     mgr_mis[ialc]->SetTitle("After");
     if(ialc==0) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{x}, #mum");
     if(ialc==1) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{y}, #mum");
     if(ialc==2) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{z}, #mum");
     if(ialc==3) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{#alpha}, mrad");
     if(ialc==4) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{#beta}, mrad");
     if(ialc==5) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{#gamma}, mrad");
     c1.Print(resname_pdf_o); //write canvas and keep the file open
     c1.Clear();


   }
 }

 //c1.Print(resname_pdf_c); //write canvas and close the file
TLegend *leg2 = new TLegend(0.78,0.58,0.98,0.98);
 leg2->SetFillColor(0);

 TMultiGraph *mgr_stat = new TMultiGraph();
 TMultiGraph *mgr_stat_kn = new TMultiGraph();
 TGraphErrors *grstatBef[nParDa], *grstatAft[nParDa];
 TGraphErrors *grstatKnossos[nParDa];
 TGraphErrors *grstatKnossos0[nParDa];
 for(int ipart=0;ipart<nParDa;ipart++){// loop over rotaion scale
   grstatBef[ipart] = new TGraphErrors(nParDt,v_Dt,TrksBef[ipart],0,0);
   grstatBef[ipart]->SetMarkerStyle(20+ipart);
   grstatBef[ipart]->SetMarkerColor(colors[ipart]);
   grstatBef[ipart]->SetMarkerSize(1.7+ipart*0.3);

   grstatKnossos0[ipart] = new TGraphErrors(nParDt,v_Dt,TrksKnossos0[ipart],0,errTrksKnossos0[ipart]);
   grstatKnossos0[ipart]->SetMarkerStyle(20+ipart);
   grstatKnossos0[ipart]->SetMarkerColor(colors[ipart]);
   grstatKnossos0[ipart]->SetLineColor(colors[ipart]);
   grstatKnossos0[ipart]->SetMarkerSize(1.7+ipart*0.3);
   mgr_stat_kn->Add(grstatKnossos0[ipart]);
   grstatKnossos[ipart] = new TGraphErrors(nParDt,v_Dt,TrksKnossos[ipart],0,errTrksKnossos[ipart]);
   grstatKnossos[ipart]->SetMarkerStyle(20+ipart);
   grstatKnossos[ipart]->SetMarkerColor(colors_a[ipart]);
   grstatKnossos[ipart]->SetLineColor(colors_a[ipart]);
   grstatKnossos[ipart]->SetMarkerSize(1.7+ipart*0.3);
   TString dRName = "#Delta_{r} = ";
   dRName +=v_Da[ipart];
  TString dRName_b = dRName+ " (input)";  
  TString dRName_a = dRName+ " (last iter)";  
  leg2->AddEntry(grstatKnossos0[ipart],dRName_b,"lep");
  leg2->AddEntry(grstatKnossos[ipart],dRName_a,"lep");

   mgr_stat_kn->Add(grstatKnossos[ipart]);
   grstatAft[ipart] = new TGraphErrors(nParDt,v_Dt,TrksAft[ipart],0,0);
   grstatAft[ipart]->SetMarkerStyle(20+ipart);
   grstatAft[ipart]->SetMarkerSize(1.7+ipart*0.3);
   grstatAft[ipart]->SetMarkerColor(kOrange+7);
   mgr_stat->Add(grstatBef[ipart]);
   mgr_stat->Add(grstatAft[ipart]);
 }// END loop over rotaion scale
 // mgr_stat->Draw("AP");
 // mgr_stat->GetXaxis()->SetTitle("#Delta_{t}, #mum");
 // mgr_stat->GetYaxis()->SetTitle("N_{REC}");
 // mgr_stat->GetHistogram()->SetMaximum(1e5);
 // mgr_stat->GetHistogram()->SetMinimum(0);
 // leg_b->Draw();
 // c1.Print(resname_pdf_o); //write canvas and keep the file open
 c1.Clear();
 mgr_stat_kn->Draw("AP");
 mgr_stat_kn->SetTitle("#trks used in Millepede (av. per sector)");
 mgr_stat_kn->GetXaxis()->SetTitle("#Delta_{t}, #mum");
 mgr_stat_kn->GetYaxis()->SetTitle("N_{REC}");
 leg2->Draw();
 c1.Print(resname_pdf_c); //write canvas and close the file
 //  
 //  mgr_stat->GetXaxis()->SetTitle("Number of sim. trks");
 //  
 //  //mgr_stat->SetTitle("Average number of rec. trks (empty = before Knossos; filled = after Knossos)");
 //  c1.Print(resname_pdf_o); //write canvas and keep the file open
 //  c1.Clear();
 //  TGraphErrors *grstatDiff = new TGraphErrors(nS,TrksSim,av_TrksDiff,0,avEr_TrksDiff);
 //  grstatDiff->SetMarkerStyle(22);
 //  grstatDiff->SetMarkerSize(2.5);
 //  grstatDiff->Draw("ALP");
 //  grstatDiff->GetXaxis()->SetTitle("Number of sim. trks");
 //  grstatDiff->GetYaxis()->SetTitle("(N^{ after}_{REC} - N^{ before}_{REC})/N_{SIM}, %");
 //  //  grstatDiff->SetTitle("Diff number of rec. trks (after - before), %");
 //  c1.Print(resname_pdf_c); //write canvas and close ps file
 //  c1.Close();
 //  TString out = resname+".root";
 //  TFile *f = new TFile(out,"RECREATE");
 //  // for (int g=0;g<6;g++){
 //  //   gr_mis_b[g]->Write();
 //  //   gr_mis_a[g]->Write();
 //  //   mgr_mis[g]->Write();
 //  // }
 //  gr_theta_b->Write();
 //  gr_theta_r->Write();
 //  gr_theta_a->Write();
 //  mgr_theta->Write();
 //  gr_theta_rms_b->Write();
 //  gr_theta_rms_r->Write();
 //  gr_theta_rms_a->Write();
 //  mgr_theta_rms->Write();

 // for(int ialc=0;ialc<6;ialc++){
 //   gr_mis_b[ialc]->Write();
 //   gr_mis_a[ialc]->Write();
 //   mgr_mis[ialc]->Write();
 // }
 // mgr_stat->Write();
 // grstatDiff->Write();
  // f->Write();
  // f->Close();
  //   }
  // }
}
