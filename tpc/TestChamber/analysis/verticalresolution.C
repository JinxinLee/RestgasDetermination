#include <iostream>
#include <fstream>
#include <ctime>
#include "TString.h"
#include "TChain.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TPad.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPaveText.h"
#include "TROOT.h"

#include "../src/TCtrack.h"
#include "../src/TCcluster.h"
#include "../src/TCclusterSort.h"
#include <algorithm>
#include <vector>
#include <set>

#include "consecCut.C"
#include "clusterSplit1.C"

Double_t doublegausf(Double_t *x, Double_t *par){
  Double_t arg1 = 0;
  Double_t arg2 = 0;
  if (fabs(par[2]) > 1.E-10) arg1 = (x[0] - par[1])/par[2];
  if (fabs(par[5]) > 1.E-10) arg2 = (x[0] - par[4])/par[5];
  Double_t fitval = par[0]*TMath::Exp(-0.5*arg1*arg1) + par[3]*TMath::Exp(-0.5*arg2*arg2);
  return fitval;
}

void resolution(TString files){

  TChain myChain("at");

  //define the histograms
  //y without 

  TH1D *yall = new TH1D("yall","",500,-0.1,0.1);
  yall->SetXTitle("Residual y [cm]");

  TH1D *y01 = new TH1D("y01","0mm < z < 10mm",500,-0.1,0.1);
  y01->SetXTitle("Residual y [cm]");
  TH1D *y02 = new TH1D("y02","10mm < z < 20mm",500,-0.1,0.1);
  y02->SetXTitle("Residual y [cm]");
  TH1D *y03 = new TH1D("y03","20mm < z < 30mm",500,-0.1,0.1);
  y03->SetXTitle("Residual y [cm]");
  TH1D *y04 = new TH1D("y04","30mm < z < 40mm",500,-0.1,0.1);
  y04->SetXTitle("Residual y [cm]");
  TH1D *y05 = new TH1D("y05","40mm < z < 50mm",500,-0.1,0.1);
  y05->SetXTitle("Residual y [cm]");
  TH1D *y06 = new TH1D("y06","50mm < z < 60mm",500,-0.1,0.1);
  y06->SetXTitle("Residual y [cm]");
  TH1D *y07 = new TH1D("y07","60mm < z < 70mm",500,-0.1,0.1);
  y07->SetXTitle("Residual y [cm]");
  TH1D *y08 = new TH1D("y08","70mm < z < 80mm",500,-0.1,0.1);
  y08->SetXTitle("Residual y [cm]");

  //z without

  TH1D *zall = new TH1D("zall","",500,-0.1,0.1);
  zall->SetXTitle("Residual z [cm]");

  TH1D *z01 = new TH1D("z01","0mm < z < 10mm",500,-0.1,0.1);
  z01->SetXTitle("Residual z [cm]");
  TH1D *z02 = new TH1D("z02","10mm < z < 20mm",500,-0.1,0.1);
  z02->SetXTitle("Residual z [cm]");
  TH1D *z03 = new TH1D("z03","20mm < z < 30mm",500,-0.1,0.1);
  z03->SetXTitle("Residual z [cm]");
  TH1D *z04 = new TH1D("z04","30mm < z < 40mm",500,-0.1,0.1);
  z04->SetXTitle("Residual z [cm]");
  TH1D *z05 = new TH1D("z05","40mm < z < 50mm",500,-0.1,0.1);
  z05->SetXTitle("Residual z [cm]");
  TH1D *z06 = new TH1D("z06","50mm < z < 60mm",500,-0.1,0.1);
  z06->SetXTitle("Residual z [cm]");
  TH1D *z07 = new TH1D("z07","60mm < z < 70mm",500,-0.1,0.1);
  z07->SetXTitle("Residual z [cm]");
  TH1D *z08 = new TH1D("z08","70mm < z < 80mm",500,-0.1,0.1);
  z08->SetXTitle("Residual z [cm]");

  //y using clusterSplit1.C

  TH1D *yallw = new TH1D("yallw","",500,-0.1,0.1);
  yallw->SetXTitle("Residual y using clusterSplit1.C [cm]");

  TH1D *y01w = new TH1D("y01w","0mm < z < 10mm",500,-0.1,0.1);
  y01w->SetXTitle("Residual y using clusterSplit1.C [cm]");
  TH1D *y02w = new TH1D("y02w","10mm < z < 20mm",500,-0.1,0.1);
  y02w->SetXTitle("Residual y using clusterSplit1.C [cm]");
  TH1D *y03w = new TH1D("y03w","20mm < z < 30mm",500,-0.1,0.1);
  y03w->SetXTitle("Residual y using clusterSplit1.C [cm]");
  TH1D *y04w = new TH1D("y04w","30mm < z < 40mm",500,-0.1,0.1);
  y04w->SetXTitle("Residual y using clusterSplit1.C [cm]");
  TH1D *y05w = new TH1D("y05w","40mm < z < 50mm",500,-0.1,0.1);
  y05w->SetXTitle("Residual y using clusterSplit1.C [cm]");
  TH1D *y06w = new TH1D("y06w","50mm < z < 60mm",500,-0.1,0.1);
  y06w->SetXTitle("Residual y using clusterSplit1.C [cm]");
  TH1D *y07w = new TH1D("y07w","60mm < z < 70mm",500,-0.1,0.1);
  y07w->SetXTitle("Residual y using clusterSplit1.C [cm]");
  TH1D *y08w = new TH1D("y08w","70mm < z < 80mm",500,-0.1,0.1);
  y08w->SetXTitle("Residual y using clusterSplit1.C [cm]");

  //z using clusterSplit1.C

  TH1D *zallw = new TH1D("zallw","",500,-0.1,0.1);
  zallw->SetXTitle("Residual z using clustersplit1.C [cm]");

  TH1D *z01w = new TH1D("z01w","0mm < z < 10mm",500,-0.1,0.1);
  z01w->SetXTitle("Residual z using clusterSplit1.C [cm]");
  TH1D *z02w = new TH1D("z02w","10mm < z < 20mm",500,-0.1,0.1);
  z02w->SetXTitle("Residual z using clusterSplit1.C [cm]");
  TH1D *z03w = new TH1D("z03w","20mm < z < 30mm",500,-0.1,0.1);
  z03w->SetXTitle("Residual z using clustersplit1.C [cm]");
  TH1D *z04w = new TH1D("z04w","30mm < z < 40mm",500,-0.1,0.1);
  z04w->SetXTitle("Residual z using clusterSplit1.C [cm]");
  TH1D *z05w = new TH1D("z05w","40mm < z < 50mm",500,-0.1,0.1);
  z05w->SetXTitle("Residual z using clusterSplit1.C [cm]");
  TH1D *z06w = new TH1D("z06w","50mm < z < 60mm",500,-0.1,0.1);
  z06w->SetXTitle("Residual z using clusterSplit1.C [cm]");
  TH1D *z07w = new TH1D("z07w","60mm < z < 70mm",500,-0.1,0.1);
  z07w->SetXTitle("Residual z using clusterSplit1.C [cm]");
  TH1D *z08w = new TH1D("z08w","70mm < z < 80mm",500,-0.1,0.1);
  z08w->SetXTitle("Residual z using clusterSPlit1.C [cm]");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;


  Int_t nevent = myChain.GetEntries();

  TCtrack *intr=0;

  myChain.SetBranchAddress("track", &intr);

  //event loop
  for (Int_t iev=0;iev<nevent;iev++){

    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    TCtrack trSplit = clusterSplit1(tr);
    trSplit.fit(260);

    for(Int_t i=0;i<tr.nCl();i++){

      TCcluster c = tr.getCl(i);
      if(c.getFit()){ 

	if (fabs(tr.getAx()) > 1.E3) continue;
	if (fabs(tr.getAy()) > 1.E3) continue;
	if (tr.getChi2()/tr.getNDF()>2) continue;
	if (tr.getChi2()/tr.getNDF()<0.01) continue;
	if (tr.nCl()<2) continue;

	yall->Fill(c.getRes().Y());
	zall->Fill(c.getRes().Z());

	double z = c.posUVW().Z();

	if(z>0&&z<1) y01->Fill(c.getRes().Y());
	if(z>1&&z<2) y02->Fill(c.getRes().Y());
	if(z>2&&z<3) y03->Fill(c.getRes().Y());
	if(z>3&&z<4) y04->Fill(c.getRes().Y());
	if(z>4&&z<5) y05->Fill(c.getRes().Y());
	if(z>5&&z<6) y06->Fill(c.getRes().Y());
	if(z>6&&z<7) y07->Fill(c.getRes().Y());
	if(z>7&&z<8) y08->Fill(c.getRes().Y());

	if(z>0&&z<1) z01->Fill(c.getRes().Z());
	if(z>1&&z<2) z02->Fill(c.getRes().Z());
	if(z>2&&z<3) z03->Fill(c.getRes().Z());
	if(z>3&&z<4) z04->Fill(c.getRes().Z());
	if(z>4&&z<5) z05->Fill(c.getRes().Z());
	if(z>5&&z<6) z06->Fill(c.getRes().Z());
	if(z>6&&z<7) z07->Fill(c.getRes().Z());
	if(z>7&&z<8) z08->Fill(c.getRes().Z());
      }
    }


    for(Int_t i=0;i<trSplit.nCl();i++){

      TCcluster d = trSplit.getCl(i);
      if(d.getFit()){ 

	if (fabs(trSplit.getAx()) > 1.E3) continue;
	if (fabs(trSplit.getAy()) > 1.E3) continue;
	if (trSplit.getChi2()/trSplit.getNDF()>2) continue;
	if (trSplit.getChi2()/trSplit.getNDF()<0.01) continue;
	if (trSplit.nCl()<2) continue;

	yallw->Fill(d.getRes().Y());
	zallw->Fill(d.getRes().Z());

	double z = d.posUVW().Z();

	if(z>0&&z<1) y01w->Fill(d.getRes().Y());
	if(z>1&&z<2) y02w->Fill(d.getRes().Y());
	if(z>2&&z<3) y03w->Fill(d.getRes().Y());
	if(z>3&&z<4) y04w->Fill(d.getRes().Y());
	if(z>4&&z<5) y05w->Fill(d.getRes().Y());
	if(z>5&&z<6) y06w->Fill(d.getRes().Y());
	if(z>6&&z<7) y07w->Fill(d.getRes().Y());
	if(z>7&&z<8) y08w->Fill(d.getRes().Y());

	if(z>0&&z<1) z01w->Fill(d.getRes().Z());
	if(z>1&&z<2) z02w->Fill(d.getRes().Z());
	if(z>2&&z<3) z03w->Fill(d.getRes().Z());
	if(z>3&&z<4) z04w->Fill(d.getRes().Z());
	if(z>4&&z<5) z05w->Fill(d.getRes().Z());
	if(z>5&&z<6) z06w->Fill(d.getRes().Z());
	if(z>6&&z<7) z07w->Fill(d.getRes().Z());
	if(z>7&&z<8) z08w->Fill(d.getRes().Z());
      }
    }
  }

  //end of event loop
  
  //y without

  Double_t yres_all = -1;
  Double_t yres_01 = -1; 
  Double_t yres_02 = -1;
  Double_t yres_03 = -1;
  Double_t yres_04 = -1;
  Double_t yres_05 = -1;
  Double_t yres_06 = -1;
  Double_t yres_07 = -1;
  Double_t yres_08 = -1;

  Double_t err_yres_all = -1;
  Double_t err_yres_01 = -1;
  Double_t err_yres_02 = -1;
  Double_t err_yres_03 = -1;
  Double_t err_yres_04 = -1;
  Double_t err_yres_05 = -1;
  Double_t err_yres_06 = -1;
  Double_t err_yres_07 = -1;
  Double_t err_yres_08 = -1;

  //z without

  Double_t zres_all = -1;
  Double_t zres_01 = -1; 
  Double_t zres_02 = -1;
  Double_t zres_03 = -1;
  Double_t zres_04 = -1;
  Double_t zres_05 = -1;
  Double_t zres_06 = -1;
  Double_t zres_07 = -1;
  Double_t zres_08 = -1;

  Double_t err_zres_all = -1;
  Double_t err_zres_01 = -1;
  Double_t err_zres_02 = -1;
  Double_t err_zres_03 = -1;
  Double_t err_zres_04 = -1;
  Double_t err_zres_05 = -1;
  Double_t err_zres_06 = -1;
  Double_t err_zres_07 = -1;
  Double_t err_zres_08 = -1;

  //y using clusterSplit1.C

  Double_t yres_allw = -1;
  Double_t yres_01w = -1; 
  Double_t yres_02w = -1;
  Double_t yres_03w = -1;
  Double_t yres_04w = -1;
  Double_t yres_05w = -1;
  Double_t yres_06w = -1;
  Double_t yres_07w = -1;
  Double_t yres_08w = -1;

  Double_t err_yres_allw = -1;
  Double_t err_yres_01w = -1;
  Double_t err_yres_02w = -1;
  Double_t err_yres_03w = -1;
  Double_t err_yres_04w = -1;
  Double_t err_yres_05w = -1;
  Double_t err_yres_06w = -1;
  Double_t err_yres_07w = -1;
  Double_t err_yres_08w = -1;

  //z using clusterSplit1.C

  Double_t zres_allw = -1;
  Double_t zres_01w = -1; 
  Double_t zres_02w = -1;
  Double_t zres_03w = -1;
  Double_t zres_04w = -1;
  Double_t zres_05w = -1;
  Double_t zres_06w = -1;
  Double_t zres_07w = -1;
  Double_t zres_08w = -1;

  Double_t err_zres_allw = -1;
  Double_t err_zres_01w = -1;
  Double_t err_zres_02w = -1;
  Double_t err_zres_03w = -1;
  Double_t err_zres_04w = -1;
  Double_t err_zres_05w = -1;
  Double_t err_zres_06w = -1;
  Double_t err_zres_07w = -1;
  Double_t err_zres_08w = -1;
  

  //y without
  //yall

  TF1 *doublegaus_yall = new TF1("doublegaus_yall",doublegausf,-.1,.1,6);
  doublegaus_yall->SetParameters(200,yall->GetMean(),yall->GetRMS(),50,yall->GetMean(),yall->GetRMS());
  yall->Fit("doublegaus_yall","R");

  TF1 *yall_f1 = new TF1("yall_f1","gaus",-0.1,0.1);
  TF1 *yall_f2 = new TF1("yall_f2","gaus",-0.1,0.1);

  yall_f1->SetParameter(0,doublegaus_yall->GetParameter(0));
  yall_f1->SetParameter(1,doublegaus_yall->GetParameter(1));
  yall_f1->SetParameter(2,doublegaus_yall->GetParameter(2));
  yall_f1->SetParError(2,doublegaus_yall->GetParError(2));
  yall_f1->SetLineColor(kRed);
  yall_f2->SetParameter(0,doublegaus_yall->GetParameter(3));
  yall_f2->SetParameter(1,doublegaus_yall->GetParameter(4));
  yall_f2->SetParameter(2,doublegaus_yall->GetParameter(5));
  yall_f2->SetParError(2,doublegaus_yall->GetParError(5));
  yall_f2->SetLineColor(kBlue);
  
  Double_t intyall_1 = yall_f1->Integral(-.1,.1);
  Double_t intyall_2 = yall_f2->Integral(-.1,.1);
  
  yres_all = (intyall_1*fabs(yall_f1->GetParameter(2))+ intyall_2*fabs(yall_f2->GetParameter(2)))/(intyall_1 + intyall_2);

  err_yres_all  = (1./(intyall_1+intyall_2));
  err_yres_all *= sqrt(intyall_1*intyall_1*pow((yall_f1->GetParError(2)),2)+intyall_2*intyall_2*pow(yall_f2->GetParError(2),2));

  cout<<endl<<"Overall resolution y without: "<<yres_all<<" +- " <<err_yres_all<<endl<<endl;


  //y01

  TF1 *doublegaus_y01 = new TF1("doublegaus_y01",doublegausf,-.1,.1,6);
  doublegaus_y01->SetParameters(200,y01->GetMean(),y01->GetRMS(),50,y01->GetMean(),y01->GetRMS());
  y01->Fit("doublegaus_y01","R");

  TF1 *y01_f1 = new TF1("y01_f1","gaus",-0.1,0.1);
  TF1 *y01_f2 = new TF1("y01_f2","gaus",-0.1,0.1);

  y01_f1->SetParameter(0,doublegaus_y01->GetParameter(0));
  y01_f1->SetParameter(1,doublegaus_y01->GetParameter(1));
  y01_f1->SetParameter(2,doublegaus_y01->GetParameter(2));
  y01_f1->SetParError(2,doublegaus_y01->GetParError(2));
  y01_f1->SetLineColor(kRed);
  y01_f2->SetParameter(0,doublegaus_y01->GetParameter(3));
  y01_f2->SetParameter(1,doublegaus_y01->GetParameter(4));
  y01_f2->SetParameter(2,doublegaus_y01->GetParameter(5));
  y01_f2->SetParError(2,doublegaus_y01->GetParError(5));
  y01_f2->SetLineColor(kBlue);
  
  Double_t inty01_1 = y01_f1->Integral(-.1,.1);
  Double_t inty01_2 = y01_f2->Integral(-.1,.1);
  
  yres_01 = (inty01_1*fabs(y01_f1->GetParameter(2))+ inty01_2*fabs(y01_f2->GetParameter(2)))/(inty01_1 + inty01_2);

  err_yres_01  = (1./(inty01_1+inty01_2));
  err_yres_01 *= sqrt(inty01_1*inty01_1*pow((y01_f1->GetParError(2)),2)+inty01_2*inty01_2*pow(y01_f2->GetParError(2),2));

  cout<<endl<<"resolution y01: "<<yres_01<<" +- " <<err_yres_01<<endl<<endl;


  //y02

  TF1 *doublegaus_y02 = new TF1("doublegaus_y02",doublegausf,-.1,.1,6);
  doublegaus_y02->SetParameters(200,y02->GetMean(),y02->GetRMS(),50,y02->GetMean(),y02->GetRMS());
  y02->Fit("doublegaus_y02","R");

  TF1 *y02_f1 = new TF1("y02_f1","gaus",-0.1,0.1);
  TF1 *y02_f2 = new TF1("y02_f2","gaus",-0.1,0.1);

  y02_f1->SetParameter(0,doublegaus_y02->GetParameter(0));
  y02_f1->SetParameter(1,doublegaus_y02->GetParameter(1));
  y02_f1->SetParameter(2,doublegaus_y02->GetParameter(2));
  y02_f1->SetParError(2,doublegaus_y02->GetParError(2));
  y02_f1->SetLineColor(kRed);
  y02_f2->SetParameter(0,doublegaus_y02->GetParameter(3));
  y02_f2->SetParameter(1,doublegaus_y02->GetParameter(4));
  y02_f2->SetParameter(2,doublegaus_y02->GetParameter(5));
  y02_f2->SetParError(2,doublegaus_y02->GetParError(5));
  y02_f2->SetLineColor(kBlue);
  
  Double_t inty02_1 = y02_f1->Integral(-.1,.1);
  Double_t inty02_2 = y02_f2->Integral(-.1,.1);
  
  yres_all = (inty02_1*fabs(y02_f1->GetParameter(2))+ inty02_2*fabs(y02_f2->GetParameter(2)))/(inty02_1 + inty02_2);

  err_yres_all  = (1./(inty02_1+inty02_2));
  err_yres_all *= sqrt(inty02_1*inty02_1*pow((y02_f1->GetParError(2)),2)+inty02_2*inty02_2*pow(y02_f2->GetParError(2),2));

  cout<<endl<<"resolution y12: "<<yres_02<<" +- " <<err_yres_02<<endl<<endl;

  //y03

  TF1 *doublegaus_y03 = new TF1("doublegaus_y03",doublegausf,-.1,.1,6);
  doublegaus_y03->SetParameters(200,y03->GetMean(),y03->GetRMS(),50,y03->GetMean(),y03->GetRMS());
  y03->Fit("doublegaus_y03","R");

  TF1 *y03_f1 = new TF1("y03_f1","gaus",-0.1,0.1);
  TF1 *y03_f2 = new TF1("y03_f2","gaus",-0.1,0.1);

  y03_f1->SetParameter(0,doublegaus_y03->GetParameter(0));
  y03_f1->SetParameter(1,doublegaus_y03->GetParameter(1));
  y03_f1->SetParameter(2,doublegaus_y03->GetParameter(2));
  y03_f1->SetParError(2,doublegaus_y03->GetParError(2));
  y03_f1->SetLineColor(kRed);
  y03_f2->SetParameter(0,doublegaus_y03->GetParameter(3));
  y03_f2->SetParameter(1,doublegaus_y03->GetParameter(4));
  y03_f2->SetParameter(2,doublegaus_y03->GetParameter(5));
  y03_f2->SetParError(2,doublegaus_y03->GetParError(5));
  y03_f2->SetLineColor(kBlue);
  
  Double_t inty03_1 = y03_f1->Integral(-.1,.1);
  Double_t inty03_2 = y03_f2->Integral(-.1,.1);
  
  yres_03 = (inty03_1*fabs(y03_f1->GetParameter(2))+ inty03_2*fabs(y03_f2->GetParameter(2)))/(inty03_1 + inty03_2);

  err_yres_03  = (1./(inty03_1+inty03_2));
  err_yres_03 *= sqrt(inty03_1*inty03_1*pow((y03_f1->GetParError(2)),2)+inty03_2*inty03_2*pow(y03_f2->GetParError(2),2));

  cout<<endl<<"resolution y23: "<<yres_03<<" +- " <<err_yres_03<<endl<<endl;

  //y04

  TF1 *doublegaus_y04 = new TF1("doublegaus_y04",doublegausf,-.1,.1,6);
  doublegaus_y04->SetParameters(200,y04->GetMean(),y04->GetRMS(),50,y04->GetMean(),y04->GetRMS());
  y04->Fit("doublegaus_y04","R");

  TF1 *y04_f1 = new TF1("y04_f1","gaus",-0.1,0.1);
  TF1 *y04_f2 = new TF1("y04_f2","gaus",-0.1,0.1);

  y04_f1->SetParameter(0,doublegaus_y04->GetParameter(0));
  y04_f1->SetParameter(1,doublegaus_y04->GetParameter(1));
  y04_f1->SetParameter(2,doublegaus_y04->GetParameter(2));
  y04_f1->SetParError(2,doublegaus_y04->GetParError(2));
  y04_f1->SetLineColor(kRed);
  y04_f2->SetParameter(0,doublegaus_y04->GetParameter(3));
  y04_f2->SetParameter(1,doublegaus_y04->GetParameter(4));
  y04_f2->SetParameter(2,doublegaus_y04->GetParameter(5));
  y04_f2->SetParError(2,doublegaus_y04->GetParError(5));
  y04_f2->SetLineColor(kBlue);
  
  Double_t inty04_1 = y04_f1->Integral(-.1,.1);
  Double_t inty04_2 = y04_f2->Integral(-.1,.1);
  
  yres_04 = (inty04_1*fabs(y04_f1->GetParameter(2))+ inty04_2*fabs(y04_f2->GetParameter(2)))/(inty04_1 + inty04_2);

  err_yres_04  = (1./(inty04_1+inty04_2));
  err_yres_04 *= sqrt(inty04_1*inty04_1*pow((y04_f1->GetParError(2)),2)+inty04_2*inty04_2*pow(y04_f2->GetParError(2),2));

  cout<<endl<<"resolution y34: "<<yres_04<<" +- " <<err_yres_04<<endl<<endl;

  //y05

  TF1 *doublegaus_y05 = new TF1("doublegaus_y05",doublegausf,-.1,.1,6);
  doublegaus_y05->SetParameters(200,y05->GetMean(),y05->GetRMS(),50,y05->GetMean(),y05->GetRMS());
  y05->Fit("doublegaus_y05","R");

  TF1 *y05_f1 = new TF1("y05_f1","gaus",-0.1,0.1);
  TF1 *y05_f2 = new TF1("y05_f2","gaus",-0.1,0.1);

  y05_f1->SetParameter(0,doublegaus_y05->GetParameter(0));
  y05_f1->SetParameter(1,doublegaus_y05->GetParameter(1));
  y05_f1->SetParameter(2,doublegaus_y05->GetParameter(2));
  y05_f1->SetParError(2,doublegaus_y05->GetParError(2));
  y05_f1->SetLineColor(kRed);
  y05_f2->SetParameter(0,doublegaus_y05->GetParameter(3));
  y05_f2->SetParameter(1,doublegaus_y05->GetParameter(4));
  y05_f2->SetParameter(2,doublegaus_y05->GetParameter(5));
  y05_f2->SetParError(2,doublegaus_y05->GetParError(5));
  y05_f2->SetLineColor(kBlue);
  
  Double_t inty05_1 = y05_f1->Integral(-.1,.1);
  Double_t inty05_2 = y05_f2->Integral(-.1,.1);
  
  yres_all = (inty05_1*fabs(y05_f1->GetParameter(2))+ inty05_2*fabs(y05_f2->GetParameter(2)))/(inty05_1 + inty05_2);

  err_yres_all  = (1./(inty05_1+inty05_2));
  err_yres_all *= sqrt(inty05_1*inty05_1*pow((y05_f1->GetParError(2)),2)+inty05_2*inty05_2*pow(y05_f2->GetParError(2),2));

  cout<<endl<<"resolution y45: "<<yres_05<<" +- " <<err_yres_05<<endl<<endl;

  //y06

  TF1 *doublegaus_y06 = new TF1("doublegaus_y06",doublegausf,-.1,.1,6);
  doublegaus_y06->SetParameters(200,y06->GetMean(),y06->GetRMS(),50,y06->GetMean(),y06->GetRMS());
  y06->Fit("doublegaus_y06","R");

  TF1 *y06_f1 = new TF1("y06_f1","gaus",-0.1,0.1);
  TF1 *y06_f2 = new TF1("y06_f2","gaus",-0.1,0.1);

  y06_f1->SetParameter(0,doublegaus_y06->GetParameter(0));
  y06_f1->SetParameter(1,doublegaus_y06->GetParameter(1));
  y06_f1->SetParameter(2,doublegaus_y06->GetParameter(2));
  y06_f1->SetParError(2,doublegaus_y06->GetParError(2));
  y06_f1->SetLineColor(kRed);
  y06_f2->SetParameter(0,doublegaus_y06->GetParameter(3));
  y06_f2->SetParameter(1,doublegaus_y06->GetParameter(4));
  y06_f2->SetParameter(2,doublegaus_y06->GetParameter(5));
  y06_f2->SetParError(2,doublegaus_y06->GetParError(5));
  y06_f2->SetLineColor(kBlue);
  
  Double_t inty06_1 = y06_f1->Integral(-.1,.1);
  Double_t inty06_2 = y06_f2->Integral(-.1,.1);
  
  yres_06 = (inty06_1*fabs(y06_f1->GetParameter(2))+ inty06_2*fabs(y06_f2->GetParameter(2)))/(inty06_1 + inty06_2);

  err_yres_06  = (1./(inty06_1+inty06_2));
  err_yres_06 *= sqrt(inty06_1*inty06_1*pow((y06_f1->GetParError(2)),2)+inty06_2*inty06_2*pow(y06_f2->GetParError(2),2));

  cout<<endl<<"resolution y56: "<<yres_06<<" +- " <<err_yres_06<<endl<<endl;

  //y07

  TF1 *doublegaus_y07 = new TF1("doublegaus_y07",doublegausf,-.1,.1,6);
  doublegaus_y07->SetParameters(200,y07->GetMean(),y07->GetRMS(),50,y07->GetMean(),y07->GetRMS());
  y07->Fit("doublegaus_y07","R");

  TF1 *y07_f1 = new TF1("y07_f1","gaus",-0.1,0.1);
  TF1 *y07_f2 = new TF1("y07_f2","gaus",-0.1,0.1);

  y07_f1->SetParameter(0,doublegaus_y07->GetParameter(0));
  y07_f1->SetParameter(1,doublegaus_y07->GetParameter(1));
  y07_f1->SetParameter(2,doublegaus_y07->GetParameter(2));
  y07_f1->SetParError(2,doublegaus_y07->GetParError(2));
  y07_f1->SetLineColor(kRed);
  y07_f2->SetParameter(0,doublegaus_y07->GetParameter(3));
  y07_f2->SetParameter(1,doublegaus_y07->GetParameter(4));
  y07_f2->SetParameter(2,doublegaus_y07->GetParameter(5));
  y07_f2->SetParError(2,doublegaus_y07->GetParError(5));
  y07_f2->SetLineColor(kBlue);
  
  Double_t inty07_1 = y07_f1->Integral(-.1,.1);
  Double_t inty07_2 = y07_f2->Integral(-.1,.1);
  
  yres_07 = (inty07_1*fabs(y07_f1->GetParameter(2))+ inty07_2*fabs(y07_f2->GetParameter(2)))/(inty07_1 + inty07_2);

  err_yres_07  = (1./(inty07_1+inty07_2));
  err_yres_07 *= sqrt(inty07_1*inty07_1*pow((y07_f1->GetParError(2)),2)+inty07_2*inty07_2*pow(y07_f2->GetParError(2),2));

  cout<<endl<<"resolution y67: "<<yres_07<<" +- " <<err_yres_07<<endl<<endl;

  //y08

  TF1 *doublegaus_y08 = new TF1("doublegaus_y08",doublegausf,-.1,.1,6);
  doublegaus_y08->SetParameters(200,y08->GetMean(),y08->GetRMS(),50,y08->GetMean(),y08->GetRMS());
  y08->Fit("doublegaus_y08","R");

  TF1 *y08_f1 = new TF1("y08_f1","gaus",-0.1,0.1);
  TF1 *y08_f2 = new TF1("y08_f2","gaus",-0.1,0.1);

  y08_f1->SetParameter(0,doublegaus_y08->GetParameter(0));
  y08_f1->SetParameter(1,doublegaus_y08->GetParameter(1));
  y08_f1->SetParameter(2,doublegaus_y08->GetParameter(2));
  y08_f1->SetParError(2,doublegaus_y08->GetParError(2));
  y08_f1->SetLineColor(kRed);
  y08_f2->SetParameter(0,doublegaus_y08->GetParameter(3));
  y08_f2->SetParameter(1,doublegaus_y08->GetParameter(4));
  y08_f2->SetParameter(2,doublegaus_y08->GetParameter(5));
  y08_f2->SetParError(2,doublegaus_y08->GetParError(5));
  y08_f2->SetLineColor(kBlue);
  
  Double_t inty08_1 = y08_f1->Integral(-.1,.1);
  Double_t inty08_2 = y08_f2->Integral(-.1,.1);
  
  yres_08 = (inty08_1*fabs(y08_f1->GetParameter(2))+ inty08_2*fabs(y08_f2->GetParameter(2)))/(inty08_1 + inty08_2);

  err_yres_08  = (1./(inty08_1+inty08_2));
  err_yres_08 *= sqrt(inty08_1*inty08_1*pow((y08_f1->GetParError(2)),2)+inty08_2*inty08_2*pow(y08_f2->GetParError(2),2));

  cout<<endl<<"resolution y78: "<<yres_08<<" +- " <<err_yres_08<<endl<<endl;

  //z without
  //zall


  TF1 *doublegaus_zall = new TF1("doublegaus_zall",doublegausf,-.1,.1,6);
  doublegaus_zall->SetParameters(200,zall->GetMean(),zall->GetRMS(),50,zall->GetMean(),zall->GetRMS());
  zall->Fit("doublegaus_zall","R");

  TF1 *zall_f1 = new TF1("zall_f1","gaus",-0.1,0.1);
  TF1 *zall_f2 = new TF1("zall_f2","gaus",-0.1,0.1);

  zall_f1->SetParameter(0,doublegaus_zall->GetParameter(0));
  zall_f1->SetParameter(1,doublegaus_zall->GetParameter(1));
  zall_f1->SetParameter(2,doublegaus_zall->GetParameter(2));
  zall_f1->SetParError(2,doublegaus_zall->GetParError(2));
  zall_f1->SetLineColor(kRed);
  zall_f2->SetParameter(0,doublegaus_zall->GetParameter(3));
  zall_f2->SetParameter(1,doublegaus_zall->GetParameter(4));
  zall_f2->SetParameter(2,doublegaus_zall->GetParameter(5));
  zall_f2->SetParError(2,doublegaus_zall->GetParError(5));
  zall_f2->SetLineColor(kBlue);
  
  Double_t intzall_1 = zall_f1->Integral(-.1,.1);
  Double_t intzall_2 = zall_f2->Integral(-.1,.1);
  
  zres_all = (intzall_1*fabs(zall_f1->GetParameter(2))+ intzall_2*fabs(zall_f2->GetParameter(2)))/(intzall_1 + intzall_2);

  err_zres_all  = (1./(intzall_1+intzall_2));
  err_zres_all *= sqrt(intzall_1*intzall_1*pow((zall_f1->GetParError(2)),2)+intzall_2*intzall_2*pow(zall_f2->GetParError(2),2));

  cout<<endl<<"Overall resolution z without: "<<zres_all<<" +- " <<err_zres_all<<endl<<endl;


  //z01

  TF1 *doublegaus_z01 = new TF1("doublegaus_z01",doublegausf,-.1,.1,6);
  doublegaus_z01->SetParameters(200,z01->GetMean(),z01->GetRMS(),50,z01->GetMean(),z01->GetRMS());
  z01->Fit("doublegaus_z01","R");

  TF1 *z01_f1 = new TF1("z01_f1","gaus",-0.1,0.1);
  TF1 *z01_f2 = new TF1("z01_f2","gaus",-0.1,0.1);

  z01_f1->SetParameter(0,doublegaus_z01->GetParameter(0));
  z01_f1->SetParameter(1,doublegaus_z01->GetParameter(1));
  z01_f1->SetParameter(2,doublegaus_z01->GetParameter(2));
  z01_f1->SetParError(2,doublegaus_z01->GetParError(2));
  z01_f1->SetLineColor(kRed);
  z01_f2->SetParameter(0,doublegaus_z01->GetParameter(3));
  z01_f2->SetParameter(1,doublegaus_z01->GetParameter(4));
  z01_f2->SetParameter(2,doublegaus_z01->GetParameter(5));
  z01_f2->SetParError(2,doublegaus_z01->GetParError(5));
  z01_f2->SetLineColor(kBlue);
  
  Double_t intz01_1 = z01_f1->Integral(-.1,.1);
  Double_t intz01_2 = z01_f2->Integral(-.1,.1);
  
  zres_01 = (intz01_1*fabs(z01_f1->GetParameter(2))+ intz01_2*fabs(z01_f2->GetParameter(2)))/(intz01_1 + intz01_2);

  err_zres_01  = (1./(intz01_1+intz01_2));
  err_zres_01 *= sqrt(intz01_1*intz01_1*pow((z01_f1->GetParError(2)),2)+intz01_2*intz01_2*pow(z01_f2->GetParError(2),2));

  cout<<endl<<"resolution z01: "<<zres_01<<" +- " <<err_zres_01<<endl<<endl;


  //z02

  TF1 *doublegaus_z02 = new TF1("doublegaus_z02",doublegausf,-.1,.1,6);
  doublegaus_z02->SetParameters(200,z02->GetMean(),z02->GetRMS(),50,z02->GetMean(),z02->GetRMS());
  z02->Fit("doublegaus_z02","R");

  TF1 *z02_f1 = new TF1("z02_f1","gaus",-0.1,0.1);
  TF1 *z02_f2 = new TF1("z02_f2","gaus",-0.1,0.1);

  z02_f1->SetParameter(0,doublegaus_z02->GetParameter(0));
  z02_f1->SetParameter(1,doublegaus_z02->GetParameter(1));
  z02_f1->SetParameter(2,doublegaus_z02->GetParameter(2));
  z02_f1->SetParError(2,doublegaus_z02->GetParError(2));
  z02_f1->SetLineColor(kRed);
  z02_f2->SetParameter(0,doublegaus_z02->GetParameter(3));
  z02_f2->SetParameter(1,doublegaus_z02->GetParameter(4));
  z02_f2->SetParameter(2,doublegaus_z02->GetParameter(5));
  z02_f2->SetParError(2,doublegaus_z02->GetParError(5));
  z02_f2->SetLineColor(kBlue);
  
  Double_t intz02_1 = z02_f1->Integral(-.1,.1);
  Double_t intz02_2 = z02_f2->Integral(-.1,.1);
  
  zres_all = (intz02_1*fabs(z02_f1->GetParameter(2))+ intz02_2*fabs(z02_f2->GetParameter(2)))/(intz02_1 + intz02_2);

  err_zres_all  = (1./(intz02_1+intz02_2));
  err_zres_all *= sqrt(intz02_1*intz02_1*pow((z02_f1->GetParError(2)),2)+intz02_2*intz02_2*pow(z02_f2->GetParError(2),2));

  cout<<endl<<"resolution z12: "<<zres_02<<" +- " <<err_zres_02<<endl<<endl;

  //z03

  TF1 *doublegaus_z03 = new TF1("doublegaus_z03",doublegausf,-.1,.1,6);
  doublegaus_z03->SetParameters(200,z03->GetMean(),z03->GetRMS(),50,z03->GetMean(),z03->GetRMS());
  z03->Fit("doublegaus_z03","R");

  TF1 *z03_f1 = new TF1("z03_f1","gaus",-0.1,0.1);
  TF1 *z03_f2 = new TF1("z03_f2","gaus",-0.1,0.1);

  z03_f1->SetParameter(0,doublegaus_z03->GetParameter(0));
  z03_f1->SetParameter(1,doublegaus_z03->GetParameter(1));
  z03_f1->SetParameter(2,doublegaus_z03->GetParameter(2));
  z03_f1->SetParError(2,doublegaus_z03->GetParError(2));
  z03_f1->SetLineColor(kRed);
  z03_f2->SetParameter(0,doublegaus_z03->GetParameter(3));
  z03_f2->SetParameter(1,doublegaus_z03->GetParameter(4));
  z03_f2->SetParameter(2,doublegaus_z03->GetParameter(5));
  z03_f2->SetParError(2,doublegaus_z03->GetParError(5));
  z03_f2->SetLineColor(kBlue);
  
  Double_t intz03_1 = z03_f1->Integral(-.1,.1);
  Double_t intz03_2 = z03_f2->Integral(-.1,.1);
  
  zres_03 = (intz03_1*fabs(z03_f1->GetParameter(2))+ intz03_2*fabs(z03_f2->GetParameter(2)))/(intz03_1 + intz03_2);

  err_zres_03  = (1./(intz03_1+intz03_2));
  err_zres_03 *= sqrt(intz03_1*intz03_1*pow((z03_f1->GetParError(2)),2)+intz03_2*intz03_2*pow(z03_f2->GetParError(2),2));

  cout<<endl<<"resolution z23: "<<zres_03<<" +- " <<err_zres_03<<endl<<endl;

  //z04

  TF1 *doublegaus_z04 = new TF1("doublegaus_z04",doublegausf,-.1,.1,6);
  doublegaus_z04->SetParameters(200,z04->GetMean(),z04->GetRMS(),50,z04->GetMean(),z04->GetRMS());
  z04->Fit("doublegaus_z04","R");

  TF1 *z04_f1 = new TF1("z04_f1","gaus",-0.1,0.1);
  TF1 *z04_f2 = new TF1("z04_f2","gaus",-0.1,0.1);

  z04_f1->SetParameter(0,doublegaus_z04->GetParameter(0));
  z04_f1->SetParameter(1,doublegaus_z04->GetParameter(1));
  z04_f1->SetParameter(2,doublegaus_z04->GetParameter(2));
  z04_f1->SetParError(2,doublegaus_z04->GetParError(2));
  z04_f1->SetLineColor(kRed);
  z04_f2->SetParameter(0,doublegaus_z04->GetParameter(3));
  z04_f2->SetParameter(1,doublegaus_z04->GetParameter(4));
  z04_f2->SetParameter(2,doublegaus_z04->GetParameter(5));
  z04_f2->SetParError(2,doublegaus_z04->GetParError(5));
  z04_f2->SetLineColor(kBlue);
  
  Double_t intz04_1 = z04_f1->Integral(-.1,.1);
  Double_t intz04_2 = z04_f2->Integral(-.1,.1);
  
  zres_04 = (intz04_1*fabs(z04_f1->GetParameter(2))+ intz04_2*fabs(z04_f2->GetParameter(2)))/(intz04_1 + intz04_2);

  err_zres_04  = (1./(intz04_1+intz04_2));
  err_zres_04 *= sqrt(intz04_1*intz04_1*pow((z04_f1->GetParError(2)),2)+intz04_2*intz04_2*pow(z04_f2->GetParError(2),2));

  cout<<endl<<"resolution z34: "<<zres_04<<" +- " <<err_zres_04<<endl<<endl;

  //z05

  TF1 *doublegaus_z05 = new TF1("doublegaus_z05",doublegausf,-.1,.1,6);
  doublegaus_z05->SetParameters(200,z05->GetMean(),z05->GetRMS(),50,z05->GetMean(),z05->GetRMS());
  z05->Fit("doublegaus_z05","R");

  TF1 *z05_f1 = new TF1("z05_f1","gaus",-0.1,0.1);
  TF1 *z05_f2 = new TF1("z05_f2","gaus",-0.1,0.1);

  z05_f1->SetParameter(0,doublegaus_z05->GetParameter(0));
  z05_f1->SetParameter(1,doublegaus_z05->GetParameter(1));
  z05_f1->SetParameter(2,doublegaus_z05->GetParameter(2));
  z05_f1->SetParError(2,doublegaus_z05->GetParError(2));
  z05_f1->SetLineColor(kRed);
  z05_f2->SetParameter(0,doublegaus_z05->GetParameter(3));
  z05_f2->SetParameter(1,doublegaus_z05->GetParameter(4));
  z05_f2->SetParameter(2,doublegaus_z05->GetParameter(5));
  z05_f2->SetParError(2,doublegaus_z05->GetParError(5));
  z05_f2->SetLineColor(kBlue);
  
  Double_t intz05_1 = z05_f1->Integral(-.1,.1);
  Double_t intz05_2 = z05_f2->Integral(-.1,.1);
  
  zres_all = (intz05_1*fabs(z05_f1->GetParameter(2))+ intz05_2*fabs(z05_f2->GetParameter(2)))/(intz05_1 + intz05_2);

  err_zres_all  = (1./(intz05_1+intz05_2));
  err_zres_all *= sqrt(intz05_1*intz05_1*pow((z05_f1->GetParError(2)),2)+intz05_2*intz05_2*pow(z05_f2->GetParError(2),2));

  cout<<endl<<"resolution z45: "<<zres_05<<" +- " <<err_zres_05<<endl<<endl;

  //z06

  TF1 *doublegaus_z06 = new TF1("doublegaus_z06",doublegausf,-.1,.1,6);
  doublegaus_z06->SetParameters(200,z06->GetMean(),z06->GetRMS(),50,z06->GetMean(),z06->GetRMS());
  z06->Fit("doublegaus_z06","R");

  TF1 *z06_f1 = new TF1("z06_f1","gaus",-0.1,0.1);
  TF1 *z06_f2 = new TF1("z06_f2","gaus",-0.1,0.1);

  z06_f1->SetParameter(0,doublegaus_z06->GetParameter(0));
  z06_f1->SetParameter(1,doublegaus_z06->GetParameter(1));
  z06_f1->SetParameter(2,doublegaus_z06->GetParameter(2));
  z06_f1->SetParError(2,doublegaus_z06->GetParError(2));
  z06_f1->SetLineColor(kRed);
  z06_f2->SetParameter(0,doublegaus_z06->GetParameter(3));
  z06_f2->SetParameter(1,doublegaus_z06->GetParameter(4));
  z06_f2->SetParameter(2,doublegaus_z06->GetParameter(5));
  z06_f2->SetParError(2,doublegaus_z06->GetParError(5));
  z06_f2->SetLineColor(kBlue);
  
  Double_t intz06_1 = z06_f1->Integral(-.1,.1);
  Double_t intz06_2 = z06_f2->Integral(-.1,.1);
  
  zres_06 = (intz06_1*fabs(z06_f1->GetParameter(2))+ intz06_2*fabs(z06_f2->GetParameter(2)))/(intz06_1 + intz06_2);

  err_zres_06  = (1./(intz06_1+intz06_2));
  err_zres_06 *= sqrt(intz06_1*intz06_1*pow((z06_f1->GetParError(2)),2)+intz06_2*intz06_2*pow(z06_f2->GetParError(2),2));

  cout<<endl<<"resolution z56: "<<zres_06<<" +- " <<err_zres_06<<endl<<endl;

  //z07

  TF1 *doublegaus_z07 = new TF1("doublegaus_z07",doublegausf,-.1,.1,6);
  doublegaus_z07->SetParameters(200,z07->GetMean(),z07->GetRMS(),50,z07->GetMean(),z07->GetRMS());
  z07->Fit("doublegaus_z07","R");

  TF1 *z07_f1 = new TF1("z07_f1","gaus",-0.1,0.1);
  TF1 *z07_f2 = new TF1("z07_f2","gaus",-0.1,0.1);

  z07_f1->SetParameter(0,doublegaus_z07->GetParameter(0));
  z07_f1->SetParameter(1,doublegaus_z07->GetParameter(1));
  z07_f1->SetParameter(2,doublegaus_z07->GetParameter(2));
  z07_f1->SetParError(2,doublegaus_z07->GetParError(2));
  z07_f1->SetLineColor(kRed);
  z07_f2->SetParameter(0,doublegaus_z07->GetParameter(3));
  z07_f2->SetParameter(1,doublegaus_z07->GetParameter(4));
  z07_f2->SetParameter(2,doublegaus_z07->GetParameter(5));
  z07_f2->SetParError(2,doublegaus_z07->GetParError(5));
  z07_f2->SetLineColor(kBlue);
  
  Double_t intz07_1 = z07_f1->Integral(-.1,.1);
  Double_t intz07_2 = z07_f2->Integral(-.1,.1);
  
  zres_07 = (intz07_1*fabs(z07_f1->GetParameter(2))+ intz07_2*fabs(z07_f2->GetParameter(2)))/(intz07_1 + intz07_2);

  err_zres_07  = (1./(intz07_1+intz07_2));
  err_zres_07 *= sqrt(intz07_1*intz07_1*pow((z07_f1->GetParError(2)),2)+intz07_2*intz07_2*pow(z07_f2->GetParError(2),2));

  cout<<endl<<"resolution z67: "<<zres_07<<" +- " <<err_zres_07<<endl<<endl;

  //z08

  TF1 *doublegaus_z08 = new TF1("doublegaus_z08",doublegausf,-.1,.1,6);
  doublegaus_z08->SetParameters(200,z08->GetMean(),z08->GetRMS(),50,z08->GetMean(),z08->GetRMS());
  z08->Fit("doublegaus_z08","R");

  TF1 *z08_f1 = new TF1("z08_f1","gaus",-0.1,0.1);
  TF1 *z08_f2 = new TF1("z08_f2","gaus",-0.1,0.1);

  z08_f1->SetParameter(0,doublegaus_z08->GetParameter(0));
  z08_f1->SetParameter(1,doublegaus_z08->GetParameter(1));
  z08_f1->SetParameter(2,doublegaus_z08->GetParameter(2));
  z08_f1->SetParError(2,doublegaus_z08->GetParError(2));
  z08_f1->SetLineColor(kRed);
  z08_f2->SetParameter(0,doublegaus_z08->GetParameter(3));
  z08_f2->SetParameter(1,doublegaus_z08->GetParameter(4));
  z08_f2->SetParameter(2,doublegaus_z08->GetParameter(5));
  z08_f2->SetParError(2,doublegaus_z08->GetParError(5));
  z08_f2->SetLineColor(kBlue);
  
  Double_t intz08_1 = z08_f1->Integral(-.1,.1);
  Double_t intz08_2 = z08_f2->Integral(-.1,.1);
  
  zres_08 = (intz08_1*fabs(z08_f1->GetParameter(2))+ intz08_2*fabs(z08_f2->GetParameter(2)))/(intz08_1 + intz08_2);

  err_zres_08  = (1./(intz08_1+intz08_2));
  err_zres_08 *= sqrt(intz08_1*intz08_1*pow((z08_f1->GetParError(2)),2)+intz08_2*intz08_2*pow(z08_f2->GetParError(2),2));

  cout<<endl<<"resolution z78: "<<zres_08<<" +- " <<err_zres_08<<endl<<endl;

  //y using clusterSplit1.C
  //yallw

  TF1 *doublegaus_yallw = new TF1("doublegaus_yallw",doublegausf,-.1,.1,6);
  doublegaus_yallw->SetParameters(200,yallw->GetMean(),yallw->GetRMS(),50,yallw->GetMean(),yallw->GetRMS());
  yallw->Fit("doublegaus_yallw","R");

  TF1 *yallw_f1 = new TF1("yallw_f1","gaus",-0.1,0.1);
  TF1 *yallw_f2 = new TF1("yallw_f2","gaus",-0.1,0.1);

  yallw_f1->SetParameter(0,doublegaus_yallw->GetParameter(0));
  yallw_f1->SetParameter(1,doublegaus_yallw->GetParameter(1));
  yallw_f1->SetParameter(2,doublegaus_yallw->GetParameter(2));
  yallw_f1->SetParError(2,doublegaus_yallw->GetParError(2));
  yallw_f1->SetLineColor(kRed);
  yallw_f2->SetParameter(0,doublegaus_yallw->GetParameter(3));
  yallw_f2->SetParameter(1,doublegaus_yallw->GetParameter(4));
  yallw_f2->SetParameter(2,doublegaus_yallw->GetParameter(5));
  yallw_f2->SetParError(2,doublegaus_yallw->GetParError(5));
  yallw_f2->SetLineColor(kBlue);
  
  Double_t intyallw_1 = yallw_f1->Integral(-.1,.1);
  Double_t intyallw_2 = yallw_f2->Integral(-.1,.1);
  
  yres_allw = (intyallw_1*fabs(yallw_f1->GetParameter(2))+ intyallw_2*fabs(yallw_f2->GetParameter(2)))/(intyallw_1 + intyallw_2);

  err_yres_allw  = (1./(intyallw_1+intyallw_2));
  err_yres_allw *= sqrt(intyallw_1*intyallw_1*pow((yallw_f1->GetParError(2)),2)+intyallw_2*intyallw_2*pow(yallw_f2->GetParError(2),2));

  cout<<endl<<"Overall resolution y using clusterSplit1.C: "<<yres_allw<<" +- " <<err_yres_allw<<endl<<endl;

  //y01w

  TF1 *doublegaus_y01w = new TF1("doublegaus_y01w",doublegausf,-.1,.1,6);
  doublegaus_y01w->SetParameters(200,y01w->GetMean(),y01w->GetRMS(),50,y01w->GetMean(),y01w->GetRMS());
  y01w->Fit("doublegaus_y01w","R");

  TF1 *y01w_f1 = new TF1("y01w_f1","gaus",-0.1,0.1);
  TF1 *y01w_f2 = new TF1("y01w_f2","gaus",-0.1,0.1);

  y01w_f1->SetParameter(0,doublegaus_y01w->GetParameter(0));
  y01w_f1->SetParameter(1,doublegaus_y01w->GetParameter(1));
  y01w_f1->SetParameter(2,doublegaus_y01w->GetParameter(2));
  y01w_f1->SetParError(2,doublegaus_y01w->GetParError(2));
  y01w_f1->SetLineColor(kRed);
  y01w_f2->SetParameter(0,doublegaus_y01w->GetParameter(3));
  y01w_f2->SetParameter(1,doublegaus_y01w->GetParameter(4));
  y01w_f2->SetParameter(2,doublegaus_y01w->GetParameter(5));
  y01w_f2->SetParError(2,doublegaus_y01w->GetParError(5));
  y01w_f2->SetLineColor(kBlue);
  
  Double_t inty01w_1 = y01w_f1->Integral(-.1,.1);
  Double_t inty01w_2 = y01w_f2->Integral(-.1,.1);
  
  yres_01w = (inty01w_1*fabs(y01w_f1->GetParameter(2))+ inty01w_2*fabs(y01w_f2->GetParameter(2)))/(inty01w_1 + inty01w_2);

  err_yres_01w  = (1./(inty01w_1+inty01w_2));
  err_yres_01w *= sqrt(inty01w_1*inty01w_1*pow((y01w_f1->GetParError(2)),2)+inty01w_2*inty01w_2*pow(y01w_f2->GetParError(2),2));

  cout<<endl<<"resolution yw01: "<<yres_01w<<" +- " <<err_yres_01w<<endl<<endl;

  //y02w

  TF1 *doublegaus_y02w = new TF1("doublegaus_y02w",doublegausf,-.1,.1,6);
  doublegaus_y02w->SetParameters(200,y02w->GetMean(),y02w->GetRMS(),50,y02w->GetMean(),y02w->GetRMS());
  y02w->Fit("doublegaus_y02w","R");

  TF1 *y02w_f1 = new TF1("y02w_f1","gaus",-0.1,0.1);
  TF1 *y02w_f2 = new TF1("y02w_f2","gaus",-0.1,0.1);

  y02w_f1->SetParameter(0,doublegaus_y02w->GetParameter(0));
  y02w_f1->SetParameter(1,doublegaus_y02w->GetParameter(1));
  y02w_f1->SetParameter(2,doublegaus_y02w->GetParameter(2));
  y02w_f1->SetParError(2,doublegaus_y02w->GetParError(2));
  y02w_f1->SetLineColor(kRed);
  y02w_f2->SetParameter(0,doublegaus_y02w->GetParameter(3));
  y02w_f2->SetParameter(1,doublegaus_y02w->GetParameter(4));
  y02w_f2->SetParameter(2,doublegaus_y02w->GetParameter(5));
  y02w_f2->SetParError(2,doublegaus_y02w->GetParError(5));
  y02w_f2->SetLineColor(kBlue);
  
  Double_t inty02w_1 = y02w_f1->Integral(-.1,.1);
  Double_t inty02w_2 = y02w_f2->Integral(-.1,.1);
  
  yres_02w = (inty02w_1*fabs(y02w_f1->GetParameter(2))+ inty02w_2*fabs(y02w_f2->GetParameter(2)))/(inty02w_1 + inty02w_2);

  err_yres_02w  = (1./(inty02w_1+inty02w_2));
  err_yres_02w *= sqrt(inty02w_1*inty02w_1*pow((y02w_f1->GetParError(2)),2)+inty02w_2*inty02w_2*pow(y02w_f2->GetParError(2),2));

  cout<<endl<<"resolution yw12: "<<yres_02w<<" +- " <<err_yres_02w<<endl<<endl;

  //y03w

  TF1 *doublegaus_y03w = new TF1("doublegaus_y03w",doublegausf,-.1,.1,6);
  doublegaus_y03w->SetParameters(200,y03w->GetMean(),y03w->GetRMS(),50,y03w->GetMean(),y03w->GetRMS());
  y03w->Fit("doublegaus_y03w","R");

  TF1 *y03w_f1 = new TF1("y03w_f1","gaus",-0.1,0.1);
  TF1 *y03w_f2 = new TF1("y03w_f2","gaus",-0.1,0.1);

  y03w_f1->SetParameter(0,doublegaus_y03w->GetParameter(0));
  y03w_f1->SetParameter(1,doublegaus_y03w->GetParameter(1));
  y03w_f1->SetParameter(2,doublegaus_y03w->GetParameter(2));
  y03w_f1->SetParError(2,doublegaus_y03w->GetParError(2));
  y03w_f1->SetLineColor(kRed);
  y03w_f2->SetParameter(0,doublegaus_y03w->GetParameter(3));
  y03w_f2->SetParameter(1,doublegaus_y03w->GetParameter(4));
  y03w_f2->SetParameter(2,doublegaus_y03w->GetParameter(5));
  y03w_f2->SetParError(2,doublegaus_y03w->GetParError(5));
  y03w_f2->SetLineColor(kBlue);
  
  Double_t inty03w_1 = y03w_f1->Integral(-.1,.1);
  Double_t inty03w_2 = y03w_f2->Integral(-.1,.1);
  
  yres_03w = (inty03w_1*fabs(y03w_f1->GetParameter(2))+ inty03w_2*fabs(y03w_f2->GetParameter(2)))/(inty03w_1 + inty03w_2);

  err_yres_03w  = (1./(inty03w_1+inty03w_2));
  err_yres_03w *= sqrt(inty03w_1*inty03w_1*pow((y03w_f1->GetParError(2)),2)+inty03w_2*inty03w_2*pow(y03w_f2->GetParError(2),2));

  cout<<endl<<"resolution yw23: "<<yres_03w<<" +- " <<err_yres_03w<<endl<<endl;

  //y04w

  TF1 *doublegaus_y04w = new TF1("doublegaus_y04w",doublegausf,-.1,.1,6);
  doublegaus_y04w->SetParameters(200,y04w->GetMean(),y04w->GetRMS(),50,y04w->GetMean(),y04w->GetRMS());
  y04w->Fit("doublegaus_y04w","R");

  TF1 *y04w_f1 = new TF1("y04w_f1","gaus",-0.1,0.1);
  TF1 *y04w_f2 = new TF1("y04w_f2","gaus",-0.1,0.1);

  y04w_f1->SetParameter(0,doublegaus_y04w->GetParameter(0));
  y04w_f1->SetParameter(1,doublegaus_y04w->GetParameter(1));
  y04w_f1->SetParameter(2,doublegaus_y04w->GetParameter(2));
  y04w_f1->SetParError(2,doublegaus_y04w->GetParError(2));
  y04w_f1->SetLineColor(kRed);
  y04w_f2->SetParameter(0,doublegaus_y04w->GetParameter(3));
  y04w_f2->SetParameter(1,doublegaus_y04w->GetParameter(4));
  y04w_f2->SetParameter(2,doublegaus_y04w->GetParameter(5));
  y04w_f2->SetParError(2,doublegaus_y04w->GetParError(5));
  y04w_f2->SetLineColor(kBlue);
  
  Double_t inty04w_1 = y04w_f1->Integral(-.1,.1);
  Double_t inty04w_2 = y04w_f2->Integral(-.1,.1);
  
  yres_04w = (inty04w_1*fabs(y04w_f1->GetParameter(2))+ inty04w_2*fabs(y04w_f2->GetParameter(2)))/(inty04w_1 + inty04w_2);

  err_yres_04w  = (1./(inty04w_1+inty04w_2));
  err_yres_04w *= sqrt(inty04w_1*inty04w_1*pow((y04w_f1->GetParError(2)),2)+inty04w_2*inty04w_2*pow(y04w_f2->GetParError(2),2));

  cout<<endl<<"resolution yw34: "<<yres_04w<<" +- " <<err_yres_04w<<endl<<endl;

  //y05w

  TF1 *doublegaus_y05w = new TF1("doublegaus_y05w",doublegausf,-.1,.1,6);
  doublegaus_y05w->SetParameters(200,y05w->GetMean(),y05w->GetRMS(),50,y05w->GetMean(),y05w->GetRMS());
  y05w->Fit("doublegaus_y05w","R");

  TF1 *y05w_f1 = new TF1("y05w_f1","gaus",-0.1,0.1);
  TF1 *y05w_f2 = new TF1("y05w_f2","gaus",-0.1,0.1);

  y05w_f1->SetParameter(0,doublegaus_y05w->GetParameter(0));
  y05w_f1->SetParameter(1,doublegaus_y05w->GetParameter(1));
  y05w_f1->SetParameter(2,doublegaus_y05w->GetParameter(2));
  y05w_f1->SetParError(2,doublegaus_y05w->GetParError(2));
  y05w_f1->SetLineColor(kRed);
  y05w_f2->SetParameter(0,doublegaus_y05w->GetParameter(3));
  y05w_f2->SetParameter(1,doublegaus_y05w->GetParameter(4));
  y05w_f2->SetParameter(2,doublegaus_y05w->GetParameter(5));
  y05w_f2->SetParError(2,doublegaus_y05w->GetParError(5));
  y05w_f2->SetLineColor(kBlue);
  
  Double_t inty05w_1 = y05w_f1->Integral(-.1,.1);
  Double_t inty05w_2 = y05w_f2->Integral(-.1,.1);
  
  yres_05w = (inty05w_1*fabs(y05w_f1->GetParameter(2))+ inty05w_2*fabs(y05w_f2->GetParameter(2)))/(inty05w_1 + inty05w_2);

  err_yres_05w  = (1./(inty05w_1+inty05w_2));
  err_yres_05w *= sqrt(inty05w_1*inty05w_1*pow((y05w_f1->GetParError(2)),2)+inty05w_2*inty05w_2*pow(y05w_f2->GetParError(2),2));

  cout<<endl<<"resolution yw45: "<<yres_05w<<" +- " <<err_yres_05w<<endl<<endl;

  //y06w

  TF1 *doublegaus_y06w = new TF1("doublegaus_y06w",doublegausf,-.1,.1,6);
  doublegaus_y06w->SetParameters(200,y06w->GetMean(),y06w->GetRMS(),50,y06w->GetMean(),y06w->GetRMS());
  y06w->Fit("doublegaus_y06w","R");

  TF1 *y06w_f1 = new TF1("y06w_f1","gaus",-0.1,0.1);
  TF1 *y06w_f2 = new TF1("y06w_f2","gaus",-0.1,0.1);

  y06w_f1->SetParameter(0,doublegaus_y06w->GetParameter(0));
  y06w_f1->SetParameter(1,doublegaus_y06w->GetParameter(1));
  y06w_f1->SetParameter(2,doublegaus_y06w->GetParameter(2));
  y06w_f1->SetParError(2,doublegaus_y06w->GetParError(2));
  y06w_f1->SetLineColor(kRed);
  y06w_f2->SetParameter(0,doublegaus_y06w->GetParameter(3));
  y06w_f2->SetParameter(1,doublegaus_y06w->GetParameter(4));
  y06w_f2->SetParameter(2,doublegaus_y06w->GetParameter(5));
  y06w_f2->SetParError(2,doublegaus_y06w->GetParError(5));
  y06w_f2->SetLineColor(kBlue);
  
  Double_t inty06w_1 = y06w_f1->Integral(-.1,.1);
  Double_t inty06w_2 = y06w_f2->Integral(-.1,.1);
  
  yres_06w = (inty06w_1*fabs(y06w_f1->GetParameter(2))+ inty06w_2*fabs(y06w_f2->GetParameter(2)))/(inty06w_1 + inty06w_2);

  err_yres_06w  = (1./(inty06w_1+inty06w_2));
  err_yres_06w *= sqrt(inty06w_1*inty06w_1*pow((y06w_f1->GetParError(2)),2)+inty06w_2*inty06w_2*pow(y06w_f2->GetParError(2),2));

  cout<<endl<<"resolution yw56: "<<yres_06w<<" +- " <<err_yres_06w<<endl<<endl;

  //y07w

  TF1 *doublegaus_y07w = new TF1("doublegaus_y07w",doublegausf,-.1,.1,6);
  doublegaus_y07w->SetParameters(200,y07w->GetMean(),y07w->GetRMS(),50,y07w->GetMean(),y07w->GetRMS());
  y07w->Fit("doublegaus_y07w","R");

  TF1 *y07w_f1 = new TF1("y07w_f1","gaus",-0.1,0.1);
  TF1 *y07w_f2 = new TF1("y07w_f2","gaus",-0.1,0.1);

  y07w_f1->SetParameter(0,doublegaus_y07w->GetParameter(0));
  y07w_f1->SetParameter(1,doublegaus_y07w->GetParameter(1));
  y07w_f1->SetParameter(2,doublegaus_y07w->GetParameter(2));
  y07w_f1->SetParError(2,doublegaus_y07w->GetParError(2));
  y07w_f1->SetLineColor(kRed);
  y07w_f2->SetParameter(0,doublegaus_y07w->GetParameter(3));
  y07w_f2->SetParameter(1,doublegaus_y07w->GetParameter(4));
  y07w_f2->SetParameter(2,doublegaus_y07w->GetParameter(5));
  y07w_f2->SetParError(2,doublegaus_y07w->GetParError(5));
  y07w_f2->SetLineColor(kBlue);
  
  Double_t inty07w_1 = y07w_f1->Integral(-.1,.1);
  Double_t inty07w_2 = y07w_f2->Integral(-.1,.1);
  
  yres_07w = (inty07w_1*fabs(y07w_f1->GetParameter(2))+ inty07w_2*fabs(y07w_f2->GetParameter(2)))/(inty07w_1 + inty07w_2);

  err_yres_07w  = (1./(inty07w_1+inty07w_2));
  err_yres_07w *= sqrt(inty07w_1*inty07w_1*pow((y07w_f1->GetParError(2)),2)+inty07w_2*inty07w_2*pow(y07w_f2->GetParError(2),2));

  cout<<endl<<"resolution yw67: "<<yres_07w<<" +- " <<err_yres_07w<<endl<<endl;

  //y08w

  TF1 *doublegaus_y08w = new TF1("doublegaus_y08w",doublegausf,-.1,.1,6);
  doublegaus_y08w->SetParameters(200,y08w->GetMean(),y08w->GetRMS(),50,y08w->GetMean(),y08w->GetRMS());
  y08w->Fit("doublegaus_y08w","R");

  TF1 *y08w_f1 = new TF1("y08w_f1","gaus",-0.1,0.1);
  TF1 *y08w_f2 = new TF1("y08w_f2","gaus",-0.1,0.1);

  y08w_f1->SetParameter(0,doublegaus_y08w->GetParameter(0));
  y08w_f1->SetParameter(1,doublegaus_y08w->GetParameter(1));
  y08w_f1->SetParameter(2,doublegaus_y08w->GetParameter(2));
  y08w_f1->SetParError(2,doublegaus_y08w->GetParError(2));
  y08w_f1->SetLineColor(kRed);
  y08w_f2->SetParameter(0,doublegaus_y08w->GetParameter(3));
  y08w_f2->SetParameter(1,doublegaus_y08w->GetParameter(4));
  y08w_f2->SetParameter(2,doublegaus_y08w->GetParameter(5));
  y08w_f2->SetParError(2,doublegaus_y08w->GetParError(5));
  y08w_f2->SetLineColor(kBlue);
  
  Double_t inty08w_1 = y08w_f1->Integral(-.1,.1);
  Double_t inty08w_2 = y08w_f2->Integral(-.1,.1);
  
  yres_08w = (inty08w_1*fabs(y08w_f1->GetParameter(2))+ inty08w_2*fabs(y08w_f2->GetParameter(2)))/(inty08w_1 + inty08w_2);

  err_yres_08w  = (1./(inty08w_1+inty08w_2));
  err_yres_08w *= sqrt(inty08w_1*inty08w_1*pow((y08w_f1->GetParError(2)),2)+inty08w_2*inty08w_2*pow(y08w_f2->GetParError(2),2));

  cout<<endl<<"resolution yw78: "<<yres_08w<<" +- " <<err_yres_08w<<endl<<endl;

  //z using clusterSplit1.C
  //zallw

  TF1 *doublegaus_zallw = new TF1("doublegaus_zallw",doublegausf,-.1,.1,6);
  doublegaus_zallw->SetParameters(200,zallw->GetMean(),zallw->GetRMS(),50,zallw->GetMean(),zallw->GetRMS());
  zallw->Fit("doublegaus_zallw","R");

  TF1 *zallw_f1 = new TF1("zallw_f1","gaus",-0.1,0.1);
  TF1 *zallw_f2 = new TF1("zallw_f2","gaus",-0.1,0.1);

  zallw_f1->SetParameter(0,doublegaus_zallw->GetParameter(0));
  zallw_f1->SetParameter(1,doublegaus_zallw->GetParameter(1));
  zallw_f1->SetParameter(2,doublegaus_zallw->GetParameter(2));
  zallw_f1->SetParError(2,doublegaus_zallw->GetParError(2));
  zallw_f1->SetLineColor(kRed);
  zallw_f2->SetParameter(0,doublegaus_zallw->GetParameter(3));
  zallw_f2->SetParameter(1,doublegaus_zallw->GetParameter(4));
  zallw_f2->SetParameter(2,doublegaus_zallw->GetParameter(5));
  zallw_f2->SetParError(2,doublegaus_zallw->GetParError(5));
  zallw_f2->SetLineColor(kBlue);
  
  Double_t intzallw_1 = zallw_f1->Integral(-.1,.1);
  Double_t intzallw_2 = zallw_f2->Integral(-.1,.1);
  
  zres_allw = (intzallw_1*fabs(zallw_f1->GetParameter(2))+ intzallw_2*fabs(zallw_f2->GetParameter(2)))/(intzallw_1 + intzallw_2);

  err_zres_allw  = (1./(intzallw_1+intzallw_2));
  err_zres_allw *= sqrt(intzallw_1*intzallw_1*pow((zallw_f1->GetParError(2)),2)+intzallw_2*intzallw_2*pow(zallw_f2->GetParError(2),2));

  cout<<endl<<"Overall resolution z using clusterSplit1.C: "<<zres_allw<<" +- " <<err_zres_allw<<endl<<endl;

  //z01w

  TF1 *doublegaus_z01w = new TF1("doublegaus_z01w",doublegausf,-.1,.1,6);
  doublegaus_z01w->SetParameters(200,z01w->GetMean(),z01w->GetRMS(),50,z01w->GetMean(),z01w->GetRMS());
  z01w->Fit("doublegaus_z01w","R");

  TF1 *z01w_f1 = new TF1("z01w_f1","gaus",-0.1,0.1);
  TF1 *z01w_f2 = new TF1("z01w_f2","gaus",-0.1,0.1);

  z01w_f1->SetParameter(0,doublegaus_z01w->GetParameter(0));
  z01w_f1->SetParameter(1,doublegaus_z01w->GetParameter(1));
  z01w_f1->SetParameter(2,doublegaus_z01w->GetParameter(2));
  z01w_f1->SetParError(2,doublegaus_z01w->GetParError(2));
  z01w_f1->SetLineColor(kRed);
  z01w_f2->SetParameter(0,doublegaus_z01w->GetParameter(3));
  z01w_f2->SetParameter(1,doublegaus_z01w->GetParameter(4));
  z01w_f2->SetParameter(2,doublegaus_z01w->GetParameter(5));
  z01w_f2->SetParError(2,doublegaus_z01w->GetParError(5));
  z01w_f2->SetLineColor(kBlue);
  
  Double_t intz01w_1 = z01w_f1->Integral(-.1,.1);
  Double_t intz01w_2 = z01w_f2->Integral(-.1,.1);
  
  zres_01w = (intz01w_1*fabs(z01w_f1->GetParameter(2))+ intz01w_2*fabs(z01w_f2->GetParameter(2)))/(intz01w_1 + intz01w_2);

  err_zres_01w  = (1./(intz01w_1+intz01w_2));
  err_zres_01w *= sqrt(intz01w_1*intz01w_1*pow((z01w_f1->GetParError(2)),2)+intz01w_2*intz01w_2*pow(z01w_f2->GetParError(2),2));

  cout<<endl<<"resolution zw01: "<<zres_01w<<" +- " <<err_zres_01w<<endl<<endl;

  //z02w

  TF1 *doublegaus_z02w = new TF1("doublegaus_z02w",doublegausf,-.1,.1,6);
  doublegaus_z02w->SetParameters(200,z02w->GetMean(),z02w->GetRMS(),50,z02w->GetMean(),z02w->GetRMS());
  z02w->Fit("doublegaus_z02w","R");

  TF1 *z02w_f1 = new TF1("z02w_f1","gaus",-0.1,0.1);
  TF1 *z02w_f2 = new TF1("z02w_f2","gaus",-0.1,0.1);

  z02w_f1->SetParameter(0,doublegaus_z02w->GetParameter(0));
  z02w_f1->SetParameter(1,doublegaus_z02w->GetParameter(1));
  z02w_f1->SetParameter(2,doublegaus_z02w->GetParameter(2));
  z02w_f1->SetParError(2,doublegaus_z02w->GetParError(2));
  z02w_f1->SetLineColor(kRed);
  z02w_f2->SetParameter(0,doublegaus_z02w->GetParameter(3));
  z02w_f2->SetParameter(1,doublegaus_z02w->GetParameter(4));
  z02w_f2->SetParameter(2,doublegaus_z02w->GetParameter(5));
  z02w_f2->SetParError(2,doublegaus_z02w->GetParError(5));
  z02w_f2->SetLineColor(kBlue);
  
  Double_t intz02w_1 = z02w_f1->Integral(-.1,.1);
  Double_t intz02w_2 = z02w_f2->Integral(-.1,.1);
  
  zres_02w = (intz02w_1*fabs(z02w_f1->GetParameter(2))+ intz02w_2*fabs(z02w_f2->GetParameter(2)))/(intz02w_1 + intz02w_2);

  err_zres_02w  = (1./(intz02w_1+intz02w_2));
  err_zres_02w *= sqrt(intz02w_1*intz02w_1*pow((z02w_f1->GetParError(2)),2)+intz02w_2*intz02w_2*pow(z02w_f2->GetParError(2),2));

  cout<<endl<<"resolution zw12: "<<zres_02w<<" +- " <<err_zres_02w<<endl<<endl;

  //z03w

  TF1 *doublegaus_z03w = new TF1("doublegaus_z03w",doublegausf,-.1,.1,6);
  doublegaus_z03w->SetParameters(200,z03w->GetMean(),z03w->GetRMS(),50,z03w->GetMean(),z03w->GetRMS());
  z03w->Fit("doublegaus_z03w","R");

  TF1 *z03w_f1 = new TF1("z03w_f1","gaus",-0.1,0.1);
  TF1 *z03w_f2 = new TF1("z03w_f2","gaus",-0.1,0.1);

  z03w_f1->SetParameter(0,doublegaus_z03w->GetParameter(0));
  z03w_f1->SetParameter(1,doublegaus_z03w->GetParameter(1));
  z03w_f1->SetParameter(2,doublegaus_z03w->GetParameter(2));
  z03w_f1->SetParError(2,doublegaus_z03w->GetParError(2));
  z03w_f1->SetLineColor(kRed);
  z03w_f2->SetParameter(0,doublegaus_z03w->GetParameter(3));
  z03w_f2->SetParameter(1,doublegaus_z03w->GetParameter(4));
  z03w_f2->SetParameter(2,doublegaus_z03w->GetParameter(5));
  z03w_f2->SetParError(2,doublegaus_z03w->GetParError(5));
  z03w_f2->SetLineColor(kBlue);
  
  Double_t intz03w_1 = z03w_f1->Integral(-.1,.1);
  Double_t intz03w_2 = z03w_f2->Integral(-.1,.1);
  
  zres_03w = (intz03w_1*fabs(z03w_f1->GetParameter(2))+ intz03w_2*fabs(z03w_f2->GetParameter(2)))/(intz03w_1 + intz03w_2);

  err_zres_03w  = (1./(intz03w_1+intz03w_2));
  err_zres_03w *= sqrt(intz03w_1*intz03w_1*pow((z03w_f1->GetParError(2)),2)+intz03w_2*intz03w_2*pow(z03w_f2->GetParError(2),2));

  cout<<endl<<"resolution zw23: "<<zres_03w<<" +- " <<err_zres_03w<<endl<<endl;

  //z04w

  TF1 *doublegaus_z04w = new TF1("doublegaus_z04w",doublegausf,-.1,.1,6);
  doublegaus_z04w->SetParameters(200,z04w->GetMean(),z04w->GetRMS(),50,z04w->GetMean(),z04w->GetRMS());
  z04w->Fit("doublegaus_z04w","R");

  TF1 *z04w_f1 = new TF1("z04w_f1","gaus",-0.1,0.1);
  TF1 *z04w_f2 = new TF1("z04w_f2","gaus",-0.1,0.1);

  z04w_f1->SetParameter(0,doublegaus_z04w->GetParameter(0));
  z04w_f1->SetParameter(1,doublegaus_z04w->GetParameter(1));
  z04w_f1->SetParameter(2,doublegaus_z04w->GetParameter(2));
  z04w_f1->SetParError(2,doublegaus_z04w->GetParError(2));
  z04w_f1->SetLineColor(kRed);
  z04w_f2->SetParameter(0,doublegaus_z04w->GetParameter(3));
  z04w_f2->SetParameter(1,doublegaus_z04w->GetParameter(4));
  z04w_f2->SetParameter(2,doublegaus_z04w->GetParameter(5));
  z04w_f2->SetParError(2,doublegaus_z04w->GetParError(5));
  z04w_f2->SetLineColor(kBlue);
  
  Double_t intz04w_1 = z04w_f1->Integral(-.1,.1);
  Double_t intz04w_2 = z04w_f2->Integral(-.1,.1);
  
  zres_04w = (intz04w_1*fabs(z04w_f1->GetParameter(2))+ intz04w_2*fabs(z04w_f2->GetParameter(2)))/(intz04w_1 + intz04w_2);

  err_zres_04w  = (1./(intz04w_1+intz04w_2));
  err_zres_04w *= sqrt(intz04w_1*intz04w_1*pow((z04w_f1->GetParError(2)),2)+intz04w_2*intz04w_2*pow(z04w_f2->GetParError(2),2));

  cout<<endl<<"resolution zw34: "<<zres_04w<<" +- " <<err_zres_04w<<endl<<endl;

  //z05w

  TF1 *doublegaus_z05w = new TF1("doublegaus_z05w",doublegausf,-.1,.1,6);
  doublegaus_z05w->SetParameters(200,z05w->GetMean(),z05w->GetRMS(),50,z05w->GetMean(),z05w->GetRMS());
  z05w->Fit("doublegaus_z05w","R");

  TF1 *z05w_f1 = new TF1("z05w_f1","gaus",-0.1,0.1);
  TF1 *z05w_f2 = new TF1("z05w_f2","gaus",-0.1,0.1);

  z05w_f1->SetParameter(0,doublegaus_z05w->GetParameter(0));
  z05w_f1->SetParameter(1,doublegaus_z05w->GetParameter(1));
  z05w_f1->SetParameter(2,doublegaus_z05w->GetParameter(2));
  z05w_f1->SetParError(2,doublegaus_z05w->GetParError(2));
  z05w_f1->SetLineColor(kRed);
  z05w_f2->SetParameter(0,doublegaus_z05w->GetParameter(3));
  z05w_f2->SetParameter(1,doublegaus_z05w->GetParameter(4));
  z05w_f2->SetParameter(2,doublegaus_z05w->GetParameter(5));
  z05w_f2->SetParError(2,doublegaus_z05w->GetParError(5));
  z05w_f2->SetLineColor(kBlue);
  
  Double_t intz05w_1 = z05w_f1->Integral(-.1,.1);
  Double_t intz05w_2 = z05w_f2->Integral(-.1,.1);
  
  zres_05w = (intz05w_1*fabs(z05w_f1->GetParameter(2))+ intz05w_2*fabs(z05w_f2->GetParameter(2)))/(intz05w_1 + intz05w_2);

  err_zres_05w  = (1./(intz05w_1+intz05w_2));
  err_zres_05w *= sqrt(intz05w_1*intz05w_1*pow((z05w_f1->GetParError(2)),2)+intz05w_2*intz05w_2*pow(z05w_f2->GetParError(2),2));

  cout<<endl<<"resolution zw45: "<<zres_05w<<" +- " <<err_zres_05w<<endl<<endl;

  //z06w

  TF1 *doublegaus_z06w = new TF1("doublegaus_z06w",doublegausf,-.1,.1,6);
  doublegaus_z06w->SetParameters(200,z06w->GetMean(),z06w->GetRMS(),50,z06w->GetMean(),z06w->GetRMS());
  z06w->Fit("doublegaus_z06w","R");

  TF1 *z06w_f1 = new TF1("z06w_f1","gaus",-0.1,0.1);
  TF1 *z06w_f2 = new TF1("z06w_f2","gaus",-0.1,0.1);

  z06w_f1->SetParameter(0,doublegaus_z06w->GetParameter(0));
  z06w_f1->SetParameter(1,doublegaus_z06w->GetParameter(1));
  z06w_f1->SetParameter(2,doublegaus_z06w->GetParameter(2));
  z06w_f1->SetParError(2,doublegaus_z06w->GetParError(2));
  z06w_f1->SetLineColor(kRed);
  z06w_f2->SetParameter(0,doublegaus_z06w->GetParameter(3));
  z06w_f2->SetParameter(1,doublegaus_z06w->GetParameter(4));
  z06w_f2->SetParameter(2,doublegaus_z06w->GetParameter(5));
  z06w_f2->SetParError(2,doublegaus_z06w->GetParError(5));
  z06w_f2->SetLineColor(kBlue);
  
  Double_t intz06w_1 = z06w_f1->Integral(-.1,.1);
  Double_t intz06w_2 = z06w_f2->Integral(-.1,.1);
  
  zres_06w = (intz06w_1*fabs(z06w_f1->GetParameter(2))+ intz06w_2*fabs(z06w_f2->GetParameter(2)))/(intz06w_1 + intz06w_2);

  err_zres_06w  = (1./(intz06w_1+intz06w_2));
  err_zres_06w *= sqrt(intz06w_1*intz06w_1*pow((z06w_f1->GetParError(2)),2)+intz06w_2*intz06w_2*pow(z06w_f2->GetParError(2),2));

  cout<<endl<<"resolution zw56: "<<zres_06w<<" +- " <<err_zres_06w<<endl<<endl;

  //z07w

  TF1 *doublegaus_z07w = new TF1("doublegaus_z07w",doublegausf,-.1,.1,6);
  doublegaus_z07w->SetParameters(200,z07w->GetMean(),z07w->GetRMS(),50,z07w->GetMean(),z07w->GetRMS());
  z07w->Fit("doublegaus_z07w","R");

  TF1 *z07w_f1 = new TF1("z07w_f1","gaus",-0.1,0.1);
  TF1 *z07w_f2 = new TF1("z07w_f2","gaus",-0.1,0.1);

  z07w_f1->SetParameter(0,doublegaus_z07w->GetParameter(0));
  z07w_f1->SetParameter(1,doublegaus_z07w->GetParameter(1));
  z07w_f1->SetParameter(2,doublegaus_z07w->GetParameter(2));
  z07w_f1->SetParError(2,doublegaus_z07w->GetParError(2));
  z07w_f1->SetLineColor(kRed);
  z07w_f2->SetParameter(0,doublegaus_z07w->GetParameter(3));
  z07w_f2->SetParameter(1,doublegaus_z07w->GetParameter(4));
  z07w_f2->SetParameter(2,doublegaus_z07w->GetParameter(5));
  z07w_f2->SetParError(2,doublegaus_z07w->GetParError(5));
  z07w_f2->SetLineColor(kBlue);
  
  Double_t intz07w_1 = z07w_f1->Integral(-.1,.1);
  Double_t intz07w_2 = z07w_f2->Integral(-.1,.1);
  
  zres_07w = (intz07w_1*fabs(z07w_f1->GetParameter(2))+ intz07w_2*fabs(z07w_f2->GetParameter(2)))/(intz07w_1 + intz07w_2);

  err_zres_07w  = (1./(intz07w_1+intz07w_2));
  err_zres_07w *= sqrt(intz07w_1*intz07w_1*pow((z07w_f1->GetParError(2)),2)+intz07w_2*intz07w_2*pow(z07w_f2->GetParError(2),2));

  cout<<endl<<"resolution zw67: "<<zres_07w<<" +- " <<err_zres_07w<<endl<<endl;

  //z08w

  TF1 *doublegaus_z08w = new TF1("doublegaus_z08w",doublegausf,-.1,.1,6);
  doublegaus_z08w->SetParameters(200,z08w->GetMean(),z08w->GetRMS(),50,z08w->GetMean(),z08w->GetRMS());
  z08w->Fit("doublegaus_z08w","R");

  TF1 *z08w_f1 = new TF1("z08w_f1","gaus",-0.1,0.1);
  TF1 *z08w_f2 = new TF1("z08w_f2","gaus",-0.1,0.1);

  z08w_f1->SetParameter(0,doublegaus_z08w->GetParameter(0));
  z08w_f1->SetParameter(1,doublegaus_z08w->GetParameter(1));
  z08w_f1->SetParameter(2,doublegaus_z08w->GetParameter(2));
  z08w_f1->SetParError(2,doublegaus_z08w->GetParError(2));
  z08w_f1->SetLineColor(kRed);
  z08w_f2->SetParameter(0,doublegaus_z08w->GetParameter(3));
  z08w_f2->SetParameter(1,doublegaus_z08w->GetParameter(4));
  z08w_f2->SetParameter(2,doublegaus_z08w->GetParameter(5));
  z08w_f2->SetParError(2,doublegaus_z08w->GetParError(5));
  z08w_f2->SetLineColor(kBlue);
  
  Double_t intz08w_1 = z08w_f1->Integral(-.1,.1);
  Double_t intz08w_2 = z08w_f2->Integral(-.1,.1);
  
  zres_08w = (intz08w_1*fabs(z08w_f1->GetParameter(2))+ intz08w_2*fabs(z08w_f2->GetParameter(2)))/(intz08w_1 + intz08w_2);

  err_zres_08w  = (1./(intz08w_1+intz08w_2));
  err_zres_08w *= sqrt(intz08w_1*intz08w_1*pow((z08w_f1->GetParError(2)),2)+intz08w_2*intz08w_2*pow(z08w_f2->GetParError(2),2));

  cout<<endl<<"resolution zw78: "<<zres_08w<<" +- " <<err_zres_08w<<endl<<endl;


  //Draw

  //y without

  TCanvas *canvas = new TCanvas();
  yall->Draw();
  doublegaus_yall->Draw("same");
  yall_f1->Draw("same");
  yall_f2->Draw("same");
  canvas = new TCanvas();
  y01->Draw();
  doublegaus_y01->Draw("same");
  y01_f1->Draw("same");
  y01_f2->Draw("same");
  canvas = new TCanvas();
  y02->Draw();
  doublegaus_y02->Draw("same");
  y02_f1->Draw("same");
  y02_f2->Draw("same");
  canvas = new TCanvas();
  y03->Draw();
  doublegaus_y03->Draw("same");
  y03_f1->Draw("same");
  y03_f2->Draw("same");
  canvas = new TCanvas();
  y04->Draw();
  doublegaus_y04->Draw("same");
  y04_f1->Draw("same");
  y04_f2->Draw("same");
  canvas = new TCanvas();
  y05->Draw();
  doublegaus_y05->Draw("same");
  y05_f1->Draw("same");
  y05_f2->Draw("same");
  canvas = new TCanvas();
  y06->Draw();
  doublegaus_y06->Draw("same");
  y06_f1->Draw("same");
  y06_f2->Draw("same");
  canvas = new TCanvas();
  y07->Draw();
  doublegaus_y07->Draw("same");
  y07_f1->Draw("same");
  y07_f2->Draw("same");
  canvas = new TCanvas();
  y08->Draw();
  doublegaus_y08->Draw("same");
  y08_f1->Draw("same");
  y08_f2->Draw("same");

  //z without

  canvas = new TCanvas();
  zall->Draw();
  doublegaus_zall->Draw("same");
  zall_f1->Draw("same");
  zall_f2->Draw("same");
  canvas = new TCanvas();
  z01->Draw();
  doublegaus_z01->Draw("same");
  z01_f1->Draw("same");
  z01_f2->Draw("same");
  canvas = new TCanvas();
  z02->Draw();
  doublegaus_z02->Draw("same");
  z02_f1->Draw("same");
  z02_f2->Draw("same");
  canvas = new TCanvas();
  z03->Draw();
  doublegaus_z03->Draw("same");
  z03_f1->Draw("same");
  z03_f2->Draw("same");
  canvas = new TCanvas();
  z04->Draw();
  doublegaus_z04->Draw("same");
  z04_f1->Draw("same");
  z04_f2->Draw("same");
  canvas = new TCanvas();
  z05->Draw();
  doublegaus_z05->Draw("same");
  z05_f1->Draw("same");
  z05_f2->Draw("same");
  canvas = new TCanvas();
  z06->Draw();
  doublegaus_z06->Draw("same");
  z06_f1->Draw("same");
  z06_f2->Draw("same");
  canvas = new TCanvas();
  z07->Draw();
  doublegaus_z07->Draw("same");
  z07_f1->Draw("same");
  z07_f2->Draw("same");
  canvas = new TCanvas();
  z08->Draw();
  doublegaus_z08->Draw("same");
  z08_f1->Draw("same");
  z08_f2->Draw("same");

  //y using clusterSplit1.C

  canvas = new TCanvas();
  yallw->Draw();
  doublegaus_yallw->Draw("same");
  yallw_f1->Draw("same");
  yallw_f2->Draw("same");
  canvas = new TCanvas();
  y01w->Draw();
  doublegaus_y01w->Draw("same");
  y01w_f1->Draw("same");
  y01w_f2->Draw("same");
  canvas = new TCanvas();
  y02w->Draw();
  doublegaus_y02w->Draw("same");
  y02w_f1->Draw("same");
  y02w_f2->Draw("same");
  canvas = new TCanvas();
  y03w->Draw();
  doublegaus_y03w->Draw("same");
  y03w_f1->Draw("same");
  y03w_f2->Draw("same");
  canvas = new TCanvas();
  y04w->Draw();
  doublegaus_y04w->Draw("same");
  y04w_f1->Draw("same");
  y04w_f2->Draw("same");
  canvas = new TCanvas();
  y05w->Draw();
  doublegaus_y05w->Draw("same");
  y05w_f1->Draw("same");
  y05w_f2->Draw("same");
  canvas = new TCanvas();
  y06w->Draw();
  doublegaus_y06w->Draw("same");
  y06w_f1->Draw("same");
  y06w_f2->Draw("same");
  canvas = new TCanvas();
  y07w->Draw();
  doublegaus_y07w->Draw("same");
  y07w_f1->Draw("same");
  y07w_f2->Draw("same");
  canvas = new TCanvas();
  y08w->Draw();
  doublegaus_y08w->Draw("same");
  y08w_f1->Draw("same");
  y08w_f2->Draw("same");

  //z using clusterSplit1.C

  canvas = new TCanvas();
  zallw->Draw();
  doublegaus_zallw->Draw("same");
  zallw_f1->Draw("same");
  zallw_f2->Draw("same");
  canvas = new TCanvas();
  z01w->Draw();
  doublegaus_z01w->Draw("same");
  z01w_f1->Draw("same");
  z01w_f2->Draw("same");
  canvas = new TCanvas();
  z02w->Draw();
  doublegaus_z02w->Draw("same");
  z02w_f1->Draw("same");
  z02w_f2->Draw("same");
  canvas = new TCanvas();
  z03w->Draw();
  doublegaus_z03w->Draw("same");
  z03w_f1->Draw("same");
  z03w_f2->Draw("same");
  canvas = new TCanvas();
  z04w->Draw();
  doublegaus_z04w->Draw("same");
  z04w_f1->Draw("same");
  z04w_f2->Draw("same");
  canvas = new TCanvas();
  z05w->Draw();
  doublegaus_z05w->Draw("same");
  z05w_f1->Draw("same");
  z05w_f2->Draw("same");
  canvas = new TCanvas();
  z06w->Draw();
  doublegaus_z06w->Draw("same");
  z06w_f1->Draw("same");
  z06w_f2->Draw("same");
  canvas = new TCanvas();
  z07w->Draw();
  doublegaus_z07w->Draw("same");
  z07w_f1->Draw("same");
  z07w_f2->Draw("same");
  canvas = new TCanvas();
  z08w->Draw();
  doublegaus_z08w->Draw("same");
  z08w_f1->Draw("same");
  z08w_f2->Draw("same");


  //overview graphs

  //y without

  double yx[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  double yxx[8] = {5, 15, 25, 35, 45, 55, 65, 73.5};
  double yex[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  double yexx[8] = {5, 5, 5, 5, 5, 5, 5, 3.5};
  double yy[8] = {yres_all*10000, yres_01*10000, yres_02*10000, yres_03*10000,
		  yres_04*10000, yres_05*10000, yres_06*10000, yres_07*10000};
  double yyy[8] = {yres_all*10, yres_01*10, yres_02*10, yres_03*10,
		   yres_04*10, yres_05*10, yres_06*10, yres_07*10};
  double yey[8] = {err_yres_all*10000, err_yres_01*10000,
		   err_yres_02*10000, err_yres_03*10000,
		   err_yres_04*10000, err_yres_05*10000,
		   err_yres_06*10000, err_yres_07*10000};
  double yeyy[8] = {err_yres_all*10, err_yres_01*10,
		    err_yres_02*10, err_yres_03*10,
		    err_yres_04*10, err_yres_05*10,
		    err_yres_06*10, err_yres_07*10};

  TH2D *hr1y = new TH2D("hr1y","hr1y",8,0.5,8.5, 10,0,500);
  hr1y->SetStats(kFALSE);
  hr1y->SetTitle("");
  hr1y->SetYTitle("Spatial Resolution y (#mum)");
  hr1y->SetLabelOffset(0.01, "X");
  hr1y->SetLabelSize(0.05, "X");

  TAxis *y_xaxis = hr1y->GetXaxis();
  y_xaxis->SetBinLabel(1," 0 < z < 10mm");
  y_xaxis->SetBinLabel(2,"10 < z < 20mm");
  y_xaxis->SetBinLabel(3,"20 < z < 30mm");
  y_xaxis->SetBinLabel(4,"30 < z < 40mm");
  y_xaxis->SetBinLabel(5,"40 < z < 50mm");
  y_xaxis->SetBinLabel(6,"50 < z < 60mm");
  y_xaxis->SetBinLabel(7,"60 < z < 70mm");
  y_xaxis->SetBinLabel(8,"70 < z < 80mm");

  TGraphErrors *yres_vs_z = new TGraphErrors(8, yx, yy, yex, yey);
  yres_vs_z->SetTitle("");
  yres_vs_z->SetMarkerStyle(21);
  yres_vs_z->SetMarkerColor(2);
  yres_vs_z->SetMarkerSize(1);
  canvas = new TCanvas();
  hr1y->Draw();
  yres_vs_z->Draw("p");

  TGraphErrors *yres_vs_z2 = new TGraphErrors(8, yxx, yyy, yexx, yeyy);
  yres_vs_z2->SetTitle("");
  yres_vs_z2->SetMarkerStyle(21);
  yres_vs_z2->SetMarkerColor(2);
  yres_vs_z2->SetMarkerSize(1);
  canvas = new TCanvas();
  yres_vs_z2->Draw("ap");


  //z without

  double zx[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  double zxx[8] = {5, 15, 25, 35, 45, 55, 65, 73.5};
  double zex[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  double zexx[8] = {5, 5, 5, 5, 5, 5, 5, 3.5};
  double zy[8] = {zres_all*10000, zres_01*10000, zres_02*10000, zres_03*10000,
		  zres_04*10000, zres_05*10000, zres_06*10000, zres_07*10000};
  double zyy[8] = {zres_all*10, zres_01*10, zres_02*10, zres_03*10,
		   zres_04*10, zres_05*10, zres_06*10, zres_07*10};
  double zey[8] = {err_zres_all*10000, err_zres_01*10000,
		   err_zres_02*10000, err_zres_03*10000,
		   err_zres_04*10000, err_zres_05*10000,
		   err_zres_06*10000, err_zres_07*10000};
  double zeyy[8] = {err_zres_all*10, err_zres_01*10,
		    err_zres_02*10, err_zres_03*10,
		    err_zres_04*10, err_zres_05*10,
		    err_zres_06*10, err_zres_07*10};

  TH2D *hr1z = new TH2D("hr1z","hr1z",8,0.5,8.5, 10,0,500);
  hr1z->SetStats(kFALSE);
  hr1z->SetTitle("");
  hr1z->SetYTitle("Spatial Resolution z (#mum)");
  hr1z->SetLabelOffset(0.01, "X");
  hr1z->SetLabelSize(0.05, "X");

  TAxis *z_xaxis = hr1z->GetXaxis();
  z_xaxis->SetBinLabel(1," 0 < z < 10mm");
  z_xaxis->SetBinLabel(2,"10 < z < 20mm");
  z_xaxis->SetBinLabel(3,"20 < z < 30mm");
  z_xaxis->SetBinLabel(4,"30 < z < 40mm");
  z_xaxis->SetBinLabel(5,"40 < z < 50mm");
  z_xaxis->SetBinLabel(6,"50 < z < 60mm");
  z_xaxis->SetBinLabel(7,"60 < z < 70mm");
  z_xaxis->SetBinLabel(8,"70 < z < 80mm");

  TGraphErrors *zres_vs_z = new TGraphErrors(8, zx, zy, zex, zey);
  zres_vs_z->SetTitle("");
  zres_vs_z->SetMarkerStyle(21);
  zres_vs_z->SetMarkerColor(2);
  zres_vs_z->SetMarkerSize(1);
  canvas = new TCanvas();
  hr1z->Draw();
  zres_vs_z->Draw("p");

  TGraphErrors *zres_vs_z2 = new TGraphErrors(8, zxx, zyy, zexx, zeyy);
  zres_vs_z2->SetTitle("");
  zres_vs_z2->SetMarkerStyle(21);
  zres_vs_z2->SetMarkerColor(2);
  zres_vs_z2->SetMarkerSize(1);
  canvas = new TCanvas();
  zres_vs_z2->Draw("ap");


  //y using clusterSplit1.C

  double ywx[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  double ywxx[8] = {5, 15, 25, 35, 45, 55, 65, 73.5};
  double ywex[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  double ywexx[8] = {5, 5, 5, 5, 5, 5, 5, 3.5};
  double ywy[8] = {yres_allw*10000, yres_01w*10000, yres_02w*10000, yres_03w*10000, yres_04w*10000, yres_05w*10000, yres_06w*10000, yres_07w*10000};
  double ywyy[8] = {yres_allw*10, yres_01w*10, yres_02w*10, yres_03w*10, yres_04w*10, yres_05w*10, yres_06w*10, yres_07w*10};
  double ywey[8] = {err_yres_allw*10000, err_yres_01w*10000,
		    err_yres_02w*10000, err_yres_03w*10000,
		    err_yres_04w*10000, err_yres_05w*10000,
		    err_yres_06w*10000, err_yres_07w*10000};
  double yweyy[8] = {err_yres_allw*10, err_yres_01w*10,
		     err_yres_02w*10, err_yres_03w*10,
		     err_yres_04w*10, err_yres_05w*10,
		     err_yres_06w*10, err_yres_07w*10};

  TH2D *hr1yw = new TH2D("hr1yw","hr1yw",8,0.5,8.5, 10,0,500);
  hr1yw->SetStats(kFALSE);
  hr1yw->SetTitle("");
  hr1yw->SetYTitle("Spatial Resolution y using clusterSplit1.C (#mum)");
  hr1yw->SetLabelOffset(0.01, "X");
  hr1yw->SetLabelSize(0.05, "X");

  TAxis *yw_xaxis = hr1yw->GetXaxis();
  yw_xaxis->SetBinLabel(1," 0 < z < 10mm");
  yw_xaxis->SetBinLabel(2,"10 < z < 20mm");
  yw_xaxis->SetBinLabel(3,"20 < z < 30mm");
  yw_xaxis->SetBinLabel(4,"30 < z < 40mm");
  yw_xaxis->SetBinLabel(5,"40 < z < 50mm");
  yw_xaxis->SetBinLabel(6,"50 < z < 60mm");
  yw_xaxis->SetBinLabel(7,"60 < z < 70mm");
  yw_xaxis->SetBinLabel(8,"70 < z < 80mm");

  TGraphErrors *yresw_vs_z = new TGraphErrors(8, ywx,ywy, ywex, ywey);
  yresw_vs_z->SetTitle("");
  yresw_vs_z->SetMarkerStyle(21);
  yresw_vs_z->SetMarkerColor(2);
  yresw_vs_z->SetMarkerSize(1);
  canvas = new TCanvas();
  hr1yw->Draw();
  yresw_vs_z->Draw("p");

  TGraphErrors *yresw_vs_z2 = new TGraphErrors(8, ywxx, ywyy, ywexx, yweyy);
  yresw_vs_z2->SetTitle("");
  yresw_vs_z2->SetMarkerStyle(21);
  yresw_vs_z2->SetMarkerColor(2);
  yresw_vs_z2->SetMarkerSize(1);
  canvas = new TCanvas();
  yresw_vs_z2->Draw("ap");


  //z using clusterSplit1.C

  double zwx[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  double zwxx[8] = {5, 15, 25, 35, 45, 55, 65, 73.5};
  double zwex[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  double zwexx[8] = {5, 5, 5, 5, 5, 5, 5, 3.5};
  double zwy[8] = {zres_allw*10000, zres_01w*10000, zres_02w*10000, zres_03w*10000, zres_04w*10000, zres_05w*10000, zres_06w*10000, zres_07w*10000};
  double zwyy[8] = {zres_allw*10, zres_01w*10, zres_02w*10, zres_03w*10, zres_04w*10, zres_05w*10, zres_06w*10, zres_07w*10};
  double zwey[8] = {err_zres_allw*10000, err_zres_01w*10000,
		    err_zres_02w*10000, err_zres_03w*10000,
		    err_zres_04w*10000, err_zres_05w*10000,
		    err_zres_06w*10000, err_zres_07w*10000};
  double zweyy[8] = {err_zres_allw*10, err_zres_01w*10,
		     err_zres_02w*10, err_zres_03w*10,
		     err_zres_04w*10, err_zres_05w*10,
		     err_zres_06w*10, err_zres_07w*10};

  TH2D *hr1zw = new TH2D("hr1zw","hr1zw",8,0.5,8.5, 10,0,500);
  hr1zw->SetStats(kFALSE);
  hr1zw->SetTitle("");
  hr1zw->SetYTitle("Spatial Resolution z using clusterSplit1.C (#mum)");
  hr1zw->SetLabelOffset(0.01, "X");
  hr1zw->SetLabelSize(0.05, "X");

  TAxis *zw_xaxis = hr1zw->GetXaxis();
  zw_xaxis->SetBinLabel(1," 0 < z < 10mm");
  zw_xaxis->SetBinLabel(2,"10 < z < 20mm");
  zw_xaxis->SetBinLabel(3,"20 < z < 30mm");
  zw_xaxis->SetBinLabel(4,"30 < z < 40mm");
  zw_xaxis->SetBinLabel(5,"40 < z < 50mm");
  zw_xaxis->SetBinLabel(6,"50 < z < 60mm");
  zw_xaxis->SetBinLabel(7,"60 < z < 70mm");
  zw_xaxis->SetBinLabel(8,"70 < z < 80mm");

  TGraphErrors *zresw_vs_z = new TGraphErrors(8, zwx,zwy, zwex, zwey);
  zresw_vs_z->SetTitle("");
  zresw_vs_z->SetMarkerStyle(21);
  zresw_vs_z->SetMarkerColor(2);
  zresw_vs_z->SetMarkerSize(1);
  canvas = new TCanvas();
  hr1zw->Draw();
  zresw_vs_z->Draw("p");

  TGraphErrors *zresw_vs_z2 = new TGraphErrors(8, zwxx, zwyy, zwexx, zweyy);
  zresw_vs_z2->SetTitle("");
  zresw_vs_z2->SetMarkerStyle(21);
  zresw_vs_z2->SetMarkerColor(2);
  zresw_vs_z2->SetMarkerSize(1);
  canvas = new TCanvas();
  zresw_vs_z2->Draw("ap");

}

