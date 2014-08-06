#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TCanvas.h>
#include <iostream>
#include <sstream>
#include <cmath>
using namespace std;
void SenseAlign(TString pathG="/home/karavdina/soft/pandaroot/macro/lmd/testPixelAlignSIM_LIMITS_10000/mom_15/")
//void SummaryAlign(TString pathG="/panda/pandaroot/macro/lmd/testPixelAlignSIM_10000/mom_15//", double tr_sc=0, double rt_sc=0)
{
  // gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gStyle->SetOptStat(0);
  gStyle->SetLabelSize(0.045,"xyz");
  gStyle->SetTitleYSize(0.04);
  gStyle->SetTitleXSize(0.04);
  const int nParDa=11;
  double v_Da[nParDa]={0,0.5,1,1.5,2,2.5,3,3.5,4,4.5,5};
  TString sv_Da[nParDa]={"0","0_5","1","1_5","2","2_5","3","3_5","4","4_5","5"};
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


  // double theta_mean_b[nParDa][nParDt], theta_mean_r[nParDa][nParDt];
  // double theta_rms_b[nParDa][nParDt], theta_rms_r[nParDa][nParDt];
  // double phi_mean_b[nParDa][nParDt], phi_mean_r[nParDa][nParDt];
  // double phi_rms_b[nParDa][nParDt], phi_rms_r[nParDa][nParDt];
  // double X_mean_b[nParDa][nParDt], X_mean_r[nParDa][nParDt];
  // double X_rms_b[nParDa][nParDt], X_rms_r[nParDa][nParDt];
  // double Y_mean_b[nParDa][nParDt], Y_mean_r[nParDa][nParDt];
  // double Y_rms_b[nParDa][nParDt], Y_rms_r[nParDa][nParDt];


  double theta_mean_b, theta_mean_r;
  double theta_rms_b, theta_rms_r;
  double phi_mean_b, phi_mean_r;
  double phi_rms_b, phi_rms_r;
  double X_mean_b, X_mean_r;
  double X_rms_b, X_rms_r;
  double Y_mean_b, Y_mean_r;
  double Y_rms_b, Y_rms_r;
  // for(int ipart=0;ipart<nParDa;ipart++){
  //   for(int i=0;i<nParDt;i++){
  //     theta_mean_b[ipart][i] = 0;
  //     theta_mean_r[ipart][i] = 0;
  //     theta_rms_b[ipart][i] = 0;
  //     theta_rms_r[ipart][i] = 0;

  //     phi_mean_b[ipart][i] = 0;
  //     phi_mean_r[ipart][i] = 0;
  //     phi_rms_b[ipart][i] = 0;
  //     phi_rms_r[ipart][i] = 0;

  //     X_mean_b[ipart][i] = 0;
  //     X_mean_r[ipart][i] = 0;
  //     X_rms_b[ipart][i] = 0;
  //     X_rms_r[ipart][i] = 0;

  //     Y_mean_b[ipart][i] = 0;
  //     Y_mean_r[ipart][i] = 0;
  //     Y_rms_b[ipart][i] = 0;
  //     Y_rms_r[ipart][i] = 0;

  //   }
  // }

TH2D *htheta_mean = new TH2D("htheta_mean","#theta_{MEAN}; #Delta_{t}, #mum;#Delta_{r}, mrad",4e1,0,v_Dt[nParDt-1],1e1,0,v_Da[nParDa-1]);
 TH2D *htheta_rms = new TH2D("htheta_rms","#theta_{RMS}; #Delta_{t}, #mum;#Delta_{r}, mrad",4e1,0,v_Dt[nParDt-1],1e1,0,v_Da[nParDa-1]);
 TH2D *hphi_mean = new TH2D("hphi_mean","#phi_{MEAN}; #Delta_{t}, #mum;#Delta_{r}, mrad",4e1,0,v_Dt[nParDt-1],1e1,0,v_Da[nParDa-1]);
 TH2D *hphi_rms = new TH2D("hphi_rms","#phi_{RMS}; #Delta_{t}, #mum;#Delta_{r}, mrad",4e1,0,v_Dt[nParDt-1],1e1,0,v_Da[nParDa-1]);
 TH2D *hX_mean = new TH2D("hX_mean","X_{MEAN}; #Delta_{t}, #mum;#Delta_{r}, mrad",4e1,0,v_Dt[nParDt-1],1e1,0,v_Da[nParDa-1]);
 TH2D *hX_rms = new TH2D("hX_rms","X_{RMS}; #Delta_{t}, #mum;#Delta_{r}, mrad",4e1,0,v_Dt[nParDt-1],1e1,0,v_Da[nParDa-1]);
 TH2D *hY_mean = new TH2D("hY_mean","Y_{MEAN}; #Delta_{t}, #mum;#Delta_{r}, mrad",4e1,0,v_Dt[nParDt-1],1e1,0,v_Da[nParDa-1]);
 TH2D *hY_rms = new TH2D("hY_rms","Y_{RMS}; #Delta_{t}, #mum;#Delta_{r}, mrad",4e1,0,v_Dt[nParDt-1],1e1,0,v_Da[nParDa-1]);
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
      // cout<<name1<<endl;
      
      // TH1 *hbefore;   
      // TH1 *hafter;
      // TH1 *hrefer;
      
      TFile *f1 = new TFile(name1,"READ");
      if (f1->IsZombie()) {
	std::cout << "!!! Error opening file " <<name1<< std::endl;
	continue;
	//   return;
      }
      


      TH1 *hbefore = (TH1*)f1->Get("NearIP/hResTheta");
      theta_mean_b  = 1e3*(hbefore->GetMean());
      theta_rms_b  = 1e3*(hbefore->GetRMS());
      hbefore = (TH1*)f1->Get("NearIP/hResPhi");
      phi_mean_b  = 1e3*(hbefore->GetMean());
      phi_rms_b  = 1e3*(hbefore->GetRMS());
      hbefore = (TH1*)f1->Get("NearIP/hResPointX");
      X_mean_b  = 1e4*(hbefore->GetMean());
      X_rms_b = 1e4*(hbefore->GetRMS());
      hbefore = (TH1*)f1->Get("NearIP/hResPointY");
      Y_mean_b  = 1e4*(hbefore->GetMean());
      Y_rms_b  = 1e4*(hbefore->GetRMS());
     
      if(ipart==0 && i==0){
	theta_mean_b  += 1e-6;
	theta_rms_b  += 1e-6;
	phi_mean_b  += 1e-6;
	phi_rms_b  += 1e-6;
	X_mean_b  += 1e-6;
	X_rms_b  += 1e-6;
	Y_mean_b  += 1e-6;
	Y_rms_b  += 1e-6;
      }


      TFile *f3 = new TFile(name3,"READ");
      if (f3->IsZombie()) {
      	std::cout << "!!! Error opening file " <<name2<< std::endl;
      	continue;
      	// return;
      }
      TH1 *hrefer = (TH1F*)f3->Get("NearIP/hResTheta");
      theta_mean_r = 1e3*(hrefer->GetMean());
      theta_rms_r  = 1e3*(hrefer->GetRMS());
      hrefer = (TH1*)f3->Get("NearIP/hResPhi");
      phi_mean_r  = 1e3*(hrefer->GetMean());
      phi_rms_r  = 1e3*(hrefer->GetRMS());
      hrefer = (TH1*)f3->Get("NearIP/hResPointX");
      X_mean_r  = 1e4*(hrefer->GetMean());
      X_rms_r  = 1e4*(hrefer->GetRMS());
      hrefer = (TH1*)f3->Get("NearIP/hResPointY");
      Y_mean_r  = 1e4*(hrefer->GetMean());
      Y_rms_r  = 1e4*(hrefer->GetRMS());
      //  cout<<"theta_mean_r["<<v_Da[ipart]<<"]["<<v_Dt[i]<<"]="<<theta_mean_r<<endl;
      /// END (Theta resolution) ---------------------------------------------------------------
      
   
      htheta_mean->Fill(v_Dt[i],v_Da[ipart],fabs((theta_mean_b-theta_mean_r)/theta_mean_r));
      htheta_rms->Fill(v_Dt[i],v_Da[ipart],fabs((theta_rms_b-theta_rms_r)/theta_rms_r));
      hphi_mean->Fill(v_Dt[i],v_Da[ipart],fabs((phi_mean_b-phi_mean_r)/phi_mean_r));
      hphi_rms->Fill(v_Dt[i],v_Da[ipart],fabs((phi_rms_b-phi_rms_r)/phi_rms_r));
      hX_mean->Fill(v_Dt[i],v_Da[ipart],fabs((X_mean_b -X_mean_r)/X_mean_r));
      hX_rms->Fill(v_Dt[i],v_Da[ipart],fabs((X_rms_b -X_rms_r)/X_rms_r));
      hY_mean->Fill(v_Dt[i],v_Da[ipart],fabs((Y_mean_b -Y_mean_r)/Y_mean_r));
      hY_rms->Fill(v_Dt[i],v_Da[ipart],fabs((Y_rms_b -Y_rms_r)/Y_rms_r));
   // f2->Close();
      f3->Close();
      //    hrefer->Clear();
      f1->Close();
      //  hbefore->Clear();
    }// trans
 }// rotation
 
 // Draw results --------------------------------
 
 cout<<"Let's draw!"<<endl;
 TCanvas c1;
 htheta_mean->GetZaxis()->SetRangeUser(0,2);
 htheta_mean->Draw("colz");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 htheta_rms->GetZaxis()->SetRangeUser(0,2);
 htheta_rms->Draw("colz");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 hphi_mean->GetZaxis()->SetRangeUser(0,2);
 hphi_mean->Draw("colz");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 hphi_rms->GetZaxis()->SetRangeUser(0,2);
 hphi_rms->Draw("colz");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 hX_mean->GetZaxis()->SetRangeUser(0,2);
 hX_mean->Draw("colz");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 hX_rms->GetZaxis()->SetRangeUser(0,2);
 hX_rms->Draw("colz");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 hY_mean->GetZaxis()->SetRangeUser(0,2);
 hY_mean->Draw("colz");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 hY_rms->GetZaxis()->SetRangeUser(0,2);
 hY_rms->Draw("colz");
 c1.Print(resname_pdf_c); //write canvas and keep the ps file open
 c1.Clear();
 TString out = resname+".root";
 TFile *f = new TFile(out,"RECREATE");
 htheta_mean->Write();
 htheta_rms->Write();
 hphi_mean->Write();
 hphi_rms->Write();
 hX_mean->Write();
 hX_rms->Write();
 hY_mean->Write();
 hY_rms->Write();

 f->Write();
 f->Close();
}
