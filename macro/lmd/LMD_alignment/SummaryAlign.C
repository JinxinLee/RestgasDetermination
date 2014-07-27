#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TCanvas.h>

#include <sstream>
using namespace std;
void SummaryAlign(TString pathG="/panda/pandaroot/macro/lmd/testPixelAlignSIM_1000000/mom_15//", double tr_sc=0, double rt_sc=0)
//void SummaryAlign(TString pathG="/panda/pandaroot/macro/lmd/testPixelAlignSIM_10000/mom_15//", double tr_sc=0, double rt_sc=0)
{
  const int ntrksSample = 1e4;
  const double i_TrksSimi = 100./ntrksSample; //relative to simulated in %
  // const int nParDt=8;
  // double v_Dt[nParDt]={0,50,100,200,300,400,500,600};
  // const int nParDa=5;
  // double v_Da[nParDa]={0,1,3,6,9};
  const int nParDt=10;
  double v_Dt[nParDt]={0, 50, 100, 200, 300, 400, 500, 600, 800, 1000};
 const int nParDa=4;
 double v_Da[nParDa]={0,1,3,5};
  // for(int iDt=0;iDt<nParDt;iDt++){
  //   tr_sc = v_Dt[iDt];
  //   for(int iDa=0;iDa<nParDa;iDa++){
  //     rt_sc= v_Da[iDa];
  //     cout<<"work with dt~"<<tr_sc<<" da~"<<rt_sc<<endl;
  //     //const int nS=10;
      const int nS=1;
      double TrksSim[nParDa][nParDt];
  //double TrksSim[nS]={10000, 100000, 200000, 400000, 600000, 800000, 1000000};
   //  const int nS=3;
   //  double TrksSim[nS]={1000,5000,10000};

  //  double TrksSim[nS]={10, 100, 200, 300, 400, 500, 700, 1000};
  double TrksBef[nParDa][nParDt];
  double TrksAft[nParDa][nParDt];
  double TrksDiff[nParDa][nParDt];
  double TrksKnossos[nParDa][nParDt], errTrksKnossos[nParDa][nParDt];

  //How to save data
  TString resname= pathG+"/Results1SampleSummary_BOX";
  TString resname_pdf = resname+".pdf";
  TString resname_pdf_o = resname_pdf+"(";
  TString resname_pdf_c = resname_pdf+")";

  double mis_b[nParDa][6][nParDt];
  double mis_a[nParDa][6][nParDt];
  double mis_v_b[nParDa][6][nParDt];
  double mis_v_a[nParDa][6][nParDt];

  double theta_mean_b[nParDa][nParDt], theta_mean_a[nParDa][nParDt], theta_mean_r[nParDa][nParDt];
  double theta_rms_b[nParDa][nParDt], theta_rms_a[nParDa][nParDt], theta_rms_r[nParDa][nParDt];
  // double theta_mean_b_sec[nS][10][nS], theta_mean_a_sec[nS][10][nS], theta_mean_r_sec[nS][10][nS];
  // double theta_rms_b_sec[nS][10][nS], theta_rms_a_sec[nS][10][nS], theta_rms_r_sec[nS][10][nS];
  TH1D *hthetabefore = new TH1D("hthetabefore","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e2,-1e-3,1e-3);
  TH1D *hthetaafter = new TH1D("hthetaafter","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e2,-1e-3,1e-3);
  TH1D *hthetaref = new TH1D("hthetaref","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e2,-1e-3,1e-3);
  for(int ipart=0;ipart<nParDa;ipart++){
    for(int i=0;i<nParDt;i++){
      TrksSim[ipart][i]=ntrksSample ; //ntrksSample events with 1trk/event
      theta_mean_b[ipart][i] = 0;
      theta_mean_a[ipart][i] = 0;
      theta_mean_r[ipart][i] = 0;
      theta_rms_b[ipart][i] = 0;
      theta_rms_a[ipart][i] = 0;
      theta_rms_r[ipart][i] = 0;
      // for(int isector=0;isector<10;isector++){
      // 	theta_mean_b_sec[ipart][isector][i] = 0;
      // 	theta_mean_a_sec[ipart][isector][i] = 0;
      // 	theta_mean_r_sec[ipart][isector][i] = 0;
      // 	theta_rms_b_sec[ipart][isector][i] = 0;
      // 	theta_rms_a_sec[ipart][isector][i] =0;
      // 	theta_rms_r_sec[ipart][isector][i] = 0;
      // }
    }
  }

 for(int ipart=0;ipart<nParDa;ipart++){
    for(int i=0;i<nParDt;i++){
      tr_sc = v_Dt[i];
      rt_sc= v_Da[ipart];
      int nTrks =  TrksSim[ipart][i];
      //Where search for files
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
      
      
      TString name3 = pathG+"/0mkm_0mrad";
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
      


      hbefore = (TH1*)f1->Get("NearIP/hResTheta");
      //hbefore->Print();
      hbefore->SetName("hResThetaBefore");
      hbefore->SetTitle("#theta resolution");
      //  hbefore->Print();
      TrksBef[ipart][i] = hbefore->GetEntries();
      //   TrksBef[ipart][i] *=i_TrksSimi;
  // //  if(i>0) TrksBef[ipart][i] += TrksBef[ipart][i-1];
 
  // //  cout<<" Trks["<<ipart<<"]["<<i<<"] = "<<Trks[ipart][i]<<endl;
  // TF1 *funrth_b = new TF1("fitrth_b","gaus",-0.01,0.01);
  // funrth_b->SetParameters(100,0,3e-3);
  // funrth_b->SetParNames("Constant","Mean","Sigma");
  // hbefore->Fit(funrth_b);
  // theta_mean_b[ipart][i]  = 1e6*(funrth_b->GetParameter("Mean"));
  // theta_rms_b[ipart][i]  = 1e6*(funrth_b->GetParameter("Sigma"));
  theta_mean_b[ipart][i]  = 1e3*(hbefore->GetMean());
  theta_rms_b[ipart][i]  = 1e3*(hbefore->GetRMS());
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
  // Trks[ipart][i] = hafter->GetEntries();
  // if(i>0) Trks[ipart][i] += Trks[ipart][i-1];
  TrksAft[ipart][i] = hafter->GetEntries();
  //  TrksAft[ipart][i]  *=i_TrksSimi;
  //  if(i>0) TrksAft[ipart][i] += TrksAft[ipart][i-1];
 
  TrksDiff[ipart][i] = (TrksAft[ipart][i] - TrksBef[ipart][i]);

  // TF1 *funrth_a = new TF1("fitrth_a","gaus",-0.01,0.01);
  // funrth_a->SetParameters(100,0,3e-3);
  // funrth_a->SetParNames("Constant","Mean","Sigma");
  // hafter->Fit(funrth_a);

  // theta_mean_a[ipart][i] = 1e6*(funrth_a->GetParameter("Mean"));
  // theta_rms_a[ipart][i] = 1e6*(funrth_a->GetParameter("Sigma"));
  // //  nsectors_a = (TNtuple*)f2->Get("nsectors");
  theta_mean_a[ipart][i]  = 1e3*(hafter->GetMean());
  theta_rms_a[ipart][i]  = 1e3*(hafter->GetRMS());
  TFile *f3 = new TFile(name3,"READ");
  if (f3->IsZombie()) {
    std::cout << "!!! Error opening file " <<name2<< std::endl;
    continue;
    // return;
  }
  hrefer = (TH1F*)f3->Get("NearIP/hResTheta");
  hrefer->SetName("hResThetaRef");
  hrefer->SetTitle("#theta resolution");

  // TF1 *funrth_r = new TF1("fitrth_r","gaus",-0.01,0.01);
  // funrth_r->SetParameters(100,0,3e-3);
  // funrth_r->SetParNames("Constant","Mean","Sigma");
  // hrefer->Fit(funrth_r);
  // theta_mean_r[ipart][i] = 1e6*(funrth_r->GetParameter("Mean"));
  // theta_rms_r[ipart][i] = 1e6*(funrth_r->GetParameter("Sigma"));
  // nsectors_r = (TNtuple*)f3->Get("nsectors");
  theta_mean_r[ipart][i]  = 1e3*(hrefer->GetMean());
  theta_rms_r[ipart][i]  = 1e3*(hrefer->GetRMS());

  // for(int isector=0;isector<10;isector++){
  //   TString cond = "sector==";
  //   cond +=isector; 
  //   nsectors_b->Project("hthetabefore","thetares",cond.Data());
  //   nsectors_a->Project("hthetaafter","thetares",cond.Data());
  //   nsectors_r->Project("hthetaref","thetares",cond.Data());
  //   hthetabefore->Fit(funrth_b);
  //   hthetaafter->Fit(funrth_a);
  //   hthetaref->Fit(funrth_r);
  //   theta_mean_b_sec[ipart][isector][i] = 1e6*(funrth_b->GetParameter("Mean"));
  //   theta_rms_b_sec[ipart][isector][i] = 1e6*(funrth_b->GetParameter("Sigma"));
  //   theta_mean_a_sec[ipart][isector][i] = 1e6*(funrth_a->GetParameter("Mean"));
  //   theta_rms_a_sec[ipart][isector][i] = 1e6*(funrth_a->GetParameter("Sigma"));
  //   theta_mean_r_sec[ipart][isector][i] = 1e6*(funrth_r->GetParameter("Mean"));
  //   theta_rms_r_sec[ipart][isector][i] = 1e6*(funrth_r->GetParameter("Sigma"));
  // }

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
    /// END (Misalignment constants) -----------------------------------------------------------
    fmisc->Close();
    }// trans
 }// rotation
 
 // Draw results --------------------------------
 TCanvas c1;
 TLegend *leg = new TLegend(0.78,0.58,0.98,0.98);
 leg->SetFillColor(0);

 TMultiGraph *mgr_theta = new TMultiGraph();  
 TMultiGraph *mgr_theta_b = new TMultiGraph();  
 TGraphErrors *gr_theta_b[nParDa],*gr_theta_a[nParDa],*gr_theta_r[nParDa];
 for(int ipart=0;ipart<nParDa;ipart++){// loop over rotaion scale
 // double [nParDt], theta_mean_a[nParDa][nParDt], theta_mean_r[nParDa][nParDt];
 //  double [nParDt], theta_rms_a[nParDa][nParDt], theta_rms_r[nParDa][nParDt];
  gr_theta_b[ipart] = new TGraphErrors(nParDt,v_Dt,theta_mean_b[ipart],0,0);
  gr_theta_b[ipart]->SetMarkerStyle(20+ipart); 
  gr_theta_b[ipart]->SetMarkerColor(kGreen-3); 
  gr_theta_b[ipart]->SetMarkerSize(1.7+ipart*0.3); 
  gr_theta_a[ipart] = new TGraphErrors(nParDt,v_Dt,theta_mean_a[ipart],0,0);
  gr_theta_a[ipart]->SetMarkerStyle(20+ipart); 
  gr_theta_a[ipart]->SetMarkerColor(kOrange+7); 
  gr_theta_a[ipart]->SetMarkerSize(1.7+ipart*0.3); 
  mgr_theta_b->Add(gr_theta_b[ipart]);
  mgr_theta->Add(gr_theta_a[ipart]);
  if(ipart==0){
  gr_theta_r[ipart] = new TGraphErrors(nParDt,v_Dt,theta_mean_r[ipart],0,0);
  gr_theta_r[ipart]->SetMarkerStyle(20+ipart); 
  gr_theta_r[ipart]->SetMarkerColor(15); 
  gr_theta_r[ipart]->SetMarkerSize(1.5); 
  mgr_theta_b->Add(gr_theta_r[ipart]);
  mgr_theta->Add(gr_theta_r[ipart]);
  leg->AddEntry(gr_theta_r[ipart],"IDEAL","lep");
  }
  TString dRName = "#Delta_{r} = ";
  dRName +=v_Da[ipart];
  TString dRName_b = dRName+ " (BEFORE)";  
  TString dRName_a = dRName+ " (AFTER)";  
  leg->AddEntry(gr_theta_b[ipart],dRName_b,"lep");
  leg->AddEntry(gr_theta_a[ipart],dRName_a,"lep");

 }// END loop over rotaion scale
  
 mgr_theta_b->Draw("AP");
 leg->Draw();
 mgr_theta_b->GetXaxis()->SetTitle("#Delta_{t}, #mum");
 mgr_theta_b->GetYaxis()->SetTitle("#theta_{mean}, #murad");
 mgr_theta_b->GetHistogram()->SetMaximum(200.);
 mgr_theta_b->GetHistogram()->SetMinimum(-200.);
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 
 
 mgr_theta->Draw("AP");
 leg->Draw();
 mgr_theta->GetXaxis()->SetTitle("#Delta_{t}, #mum");
 mgr_theta->GetYaxis()->SetTitle("#theta_{mean}, #murad");
 mgr_theta->GetHistogram()->SetMaximum(10.);
 mgr_theta->GetHistogram()->SetMinimum(-10.);

 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 ///// theta rms
 TMultiGraph *mgr_theta_rms = new TMultiGraph();  
 TMultiGraph *mgr_theta_b_rms = new TMultiGraph();  
 for(int ipart=0;ipart<nParDa;ipart++){// loop over rotaion scale
   gr_theta_b[ipart] = new TGraphErrors(nParDt,v_Dt,theta_rms_b[ipart],0,0);
   gr_theta_b[ipart]->SetMarkerStyle(20+ipart); 
   gr_theta_b[ipart]->SetMarkerColor(kGreen-3); 
   gr_theta_b[ipart]->SetMarkerSize(1.7+ipart*0.3); 
   gr_theta_a[ipart] = new TGraphErrors(nParDt,v_Dt,theta_rms_a[ipart],0,0);
  gr_theta_a[ipart]->SetMarkerStyle(20+ipart); 
  gr_theta_a[ipart]->SetMarkerColor(kOrange+7); 
  gr_theta_a[ipart]->SetMarkerSize(1.7+ipart*0.3); 
  mgr_theta_b_rms->Add(gr_theta_b[ipart]);
  mgr_theta_rms->Add(gr_theta_a[ipart]);
  if(ipart==0){
    gr_theta_r[ipart] = new TGraphErrors(nParDt,v_Dt,theta_rms_r[ipart],0,0);
    gr_theta_r[ipart]->SetMarkerStyle(20+ipart); 
    gr_theta_r[ipart]->SetMarkerColor(15); 
    gr_theta_r[ipart]->SetMarkerSize(1.5);
    mgr_theta_b_rms->Add(gr_theta_r[ipart]);
    mgr_theta_rms->Add(gr_theta_r[ipart]);
  }
 }// END loop over rotaion scale
  
 mgr_theta_b_rms->Draw("AP");
 leg->Draw();
 mgr_theta_b_rms->GetXaxis()->SetTitle("#Delta_{t}, #mum");
 mgr_theta_b_rms->GetYaxis()->SetTitle("#theta_{rms}, #murad");
 mgr_theta_b_rms->GetHistogram()->SetMaximum(1500.);
 mgr_theta_b_rms->GetHistogram()->SetMinimum(90.);

 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 
 
 mgr_theta_rms->Draw("AP");
 leg->Draw();
 mgr_theta_rms->GetXaxis()->SetTitle("#Delta_{t}, #mum");
 mgr_theta_rms->GetYaxis()->SetTitle("#theta_{rms}, #murad");
 mgr_theta_rms->GetHistogram()->SetMaximum(200.);
 mgr_theta_rms->GetHistogram()->SetMinimum(90.);

 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 
 //put together missalignment consts results
  TGraphErrors *gr_mis_b[6][nParDa];
  TGraphErrors *gr_mis_a[6][nParDa];
  TMultiGraph *mgr_mis[6];
  for(int ialc=0;ialc<6;ialc++){
    mgr_mis[ialc] = new TMultiGraph();
  }
  for(int ipart=0;ipart<nParDa;ipart++){// loop over rotaion scale
    for(int ialc=0;ialc<6;ialc++){
      gr_mis_b[ialc][ipart] = new TGraphErrors(nParDt,v_Dt,mis_v_b[ipart][ialc],0,mis_b[ipart][ialc]);
      gr_mis_b[ialc][ipart]->SetMarkerStyle(20+ipart);
      gr_mis_b[ialc][ipart]->SetMarkerColor(kGreen-3);
      gr_mis_b[ialc][ipart]->SetMarkerSize(1.7+ipart*0.3);
      gr_mis_a[ialc][ipart] = new TGraphErrors(nParDt,v_Dt,mis_v_a[ipart][ialc],0,mis_a[ipart][ialc]);
      gr_mis_a[ialc][ipart]->SetMarkerStyle(20+ipart);
      gr_mis_a[ialc][ipart]->SetMarkerColor(kOrange+7);
      gr_mis_a[ialc][ipart]->SetMarkerSize(1.7+ipart*0.3);
     
      //  mgr_mis[ialc]->Add(gr_mis_b[ialc][ipart]);
      mgr_mis[ialc]->Add(gr_mis_a[ialc][ipart]);
    }
  }// END loop over rotaion scale
 for(int ialc=0;ialc<6;ialc++){
   if(ialc<2 || ialc==5){
     mgr_mis[ialc]->Draw("AP");
     leg->Draw();
     mgr_mis[ialc]->GetXaxis()->SetTitle("#Delta_{t}, #mum");
     mgr_mis[ialc]->GetHistogram()->SetMaximum(10.);
     mgr_mis[ialc]->GetHistogram()->SetMinimum(-10.);

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


 TMultiGraph *mgr_stat = new TMultiGraph();
 TMultiGraph *mgr_stat_kn = new TMultiGraph();
 TGraphErrors *grstatBef[nParDa], *grstatAft[nParDa];
 TGraphErrors *grstatKnossos[nParDa];
 for(int ipart=0;ipart<nParDa;ipart++){// loop over rotaion scale
   grstatBef[ipart] = new TGraphErrors(nParDt,v_Dt,TrksBef[ipart],0,0);
   grstatBef[ipart]->SetMarkerStyle(20+ipart);
   grstatBef[ipart]->SetMarkerColor(kGreen-3);
   grstatBef[ipart]->SetMarkerSize(1.7+ipart*0.3);

   grstatKnossos[ipart] = new TGraphErrors(nParDt,v_Dt,TrksKnossos[ipart],0,errTrksKnossos[ipart]);
   grstatKnossos[ipart]->SetMarkerStyle(20+ipart);
   grstatKnossos[ipart]->SetMarkerColor(kGreen-3);
   grstatKnossos[ipart]->SetMarkerSize(1.7+ipart*0.3);
   mgr_stat_kn->Add(grstatKnossos[ipart]);
   grstatAft[ipart] = new TGraphErrors(nParDt,v_Dt,TrksAft[ipart],0,0);
   grstatAft[ipart]->SetMarkerStyle(20+ipart);
   grstatAft[ipart]->SetMarkerSize(1.7+ipart*0.3);
   grstatAft[ipart]->SetMarkerColor(kOrange+7);
   mgr_stat->Add(grstatBef[ipart]);
   mgr_stat->Add(grstatAft[ipart]);
 }// END loop over rotaion scale
 mgr_stat->Draw("AP");
 mgr_stat->GetXaxis()->SetTitle("#Delta_{t}, #mum");
 mgr_stat->GetYaxis()->SetTitle("N_{REC}");
 mgr_stat->GetHistogram()->SetMaximum(1e5);
 mgr_stat->GetHistogram()->SetMinimum(0);
 leg->Draw();
 c1.Print(resname_pdf_o); //write canvas and keep the file open
 c1.Clear();
 mgr_stat_kn->Draw("AP");
 mgr_stat_kn->SetTitle("#trks used in Millepede (per module)");
 mgr_stat_kn->GetXaxis()->SetTitle("#Delta_{t}, #mum");
 mgr_stat_kn->GetYaxis()->SetTitle("N_{REC}");
 // leg->Draw();
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
