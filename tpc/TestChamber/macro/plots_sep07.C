#include <iostream>
#include <fstream>
#include "TString.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TF1.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPaveText.h"

#include "../src/AnalysisEvent.h"
#include "../src/AnalysisCluster.h"
#define WITH_CUTS


Double_t doublegausf(Double_t *x,Double_t *par)
{
  Double_t arg1 = 0;
  Double_t arg2 = 0;
  if (fabs(par[2]) > 1.E-10) arg1 = (x[0] - par[1])/par[2];
  if (fabs(par[5]) > 1.E-10) arg2 = (x[0] - par[4])/par[5];
  Double_t fitval = par[0]*TMath::Exp(-0.5*arg1*arg1)+
	par[3]*TMath::Exp(-0.5*arg2*arg2);
  return fitval;
}

void plots(TString files){

  gSystem->Load("../libanalysis.so"); 


  gROOT->Macro("christian_style.C"); 

  TChain myChain("at");

  TH1D *yresidall = new TH1D("yresidall","",500,-1,1);
  yresidall->SetXTitle("residual y [mm]");

  TH1D *yresid1 = new TH1D("yresid1","10mm < z < 20mm",100,-1,1);
  yresid1->SetXTitle("residual y [mm]");
  TH1D *yresid2 = new TH1D("yresid2","20mm < z < 30mm",100,-1,1);
  yresid2->SetXTitle("residual y [mm]");
  TH1D *yresid3 = new TH1D("yresid3","30mm < z < 40mm",100,-1,1);
  yresid3->SetXTitle("residual y [mm]");
  TH1D *yresid4 = new TH1D("yresid4","40mm < z < 50mm",100,-1,1);
  yresid4->SetXTitle("residual y [mm]");
  TH1D *yresid5 = new TH1D("yresid5","50mm < z < 60mm",100,-1,1);
  yresid5->SetXTitle("residual y [mm]");
  TH1D *yresid6 = new TH1D("yresid6","60mm < z < 70mm",100,-1,1);
  yresid6->SetXTitle("residual y [mm]");

  TH1D *ypullall = new TH1D("ypullall","",500,-5,5);
  ypullall->SetXTitle("pull y");

  TH1D *zresidall = new TH1D("zresidall","",500,-1,1);
  zresidall->SetXTitle("residual z [mm]");

  TH1D *zpos = new TH1D("zresidall","",200,0,80);
  zpos->SetXTitle("position z [mm]");


  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  

  AnalysisEvent   *event;

  myChain.SetBranchAddress("EventBranch", &event);
 
  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);

	//global cuts

	if(event->th()>30 || event->thX()<1) continue;

	for(unsigned int j=0;j<event->nClusters();++j) {

	  yresidall->Fill(event->getCluster(j)->resid().y() );
	  ypullall->Fill(event->getCluster(j)->resid().y() /
					 event->getCluster(j)->err().y());
	  zresidall->Fill(event->getCluster(j)->resid().z() );
	  zpos->Fill(event->getCluster(j)->pos().z() );

	  double z = event->getCluster(j)->pos().z();

	  if(z>10&&z<20) yresid1->Fill(event->getCluster(j)->resid().y() );
	  if(z>20&&z<30) yresid2->Fill(event->getCluster(j)->resid().y() );
	  if(z>30&&z<40) yresid3->Fill(event->getCluster(j)->resid().y() );
	  if(z>40&&z<50) yresid4->Fill(event->getCluster(j)->resid().y() );
	  if(z>50&&z<60) yresid5->Fill(event->getCluster(j)->resid().y() );
	  if(z>60&&z<70) yresid6->Fill(event->getCluster(j)->resid().y() );

	}
  }
  
  TF1 *zresidall_f = new TF1("zresidall_f","gaus",-1,1);
  //  zresidall_f->SetLineColor(kRed);
  zresidall->Fit("zresidall_f","R");


  TF1 *ypullall_f = new TF1("ypullall_f","gaus",-5,5);
  //  zresidall_f->SetLineColor(kRed);
  ypullall->Fit("ypullall_f","R");

  TF1 *doublegaus_f = new TF1("doublegaus_f",doublegausf,-1,1,6);
  doublegaus_f->SetParameters(200,yresidall->GetMean(),yresidall->GetRMS(),
							 50,yresidall->GetMean(),yresidall->GetRMS());
  
  yresidall->Fit("doublegaus_f","R");
  TF1 *yresidall_f1 = new TF1("yresidall_f1","gaus",-1.,1.);
  TF1 *yresidall_f2 = new TF1("yresidall_f2","gaus",-1.,1.);
  yresidall_f1->SetParameter(0,doublegaus_f->GetParameter(0));
  yresidall_f1->SetParameter(1,doublegaus_f->GetParameter(1));
  yresidall_f1->SetParameter(2,doublegaus_f->GetParameter(2));
  yresidall_f1->SetLineColor(kRed);
  yresidall_f2->SetParameter(0,doublegaus_f->GetParameter(3));
  yresidall_f2->SetParameter(1,doublegaus_f->GetParameter(4));
  yresidall_f2->SetParameter(2,doublegaus_f->GetParameter(5));
  yresidall_f2->SetLineColor(kBlue);




  TCanvas *canvas = new TCanvas();//"canvas","",0,0,1200,800);
  yresidall->Draw();
  yresidall_f1->Draw("same");
  yresidall_f2->Draw("same");
  canvas->SaveAs("output/yresidall.eps");

  canvas = new TCanvas();//"canvas","",0,0,1200,800);
  zresidall->Draw();
  canvas->SaveAs("output/zresidall.eps");

  canvas = new TCanvas();//"canvas","",0,0,1200,800);
  zpos->Draw();
  canvas->SaveAs("output/zpos.eps");

  canvas = new TCanvas();//"canvas","",0,0,1200,800);
  ypullall->Draw();
  canvas->SaveAs("output/ypullall.eps");

  canvas = new TCanvas();
  yresid1->Draw();
  canvas->SaveAs("output/yresid1.eps");
  canvas = new TCanvas();
  yresid2->Draw();
  canvas->SaveAs("output/yresid2.eps");
  canvas = new TCanvas();
  yresid3->Draw();
  canvas->SaveAs("output/yresid3.eps");
  canvas = new TCanvas();
  yresid4->Draw();
  canvas->SaveAs("output/yresid4.eps");
  canvas = new TCanvas();
  yresid5->Draw();
  canvas->SaveAs("output/yresid5.eps");
  canvas = new TCanvas();
  yresid6->Draw();
  canvas->SaveAs("output/yresid6.eps");

  
	//  canvas->cd(2)->SetLogy(); 
  //  tdist_z_shift->Draw();
	//  canvas->cd(3)->SetLogy(); 
  //  tdist_zoom_shift->Draw();
	//  canvas->cd(4)->SetLogy(); 
  //  thetadist->Draw();
  //tdist_zoom_shift_zoom->Draw();
 
}

