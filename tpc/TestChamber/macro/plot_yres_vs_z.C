#include <iostream>
#include <fstream>
#include "TString.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPaveText.h"
#include "TGraphErrors.h"

#include "../src/AnalysisEvent.h"
#include "../src/AnalysisCluster.h"
#define WITH_CUTS


Double_t doublegausf(Double_t *x,Double_t *par)
{
  Double_t arg1 = 0;
  Double_t arg2 = 0;
  if (fabs(par[2]) > 1.E-10) arg1 = (x[0] - par[1])/par[2];
  if (fabs(par[5]) > 1.E-10) arg2 = (x[0] - par[4])/par[5];
  Double_t fitval = par[0]*TMath::Exp(-0.5*arg1*arg1) + par[3]*TMath::Exp(-0.5*arg2*arg2);
  return fitval;
}

void plot_yres_vs_z(TString files){

  gSystem->Load("../libanalysis.so"); 
  gROOT->Macro("christian_style.C"); 


  TChain myChain("at");


  TH1D *yresidall = new TH1D("yresidall","",500,-1,1);
  yresidall->SetXTitle("Residual y [mm]");

  TH1D *yresid0 = new TH1D("yresid0"," 0mm < z < 10mm",100,-1,1);
  yresid0->SetXTitle("Residual y [mm]");
  TH1D *yresid1 = new TH1D("yresid1","10mm < z < 20mm",100,-1,1);
  yresid1->SetXTitle("Residual y [mm]");
  TH1D *yresid2 = new TH1D("yresid2","20mm < z < 30mm",100,-1,1);
  yresid2->SetXTitle("Residual y [mm]");
  TH1D *yresid3 = new TH1D("yresid3","30mm < z < 40mm",100,-1,1);
  yresid3->SetXTitle("Residual y [mm]");
  TH1D *yresid4 = new TH1D("yresid4","40mm < z < 50mm",100,-1,1);
  yresid4->SetXTitle("Residual y [mm]");
  TH1D *yresid5 = new TH1D("yresid5","50mm < z < 60mm",100,-1,1);
  yresid5->SetXTitle("Residual y [mm]");
  TH1D *yresid6 = new TH1D("yresid6","60mm < z < 70mm",100,-1,1);
  yresid6->SetXTitle("Residual y [mm]");
  TH1D *yresid7 = new TH1D("yresid7","70mm < z < 80mm",100,-1,1);
  yresid7->SetXTitle("Residual y [mm]");


  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;


  Int_t nevent = myChain.GetEntries();

  AnalysisEvent *event = new AnalysisEvent();

  myChain.SetBranchAddress("EventBranch", &event);

 
  //event loop
  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);

    //global cuts
    
    //if(event->th()>30 || fabs(event->thX())<1) continue;
    //if( (event->th()>30) || (event->th()<1) ) continue;
    if(event->th()>30) continue;
    if( (event->getNselhits()) < 4 ) continue;

    for(unsigned int j=0;j<event->nClusters();++j) {
      
      yresidall->Fill(event->getCluster(j)->resid().y() );
      //cout << j << " " << event->nClusters() << endl;
      double z = event->getCluster(j)->pos().z();
      
      if(z>0 &&z<10) yresid0->Fill(event->getCluster(j)->resid().y() );
      if(z>10&&z<20) yresid1->Fill(event->getCluster(j)->resid().y() );
      if(z>20&&z<30) yresid2->Fill(event->getCluster(j)->resid().y() );
      if(z>30&&z<40) yresid3->Fill(event->getCluster(j)->resid().y() );
      if(z>40&&z<50) yresid4->Fill(event->getCluster(j)->resid().y() );
      if(z>50&&z<60) yresid5->Fill(event->getCluster(j)->resid().y() );
      if(z>60&&z<70) yresid6->Fill(event->getCluster(j)->resid().y() );
      if(z>70&&z<80) yresid7->Fill(event->getCluster(j)->resid().y() );

    }
  }//end of event loop


  Double_t yresolall  = -1;
  Double_t yresol0    = -1;
  Double_t yresol1    = -1;
  Double_t yresol2    = -1;
  Double_t yresol3    = -1;
  Double_t yresol4    = -1;
  Double_t yresol5    = -1;
  Double_t yresol6    = -1;
  Double_t yresol7    = -1;

  Double_t err_yresolall  = -1;
  Double_t err_yresol0    = -1;
  Double_t err_yresol1    = -1;
  Double_t err_yresol2    = -1;
  Double_t err_yresol3    = -1;
  Double_t err_yresol4    = -1;
  Double_t err_yresol5    = -1;
  Double_t err_yresol6    = -1;
  Double_t err_yresol7    = -1;

  
  TF1 *doublegaus_f = new TF1("doublegaus_f",doublegausf,-1,1,6);

  doublegaus_f->SetParLimits(0,0,10000);
  doublegaus_f->SetParLimits(2,0.05,5);
  doublegaus_f->SetParLimits(3,0,10000);
  doublegaus_f->SetParLimits(5,0.05,5);

  doublegaus_f->SetParameters(200,yresidall->GetMean(),yresidall->GetRMS(),
			      50,yresidall->GetMean(),yresidall->GetRMS());


  //yresidall
  yresidall->Fit("doublegaus_f","R");

  TF1 *yresidall_f1 = new TF1("yresidall_f1","gaus",-1.,1.);
  TF1 *yresidall_f2 = new TF1("yresidall_f2","gaus",-1.,1.);

  yresidall_f1->SetParameter(0,doublegaus_f->GetParameter(0));
  yresidall_f1->SetParameter(1,doublegaus_f->GetParameter(1));
  yresidall_f1->SetParameter(2,doublegaus_f->GetParameter(2));
  yresidall_f1->SetParError(2,doublegaus_f->GetParError(2));
  yresidall_f1->SetLineColor(kRed);
  yresidall_f2->SetParameter(0,doublegaus_f->GetParameter(3));
  yresidall_f2->SetParameter(1,doublegaus_f->GetParameter(4));
  yresidall_f2->SetParameter(2,doublegaus_f->GetParameter(5));
  yresidall_f2->SetParError(2,doublegaus_f->GetParError(5));
  yresidall_f2->SetLineColor(kBlue);

  Double_t int1 = yresidall_f1->Integral(-1.,1.);
  Double_t int2 = yresidall_f2->Integral(-1.,1.);
  
  yresolall = (int1*fabs(yresidall_f1->GetParameter(2))+
	       int2*fabs(yresidall_f2->GetParameter(2)))/(int1 + int2);

  err_yresolall  = (1./(int1+int2));
  err_yresolall *= sqrt(int1*int1*pow((yresidall_f1->GetParError(2)),2)+
			int2*int2*pow(yresidall_f2->GetParError(2),2));

  cout<<endl<<"Overall resolution: "<<yresolall<<" +- "
      <<err_yresolall<<endl<<endl;

  //yresid0
  doublegaus_f->SetParameters(200,yresid0->GetMean(),yresid0->GetRMS(),
			      50,yresid0->GetMean(),yresid0->GetRMS());
  
  yresid0->Fit("doublegaus_f","R");

  TF1 *yresid0_f1 = new TF1("yresid0_f1","gaus",-1.,1.);
  TF1 *yresid0_f2 = new TF1("yresid0_f2","gaus",-1.,1.);

  yresid0_f1->SetParameter(0,doublegaus_f->GetParameter(0));
  yresid0_f1->SetParameter(1,doublegaus_f->GetParameter(1));
  yresid0_f1->SetParameter(2,doublegaus_f->GetParameter(2));
  yresid0_f1->SetParError(2,doublegaus_f->GetParError(2));
  yresid0_f1->SetLineColor(kRed);
  yresid0_f2->SetParameter(0,doublegaus_f->GetParameter(3));
  yresid0_f2->SetParameter(1,doublegaus_f->GetParameter(4));
  yresid0_f2->SetParameter(2,doublegaus_f->GetParameter(5));
  yresid0_f2->SetParError(2,doublegaus_f->GetParError(5));
  yresid0_f2->SetLineColor(kBlue);

  int1 = yresid0_f1->Integral(-1.,1.);
  int2 = yresid0_f2->Integral(-1.,1.);
  
  yresol0 = (int1*fabs(yresid0_f1->GetParameter(2))+
	     int2*fabs(yresid0_f2->GetParameter(2)))/(int1 + int2);

  err_yresol0  = (1./(int1+int2));
  err_yresol0 *= sqrt(int1*int1*pow(yresid0_f1->GetParError(2),2)+
			int2*int2*pow(yresid0_f2->GetParError(2),2));

  cout<<endl<<" O-10mm resolution: "<<yresol0<<" +- "<<err_yresol0<<endl<<endl;

  //yresid1
  doublegaus_f->SetParameters(200,yresid1->GetMean(),yresid1->GetRMS(),
			      50,yresid1->GetMean(),yresid1->GetRMS());

  yresid1->Fit("doublegaus_f","R");

  TF1 *yresid1_f1 = new TF1("yresid1_f1","gaus",-1.,1.);
  TF1 *yresid1_f2 = new TF1("yresid1_f2","gaus",-1.,1.);

  yresid1_f1->SetParameter(0,doublegaus_f->GetParameter(0));
  yresid1_f1->SetParameter(1,doublegaus_f->GetParameter(1));
  yresid1_f1->SetParameter(2,doublegaus_f->GetParameter(2));
  yresid1_f1->SetParError(2,doublegaus_f->GetParError(2));
  yresid1_f1->SetLineColor(kRed);
  yresid1_f2->SetParameter(0,doublegaus_f->GetParameter(3));
  yresid1_f2->SetParameter(1,doublegaus_f->GetParameter(4));
  yresid1_f2->SetParameter(2,doublegaus_f->GetParameter(5));
  yresid1_f2->SetParError(2,doublegaus_f->GetParError(5));
  yresid1_f2->SetLineColor(kBlue);

  int1 = yresid1_f1->Integral(-1.,1.);
  int2 = yresid1_f2->Integral(-1.,1.);
  
  yresol1 = (int1*fabs(yresid1_f1->GetParameter(2))+
	     int2*fabs(yresid1_f2->GetParameter(2)))/(int1 + int2);

  err_yresol1  = (1./(int1+int2));
  err_yresol1 *= sqrt(int1*int1*pow(yresid1_f1->GetParError(2),2)+
			int2*int2*pow(yresid1_f2->GetParError(2),2));

  cout<<endl<<"10-20mm resolution: "<<yresol1<<" +- "<<err_yresol1<<endl<<endl;

  //yresid2
  doublegaus_f->SetParameters(200,yresid2->GetMean(),yresid2->GetRMS(),
			      50,yresid2->GetMean(),yresid2->GetRMS());

  yresid2->Fit("doublegaus_f","R");

  TF1 *yresid2_f1 = new TF1("yresid2_f1","gaus",-1.,1.);
  TF1 *yresid2_f2 = new TF1("yresid2_f2","gaus",-1.,1.);

  yresid2_f1->SetParameter(0,doublegaus_f->GetParameter(0));
  yresid2_f1->SetParameter(1,doublegaus_f->GetParameter(1));
  yresid2_f1->SetParameter(2,doublegaus_f->GetParameter(2));
  yresid2_f1->SetParError(2,doublegaus_f->GetParError(2));
  yresid2_f1->SetLineColor(kRed);
  yresid2_f2->SetParameter(0,doublegaus_f->GetParameter(3));
  yresid2_f2->SetParameter(1,doublegaus_f->GetParameter(4));
  yresid2_f2->SetParameter(2,doublegaus_f->GetParameter(5));
  yresid2_f2->SetParError(2,doublegaus_f->GetParError(5));
  yresid2_f2->SetLineColor(kBlue);

  int1 = yresid2_f1->Integral(-1.,1.);
  int2 = yresid2_f2->Integral(-1.,1.);
  
  yresol2 = (int1*fabs(yresid2_f1->GetParameter(2))+
	     int2*fabs(yresid2_f2->GetParameter(2)))/(int1 + int2);

  err_yresol2  = (1./(int1+int2));
  err_yresol2 *= sqrt(int1*int1*pow(yresid2_f1->GetParError(2),2)+
			int2*int2*pow(yresid2_f2->GetParError(2),2));

  cout<<endl<<"20-30mm resolution: "<<yresol2<<" +- "<<err_yresol2<<endl<<endl;


  //yresid3
  doublegaus_f->SetParameters(200,yresid3->GetMean(),yresid3->GetRMS(),
			      50,yresid3->GetMean(),yresid3->GetRMS());

  yresid3->Fit("doublegaus_f","R");

  TF1 *yresid3_f1 = new TF1("yresid3_f1","gaus",-1.,1.);
  TF1 *yresid3_f2 = new TF1("yresid3_f2","gaus",-1.,1.);

  yresid3_f1->SetParameter(0,doublegaus_f->GetParameter(0));
  yresid3_f1->SetParameter(1,doublegaus_f->GetParameter(1));
  yresid3_f1->SetParameter(2,doublegaus_f->GetParameter(2));
  yresid3_f1->SetParError(2,doublegaus_f->GetParError(2));
  yresid3_f1->SetLineColor(kRed);
  yresid3_f2->SetParameter(0,doublegaus_f->GetParameter(3));
  yresid3_f2->SetParameter(1,doublegaus_f->GetParameter(4));
  yresid3_f2->SetParameter(2,doublegaus_f->GetParameter(5));
  yresid3_f2->SetParError(2,doublegaus_f->GetParError(5));
  yresid3_f2->SetLineColor(kBlue);

  int1 = yresid3_f1->Integral(-1.,1.);
  int2 = yresid3_f2->Integral(-1.,1.);
  
  yresol3 = (int1*fabs(yresid3_f1->GetParameter(2))+
	     int2*fabs(yresid3_f2->GetParameter(2)))/(int1 + int2);

  err_yresol3  = (1./(int1+int2));
  err_yresol3 *= sqrt(int1*int1*pow(yresid3_f1->GetParError(2),2)+
			int2*int2*pow(yresid3_f2->GetParError(2),2));

  cout<<endl<<"30-40mm resolution: "<<yresol3<<" +- "<<err_yresol3<<endl<<endl;


  //yresid4
  doublegaus_f->SetParameters(200,yresid4->GetMean(),yresid4->GetRMS(),
			      50,yresid4->GetMean(),yresid4->GetRMS());

  yresid4->Fit("doublegaus_f","R");

  TF1 *yresid4_f1 = new TF1("yresid4_f1","gaus",-1.,1.);
  TF1 *yresid4_f2 = new TF1("yresid4_f2","gaus",-1.,1.);

  yresid4_f1->SetParameter(0,doublegaus_f->GetParameter(0));
  yresid4_f1->SetParameter(1,doublegaus_f->GetParameter(1));
  yresid4_f1->SetParameter(2,doublegaus_f->GetParameter(2));
  yresid4_f1->SetParError(2,doublegaus_f->GetParError(2));
  yresid4_f1->SetLineColor(kRed);
  yresid4_f2->SetParameter(0,doublegaus_f->GetParameter(3));
  yresid4_f2->SetParameter(1,doublegaus_f->GetParameter(4));
  yresid4_f2->SetParameter(2,doublegaus_f->GetParameter(5));
  yresid4_f2->SetParError(2,doublegaus_f->GetParError(5));
  yresid4_f2->SetLineColor(kBlue);

  int1 = yresid4_f1->Integral(-1.,1.);
  int2 = yresid4_f2->Integral(-1.,1.);
  
  yresol4 = (int1*fabs(yresid4_f1->GetParameter(2))+
	     int2*fabs(yresid4_f2->GetParameter(2)))/(int1 + int2);

  err_yresol4  = (1./(int1+int2));
  err_yresol4 *= sqrt(int1*int1*pow(yresid4_f1->GetParError(2),2)+
			int2*int2*pow(yresid4_f2->GetParError(2),2));

  cout<<endl<<"40-50mm resolution: "<<yresol4<<" +- "<<err_yresol4<<endl<<endl;


  //yresid5
  doublegaus_f->SetParameters(200,yresid5->GetMean(),yresid5->GetRMS(),
			      50,yresid5->GetMean(),yresid5->GetRMS());

  yresid5->Fit("doublegaus_f","R");

  TF1 *yresid5_f1 = new TF1("yresid5_f1","gaus",-1.,1.);
  TF1 *yresid5_f2 = new TF1("yresid5_f2","gaus",-1.,1.);

  yresid5_f1->SetParameter(0,doublegaus_f->GetParameter(0));
  yresid5_f1->SetParameter(1,doublegaus_f->GetParameter(1));
  yresid5_f1->SetParameter(2,doublegaus_f->GetParameter(2));
  yresid5_f1->SetParError(2,doublegaus_f->GetParError(2));
  yresid5_f1->SetLineColor(kRed);
  yresid5_f2->SetParameter(0,doublegaus_f->GetParameter(3));
  yresid5_f2->SetParameter(1,doublegaus_f->GetParameter(4));
  yresid5_f2->SetParameter(2,doublegaus_f->GetParameter(5));
  yresid5_f2->SetParError(2,doublegaus_f->GetParError(5));
  yresid5_f2->SetLineColor(kBlue);

  int1 = yresid5_f1->Integral(-1.,1.);
  int2 = yresid5_f2->Integral(-1.,1.);
  
  yresol5 = (int1*fabs(yresid5_f1->GetParameter(2))+
	     int2*fabs(yresid5_f2->GetParameter(2)))/(int1 + int2);

  err_yresol5  = (1./(int1+int2));
  err_yresol5 *= sqrt(int1*int1*pow(yresid5_f1->GetParError(2),2)+
			int2*int2*pow(yresid5_f2->GetParError(2),2));

  cout<<endl<<"50-60mm resolution: "<<yresol5<<" +- "<<err_yresol5<<endl<<endl;


  //yresid6
  doublegaus_f->SetParameters(200,yresid6->GetMean(),yresid6->GetRMS(),
			      50,yresid6->GetMean(),yresid6->GetRMS());

  yresid6->Fit("doublegaus_f","R");

  TF1 *yresid6_f1 = new TF1("yresid6_f1","gaus",-1.,1.);
  TF1 *yresid6_f2 = new TF1("yresid6_f2","gaus",-1.,1.);

  yresid6_f1->SetParameter(0,doublegaus_f->GetParameter(0));
  yresid6_f1->SetParameter(1,doublegaus_f->GetParameter(1));
  yresid6_f1->SetParameter(2,doublegaus_f->GetParameter(2));
  yresid6_f1->SetParError(2,doublegaus_f->GetParError(2));
  yresid6_f1->SetLineColor(kRed);
  yresid6_f2->SetParameter(0,doublegaus_f->GetParameter(3));
  yresid6_f2->SetParameter(1,doublegaus_f->GetParameter(4));
  yresid6_f2->SetParameter(2,doublegaus_f->GetParameter(5));
  yresid6_f2->SetParError(2,doublegaus_f->GetParError(5));
  yresid6_f2->SetLineColor(kBlue);

  int1 = yresid6_f1->Integral(-1.,1.);
  int2 = yresid6_f2->Integral(-1.,1.);
  
  yresol6 = (int1*fabs(yresid6_f1->GetParameter(2))+
	     int2*fabs(yresid6_f2->GetParameter(2)))/(int1 + int2);

  err_yresol6  = (1./(int1+int2));
  err_yresol6 *= sqrt(int1*int1*pow(yresid6_f1->GetParError(2),2)+
			int2*int2*pow(yresid6_f2->GetParError(2),2));

  cout<<endl<<"60-70mm resolution: "<<yresol6<<" +- "<<err_yresol6<<endl<<endl;


  //yresid7
  doublegaus_f->SetParameters(200,yresid7->GetMean(),yresid7->GetRMS(),
			      50,yresid7->GetMean(),yresid7->GetRMS());

  yresid7->Fit("doublegaus_f","R");

  TF1 *yresid7_f1 = new TF1("yresid7_f1","gaus",-1.,1.);
  TF1 *yresid7_f2 = new TF1("yresid7_f2","gaus",-1.,1.);

  yresid7_f1->SetParameter(0,doublegaus_f->GetParameter(0));
  yresid7_f1->SetParameter(1,doublegaus_f->GetParameter(1));
  yresid7_f1->SetParameter(2,doublegaus_f->GetParameter(2));
  yresid7_f1->SetParError(2,doublegaus_f->GetParError(2));
  yresid7_f1->SetLineColor(kRed);
  yresid7_f2->SetParameter(0,doublegaus_f->GetParameter(3));
  yresid7_f2->SetParameter(1,doublegaus_f->GetParameter(4));
  yresid7_f2->SetParameter(2,doublegaus_f->GetParameter(5));
  yresid7_f2->SetParError(2,doublegaus_f->GetParError(5));
  yresid7_f2->SetLineColor(kBlue);

  int1 = yresid7_f1->Integral(-1.,1.);
  int2 = yresid7_f2->Integral(-1.,1.);
  
  yresol7 = (int1*fabs(yresid7_f1->GetParameter(2))+
	     int2*fabs(yresid7_f2->GetParameter(2)))/(int1 + int2);

  err_yresol7  = (1./(int1+int2));
  err_yresol7 *= sqrt(int1*int1*pow(yresid7_f1->GetParError(2),2)+
			int2*int2*pow(yresid7_f2->GetParError(2),2));

  cout<<endl<<"70-80mm resolution: "<<yresol7<<" +- "<<err_yresol7<<endl<<endl;


  //prepare overview graph

  double x[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  double xx[8] = {5, 15, 25, 35, 45, 55, 65, 73.5};
  double ex[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  double exx[8] = {5, 5, 5, 5, 5, 5, 5, 3.5};
  double y[8] = {yresol0*1000, yresol1*1000, yresol2*1000, yresol3*1000,
		 yresol4*1000, yresol5*1000, yresol6*1000, yresol7*1000};
  double yy[8] = {yresol0, yresol1, yresol2, yresol3,
		 yresol4, yresol5, yresol6, yresol7};
  double ey[8] = {err_yresol0*1000, err_yresol1*1000,
		  err_yresol2*1000, err_yresol3*1000,
		  err_yresol4*1000, err_yresol5*1000,
		  err_yresol6*1000, err_yresol7*1000};
  double eyy[8] = {err_yresol0, err_yresol1,
		  err_yresol2, err_yresol3,
		  err_yresol4, err_yresol5,
		  err_yresol6, err_yresol7};

  TGraphErrors *yres_vs_z = new TGraphErrors(8, x, y, ex, ey);
  yres_vs_z->SetTitle("");
  yres_vs_z->SetMarkerStyle(21);
  yres_vs_z->SetMarkerColor(2);
  yres_vs_z->SetMarkerSize(1);

  TGraphErrors *yres_vs_z2 = new TGraphErrors(8, xx, yy, exx, eyy);
  yres_vs_z2->SetTitle("");
  yres_vs_z2->SetMarkerStyle(21);
  yres_vs_z2->SetMarkerColor(2);
  yres_vs_z2->SetMarkerSize(1);

  TH2D *hr1 = new TH2D("hr1","hr1",8,0.5,8.5, 10,65.5,285.5);
  hr1->SetStats(kFALSE);
  hr1->SetTitle("");
  hr1->SetYTitle("Spatial Resolution y (#mum)");
  hr1->SetLabelOffset(0.01, "X");
  hr1->SetLabelSize(0.05, "X");

  TAxis *xaxis = hr1->GetXaxis();
  xaxis->SetBinLabel(1," 0 < z < 10mm");
  xaxis->SetBinLabel(2,"10 < z < 20mm");
  xaxis->SetBinLabel(3,"20 < z < 30mm");
  xaxis->SetBinLabel(4,"30 < z < 40mm");
  xaxis->SetBinLabel(5,"40 < z < 50mm");
  xaxis->SetBinLabel(6,"50 < z < 60mm");
  xaxis->SetBinLabel(7,"60 < z < 70mm");
  xaxis->SetBinLabel(8,"70 < z < 80mm");


  //plot and save all that stuff

  
  TCanvas *canvas = new TCanvas();//"canvas","",0,0,1200,800);  
  yresidall->Draw();
  yresidall_f1->Draw("same");
  yresidall_f2->Draw("same");
  canvas->SaveAs("output/yresidall.eps");
  

  
  canvas = new TCanvas();
  yresid0->Draw();
  yresid0_f1->Draw("same");
  yresid0_f2->Draw("same");
  //canvas->SaveAs("output/yresid0.eps");
  canvas = new TCanvas();
  yresid1->Draw();
  yresid1_f1->Draw("same");
  yresid1_f2->Draw("same");
  //canvas->SaveAs("output/yresid1.eps");
  canvas = new TCanvas();
  yresid2->Draw();
  yresid2_f1->Draw("same");
  yresid2_f2->Draw("same");
  //canvas->SaveAs("output/yresid2.eps");
  canvas = new TCanvas();
  yresid3->Draw();
  yresid3_f1->Draw("same");
  yresid3_f2->Draw("same");
  //canvas->SaveAs("output/yresid3.eps");
  canvas = new TCanvas();
  yresid4->Draw();
  yresid4_f1->Draw("same");
  yresid4_f2->Draw("same");
  //canvas->SaveAs("output/yresid4.eps");
  canvas = new TCanvas();
  yresid5->Draw();
  yresid5_f1->Draw("same");
  yresid5_f2->Draw("same");
  //canvas->SaveAs("output/yresid5.eps");
  canvas = new TCanvas();
  yresid6->Draw();
  yresid6_f1->Draw("same");
  yresid6_f2->Draw("same");
  //canvas->SaveAs("output/yresid6.eps");
  canvas = new TCanvas();
  yresid7->Draw();
  yresid7_f1->Draw("same");
  yresid7_f2->Draw("same");
  //canvas->SaveAs("output/yresid7.eps");
  
  
  TCanvas *canvas2 = new TCanvas();
  hr1->Draw();
  yres_vs_z->Draw("p");
  canvas2->SaveAs("output/yres_vs_z.eps");
  
  TCanvas *canvas3 = new TCanvas();
  //hr1->Draw();
  yres_vs_z2->Draw("ap");
  //canvas3->SaveAs("output/yres_vs_z.eps");

  TF1 *diff_func = new TF1("diff_func","sqrt([0]+[1]*x)",0,77.);
  diff_func->SetParameter(0,0.02);

  yres_vs_z2->Fit("diff_func","r");

}

