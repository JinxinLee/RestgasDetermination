#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TCanvas.h>

#include <sstream>
using namespace std;
void SenseAlign(TString pathG="/home/karavdina/soft/pandaroot/macro/lmd/testPixelAlignSIM_LIMITS_2000/mom_15/")
//void SummaryAlign(TString pathG="/panda/pandaroot/macro/lmd/testPixelAlignSIM_10000/mom_15//", double tr_sc=0, double rt_sc=0)
{
  gStyle->SetOptStat(0);
 
  const int nParDa=9;
  double v_Da[nParDa]={0,0.5,1,1.5,2,2.5,3,3.5,4};
  TString sv_Da[nParDa]={"0","0_5","1","1_5","2","2_5","3","3_5","4"};
  // const int nParDt=15;
  // double v_Dt[nParDt]={0,5,10,15,20,25,30,35,40,50,100,150,200,250,300};
  const int nParDt=41;
  double v_Dt[nParDt];
  for(int i=0;i<nParDt;i++)
    v_Dt[i] = i*5;

  const int nS=1;
  
  //How to save data
  TString resname= pathG+"/ResultsLimitsSummary_BOX";
  TString resname_pdf = resname+".pdf";
  TString resname_pdf_o = resname_pdf+"(";
  TString resname_pdf_c = resname_pdf+")";


  double theta_mean_b[nParDa][nParDt], theta_mean_r[nParDa][nParDt];
  double theta_rms_b[nParDa][nParDt], theta_rms_r[nParDa][nParDt];
  double phi_mean_b[nParDa][nParDt], phi_mean_r[nParDa][nParDt];
  double phi_rms_b[nParDa][nParDt], phi_rms_r[nParDa][nParDt];
  double X_mean_b[nParDa][nParDt], X_mean_r[nParDa][nParDt];
  double X_rms_b[nParDa][nParDt], X_rms_r[nParDa][nParDt];
  double Y_mean_b[nParDa][nParDt], Y_mean_r[nParDa][nParDt];
  double Y_rms_b[nParDa][nParDt], Y_rms_r[nParDa][nParDt];

  for(int ipart=0;ipart<nParDa;ipart++){
    for(int i=0;i<nParDt;i++){
      theta_mean_b[ipart][i] = 0;
      theta_mean_r[ipart][i] = 0;
      theta_rms_b[ipart][i] = 0;
      theta_rms_r[ipart][i] = 0;

      phi_mean_b[ipart][i] = 0;
      phi_mean_r[ipart][i] = 0;
      phi_rms_b[ipart][i] = 0;
      phi_rms_r[ipart][i] = 0;

      X_mean_b[ipart][i] = 0;
      X_mean_r[ipart][i] = 0;
      X_rms_b[ipart][i] = 0;
      X_rms_r[ipart][i] = 0;

      Y_mean_b[ipart][i] = 0;
      Y_mean_r[ipart][i] = 0;
      Y_rms_b[ipart][i] = 0;
      Y_rms_r[ipart][i] = 0;

    }
  }

 for(int ipart=0;ipart<nParDa;ipart++){
    for(int i=0;i<nParDt;i++){
      double tr_sc = v_Dt[i];
      //  rt_sc= v_Da[ipart];
      TString rt_sc= sv_Da[ipart];
      //Where search for files
      TString path = pathG+"/";
      path+=tr_sc;
      path+="mkm_";
      path+=rt_sc;
      path+="mrad/";

      /// Theta resolution--------------------------------------------------------------------------------------  
      TString name1 = path + "/Lumi_QA_before.root";
      TString name2 = path + "/Lumi_QA_after.root";
      TString name3 = pathG+"/0mkm_0mrad";
      name3+="/Lumi_QA_before.root";
      //   cout<<name1<<endl;
      
      TH1 *hbefore;   
      TH1 *hafter;
      TH1 *hrefer;
      
      TFile *f1 = new TFile(name1,"READ");
      if (f1->IsZombie()) {
	std::cout << "!!! Error opening file " <<name1<< std::endl;
	continue;
	//   return;
      }
      


      hbefore = (TH1*)f1->Get("NearIP/hResTheta");
      theta_mean_b[ipart][i]  = 1e3*(hbefore->GetMean());
      theta_rms_b[ipart][i]  = 1e3*(hbefore->GetRMS());
      hbefore = (TH1*)f1->Get("NearIP/hResPhi");
      phi_mean_b[ipart][i]  = 1e3*(hbefore->GetMean());
      phi_rms_b[ipart][i]  = 1e3*(hbefore->GetRMS());
      hbefore = (TH1*)f1->Get("NearIP/hResPointX");
      X_mean_b[ipart][i]  = 1e4*(hbefore->GetMean());
      X_rms_b[ipart][i]  = 1e4*(hbefore->GetRMS());
      hbefore = (TH1*)f1->Get("NearIP/hResPointY");
      Y_mean_b[ipart][i]  = 1e4*(hbefore->GetMean());
      Y_rms_b[ipart][i]  = 1e4*(hbefore->GetRMS());

      if(ipart==0 && i==0){
	theta_mean_b[ipart][i]  += 1e-6;
	theta_rms_b[ipart][i]  += 1e-6;
	phi_mean_b[ipart][i]  += 1e-6;
	phi_rms_b[ipart][i]  += 1e-6;
	X_mean_b[ipart][i]  += 1e-6;
	X_rms_b[ipart][i]  += 1e-6;
	Y_mean_b[ipart][i]  += 1e-6;
	Y_rms_b[ipart][i]  += 1e-6;
      }


      TFile *f3 = new TFile(name3,"READ");
      if (f3->IsZombie()) {
      	std::cout << "!!! Error opening file " <<name2<< std::endl;
      	continue;
      	// return;
      }
      hrefer = (TH1F*)f3->Get("NearIP/hResTheta");
      theta_mean_r[ipart][i]  = 1e3*(hrefer->GetMean());
      theta_rms_r[ipart][i]  = 1e3*(hrefer->GetRMS());
      hrefer = (TH1*)f3->Get("NearIP/hResPhi");
      phi_mean_r[ipart][i]  = 1e3*(hrefer->GetMean());
      phi_rms_r[ipart][i]  = 1e3*(hrefer->GetRMS());
      hrefer = (TH1*)f3->Get("NearIP/hResPointX");
      X_mean_r[ipart][i]  = 1e4*(hrefer->GetMean());
      X_rms_r[ipart][i]  = 1e4*(hrefer->GetRMS());
      hrefer = (TH1*)f3->Get("NearIP/hResPointY");
      Y_mean_r[ipart][i]  = 1e4*(hrefer->GetMean());
      Y_rms_r[ipart][i]  = 1e4*(hrefer->GetRMS());
      //   cout<<"theta_mean_r["<<v_Da[ipart]<<"]["<<v_Dt[i]<<"]="<<theta_mean_r[ipart][i]<<endl;
      /// END (Theta resolution) ---------------------------------------------------------------
      f1->Close();
      // f2->Close();
      f3->Close();
    }// trans
 }// rotation
 
 // Draw results --------------------------------
 
 cout<<"Let's draw!"<<endl;
 TCanvas c1;
 TH2D *htheta_mean = new TH2D("htheta_mean","#theta_{MEAN}; #Delta_{t}, #mum;#Delta_{r}, mrad",4e1,0,v_Dt[nParDt-1],8e0,0,v_Da[nParDa-1]);
 TH2D *htheta_rms = new TH2D("htheta_rms","#theta_{RMS}; #Delta_{t}, #mum;#Delta_{r}, mrad",4e1,0,v_Dt[nParDt-1],8e0,0,v_Da[nParDa-1]);
 for(int ipart=0;ipart<nParDa;ipart++){
   for(int i=0;i<nParDt;i++){
     // cout<<fabs((theta_rms_b[ipart][i]-theta_rms_r[ipart][i])/theta_rms_r[ipart][i])<<endl;
     htheta_mean->Fill(v_Dt[i],v_Da[ipart],fabs((theta_mean_b[ipart][i]-theta_mean_r[ipart][i])/theta_mean_r[ipart][i]));
     htheta_rms->Fill(v_Dt[i],v_Da[ipart],fabs((theta_rms_b[ipart][i]-theta_rms_r[ipart][i])/theta_rms_r[ipart][i]));
   }
 }
 // htheta_mean->GetZaxis()->SetLimits(0,10);
 htheta_mean->Draw("colz");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 htheta_rms->Draw("colz");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();

 TH2D *hphi_mean = new TH2D("hphi_mean","#phi_{MEAN}; #Delta_{t}, #mum;#Delta_{r}, mrad",4e1,0,v_Dt[nParDt-1],8e0,0,v_Da[nParDa-1]);
 TH2D *hphi_rms = new TH2D("hphi_rms","#phi_{RMS}; #Delta_{t}, #mum;#Delta_{r}, mrad",4e1,0,v_Dt[nParDt-1],8e0,0,v_Da[nParDa-1]);
 for(int ipart=0;ipart<nParDa;ipart++){
   for(int i=0;i<nParDt;i++){
     cout<<fabs((phi_mean_b[ipart][i]-phi_mean_r[ipart][i])/phi_mean_r[ipart][i])<<endl;
     hphi_mean->Fill(v_Dt[i],v_Da[ipart],fabs((phi_mean_b[ipart][i]-phi_mean_r[ipart][i])/phi_mean_r[ipart][i]));
     hphi_rms->Fill(v_Dt[i],v_Da[ipart],fabs((phi_rms_b[ipart][i]-phi_rms_r[ipart][i])/phi_rms_r[ipart][i]));
   }
 }
 hphi_mean->Draw("colz");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 hphi_rms->Draw("colz");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();

 TH2D *hX_mean = new TH2D("hX_mean","X_{MEAN}; #Delta_{t}, #mum;#Delta_{r}, mrad",4e1,0,v_Dt[nParDt-1],8e0,0,v_Da[nParDa-1]);
 TH2D *hX_rms = new TH2D("hX_rms","X_{RMS}; #Delta_{t}, #mum;#Delta_{r}, mrad",4e1,0,v_Dt[nParDt-1],8e0,0,v_Da[nParDa-1]);
 for(int ipart=0;ipart<nParDa;ipart++){
   for(int i=0;i<nParDt;i++){
     //  cout<<fabs(X_mean_b[ipart][i]-X_mean_r[ipart][i])/X_mean_r[ipart][i]<<endl;
     hX_mean->Fill(v_Dt[i],v_Da[ipart],fabs((X_mean_b[ipart][i]-X_mean_r[ipart][i])/X_mean_r[ipart][i]));
     hX_rms->Fill(v_Dt[i],v_Da[ipart],fabs((X_rms_b[ipart][i]-X_rms_r[ipart][i])/X_rms_r[ipart][i]));
   }
 }
 hX_mean->Draw("colz");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 hX_rms->Draw("colz");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();

 TH2D *hY_mean = new TH2D("hY_mean","Y_{MEAN}; #Delta_{t}, #mum;#Delta_{r}, mrad",4e1,0,v_Dt[nParDt-1],8e0,0,v_Da[nParDa-1]);
 TH2D *hY_rms = new TH2D("hY_rms","Y_{RMS}; #Delta_{t}, #mum;#Delta_{r}, mrad",4e1,0,v_Dt[nParDt-1],8e0,0,v_Da[nParDa-1]);
 for(int ipart=0;ipart<nParDa;ipart++){
   for(int i=0;i<nParDt;i++){
     //  cout<<fabs(Y_mean_b[ipart][i]-Y_mean_r[ipart][i])/Y_mean_r[ipart][i]<<endl;
     hY_mean->Fill(v_Dt[i],v_Da[ipart],fabs((Y_mean_b[ipart][i]-Y_mean_r[ipart][i])/Y_mean_r[ipart][i]));
     hY_rms->Fill(v_Dt[i],v_Da[ipart],fabs((Y_rms_b[ipart][i]-Y_rms_r[ipart][i])/Y_rms_r[ipart][i]));
   }
 }
 hY_mean->Draw("colz");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 hY_rms->Draw("colz");
 c1.Print(resname_pdf_c); //write canvas and keep the ps file open
 c1.Clear();

//  TLegend *leg_b = new TLegend(0.83,0.65,0.97,0.97);
//  leg_b->SetFillColor(0);
//  TLegend *leg_a = new TLegend(0.83,0.65,0.97,0.97);
//  leg_a->SetFillColor(0);
//  // TLegend *leg_b = new TLegend(0.78,0.51,0.98,0.98);
//  // leg_b->SetFillColor(0);

//  TMultiGraph *mgr_theta = new TMultiGraph();  
//  TMultiGraph *mgr_theta_b = new TMultiGraph();  
//  TGraphErrors *gr_theta_b[nParDa],*gr_theta_a[nParDa],*gr_theta_r[nParDa];
//  for(int ipart=0;ipart<nParDa;ipart++){// loop over rotaion scale
//  // double [nParDt], theta_mean_a[nParDa][nParDt], theta_mean_r[nParDa][nParDt];
//  //  double [nParDt], theta_rms_a[nParDa][nParDt], theta_rms_r[nParDa][nParDt];
//   gr_theta_b[ipart] = new TGraphErrors(nParDt,v_Dt,theta_mean_b[ipart],0,0);
//   gr_theta_b[ipart]->SetMarkerStyle(20+ipart); 
//   //  gr_theta_b[ipart]->SetMarkerColor(kGreen-3); 
//   //  gr_theta_b[ipart]->SetMarkerColor(30*ipart+8);
//   gr_theta_b[ipart]->SetMarkerColor(colors[ipart]);
//   gr_theta_b[ipart]->SetLineColor(colors[ipart]);
//   // gr_theta_b[ipart]->SetLineColor(kGreen-3+ipart);
//   gr_theta_b[ipart]->SetMarkerSize(1.7+ipart*0.3); 
//   gr_theta_a[ipart] = new TGraphErrors(nParDt,v_Dt,theta_mean_a[ipart],0,0);
//   gr_theta_a[ipart]->SetMarkerStyle(20+ipart); 
//   //gr_theta_a[ipart]->SetMarkerColor(kOrange+7); 
//   gr_theta_a[ipart]->SetMarkerColor(colors_a[ipart]); 
//   gr_theta_a[ipart]->SetLineColor(colors_a[ipart]);
//   gr_theta_a[ipart]->SetMarkerSize(1.7+ipart*0.3); 
//   mgr_theta_b->Add(gr_theta_b[ipart]);
//   mgr_theta->Add(gr_theta_a[ipart]);
//   if(ipart==0){
//   gr_theta_r[ipart] = new TGraphErrors(nParDt,v_Dt,theta_mean_r[ipart],0,0);
//   gr_theta_r[ipart]->SetMarkerStyle(20+ipart); 
//   gr_theta_r[ipart]->SetMarkerColor(kGray+3); 
//   gr_theta_r[ipart]->SetMarkerSize(2.7); 
//   mgr_theta_b->Add(gr_theta_r[ipart]);
//   mgr_theta->Add(gr_theta_r[ipart]);
//   // leg->AddEntry(gr_theta_r[ipart],"ideal","lep");
//   leg_b->AddEntry(gr_theta_r[ipart],"ideal","lep");
//   leg_a->AddEntry(gr_theta_r[ipart],"ideal","lep");
//   }
//   TString dRName = "#Delta_{r} = ";
//   dRName +=v_Da[ipart];
//   //  TString dRName_b = dRName+ " (BEFORE)";  
//   // TString dRName_a = dRName+ " (AFTER)";  
//   // leg->AddEntry(gr_theta_b[ipart],dRName_b,"lep");
//   // leg->AddEntry(gr_theta_a[ipart],dRName_a,"lep");
//   TString dRName_b = dRName;  
//   leg_b->AddEntry(gr_theta_b[ipart],dRName_b,"lep");
//   leg_a->AddEntry(gr_theta_a[ipart],dRName_b,"lep");
//  }// END loop over rotaion scale
  
//  mgr_theta_b->Draw("AP");
//  leg_b->Draw();
//  mgr_theta_b->SetTitle("Before");
//  mgr_theta_b->GetXaxis()->SetTitle("#Delta_{t}, #mum");
//  mgr_theta_b->GetYaxis()->SetTitle("#theta_{mean}, #murad");
//  mgr_theta_b->GetHistogram()->SetMaximum(1.15*max_theta_mean_b);
//  mgr_theta_b->GetHistogram()->SetMinimum(0.85*min_theta_mean_b);
//  c1.Print(resname_pdf_o); //write canvas and keep the ps file open
//  c1.Clear();
 
 
//  mgr_theta->Draw("AP");
//  leg_a->Draw();
//  mgr_theta->SetTitle("After");
//  mgr_theta->GetXaxis()->SetTitle("#Delta_{t}, #mum");
//  mgr_theta->GetYaxis()->SetTitle("#theta_{mean}, #murad");
//  mgr_theta->GetHistogram()->SetMaximum(1.15*max_theta_mean_a);
//  mgr_theta->GetHistogram()->SetMinimum(0.85*min_theta_mean_a);

//  c1.Print(resname_pdf_o); //write canvas and keep the ps file open
//  c1.Clear();
//  ///// theta rms
//  TMultiGraph *mgr_theta_rms = new TMultiGraph();  
//  TMultiGraph *mgr_theta_b_rms = new TMultiGraph();  
//  for(int ipart=0;ipart<nParDa;ipart++){// loop over rotaion scale
//    gr_theta_b[ipart] = new TGraphErrors(nParDt,v_Dt,theta_rms_b[ipart],0,0);
//    gr_theta_b[ipart]->SetMarkerStyle(20+ipart); 
//    //  gr_theta_b[ipart]->SetMarkerColor(kGreen-3); 
//    gr_theta_b[ipart]->SetMarkerColor(colors[ipart]); 
//    gr_theta_b[ipart]->SetLineColor(colors[ipart]);
//    gr_theta_b[ipart]->SetMarkerSize(1.7+ipart*0.3); 
//    gr_theta_a[ipart] = new TGraphErrors(nParDt,v_Dt,theta_rms_a[ipart],0,0);
//   gr_theta_a[ipart]->SetMarkerStyle(20+ipart); 
//   //  gr_theta_a[ipart]->SetMarkerColor(kOrange+7); 
//   gr_theta_a[ipart]->SetMarkerColor(colors_a[ipart]); 
//   gr_theta_a[ipart]->SetLineColor(colors_a[ipart]);
//   gr_theta_a[ipart]->SetMarkerSize(1.7+ipart*0.3); 
//   mgr_theta_b_rms->Add(gr_theta_b[ipart]);
//   mgr_theta_rms->Add(gr_theta_a[ipart]);
//   if(ipart==0){
//     gr_theta_r[ipart] = new TGraphErrors(nParDt,v_Dt,theta_rms_r[ipart],0,0);
//     gr_theta_r[ipart]->SetMarkerStyle(20+ipart); 
//     gr_theta_r[ipart]->SetMarkerColor(kGray+3); 
//     gr_theta_r[ipart]->SetMarkerSize(2.7);
//     mgr_theta_b_rms->Add(gr_theta_r[ipart]);
//     mgr_theta_rms->Add(gr_theta_r[ipart]);
//   }
//  }// END loop over rotaion scale
  
//  mgr_theta_b_rms->Draw("AP");
//  leg_b->Draw();
//  mgr_theta_b_rms->GetXaxis()->SetTitle("#Delta_{t}, #mum");
//  mgr_theta_b_rms->GetYaxis()->SetTitle("#theta_{rms}, #murad");
//  mgr_theta_b_rms->GetHistogram()->SetMaximum(1.15*max_theta_rms_b);
//  mgr_theta_b_rms->GetHistogram()->SetMinimum(0.85*min_theta_rms_b);
//  mgr_theta_b_rms->SetTitle("Before");
//  c1.Print(resname_pdf_o); //write canvas and keep the ps file open
//  c1.Clear();
 
 
//  mgr_theta_rms->Draw("AP");
//  leg_a->Draw();
//  mgr_theta_rms->GetXaxis()->SetTitle("#Delta_{t}, #mum");
//  mgr_theta_rms->GetYaxis()->SetTitle("#theta_{rms}, #murad");
//  mgr_theta_rms->GetHistogram()->SetMaximum(1.15*max_theta_rms_a);
//  mgr_theta_rms->GetHistogram()->SetMinimum(0.85*min_theta_rms_a);
//  mgr_theta_rms->SetTitle("After");
//  c1.Print(resname_pdf_o); //write canvas and keep the ps file open
//  c1.Clear();
 
//  //put together missalignment consts results
//   TGraphErrors *gr_mis_b[6][nParDa];
//   TGraphErrors *gr_mis_a[6][nParDa];
//   TMultiGraph *mgr_mis[6]; 
//   TMultiGraph *mgr_mis_b[6];
//   for(int ialc=0;ialc<6;ialc++){
//     mgr_mis[ialc] = new TMultiGraph();
//     mgr_mis_b[ialc] = new TMultiGraph();
//   }
//   for(int ipart=0;ipart<nParDa;ipart++){// loop over rotaion scale
//     for(int ialc=0;ialc<6;ialc++){
//       gr_mis_b[ialc][ipart] = new TGraphErrors(nParDt,v_Dt,mis_v_b[ipart][ialc],0,mis_b[ipart][ialc]);
//       gr_mis_b[ialc][ipart]->SetMarkerStyle(20+ipart);
//       gr_mis_b[ialc][ipart]->SetMarkerColor(colors[ipart]);
//       gr_mis_b[ialc][ipart]->SetLineColor(colors[ipart]);
//       gr_mis_b[ialc][ipart]->SetMarkerSize(1.7+ipart*0.3);
//       gr_mis_a[ialc][ipart] = new TGraphErrors(nParDt,v_Dt,mis_v_a[ipart][ialc],0,mis_a[ipart][ialc]);
//       gr_mis_a[ialc][ipart]->SetMarkerStyle(20+ipart);
//       gr_mis_a[ialc][ipart]->SetMarkerColor(colors_a[ipart]);
//       gr_mis_a[ialc][ipart]->SetMarkerSize(1.7+ipart*0.3);
//       gr_mis_a[ialc][ipart]->SetLineColor(colors_a[ipart]);
//       mgr_mis_b[ialc]->Add(gr_mis_b[ialc][ipart]);
//       mgr_mis[ialc]->Add(gr_mis_a[ialc][ipart]);
//     }
//   }// END loop over rotaion scale
//  for(int ialc=0;ialc<6;ialc++){
//    if(ialc<2 || ialc==5){
//      mgr_mis_b[ialc]->Draw("AP");
//      leg_b->Draw();
//      mgr_mis_b[ialc]->GetXaxis()->SetTitle("#Delta_{t}, #mum");
//      mgr_mis_b[ialc]->GetHistogram()->SetMaximum(1200.);
//      mgr_mis_b[ialc]->GetHistogram()->SetMinimum(-1200.);
//      if(ialc==5){
//        mgr_mis_b[ialc]->GetHistogram()->SetMaximum(10.);
//        mgr_mis_b[ialc]->GetHistogram()->SetMinimum(-10.);
//      }
//      mgr_mis_b[ialc]->SetTitle("Before");
//      if(ialc==0) mgr_mis_b[ialc]->GetYaxis()->SetTitle("#Delta_{x}, #mum");
//      if(ialc==1) mgr_mis_b[ialc]->GetYaxis()->SetTitle("#Delta_{y}, #mum");
//      if(ialc==2) mgr_mis_b[ialc]->GetYaxis()->SetTitle("#Delta_{z}, #mum");
//      if(ialc==3) mgr_mis_b[ialc]->GetYaxis()->SetTitle("#Delta_{#alpha}, mrad");
//      if(ialc==4) mgr_mis_b[ialc]->GetYaxis()->SetTitle("#Delta_{#beta}, mrad");
//      if(ialc==5) mgr_mis_b[ialc]->GetYaxis()->SetTitle("#Delta_{#gamma}, mrad");
//      c1.Print(resname_pdf_o); //write canvas and keep the file open
//      c1.Clear();

//      mgr_mis[ialc]->Draw("AP");
//      leg_a->Draw();
//      mgr_mis[ialc]->GetXaxis()->SetTitle("#Delta_{t}, #mum");
//      mgr_mis[ialc]->GetHistogram()->SetMaximum(10.);
//      mgr_mis[ialc]->GetHistogram()->SetMinimum(-10.);
//      if(ialc==5){
//        mgr_mis[ialc]->GetHistogram()->SetMaximum(5.);
//        mgr_mis[ialc]->GetHistogram()->SetMinimum(-5.);
//      }
//      mgr_mis[ialc]->SetTitle("After");
//      if(ialc==0) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{x}, #mum");
//      if(ialc==1) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{y}, #mum");
//      if(ialc==2) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{z}, #mum");
//      if(ialc==3) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{#alpha}, mrad");
//      if(ialc==4) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{#beta}, mrad");
//      if(ialc==5) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{#gamma}, mrad");
//      c1.Print(resname_pdf_o); //write canvas and keep the file open
//      c1.Clear();


//    }
//  }

//  //c1.Print(resname_pdf_c); //write canvas and close the file
// TLegend *leg2 = new TLegend(0.78,0.58,0.98,0.98);
//  leg2->SetFillColor(0);

//  TMultiGraph *mgr_stat = new TMultiGraph();
//  TMultiGraph *mgr_stat_kn = new TMultiGraph();
//  TGraphErrors *grstatBef[nParDa], *grstatAft[nParDa];
//  TGraphErrors *grstatKnossos[nParDa];
//  TGraphErrors *grstatKnossos0[nParDa];
//  for(int ipart=0;ipart<nParDa;ipart++){// loop over rotaion scale
//    grstatBef[ipart] = new TGraphErrors(nParDt,v_Dt,TrksBef[ipart],0,0);
//    grstatBef[ipart]->SetMarkerStyle(20+ipart);
//    grstatBef[ipart]->SetMarkerColor(colors[ipart]);
//    grstatBef[ipart]->SetMarkerSize(1.7+ipart*0.3);

//    grstatKnossos0[ipart] = new TGraphErrors(nParDt,v_Dt,TrksKnossos0[ipart],0,errTrksKnossos0[ipart]);
//    grstatKnossos0[ipart]->SetMarkerStyle(20+ipart);
//    grstatKnossos0[ipart]->SetMarkerColor(colors[ipart]);
//    grstatKnossos0[ipart]->SetLineColor(colors[ipart]);
//    grstatKnossos0[ipart]->SetMarkerSize(1.7+ipart*0.3);
//    mgr_stat_kn->Add(grstatKnossos0[ipart]);
//    grstatKnossos[ipart] = new TGraphErrors(nParDt,v_Dt,TrksKnossos[ipart],0,errTrksKnossos[ipart]);
//    grstatKnossos[ipart]->SetMarkerStyle(20+ipart);
//    grstatKnossos[ipart]->SetMarkerColor(colors_a[ipart]);
//    grstatKnossos[ipart]->SetLineColor(colors_a[ipart]);
//    grstatKnossos[ipart]->SetMarkerSize(1.7+ipart*0.3);
//    TString dRName = "#Delta_{r} = ";
//    dRName +=v_Da[ipart];
//   TString dRName_b = dRName+ " (input)";  
//   TString dRName_a = dRName+ " (last iter)";  
//   leg2->AddEntry(grstatKnossos0[ipart],dRName_b,"lep");
//   leg2->AddEntry(grstatKnossos[ipart],dRName_a,"lep");

//    mgr_stat_kn->Add(grstatKnossos[ipart]);
//    grstatAft[ipart] = new TGraphErrors(nParDt,v_Dt,TrksAft[ipart],0,0);
//    grstatAft[ipart]->SetMarkerStyle(20+ipart);
//    grstatAft[ipart]->SetMarkerSize(1.7+ipart*0.3);
//    grstatAft[ipart]->SetMarkerColor(kOrange+7);
//    mgr_stat->Add(grstatBef[ipart]);
//    mgr_stat->Add(grstatAft[ipart]);
//  }// END loop over rotaion scale
//  // mgr_stat->Draw("AP");
//  // mgr_stat->GetXaxis()->SetTitle("#Delta_{t}, #mum");
//  // mgr_stat->GetYaxis()->SetTitle("N_{REC}");
//  // mgr_stat->GetHistogram()->SetMaximum(1e5);
//  // mgr_stat->GetHistogram()->SetMinimum(0);
//  // leg_b->Draw();
//  // c1.Print(resname_pdf_o); //write canvas and keep the file open
//  c1.Clear();
//  mgr_stat_kn->Draw("AP");
//  mgr_stat_kn->SetTitle("#trks used in Millepede (av. per sector)");
//  mgr_stat_kn->GetXaxis()->SetTitle("#Delta_{t}, #mum");
//  mgr_stat_kn->GetYaxis()->SetTitle("N_{REC}");
//  leg2->Draw();
//  c1.Print(resname_pdf_c); //write canvas and close the file
//  //  
//  //  mgr_stat->GetXaxis()->SetTitle("Number of sim. trks");
//  //  
//  //  //mgr_stat->SetTitle("Average number of rec. trks (empty = before Knossos; filled = after Knossos)");
//  //  c1.Print(resname_pdf_o); //write canvas and keep the file open
//  //  c1.Clear();
//  //  TGraphErrors *grstatDiff = new TGraphErrors(nS,TrksSim,av_TrksDiff,0,avEr_TrksDiff);
//  //  grstatDiff->SetMarkerStyle(22);
//  //  grstatDiff->SetMarkerSize(2.5);
//  //  grstatDiff->Draw("ALP");
//  //  grstatDiff->GetXaxis()->SetTitle("Number of sim. trks");
//  //  grstatDiff->GetYaxis()->SetTitle("(N^{ after}_{REC} - N^{ before}_{REC})/N_{SIM}, %");
//  //  //  grstatDiff->SetTitle("Diff number of rec. trks (after - before), %");
//  //  c1.Print(resname_pdf_c); //write canvas and close ps file
//  //  c1.Close();
//  //  TString out = resname+".root";
//  //  TFile *f = new TFile(out,"RECREATE");
//  //  // for (int g=0;g<6;g++){
//  //  //   gr_mis_b[g]->Write();
//  //  //   gr_mis_a[g]->Write();
//  //  //   mgr_mis[g]->Write();
//  //  // }
//  //  gr_theta_b->Write();
//  //  gr_theta_r->Write();
//  //  gr_theta_a->Write();
//  //  mgr_theta->Write();
//  //  gr_theta_rms_b->Write();
//  //  gr_theta_rms_r->Write();
//  //  gr_theta_rms_a->Write();
//  //  mgr_theta_rms->Write();

//  // for(int ialc=0;ialc<6;ialc++){
//  //   gr_mis_b[ialc]->Write();
//  //   gr_mis_a[ialc]->Write();
//  //   mgr_mis[ialc]->Write();
//  // }
//  // mgr_stat->Write();
//  // grstatDiff->Write();
//   // f->Write();
//   // f->Close();
//   //   }
//   // }
}
