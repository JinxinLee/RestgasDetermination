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
  //v without 

  TH1D *vall = new TH1D("vall","",500,-0.1,0.1);
  vall->SetXTitle("Residual v [cm]");

  TH1D *v01 = new TH1D("v01","0mm < w < 10mm",500,-0.1,0.1);
  v01->SetXTitle("Residual v [cm]");
  TH1D *v02 = new TH1D("v02","10mm < w < 20mm",500,-0.1,0.1);
  v02->SetXTitle("Residual v [cm]");
  TH1D *v03 = new TH1D("v03","20mm < w < 30mm",500,-0.1,0.1);
  v03->SetXTitle("Residual v [cm]");
  TH1D *v04 = new TH1D("v04","30mm < w < 40mm",500,-0.1,0.1);
  v04->SetXTitle("Residual v [cm]");
  TH1D *v05 = new TH1D("v05","40mm < w < 50mm",500,-0.1,0.1);
  v05->SetXTitle("Residual v [cm]");
  TH1D *v06 = new TH1D("v06","50mm < w < 60mm",500,-0.1,0.1);
  v06->SetXTitle("Residual v [cm]");
  TH1D *v07 = new TH1D("v07","60mm < w < 70mm",500,-0.1,0.1);
  v07->SetXTitle("Residual v [cm]");
  TH1D *v08 = new TH1D("v08","70mm < w < 80mm",500,-0.1,0.1);
  v08->SetXTitle("Residual v [cm]");

  //w without

  TH1D *wall = new TH1D("wall","",500,-0.1,0.1);
  wall->SetXTitle("Residual w [cm]");

  TH1D *w01 = new TH1D("w01","0mm < w < 10mm",500,-0.1,0.1);
  w01->SetXTitle("Residual w [cm]");
  TH1D *w02 = new TH1D("w02","10mm < w < 20mm",500,-0.1,0.1);
  w02->SetXTitle("Residual w [cm]");
  TH1D *w03 = new TH1D("w03","20mm < w < 30mm",500,-0.1,0.1);
  w03->SetXTitle("Residual w [cm]");
  TH1D *w04 = new TH1D("w04","30mm < w < 40mm",500,-0.1,0.1);
  w04->SetXTitle("Residual w [cm]");
  TH1D *w05 = new TH1D("w05","40mm < w < 50mm",500,-0.1,0.1);
  w05->SetXTitle("Residual w [cm]");
  TH1D *w06 = new TH1D("w06","50mm < w < 60mm",500,-0.1,0.1);
  w06->SetXTitle("Residual w [cm]");
  TH1D *w07 = new TH1D("w07","60mm < w < 70mm",500,-0.1,0.1);
  w07->SetXTitle("Residual w [cm]");
  TH1D *w08 = new TH1D("w08","70mm < w < 80mm",500,-0.1,0.1);
  w08->SetXTitle("Residual w [cm]");

  //v using clusterSplit1.C

  TH1D *vallw = new TH1D("vallw","",500,-0.1,0.1);
  vallw->SetXTitle("Residual v using clusterSplit1.C [cm]");

  TH1D *v01w = new TH1D("v01w","0mm < w < 10mm",500,-0.1,0.1);
  v01w->SetXTitle("Residual v using clusterSplit1.C [cm]");
  TH1D *v02w = new TH1D("v02w","10mm < w < 20mm",500,-0.1,0.1);
  v02w->SetXTitle("Residual v using clusterSplit1.C [cm]");
  TH1D *v03w = new TH1D("v03w","20mm < w < 30mm",500,-0.1,0.1);
  v03w->SetXTitle("Residual v using clusterSplit1.C [cm]");
  TH1D *v04w = new TH1D("v04w","30mm < w < 40mm",500,-0.1,0.1);
  v04w->SetXTitle("Residual v using clusterSplit1.C [cm]");
  TH1D *v05w = new TH1D("v05w","40mm < w < 50mm",500,-0.1,0.1);
  v05w->SetXTitle("Residual v using clusterSplit1.C [cm]");
  TH1D *v06w = new TH1D("v06w","50mm < w < 60mm",500,-0.1,0.1);
  v06w->SetXTitle("Residual v using clusterSplit1.C [cm]");
  TH1D *v07w = new TH1D("v07w","60mm < w < 70mm",500,-0.1,0.1);
  v07w->SetXTitle("Residual v using clusterSplit1.C [cm]");
  TH1D *v08w = new TH1D("v08w","70mm < w < 80mm",500,-0.1,0.1);
  v08w->SetXTitle("Residual v using clusterSplit1.C [cm]");

  //w using clusterSplit1.C

  TH1D *wallw = new TH1D("wallw","",500,-0.1,0.1);
  wallw->SetXTitle("Residual w using clustersplit1.C [cm]");

  TH1D *w01w = new TH1D("w01w","0mm < w < 10mm",500,-0.1,0.1);
  w01w->SetXTitle("Residual w using clusterSplit1.C [cm]");
  TH1D *w02w = new TH1D("w02w","10mm < w < 20mm",500,-0.1,0.1);
  w02w->SetXTitle("Residual w using clusterSplit1.C [cm]");
  TH1D *w03w = new TH1D("w03w","20mm < w < 30mm",500,-0.1,0.1);
  w03w->SetXTitle("Residual w using clustersplit1.C [cm]");
  TH1D *w04w = new TH1D("w04w","30mm < w < 40mm",500,-0.1,0.1);
  w04w->SetXTitle("Residual w using clusterSplit1.C [cm]");
  TH1D *w05w = new TH1D("w05w","40mm < w < 50mm",500,-0.1,0.1);
  w05w->SetXTitle("Residual w using clusterSplit1.C [cm]");
  TH1D *w06w = new TH1D("w06w","50mm < w < 60mm",500,-0.1,0.1);
  w06w->SetXTitle("Residual w using clusterSplit1.C [cm]");
  TH1D *w07w = new TH1D("w07w","60mm < w < 70mm",500,-0.1,0.1);
  w07w->SetXTitle("Residual w using clusterSplit1.C [cm]");
  TH1D *w08w = new TH1D("w08w","70mm < w < 80mm",500,-0.1,0.1);
  w08w->SetXTitle("Residual w using clusterSPlit1.C [cm]");

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

	vall->Fill(c.getRes().Y());
	wall->Fill(c.getRes().Z());

	double w = c.posUVW().Z();

	if(w>0&&w<1) v01->Fill(c.getRes().Y());
	if(w>1&&w<2) v02->Fill(c.getRes().Y());
	if(w>2&&w<3) v03->Fill(c.getRes().Y());
	if(w>3&&w<4) v04->Fill(c.getRes().Y());
	if(w>4&&w<5) v05->Fill(c.getRes().Y());
	if(w>5&&w<6) v06->Fill(c.getRes().Y());
	if(w>6&&w<7) v07->Fill(c.getRes().Y());
	if(w>7&&w<8) v08->Fill(c.getRes().Y());

	if(w>0&&w<1) w01->Fill(c.getRes().Z());
	if(w>1&&w<2) w02->Fill(c.getRes().Z());
	if(w>2&&w<3) w03->Fill(c.getRes().Z());
	if(w>3&&w<4) w04->Fill(c.getRes().Z());
	if(w>4&&w<5) w05->Fill(c.getRes().Z());
	if(w>5&&w<6) w06->Fill(c.getRes().Z());
	if(w>6&&w<7) w07->Fill(c.getRes().Z());
	if(w>7&&w<8) w08->Fill(c.getRes().Z());
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
	

	vallw->Fill(d.getRes().Y());
	wallw->Fill(d.getRes().Z());

	double w = d.posUVW().Z();

	if(w>0&&w<1) v01w->Fill(d.getRes().Y());
	if(w>1&&w<2) v02w->Fill(d.getRes().Y());
	if(w>2&&w<3) v03w->Fill(d.getRes().Y());
	if(w>3&&w<4) v04w->Fill(d.getRes().Y());
	if(w>4&&w<5) v05w->Fill(d.getRes().Y());
	if(w>5&&w<6) v06w->Fill(d.getRes().Y());
	if(w>6&&w<7) v07w->Fill(d.getRes().Y());
	if(w>7&&w<8) v08w->Fill(d.getRes().Y());

	if(w>0&&w<1) w01w->Fill(d.getRes().Z());
	if(w>1&&w<2) w02w->Fill(d.getRes().Z());
	if(w>2&&w<3) w03w->Fill(d.getRes().Z());
	if(w>3&&w<4) w04w->Fill(d.getRes().Z());
	if(w>4&&w<5) w05w->Fill(d.getRes().Z());
	if(w>5&&w<6) w06w->Fill(d.getRes().Z());
	if(w>6&&w<7) w07w->Fill(d.getRes().Z());
	if(w>7&&w<8) w08w->Fill(d.getRes().Z());
      }
    }
  }

  //end of event loop
  
  //v without

  Double_t vres_all = -1;
  Double_t vres_01 = -1; 
  Double_t vres_02 = -1;
  Double_t vres_03 = -1;
  Double_t vres_04 = -1;
  Double_t vres_05 = -1;
  Double_t vres_06 = -1;
  Double_t vres_07 = -1;
  Double_t vres_08 = -1;

  Double_t err_vres_all = -1;
  Double_t err_vres_01 = -1;
  Double_t err_vres_02 = -1;
  Double_t err_vres_03 = -1;
  Double_t err_vres_04 = -1;
  Double_t err_vres_05 = -1;
  Double_t err_vres_06 = -1;
  Double_t err_vres_07 = -1;
  Double_t err_vres_08 = -1;

  //w without

  Double_t wres_all = -1;
  Double_t wres_01 = -1; 
  Double_t wres_02 = -1;
  Double_t wres_03 = -1;
  Double_t wres_04 = -1;
  Double_t wres_05 = -1;
  Double_t wres_06 = -1;
  Double_t wres_07 = -1;
  Double_t wres_08 = -1;

  Double_t err_wres_all = -1;
  Double_t err_wres_01 = -1;
  Double_t err_wres_02 = -1;
  Double_t err_wres_03 = -1;
  Double_t err_wres_04 = -1;
  Double_t err_wres_05 = -1;
  Double_t err_wres_06 = -1;
  Double_t err_wres_07 = -1;
  Double_t err_wres_08 = -1;

  //v using clusterSplit1.C

  Double_t vres_allw = -1;
  Double_t vres_01w = -1; 
  Double_t vres_02w = -1;
  Double_t vres_03w = -1;
  Double_t vres_04w = -1;
  Double_t vres_05w = -1;
  Double_t vres_06w = -1;
  Double_t vres_07w = -1;
  Double_t vres_08w = -1;

  Double_t err_vres_allw = -1;
  Double_t err_vres_01w = -1;
  Double_t err_vres_02w = -1;
  Double_t err_vres_03w = -1;
  Double_t err_vres_04w = -1;
  Double_t err_vres_05w = -1;
  Double_t err_vres_06w = -1;
  Double_t err_vres_07w = -1;
  Double_t err_vres_08w = -1;

  //w using clusterSplit1.C

  Double_t wres_allw = -1;
  Double_t wres_01w = -1; 
  Double_t wres_02w = -1;
  Double_t wres_03w = -1;
  Double_t wres_04w = -1;
  Double_t wres_05w = -1;
  Double_t wres_06w = -1;
  Double_t wres_07w = -1;
  Double_t wres_08w = -1;

  Double_t err_wres_allw = -1;
  Double_t err_wres_01w = -1;
  Double_t err_wres_02w = -1;
  Double_t err_wres_03w = -1;
  Double_t err_wres_04w = -1;
  Double_t err_wres_05w = -1;
  Double_t err_wres_06w = -1;
  Double_t err_wres_07w = -1;
  Double_t err_wres_08w = -1;
  

  //v without
  //vall

  TF1 *doublegaus_vall = new TF1("doublegaus_vall",doublegausf,-.1,.1,6);

  doublegaus_vall->SetParLimits(0,0,100);
  doublegaus_vall->SetParLimits(2,0.005,0.05);
  doublegaus_vall->SetParLimits(3,0,100);
  doublegaus_vall->SetParLimits(5,0.005,0.05);

  doublegaus_vall->SetParameters(20,vall->GetMean(),vall->GetRMS(),5,vall->GetMean(),vall->GetRMS());
  vall->Fit("doublegaus_vall","R");

  TF1 *vall_f1 = new TF1("vall_f1","gaus",-0.1,0.1);
  TF1 *vall_f2 = new TF1("vall_f2","gaus",-0.1,0.1);

  vall_f1->SetParameter(0,doublegaus_vall->GetParameter(0));
  vall_f1->SetParameter(1,doublegaus_vall->GetParameter(1));
  vall_f1->SetParameter(2,doublegaus_vall->GetParameter(2));
  vall_f1->SetParError(2,doublegaus_vall->GetParError(2));
  vall_f1->SetLineColor(kRed);
  vall_f2->SetParameter(0,doublegaus_vall->GetParameter(3));
  vall_f2->SetParameter(1,doublegaus_vall->GetParameter(4));
  vall_f2->SetParameter(2,doublegaus_vall->GetParameter(5));
  vall_f2->SetParError(2,doublegaus_vall->GetParError(5));
  vall_f2->SetLineColor(kBlue);
  
  Double_t intvall_1 = vall_f1->Integral(-.1,.1);
  Double_t intvall_2 = vall_f2->Integral(-.1,.1);
  
  vres_all = (intvall_1*fabs(vall_f1->GetParameter(2))+ intvall_2*fabs(vall_f2->GetParameter(2)))/(intvall_1 + intvall_2);

  err_vres_all  = (1./(intvall_1+intvall_2));
  err_vres_all *= sqrt(intvall_1*intvall_1*pow((vall_f1->GetParError(2)),2)+intvall_2*intvall_2*pow(vall_f2->GetParError(2),2));

  cout<<endl<<"Overall resolution v without: "<<vres_all<<" +- " <<err_vres_all<<endl<<endl;


  //v01

  TF1 *doublegaus_v01 = new TF1("doublegaus_v01",doublegausf,-.1,.1,6);

  doublegaus_v01->SetParLimits(0,0,100);
  doublegaus_v01->SetParLimits(2,0.005,0.05);
  doublegaus_v01->SetParLimits(3,0,100);
  doublegaus_v01->SetParLimits(5,0.005,0.05);

  doublegaus_v01->SetParameters(20,v01->GetMean(),v01->GetRMS(),5,v01->GetMean(),v01->GetRMS());
  v01->Fit("doublegaus_v01","R");

  TF1 *v01_f1 = new TF1("v01_f1","gaus",-0.1,0.1);
  TF1 *v01_f2 = new TF1("v01_f2","gaus",-0.1,0.1);

  v01_f1->SetParameter(0,doublegaus_v01->GetParameter(0));
  v01_f1->SetParameter(1,doublegaus_v01->GetParameter(1));
  v01_f1->SetParameter(2,doublegaus_v01->GetParameter(2));
  v01_f1->SetParError(2,doublegaus_v01->GetParError(2));
  v01_f1->SetLineColor(kRed);
  v01_f2->SetParameter(0,doublegaus_v01->GetParameter(3));
  v01_f2->SetParameter(1,doublegaus_v01->GetParameter(4));
  v01_f2->SetParameter(2,doublegaus_v01->GetParameter(5));
  v01_f2->SetParError(2,doublegaus_v01->GetParError(5));
  v01_f2->SetLineColor(kBlue);
  
  Double_t intv01_1 = v01_f1->Integral(-.1,.1);
  Double_t intv01_2 = v01_f2->Integral(-.1,.1);
  
  vres_01 = (intv01_1*fabs(v01_f1->GetParameter(2))+ intv01_2*fabs(v01_f2->GetParameter(2)))/(intv01_1 + intv01_2);

  err_vres_01  = (1./(intv01_1+intv01_2));
  err_vres_01 *= sqrt(intv01_1*intv01_1*pow((v01_f1->GetParError(2)),2)+intv01_2*intv01_2*pow(v01_f2->GetParError(2),2));

  cout<<endl<<"resolution v01: "<<vres_01<<" +- " <<err_vres_01<<endl<<endl;


  //v02

  TF1 *doublegaus_v02 = new TF1("doublegaus_v02",doublegausf,-.1,.1,6);

  doublegaus_v02->SetParLimits(0,0,100);
  doublegaus_v02->SetParLimits(2,0.005,0.05);
  doublegaus_v02->SetParLimits(3,0,100);
  doublegaus_v02->SetParLimits(5,0.005,0.05);

  doublegaus_v02->SetParameters(20,v02->GetMean(),v02->GetRMS(),5,v02->GetMean(),v02->GetRMS());
  v02->Fit("doublegaus_v02","R");

  TF1 *v02_f1 = new TF1("v02_f1","gaus",-0.1,0.1);
  TF1 *v02_f2 = new TF1("v02_f2","gaus",-0.1,0.1);

  v02_f1->SetParameter(0,doublegaus_v02->GetParameter(0));
  v02_f1->SetParameter(1,doublegaus_v02->GetParameter(1));
  v02_f1->SetParameter(2,doublegaus_v02->GetParameter(2));
  v02_f1->SetParError(2,doublegaus_v02->GetParError(2));
  v02_f1->SetLineColor(kRed);
  v02_f2->SetParameter(0,doublegaus_v02->GetParameter(3));
  v02_f2->SetParameter(1,doublegaus_v02->GetParameter(4));
  v02_f2->SetParameter(2,doublegaus_v02->GetParameter(5));
  v02_f2->SetParError(2,doublegaus_v02->GetParError(5));
  v02_f2->SetLineColor(kBlue);
  
  Double_t intv02_1 = v02_f1->Integral(-.1,.1);
  Double_t intv02_2 = v02_f2->Integral(-.1,.1);
  
  vres_02 = (intv02_1*fabs(v02_f1->GetParameter(2))+ intv02_2*fabs(v02_f2->GetParameter(2)))/(intv02_1 + intv02_2);

  err_vres_02  = (1./(intv02_1+intv02_2));
  err_vres_02 *= sqrt(intv02_1*intv02_1*pow((v02_f1->GetParError(2)),2)+intv02_2*intv02_2*pow(v02_f2->GetParError(2),2));

  cout<<endl<<"resolution v12: "<<vres_02<<" +- " <<err_vres_02<<endl<<endl;

  //v03

  TF1 *doublegaus_v03 = new TF1("doublegaus_v03",doublegausf,-.1,.1,6);

  doublegaus_v03->SetParLimits(0,0,100);
  doublegaus_v03->SetParLimits(2,0.005,0.05);
  doublegaus_v03->SetParLimits(3,0,100);
  doublegaus_v03->SetParLimits(5,0.005,0.05);

  doublegaus_v03->SetParameters(20,v03->GetMean(),v03->GetRMS(),5,v03->GetMean(),v03->GetRMS());
  v03->Fit("doublegaus_v03","R");

  TF1 *v03_f1 = new TF1("v03_f1","gaus",-0.1,0.1);
  TF1 *v03_f2 = new TF1("v03_f2","gaus",-0.1,0.1);

  v03_f1->SetParameter(0,doublegaus_v03->GetParameter(0));
  v03_f1->SetParameter(1,doublegaus_v03->GetParameter(1));
  v03_f1->SetParameter(2,doublegaus_v03->GetParameter(2));
  v03_f1->SetParError(2,doublegaus_v03->GetParError(2));
  v03_f1->SetLineColor(kRed);
  v03_f2->SetParameter(0,doublegaus_v03->GetParameter(3));
  v03_f2->SetParameter(1,doublegaus_v03->GetParameter(4));
  v03_f2->SetParameter(2,doublegaus_v03->GetParameter(5));
  v03_f2->SetParError(2,doublegaus_v03->GetParError(5));
  v03_f2->SetLineColor(kBlue);
  
  Double_t intv03_1 = v03_f1->Integral(-.1,.1);
  Double_t intv03_2 = v03_f2->Integral(-.1,.1);
  
  vres_03 = (intv03_1*fabs(v03_f1->GetParameter(2))+ intv03_2*fabs(v03_f2->GetParameter(2)))/(intv03_1 + intv03_2);

  err_vres_03  = (1./(intv03_1+intv03_2));
  err_vres_03 *= sqrt(intv03_1*intv03_1*pow((v03_f1->GetParError(2)),2)+intv03_2*intv03_2*pow(v03_f2->GetParError(2),2));

  cout<<endl<<"resolution v23: "<<vres_03<<" +- " <<err_vres_03<<endl<<endl;

  //v04

  TF1 *doublegaus_v04 = new TF1("doublegaus_v04",doublegausf,-.1,.1,6);

  doublegaus_v04->SetParLimits(0,0,100);
  doublegaus_v04->SetParLimits(2,0.005,0.05);
  doublegaus_v04->SetParLimits(3,0,100);
  doublegaus_v04->SetParLimits(5,0.005,0.05);

  doublegaus_v04->SetParameters(20,v04->GetMean(),v04->GetRMS(),5,v04->GetMean(),v04->GetRMS());
  v04->Fit("doublegaus_v04","R");

  TF1 *v04_f1 = new TF1("v04_f1","gaus",-0.1,0.1);
  TF1 *v04_f2 = new TF1("v04_f2","gaus",-0.1,0.1);

  v04_f1->SetParameter(0,doublegaus_v04->GetParameter(0));
  v04_f1->SetParameter(1,doublegaus_v04->GetParameter(1));
  v04_f1->SetParameter(2,doublegaus_v04->GetParameter(2));
  v04_f1->SetParError(2,doublegaus_v04->GetParError(2));
  v04_f1->SetLineColor(kRed);
  v04_f2->SetParameter(0,doublegaus_v04->GetParameter(3));
  v04_f2->SetParameter(1,doublegaus_v04->GetParameter(4));
  v04_f2->SetParameter(2,doublegaus_v04->GetParameter(5));
  v04_f2->SetParError(2,doublegaus_v04->GetParError(5));
  v04_f2->SetLineColor(kBlue);
  
  Double_t intv04_1 = v04_f1->Integral(-.1,.1);
  Double_t intv04_2 = v04_f2->Integral(-.1,.1);
  
  vres_04 = (intv04_1*fabs(v04_f1->GetParameter(2))+ intv04_2*fabs(v04_f2->GetParameter(2)))/(intv04_1 + intv04_2);

  err_vres_04  = (1./(intv04_1+intv04_2));
  err_vres_04 *= sqrt(intv04_1*intv04_1*pow((v04_f1->GetParError(2)),2)+intv04_2*intv04_2*pow(v04_f2->GetParError(2),2));

  cout<<endl<<"resolution v34: "<<vres_04<<" +- " <<err_vres_04<<endl<<endl;

  //v05

  TF1 *doublegaus_v05 = new TF1("doublegaus_v05",doublegausf,-.1,.1,6);

  doublegaus_v05->SetParLimits(0,0,100);
  doublegaus_v05->SetParLimits(2,0.005,0.05);
  doublegaus_v05->SetParLimits(3,0,100);
  doublegaus_v05->SetParLimits(5,0.005,0.05);

  doublegaus_v05->SetParameters(20,v05->GetMean(),v05->GetRMS(),5,v05->GetMean(),v05->GetRMS());
  v05->Fit("doublegaus_v05","R");

  TF1 *v05_f1 = new TF1("v05_f1","gaus",-0.1,0.1);
  TF1 *v05_f2 = new TF1("v05_f2","gaus",-0.1,0.1);

  v05_f1->SetParameter(0,doublegaus_v05->GetParameter(0));
  v05_f1->SetParameter(1,doublegaus_v05->GetParameter(1));
  v05_f1->SetParameter(2,doublegaus_v05->GetParameter(2));
  v05_f1->SetParError(2,doublegaus_v05->GetParError(2));
  v05_f1->SetLineColor(kRed);
  v05_f2->SetParameter(0,doublegaus_v05->GetParameter(3));
  v05_f2->SetParameter(1,doublegaus_v05->GetParameter(4));
  v05_f2->SetParameter(2,doublegaus_v05->GetParameter(5));
  v05_f2->SetParError(2,doublegaus_v05->GetParError(5));
  v05_f2->SetLineColor(kBlue);
  
  Double_t intv05_1 = v05_f1->Integral(-.1,.1);
  Double_t intv05_2 = v05_f2->Integral(-.1,.1);
  
  vres_05 = (intv05_1*fabs(v05_f1->GetParameter(2))+ intv05_2*fabs(v05_f2->GetParameter(2)))/(intv05_1 + intv05_2);

  err_vres_05 = (1./(intv05_1+intv05_2));
  err_vres_05 *= sqrt(intv05_1*intv05_1*pow((v05_f1->GetParError(2)),2)+intv05_2*intv05_2*pow(v05_f2->GetParError(2),2));

  cout<<endl<<"resolution v45: "<<vres_05<<" +- " <<err_vres_05<<endl<<endl;

  //v06

  TF1 *doublegaus_v06 = new TF1("doublegaus_v06",doublegausf,-.1,.1,6);

  doublegaus_v06->SetParLimits(0,0,100);
  doublegaus_v06->SetParLimits(2,0.005,0.05);
  doublegaus_v06->SetParLimits(3,0,100);
  doublegaus_v06->SetParLimits(5,0.005,0.05);

  doublegaus_v06->SetParameters(20,v06->GetMean(),v06->GetRMS(),5,v06->GetMean(),v06->GetRMS());
  v06->Fit("doublegaus_v06","R");

  TF1 *v06_f1 = new TF1("v06_f1","gaus",-0.1,0.1);
  TF1 *v06_f2 = new TF1("v06_f2","gaus",-0.1,0.1);

  v06_f1->SetParameter(0,doublegaus_v06->GetParameter(0));
  v06_f1->SetParameter(1,doublegaus_v06->GetParameter(1));
  v06_f1->SetParameter(2,doublegaus_v06->GetParameter(2));
  v06_f1->SetParError(2,doublegaus_v06->GetParError(2));
  v06_f1->SetLineColor(kRed);
  v06_f2->SetParameter(0,doublegaus_v06->GetParameter(3));
  v06_f2->SetParameter(1,doublegaus_v06->GetParameter(4));
  v06_f2->SetParameter(2,doublegaus_v06->GetParameter(5));
  v06_f2->SetParError(2,doublegaus_v06->GetParError(5));
  v06_f2->SetLineColor(kBlue);
  
  Double_t intv06_1 = v06_f1->Integral(-.1,.1);
  Double_t intv06_2 = v06_f2->Integral(-.1,.1);
  
  vres_06 = (intv06_1*fabs(v06_f1->GetParameter(2))+ intv06_2*fabs(v06_f2->GetParameter(2)))/(intv06_1 + intv06_2);

  err_vres_06  = (1./(intv06_1+intv06_2));
  err_vres_06 *= sqrt(intv06_1*intv06_1*pow((v06_f1->GetParError(2)),2)+intv06_2*intv06_2*pow(v06_f2->GetParError(2),2));

  cout<<endl<<"resolution v56: "<<vres_06<<" +- " <<err_vres_06<<endl<<endl;

  //v07

  TF1 *doublegaus_v07 = new TF1("doublegaus_v07",doublegausf,-.1,.1,6);

  doublegaus_v07->SetParLimits(0,0,100);
  doublegaus_v07->SetParLimits(2,0.005,0.05);
  doublegaus_v07->SetParLimits(3,0,100);
  doublegaus_v07->SetParLimits(5,0.005,0.05);

  doublegaus_v07->SetParameters(20,v07->GetMean(),v07->GetRMS(),5,v07->GetMean(),v07->GetRMS());
  v07->Fit("doublegaus_v07","R");

  TF1 *v07_f1 = new TF1("v07_f1","gaus",-0.1,0.1);
  TF1 *v07_f2 = new TF1("v07_f2","gaus",-0.1,0.1);

  v07_f1->SetParameter(0,doublegaus_v07->GetParameter(0));
  v07_f1->SetParameter(1,doublegaus_v07->GetParameter(1));
  v07_f1->SetParameter(2,doublegaus_v07->GetParameter(2));
  v07_f1->SetParError(2,doublegaus_v07->GetParError(2));
  v07_f1->SetLineColor(kRed);
  v07_f2->SetParameter(0,doublegaus_v07->GetParameter(3));
  v07_f2->SetParameter(1,doublegaus_v07->GetParameter(4));
  v07_f2->SetParameter(2,doublegaus_v07->GetParameter(5));
  v07_f2->SetParError(2,doublegaus_v07->GetParError(5));
  v07_f2->SetLineColor(kBlue);
  
  Double_t intv07_1 = v07_f1->Integral(-.1,.1);
  Double_t intv07_2 = v07_f2->Integral(-.1,.1);
  
  vres_07 = (intv07_1*fabs(v07_f1->GetParameter(2))+ intv07_2*fabs(v07_f2->GetParameter(2)))/(intv07_1 + intv07_2);

  err_vres_07  = (1./(intv07_1+intv07_2));
  err_vres_07 *= sqrt(intv07_1*intv07_1*pow((v07_f1->GetParError(2)),2)+intv07_2*intv07_2*pow(v07_f2->GetParError(2),2));

  cout<<endl<<"resolution v67: "<<vres_07<<" +- " <<err_vres_07<<endl<<endl;

  //v08

  TF1 *doublegaus_v08 = new TF1("doublegaus_v08",doublegausf,-.1,.1,6);

  doublegaus_v08->SetParLimits(0,0,100);
  doublegaus_v08->SetParLimits(2,0.005,0.05);
  doublegaus_v08->SetParLimits(3,0,100);
  doublegaus_v08->SetParLimits(5,0.005,0.05);

  doublegaus_v08->SetParameters(20,v08->GetMean(),v08->GetRMS(),5,v08->GetMean(),v08->GetRMS());
  v08->Fit("doublegaus_v08","R");

  TF1 *v08_f1 = new TF1("v08_f1","gaus",-0.1,0.1);
  TF1 *v08_f2 = new TF1("v08_f2","gaus",-0.1,0.1);

  v08_f1->SetParameter(0,doublegaus_v08->GetParameter(0));
  v08_f1->SetParameter(1,doublegaus_v08->GetParameter(1));
  v08_f1->SetParameter(2,doublegaus_v08->GetParameter(2));
  v08_f1->SetParError(2,doublegaus_v08->GetParError(2));
  v08_f1->SetLineColor(kRed);
  v08_f2->SetParameter(0,doublegaus_v08->GetParameter(3));
  v08_f2->SetParameter(1,doublegaus_v08->GetParameter(4));
  v08_f2->SetParameter(2,doublegaus_v08->GetParameter(5));
  v08_f2->SetParError(2,doublegaus_v08->GetParError(5));
  v08_f2->SetLineColor(kBlue);
  
  Double_t intv08_1 = v08_f1->Integral(-.1,.1);
  Double_t intv08_2 = v08_f2->Integral(-.1,.1);
  
  vres_08 = (intv08_1*fabs(v08_f1->GetParameter(2))+ intv08_2*fabs(v08_f2->GetParameter(2)))/(intv08_1 + intv08_2);

  err_vres_08  = (1./(intv08_1+intv08_2));
  err_vres_08 *= sqrt(intv08_1*intv08_1*pow((v08_f1->GetParError(2)),2)+intv08_2*intv08_2*pow(v08_f2->GetParError(2),2));

  cout<<endl<<"resolution v78: "<<vres_08<<" +- " <<err_vres_08<<endl<<endl;

  //w without
  //wall


  TF1 *doublegaus_wall = new TF1("doublegaus_wall",doublegausf,-.1,.1,6);

  doublegaus_wall->SetParameters(20,wall->GetMean(),wall->GetRMS(),5,wall->GetMean(),wall->GetRMS());
  wall->Fit("doublegaus_wall","R");

  TF1 *wall_f1 = new TF1("wall_f1","gaus",-0.1,0.1);
  TF1 *wall_f2 = new TF1("wall_f2","gaus",-0.1,0.1);

  wall_f1->SetParameter(0,doublegaus_wall->GetParameter(0));
  wall_f1->SetParameter(1,doublegaus_wall->GetParameter(1));
  wall_f1->SetParameter(2,doublegaus_wall->GetParameter(2));
  wall_f1->SetParError(2,doublegaus_wall->GetParError(2));
  wall_f1->SetLineColor(kRed);
  wall_f2->SetParameter(0,doublegaus_wall->GetParameter(3));
  wall_f2->SetParameter(1,doublegaus_wall->GetParameter(4));
  wall_f2->SetParameter(2,doublegaus_wall->GetParameter(5));
  wall_f2->SetParError(2,doublegaus_wall->GetParError(5));
  wall_f2->SetLineColor(kBlue);
  
  Double_t intwall_1 = wall_f1->Integral(-.1,.1);
  Double_t intwall_2 = wall_f2->Integral(-.1,.1);
  
  wres_all = (intwall_1*fabs(wall_f1->GetParameter(2))+ intwall_2*fabs(wall_f2->GetParameter(2)))/(intwall_1 + intwall_2);

  err_wres_all  = (1./(intwall_1+intwall_2));
  err_wres_all *= sqrt(intwall_1*intwall_1*pow((wall_f1->GetParError(2)),2)+intwall_2*intwall_2*pow(wall_f2->GetParError(2),2));

  cout<<endl<<"Overall resolution w without: "<<wres_all<<" +- " <<err_wres_all<<endl<<endl;


  //w01

  TF1 *doublegaus_w01 = new TF1("doublegaus_w01",doublegausf,-.1,.1,6);

  doublegaus_w01->SetParameters(20,w01->GetMean(),w01->GetRMS(),5,w01->GetMean(),w01->GetRMS());
  w01->Fit("doublegaus_w01","R");

  TF1 *w01_f1 = new TF1("w01_f1","gaus",-0.1,0.1);
  TF1 *w01_f2 = new TF1("w01_f2","gaus",-0.1,0.1);

  w01_f1->SetParameter(0,doublegaus_w01->GetParameter(0));
  w01_f1->SetParameter(1,doublegaus_w01->GetParameter(1));
  w01_f1->SetParameter(2,doublegaus_w01->GetParameter(2));
  w01_f1->SetParError(2,doublegaus_w01->GetParError(2));
  w01_f1->SetLineColor(kRed);
  w01_f2->SetParameter(0,doublegaus_w01->GetParameter(3));
  w01_f2->SetParameter(1,doublegaus_w01->GetParameter(4));
  w01_f2->SetParameter(2,doublegaus_w01->GetParameter(5));
  w01_f2->SetParError(2,doublegaus_w01->GetParError(5));
  w01_f2->SetLineColor(kBlue);
  
  Double_t intw01_1 = w01_f1->Integral(-.1,.1);
  Double_t intw01_2 = w01_f2->Integral(-.1,.1);
  
  wres_01 = (intw01_1*fabs(w01_f1->GetParameter(2))+ intw01_2*fabs(w01_f2->GetParameter(2)))/(intw01_1 + intw01_2);

  err_wres_01  = (1./(intw01_1+intw01_2));
  err_wres_01 *= sqrt(intw01_1*intw01_1*pow((w01_f1->GetParError(2)),2)+intw01_2*intw01_2*pow(w01_f2->GetParError(2),2));

  cout<<endl<<"resolution w01: "<<wres_01<<" +- " <<err_wres_01<<endl<<endl;


  //w02

  TF1 *doublegaus_w02 = new TF1("doublegaus_w02",doublegausf,-.1,.1,6);

  doublegaus_w02->SetParameters(20,w02->GetMean(),w02->GetRMS(),5,w02->GetMean(),w02->GetRMS());
  w02->Fit("doublegaus_w02","R");

  TF1 *w02_f1 = new TF1("w02_f1","gaus",-0.1,0.1);
  TF1 *w02_f2 = new TF1("w02_f2","gaus",-0.1,0.1);

  w02_f1->SetParameter(0,doublegaus_w02->GetParameter(0));
  w02_f1->SetParameter(1,doublegaus_w02->GetParameter(1));
  w02_f1->SetParameter(2,doublegaus_w02->GetParameter(2));
  w02_f1->SetParError(2,doublegaus_w02->GetParError(2));
  w02_f1->SetLineColor(kRed);
  w02_f2->SetParameter(0,doublegaus_w02->GetParameter(3));
  w02_f2->SetParameter(1,doublegaus_w02->GetParameter(4));
  w02_f2->SetParameter(2,doublegaus_w02->GetParameter(5));
  w02_f2->SetParError(2,doublegaus_w02->GetParError(5));
  w02_f2->SetLineColor(kBlue);
  
  Double_t intw02_1 = w02_f1->Integral(-.1,.1);
  Double_t intw02_2 = w02_f2->Integral(-.1,.1);
  
  wres_02 = (intw02_1*fabs(w02_f1->GetParameter(2))+ intw02_2*fabs(w02_f2->GetParameter(2)))/(intw02_1 + intw02_2);

  err_wres_02  = (1./(intw02_1+intw02_2));
  err_wres_02 *= sqrt(intw02_1*intw02_1*pow((w02_f1->GetParError(2)),2)+intw02_2*intw02_2*pow(w02_f2->GetParError(2),2));

  cout<<endl<<"resolution w12: "<<wres_02<<" +- " <<err_wres_02<<endl<<endl;

  //w03

  TF1 *doublegaus_w03 = new TF1("doublegaus_w03",doublegausf,-.1,.1,6);

  doublegaus_w03->SetParameters(20,w03->GetMean(),w03->GetRMS(),5,w03->GetMean(),w03->GetRMS());
  w03->Fit("doublegaus_w03","R");

  TF1 *w03_f1 = new TF1("w03_f1","gaus",-0.1,0.1);
  TF1 *w03_f2 = new TF1("w03_f2","gaus",-0.1,0.1);

  w03_f1->SetParameter(0,doublegaus_w03->GetParameter(0));
  w03_f1->SetParameter(1,doublegaus_w03->GetParameter(1));
  w03_f1->SetParameter(2,doublegaus_w03->GetParameter(2));
  w03_f1->SetParError(2,doublegaus_w03->GetParError(2));
  w03_f1->SetLineColor(kRed);
  w03_f2->SetParameter(0,doublegaus_w03->GetParameter(3));
  w03_f2->SetParameter(1,doublegaus_w03->GetParameter(4));
  w03_f2->SetParameter(2,doublegaus_w03->GetParameter(5));
  w03_f2->SetParError(2,doublegaus_w03->GetParError(5));
  w03_f2->SetLineColor(kBlue);
  
  Double_t intw03_1 = w03_f1->Integral(-.1,.1);
  Double_t intw03_2 = w03_f2->Integral(-.1,.1);
  
  wres_03 = (intw03_1*fabs(w03_f1->GetParameter(2))+ intw03_2*fabs(w03_f2->GetParameter(2)))/(intw03_1 + intw03_2);

  err_wres_03  = (1./(intw03_1+intw03_2));
  err_wres_03 *= sqrt(intw03_1*intw03_1*pow((w03_f1->GetParError(2)),2)+intw03_2*intw03_2*pow(w03_f2->GetParError(2),2));

  cout<<endl<<"resolution w23: "<<wres_03<<" +- " <<err_wres_03<<endl<<endl;

  //w04

  TF1 *doublegaus_w04 = new TF1("doublegaus_w04",doublegausf,-.1,.1,6);

  doublegaus_w04->SetParameters(20,w04->GetMean(),w04->GetRMS(),5,w04->GetMean(),w04->GetRMS());
  w04->Fit("doublegaus_w04","R");

  TF1 *w04_f1 = new TF1("w04_f1","gaus",-0.1,0.1);
  TF1 *w04_f2 = new TF1("w04_f2","gaus",-0.1,0.1);

  w04_f1->SetParameter(0,doublegaus_w04->GetParameter(0));
  w04_f1->SetParameter(1,doublegaus_w04->GetParameter(1));
  w04_f1->SetParameter(2,doublegaus_w04->GetParameter(2));
  w04_f1->SetParError(2,doublegaus_w04->GetParError(2));
  w04_f1->SetLineColor(kRed);
  w04_f2->SetParameter(0,doublegaus_w04->GetParameter(3));
  w04_f2->SetParameter(1,doublegaus_w04->GetParameter(4));
  w04_f2->SetParameter(2,doublegaus_w04->GetParameter(5));
  w04_f2->SetParError(2,doublegaus_w04->GetParError(5));
  w04_f2->SetLineColor(kBlue);
  
  Double_t intw04_1 = w04_f1->Integral(-.1,.1);
  Double_t intw04_2 = w04_f2->Integral(-.1,.1);
  
  wres_04 = (intw04_1*fabs(w04_f1->GetParameter(2))+ intw04_2*fabs(w04_f2->GetParameter(2)))/(intw04_1 + intw04_2);

  err_wres_04  = (1./(intw04_1+intw04_2));
  err_wres_04 *= sqrt(intw04_1*intw04_1*pow((w04_f1->GetParError(2)),2)+intw04_2*intw04_2*pow(w04_f2->GetParError(2),2));

  cout<<endl<<"resolution w34: "<<wres_04<<" +- " <<err_wres_04<<endl<<endl;

  //w05

  TF1 *doublegaus_w05 = new TF1("doublegaus_w05",doublegausf,-.1,.1,6);

  doublegaus_w05->SetParameters(20,w05->GetMean(),w05->GetRMS(),5,w05->GetMean(),w05->GetRMS());
  w05->Fit("doublegaus_w05","R");

  TF1 *w05_f1 = new TF1("w05_f1","gaus",-0.1,0.1);
  TF1 *w05_f2 = new TF1("w05_f2","gaus",-0.1,0.1);

  w05_f1->SetParameter(0,doublegaus_w05->GetParameter(0));
  w05_f1->SetParameter(1,doublegaus_w05->GetParameter(1));
  w05_f1->SetParameter(2,doublegaus_w05->GetParameter(2));
  w05_f1->SetParError(2,doublegaus_w05->GetParError(2));
  w05_f1->SetLineColor(kRed);
  w05_f2->SetParameter(0,doublegaus_w05->GetParameter(3));
  w05_f2->SetParameter(1,doublegaus_w05->GetParameter(4));
  w05_f2->SetParameter(2,doublegaus_w05->GetParameter(5));
  w05_f2->SetParError(2,doublegaus_w05->GetParError(5));
  w05_f2->SetLineColor(kBlue);
  
  Double_t intw05_1 = w05_f1->Integral(-.1,.1);
  Double_t intw05_2 = w05_f2->Integral(-.1,.1);
  
  wres_05 = (intw05_1*fabs(w05_f1->GetParameter(2))+ intw05_2*fabs(w05_f2->GetParameter(2)))/(intw05_1 + intw05_2);

  err_wres_05  = (1./(intw05_1+intw05_2));
  err_wres_05 *= sqrt(intw05_1*intw05_1*pow((w05_f1->GetParError(2)),2)+intw05_2*intw05_2*pow(w05_f2->GetParError(2),2));

  cout<<endl<<"resolution w45: "<<wres_05<<" +- " <<err_wres_05<<endl<<endl;

  //w06

  TF1 *doublegaus_w06 = new TF1("doublegaus_w06",doublegausf,-.1,.1,6);

  doublegaus_w06->SetParameters(20,w06->GetMean(),w06->GetRMS(),5,w06->GetMean(),w06->GetRMS());
  w06->Fit("doublegaus_w06","R");

  TF1 *w06_f1 = new TF1("w06_f1","gaus",-0.1,0.1);
  TF1 *w06_f2 = new TF1("w06_f2","gaus",-0.1,0.1);

  w06_f1->SetParameter(0,doublegaus_w06->GetParameter(0));
  w06_f1->SetParameter(1,doublegaus_w06->GetParameter(1));
  w06_f1->SetParameter(2,doublegaus_w06->GetParameter(2));
  w06_f1->SetParError(2,doublegaus_w06->GetParError(2));
  w06_f1->SetLineColor(kRed);
  w06_f2->SetParameter(0,doublegaus_w06->GetParameter(3));
  w06_f2->SetParameter(1,doublegaus_w06->GetParameter(4));
  w06_f2->SetParameter(2,doublegaus_w06->GetParameter(5));
  w06_f2->SetParError(2,doublegaus_w06->GetParError(5));
  w06_f2->SetLineColor(kBlue);
  
  Double_t intw06_1 = w06_f1->Integral(-.1,.1);
  Double_t intw06_2 = w06_f2->Integral(-.1,.1);
  
  wres_06 = (intw06_1*fabs(w06_f1->GetParameter(2))+ intw06_2*fabs(w06_f2->GetParameter(2)))/(intw06_1 + intw06_2);

  err_wres_06  = (1./(intw06_1+intw06_2));
  err_wres_06 *= sqrt(intw06_1*intw06_1*pow((w06_f1->GetParError(2)),2)+intw06_2*intw06_2*pow(w06_f2->GetParError(2),2));

  cout<<endl<<"resolution w56: "<<wres_06<<" +- " <<err_wres_06<<endl<<endl;

  //w07

  TF1 *doublegaus_w07 = new TF1("doublegaus_w07",doublegausf,-.1,.1,6);

  doublegaus_w07->SetParameters(20,w07->GetMean(),w07->GetRMS(),5,w07->GetMean(),w07->GetRMS());
  w07->Fit("doublegaus_w07","R");

  TF1 *w07_f1 = new TF1("w07_f1","gaus",-0.1,0.1);
  TF1 *w07_f2 = new TF1("w07_f2","gaus",-0.1,0.1);

  w07_f1->SetParameter(0,doublegaus_w07->GetParameter(0));
  w07_f1->SetParameter(1,doublegaus_w07->GetParameter(1));
  w07_f1->SetParameter(2,doublegaus_w07->GetParameter(2));
  w07_f1->SetParError(2,doublegaus_w07->GetParError(2));
  w07_f1->SetLineColor(kRed);
  w07_f2->SetParameter(0,doublegaus_w07->GetParameter(3));
  w07_f2->SetParameter(1,doublegaus_w07->GetParameter(4));
  w07_f2->SetParameter(2,doublegaus_w07->GetParameter(5));
  w07_f2->SetParError(2,doublegaus_w07->GetParError(5));
  w07_f2->SetLineColor(kBlue);
  
  Double_t intw07_1 = w07_f1->Integral(-.1,.1);
  Double_t intw07_2 = w07_f2->Integral(-.1,.1);
  
  wres_07 = (intw07_1*fabs(w07_f1->GetParameter(2))+ intw07_2*fabs(w07_f2->GetParameter(2)))/(intw07_1 + intw07_2);

  err_wres_07  = (1./(intw07_1+intw07_2));
  err_wres_07 *= sqrt(intw07_1*intw07_1*pow((w07_f1->GetParError(2)),2)+intw07_2*intw07_2*pow(w07_f2->GetParError(2),2));

  cout<<endl<<"resolution w67: "<<wres_07<<" +- " <<err_wres_07<<endl<<endl;

  //w08

  TF1 *doublegaus_w08 = new TF1("doublegaus_w08",doublegausf,-.1,.1,6);

  doublegaus_w08->SetParameters(20,w08->GetMean(),w08->GetRMS(),5,w08->GetMean(),w08->GetRMS());
  w08->Fit("doublegaus_w08","R");

  TF1 *w08_f1 = new TF1("w08_f1","gaus",-0.1,0.1);
  TF1 *w08_f2 = new TF1("w08_f2","gaus",-0.1,0.1);

  w08_f1->SetParameter(0,doublegaus_w08->GetParameter(0));
  w08_f1->SetParameter(1,doublegaus_w08->GetParameter(1));
  w08_f1->SetParameter(2,doublegaus_w08->GetParameter(2));
  w08_f1->SetParError(2,doublegaus_w08->GetParError(2));
  w08_f1->SetLineColor(kRed);
  w08_f2->SetParameter(0,doublegaus_w08->GetParameter(3));
  w08_f2->SetParameter(1,doublegaus_w08->GetParameter(4));
  w08_f2->SetParameter(2,doublegaus_w08->GetParameter(5));
  w08_f2->SetParError(2,doublegaus_w08->GetParError(5));
  w08_f2->SetLineColor(kBlue);
  
  Double_t intw08_1 = w08_f1->Integral(-.1,.1);
  Double_t intw08_2 = w08_f2->Integral(-.1,.1);
  
  wres_08 = (intw08_1*fabs(w08_f1->GetParameter(2))+ intw08_2*fabs(w08_f2->GetParameter(2)))/(intw08_1 + intw08_2);

  err_wres_08  = (1./(intw08_1+intw08_2));
  err_wres_08 *= sqrt(intw08_1*intw08_1*pow((w08_f1->GetParError(2)),2)+intw08_2*intw08_2*pow(w08_f2->GetParError(2),2));

  cout<<endl<<"resolution w78: "<<wres_08<<" +- " <<err_wres_08<<endl<<endl;

  //v using clusterSplit1.C
  //vallw

  TF1 *doublegaus_vallw = new TF1("doublegaus_vallw",doublegausf,-.1,.1,6);

  doublegaus_vallw->SetParLimits(0,0,10000);
  doublegaus_vallw->SetParLimits(2,0.005,0.05);
  doublegaus_vallw->SetParLimits(3,0,10000);
  doublegaus_vallw->SetParLimits(5,0.005,0.05);

  doublegaus_vallw->SetParameters(20,vallw->GetMean(),vallw->GetRMS(),5,vallw->GetMean(),vallw->GetRMS());
  vallw->Fit("doublegaus_vallw","R");

  TF1 *vallw_f1 = new TF1("vallw_f1","gaus",-0.1,0.1);
  TF1 *vallw_f2 = new TF1("vallw_f2","gaus",-0.1,0.1);

  vallw_f1->SetParameter(0,doublegaus_vallw->GetParameter(0));
  vallw_f1->SetParameter(1,doublegaus_vallw->GetParameter(1));
  vallw_f1->SetParameter(2,doublegaus_vallw->GetParameter(2));
  vallw_f1->SetParError(2,doublegaus_vallw->GetParError(2));
  vallw_f1->SetLineColor(kRed);
  vallw_f2->SetParameter(0,doublegaus_vallw->GetParameter(3));
  vallw_f2->SetParameter(1,doublegaus_vallw->GetParameter(4));
  vallw_f2->SetParameter(2,doublegaus_vallw->GetParameter(5));
  vallw_f2->SetParError(2,doublegaus_vallw->GetParError(5));
  vallw_f2->SetLineColor(kBlue);
  
  Double_t intvallw_1 = vallw_f1->Integral(-.1,.1);
  Double_t intvallw_2 = vallw_f2->Integral(-.1,.1);
  
  vres_allw = (intvallw_1*fabs(vallw_f1->GetParameter(2))+ intvallw_2*fabs(vallw_f2->GetParameter(2)))/(intvallw_1 + intvallw_2);

  err_vres_allw  = (1./(intvallw_1+intvallw_2));
  err_vres_allw *= sqrt(intvallw_1*intvallw_1*pow((vallw_f1->GetParError(2)),2)+intvallw_2*intvallw_2*pow(vallw_f2->GetParError(2),2));

  cout<<endl<<"Overall resolution v using clusterSplit1.C: "<<vres_allw<<" +- " <<err_vres_allw<<endl<<endl;

  //v01w

  TF1 *doublegaus_v01w = new TF1("doublegaus_v01w",doublegausf,-.1,.1,6);

  doublegaus_v01w->SetParLimits(0,0,100);
  doublegaus_v01w->SetParLimits(2,0.005,0.05);
  doublegaus_v01w->SetParLimits(3,0,100);
  doublegaus_v01w->SetParLimits(5,0.005,0.05);

  doublegaus_v01w->SetParameters(20,v01w->GetMean(),v01w->GetRMS(),5,v01w->GetMean(),v01w->GetRMS());
  v01w->Fit("doublegaus_v01w","R");

  TF1 *v01w_f1 = new TF1("v01w_f1","gaus",-0.1,0.1);
  TF1 *v01w_f2 = new TF1("v01w_f2","gaus",-0.1,0.1);

  v01w_f1->SetParameter(0,doublegaus_v01w->GetParameter(0));
  v01w_f1->SetParameter(1,doublegaus_v01w->GetParameter(1));
  v01w_f1->SetParameter(2,doublegaus_v01w->GetParameter(2));
  v01w_f1->SetParError(2,doublegaus_v01w->GetParError(2));
  v01w_f1->SetLineColor(kRed);
  v01w_f2->SetParameter(0,doublegaus_v01w->GetParameter(3));
  v01w_f2->SetParameter(1,doublegaus_v01w->GetParameter(4));
  v01w_f2->SetParameter(2,doublegaus_v01w->GetParameter(5));
  v01w_f2->SetParError(2,doublegaus_v01w->GetParError(5));
  v01w_f2->SetLineColor(kBlue);
  
  Double_t intv01w_1 = v01w_f1->Integral(-.1,.1);
  Double_t intv01w_2 = v01w_f2->Integral(-.1,.1);
  
  vres_01w = (intv01w_1*fabs(v01w_f1->GetParameter(2))+ intv01w_2*fabs(v01w_f2->GetParameter(2)))/(intv01w_1 + intv01w_2);

  err_vres_01w  = (1./(intv01w_1+intv01w_2));
  err_vres_01w *= sqrt(intv01w_1*intv01w_1*pow((v01w_f1->GetParError(2)),2)+intv01w_2*intv01w_2*pow(v01w_f2->GetParError(2),2));

  cout<<endl<<"resolution vw01: "<<vres_01w<<" +- " <<err_vres_01w<<endl<<endl;

  //v02w

  TF1 *doublegaus_v02w = new TF1("doublegaus_v02w",doublegausf,-.1,.1,6);

  doublegaus_v02w->SetParLimits(0,0,100);
  doublegaus_v02w->SetParLimits(2,0.005,0.05);
  doublegaus_v02w->SetParLimits(3,0,100);
  doublegaus_v02w->SetParLimits(5,0.005,0.05);

  doublegaus_v02w->SetParameters(20,v02w->GetMean(),v02w->GetRMS(),5,v02w->GetMean(),v02w->GetRMS());
  v02w->Fit("doublegaus_v02w","R");

  TF1 *v02w_f1 = new TF1("v02w_f1","gaus",-0.1,0.1);
  TF1 *v02w_f2 = new TF1("v02w_f2","gaus",-0.1,0.1);

  v02w_f1->SetParameter(0,doublegaus_v02w->GetParameter(0));
  v02w_f1->SetParameter(1,doublegaus_v02w->GetParameter(1));
  v02w_f1->SetParameter(2,doublegaus_v02w->GetParameter(2));
  v02w_f1->SetParError(2,doublegaus_v02w->GetParError(2));
  v02w_f1->SetLineColor(kRed);
  v02w_f2->SetParameter(0,doublegaus_v02w->GetParameter(3));
  v02w_f2->SetParameter(1,doublegaus_v02w->GetParameter(4));
  v02w_f2->SetParameter(2,doublegaus_v02w->GetParameter(5));
  v02w_f2->SetParError(2,doublegaus_v02w->GetParError(5));
  v02w_f2->SetLineColor(kBlue);
  
  Double_t intv02w_1 = v02w_f1->Integral(-.1,.1);
  Double_t intv02w_2 = v02w_f2->Integral(-.1,.1);
  
  vres_02w = (intv02w_1*fabs(v02w_f1->GetParameter(2))+ intv02w_2*fabs(v02w_f2->GetParameter(2)))/(intv02w_1 + intv02w_2);

  err_vres_02w  = (1./(intv02w_1+intv02w_2));
  err_vres_02w *= sqrt(intv02w_1*intv02w_1*pow((v02w_f1->GetParError(2)),2)+intv02w_2*intv02w_2*pow(v02w_f2->GetParError(2),2));

  cout<<endl<<"resolution vw12: "<<vres_02w<<" +- " <<err_vres_02w<<endl<<endl;

  //v03w

  TF1 *doublegaus_v03w = new TF1("doublegaus_v03w",doublegausf,-.1,.1,6);

  doublegaus_v03w->SetParLimits(0,0,100);
  doublegaus_v03w->SetParLimits(2,0.005,0.05);
  doublegaus_v03w->SetParLimits(3,0,100);
  doublegaus_v03w->SetParLimits(5,0.005,0.05);

  doublegaus_v03w->SetParameters(20,v03w->GetMean(),v03w->GetRMS(),5,v03w->GetMean(),v03w->GetRMS());
  v03w->Fit("doublegaus_v03w","R");

  TF1 *v03w_f1 = new TF1("v03w_f1","gaus",-0.1,0.1);
  TF1 *v03w_f2 = new TF1("v03w_f2","gaus",-0.1,0.1);

  v03w_f1->SetParameter(0,doublegaus_v03w->GetParameter(0));
  v03w_f1->SetParameter(1,doublegaus_v03w->GetParameter(1));
  v03w_f1->SetParameter(2,doublegaus_v03w->GetParameter(2));
  v03w_f1->SetParError(2,doublegaus_v03w->GetParError(2));
  v03w_f1->SetLineColor(kRed);
  v03w_f2->SetParameter(0,doublegaus_v03w->GetParameter(3));
  v03w_f2->SetParameter(1,doublegaus_v03w->GetParameter(4));
  v03w_f2->SetParameter(2,doublegaus_v03w->GetParameter(5));
  v03w_f2->SetParError(2,doublegaus_v03w->GetParError(5));
  v03w_f2->SetLineColor(kBlue);
  
  Double_t intv03w_1 = v03w_f1->Integral(-.1,.1);
  Double_t intv03w_2 = v03w_f2->Integral(-.1,.1);
  
  vres_03w = (intv03w_1*fabs(v03w_f1->GetParameter(2))+ intv03w_2*fabs(v03w_f2->GetParameter(2)))/(intv03w_1 + intv03w_2);

  err_vres_03w  = (1./(intv03w_1+intv03w_2));
  err_vres_03w *= sqrt(intv03w_1*intv03w_1*pow((v03w_f1->GetParError(2)),2)+intv03w_2*intv03w_2*pow(v03w_f2->GetParError(2),2));

  cout<<endl<<"resolution vw23: "<<vres_03w<<" +- " <<err_vres_03w<<endl<<endl;

  //v04w

  TF1 *doublegaus_v04w = new TF1("doublegaus_v04w",doublegausf,-.1,.1,6);

  doublegaus_v04w->SetParLimits(0,0,100);
  doublegaus_v04w->SetParLimits(2,0.005,0.05);
  doublegaus_v04w->SetParLimits(3,0,100);
  doublegaus_v04w->SetParLimits(5,0.005,0.05);

  doublegaus_v04w->SetParameters(20,v04w->GetMean(),v04w->GetRMS(),5,v04w->GetMean(),v04w->GetRMS());
  v04w->Fit("doublegaus_v04w","R");

  TF1 *v04w_f1 = new TF1("v04w_f1","gaus",-0.1,0.1);
  TF1 *v04w_f2 = new TF1("v04w_f2","gaus",-0.1,0.1);

  v04w_f1->SetParameter(0,doublegaus_v04w->GetParameter(0));
  v04w_f1->SetParameter(1,doublegaus_v04w->GetParameter(1));
  v04w_f1->SetParameter(2,doublegaus_v04w->GetParameter(2));
  v04w_f1->SetParError(2,doublegaus_v04w->GetParError(2));
  v04w_f1->SetLineColor(kRed);
  v04w_f2->SetParameter(0,doublegaus_v04w->GetParameter(3));
  v04w_f2->SetParameter(1,doublegaus_v04w->GetParameter(4));
  v04w_f2->SetParameter(2,doublegaus_v04w->GetParameter(5));
  v04w_f2->SetParError(2,doublegaus_v04w->GetParError(5));
  v04w_f2->SetLineColor(kBlue);
  
  Double_t intv04w_1 = v04w_f1->Integral(-.1,.1);
  Double_t intv04w_2 = v04w_f2->Integral(-.1,.1);
  
  vres_04w = (intv04w_1*fabs(v04w_f1->GetParameter(2))+ intv04w_2*fabs(v04w_f2->GetParameter(2)))/(intv04w_1 + intv04w_2);

  err_vres_04w  = (1./(intv04w_1+intv04w_2));
  err_vres_04w *= sqrt(intv04w_1*intv04w_1*pow((v04w_f1->GetParError(2)),2)+intv04w_2*intv04w_2*pow(v04w_f2->GetParError(2),2));

  cout<<endl<<"resolution vw34: "<<vres_04w<<" +- " <<err_vres_04w<<endl<<endl;

  //v05w

  TF1 *doublegaus_v05w = new TF1("doublegaus_v05w",doublegausf,-.1,.1,6);

  doublegaus_v05w->SetParLimits(0,0,100);
  doublegaus_v05w->SetParLimits(2,0.005,0.05);
  doublegaus_v05w->SetParLimits(3,0,100);
  doublegaus_v05w->SetParLimits(5,0.005,0.05);

  doublegaus_v05w->SetParameters(20,v05w->GetMean(),v05w->GetRMS(),5,v05w->GetMean(),v05w->GetRMS());
  v05w->Fit("doublegaus_v05w","R");

  TF1 *v05w_f1 = new TF1("v05w_f1","gaus",-0.1,0.1);
  TF1 *v05w_f2 = new TF1("v05w_f2","gaus",-0.1,0.1);

  v05w_f1->SetParameter(0,doublegaus_v05w->GetParameter(0));
  v05w_f1->SetParameter(1,doublegaus_v05w->GetParameter(1));
  v05w_f1->SetParameter(2,doublegaus_v05w->GetParameter(2));
  v05w_f1->SetParError(2,doublegaus_v05w->GetParError(2));
  v05w_f1->SetLineColor(kRed);
  v05w_f2->SetParameter(0,doublegaus_v05w->GetParameter(3));
  v05w_f2->SetParameter(1,doublegaus_v05w->GetParameter(4));
  v05w_f2->SetParameter(2,doublegaus_v05w->GetParameter(5));
  v05w_f2->SetParError(2,doublegaus_v05w->GetParError(5));
  v05w_f2->SetLineColor(kBlue);
  
  Double_t intv05w_1 = v05w_f1->Integral(-.1,.1);
  Double_t intv05w_2 = v05w_f2->Integral(-.1,.1);
  
  vres_05w = (intv05w_1*fabs(v05w_f1->GetParameter(2))+ intv05w_2*fabs(v05w_f2->GetParameter(2)))/(intv05w_1 + intv05w_2);

  err_vres_05w  = (1./(intv05w_1+intv05w_2));
  err_vres_05w *= sqrt(intv05w_1*intv05w_1*pow((v05w_f1->GetParError(2)),2)+intv05w_2*intv05w_2*pow(v05w_f2->GetParError(2),2));

  cout<<endl<<"resolution vw45: "<<vres_05w<<" +- " <<err_vres_05w<<endl<<endl;

  //v06w

  TF1 *doublegaus_v06w = new TF1("doublegaus_v06w",doublegausf,-.1,.1,6);

  doublegaus_v06w->SetParLimits(0,0,100);
  doublegaus_v06w->SetParLimits(2,0.005,0.05);
  doublegaus_v06w->SetParLimits(3,0,100);
  doublegaus_v06w->SetParLimits(5,0.005,0.05);

  doublegaus_v06w->SetParameters(20,v06w->GetMean(),v06w->GetRMS(),5,v06w->GetMean(),v06w->GetRMS());
  v06w->Fit("doublegaus_v06w","R");

  TF1 *v06w_f1 = new TF1("v06w_f1","gaus",-0.1,0.1);
  TF1 *v06w_f2 = new TF1("v06w_f2","gaus",-0.1,0.1);

  v06w_f1->SetParameter(0,doublegaus_v06w->GetParameter(0));
  v06w_f1->SetParameter(1,doublegaus_v06w->GetParameter(1));
  v06w_f1->SetParameter(2,doublegaus_v06w->GetParameter(2));
  v06w_f1->SetParError(2,doublegaus_v06w->GetParError(2));
  v06w_f1->SetLineColor(kRed);
  v06w_f2->SetParameter(0,doublegaus_v06w->GetParameter(3));
  v06w_f2->SetParameter(1,doublegaus_v06w->GetParameter(4));
  v06w_f2->SetParameter(2,doublegaus_v06w->GetParameter(5));
  v06w_f2->SetParError(2,doublegaus_v06w->GetParError(5));
  v06w_f2->SetLineColor(kBlue);
  
  Double_t intv06w_1 = v06w_f1->Integral(-.1,.1);
  Double_t intv06w_2 = v06w_f2->Integral(-.1,.1);
  
  vres_06w = (intv06w_1*fabs(v06w_f1->GetParameter(2))+ intv06w_2*fabs(v06w_f2->GetParameter(2)))/(intv06w_1 + intv06w_2);

  err_vres_06w  = (1./(intv06w_1+intv06w_2));
  err_vres_06w *= sqrt(intv06w_1*intv06w_1*pow((v06w_f1->GetParError(2)),2)+intv06w_2*intv06w_2*pow(v06w_f2->GetParError(2),2));

  cout<<endl<<"resolution vw56: "<<vres_06w<<" +- " <<err_vres_06w<<endl<<endl;

  //v07w

  TF1 *doublegaus_v07w = new TF1("doublegaus_v07w",doublegausf,-.1,.1,6);

  doublegaus_v07w->SetParLimits(0,0,100);
  doublegaus_v07w->SetParLimits(2,0.005,0.05);
  doublegaus_v07w->SetParLimits(3,0,100);
  doublegaus_v07w->SetParLimits(5,0.005,0.05);

  doublegaus_v07w->SetParameters(20,v07w->GetMean(),v07w->GetRMS(),5,v07w->GetMean(),v07w->GetRMS());
  v07w->Fit("doublegaus_v07w","R");

  TF1 *v07w_f1 = new TF1("v07w_f1","gaus",-0.1,0.1);
  TF1 *v07w_f2 = new TF1("v07w_f2","gaus",-0.1,0.1);

  v07w_f1->SetParameter(0,doublegaus_v07w->GetParameter(0));
  v07w_f1->SetParameter(1,doublegaus_v07w->GetParameter(1));
  v07w_f1->SetParameter(2,doublegaus_v07w->GetParameter(2));
  v07w_f1->SetParError(2,doublegaus_v07w->GetParError(2));
  v07w_f1->SetLineColor(kRed);
  v07w_f2->SetParameter(0,doublegaus_v07w->GetParameter(3));
  v07w_f2->SetParameter(1,doublegaus_v07w->GetParameter(4));
  v07w_f2->SetParameter(2,doublegaus_v07w->GetParameter(5));
  v07w_f2->SetParError(2,doublegaus_v07w->GetParError(5));
  v07w_f2->SetLineColor(kBlue);
  
  Double_t intv07w_1 = v07w_f1->Integral(-.1,.1);
  Double_t intv07w_2 = v07w_f2->Integral(-.1,.1);
  
  vres_07w = (intv07w_1*fabs(v07w_f1->GetParameter(2))+ intv07w_2*fabs(v07w_f2->GetParameter(2)))/(intv07w_1 + intv07w_2);

  err_vres_07w  = (1./(intv07w_1+intv07w_2));
  err_vres_07w *= sqrt(intv07w_1*intv07w_1*pow((v07w_f1->GetParError(2)),2)+intv07w_2*intv07w_2*pow(v07w_f2->GetParError(2),2));

  cout<<endl<<"resolution vw67: "<<vres_07w<<" +- " <<err_vres_07w<<endl<<endl;

  //v08w

  TF1 *doublegaus_v08w = new TF1("doublegaus_v08w",doublegausf,-.1,.1,6);

  doublegaus_v08w->SetParLimits(0,0,100);
  doublegaus_v08w->SetParLimits(2,0.005,0.05);
  doublegaus_v08w->SetParLimits(3,0,100);
  doublegaus_v08w->SetParLimits(5,0.005,0.05);

  doublegaus_v08w->SetParameters(20,v08w->GetMean(),v08w->GetRMS(),5,v08w->GetMean(),v08w->GetRMS());
  v08w->Fit("doublegaus_v08w","R");

  TF1 *v08w_f1 = new TF1("v08w_f1","gaus",-0.1,0.1);
  TF1 *v08w_f2 = new TF1("v08w_f2","gaus",-0.1,0.1);

  v08w_f1->SetParameter(0,doublegaus_v08w->GetParameter(0));
  v08w_f1->SetParameter(1,doublegaus_v08w->GetParameter(1));
  v08w_f1->SetParameter(2,doublegaus_v08w->GetParameter(2));
  v08w_f1->SetParError(2,doublegaus_v08w->GetParError(2));
  v08w_f1->SetLineColor(kRed);
  v08w_f2->SetParameter(0,doublegaus_v08w->GetParameter(3));
  v08w_f2->SetParameter(1,doublegaus_v08w->GetParameter(4));
  v08w_f2->SetParameter(2,doublegaus_v08w->GetParameter(5));
  v08w_f2->SetParError(2,doublegaus_v08w->GetParError(5));
  v08w_f2->SetLineColor(kBlue);
  
  Double_t intv08w_1 = v08w_f1->Integral(-.1,.1);
  Double_t intv08w_2 = v08w_f2->Integral(-.1,.1);
  
  vres_08w = (intv08w_1*fabs(v08w_f1->GetParameter(2))+ intv08w_2*fabs(v08w_f2->GetParameter(2)))/(intv08w_1 + intv08w_2);

  err_vres_08w  = (1./(intv08w_1+intv08w_2));
  err_vres_08w *= sqrt(intv08w_1*intv08w_1*pow((v08w_f1->GetParError(2)),2)+intv08w_2*intv08w_2*pow(v08w_f2->GetParError(2),2));

  cout<<endl<<"resolution vw78: "<<vres_08w<<" +- " <<err_vres_08w<<endl<<endl;

  //w using clusterSplit1.C
  //wallw

  TF1 *doublegaus_wallw = new TF1("doublegaus_wallw",doublegausf,-.1,.1,6);

  doublegaus_wallw->SetParameters(20,wallw->GetMean(),wallw->GetRMS(),5,wallw->GetMean(),wallw->GetRMS());
  wallw->Fit("doublegaus_wallw","R");

  TF1 *wallw_f1 = new TF1("wallw_f1","gaus",-0.1,0.1);
  TF1 *wallw_f2 = new TF1("wallw_f2","gaus",-0.1,0.1);

  wallw_f1->SetParameter(0,doublegaus_wallw->GetParameter(0));
  wallw_f1->SetParameter(1,doublegaus_wallw->GetParameter(1));
  wallw_f1->SetParameter(2,doublegaus_wallw->GetParameter(2));
  wallw_f1->SetParError(2,doublegaus_wallw->GetParError(2));
  wallw_f1->SetLineColor(kRed);
  wallw_f2->SetParameter(0,doublegaus_wallw->GetParameter(3));
  wallw_f2->SetParameter(1,doublegaus_wallw->GetParameter(4));
  wallw_f2->SetParameter(2,doublegaus_wallw->GetParameter(5));
  wallw_f2->SetParError(2,doublegaus_wallw->GetParError(5));
  wallw_f2->SetLineColor(kBlue);
  
  Double_t intwallw_1 = wallw_f1->Integral(-.1,.1);
  Double_t intwallw_2 = wallw_f2->Integral(-.1,.1);
  
  wres_allw = (intwallw_1*fabs(wallw_f1->GetParameter(2))+ intwallw_2*fabs(wallw_f2->GetParameter(2)))/(intwallw_1 + intwallw_2);

  err_wres_allw  = (1./(intwallw_1+intwallw_2));
  err_wres_allw *= sqrt(intwallw_1*intwallw_1*pow((wallw_f1->GetParError(2)),2)+intwallw_2*intwallw_2*pow(wallw_f2->GetParError(2),2));

  cout<<endl<<"Overall resolution w using clusterSplit1.C: "<<wres_allw<<" +- " <<err_wres_allw<<endl<<endl;

  //w01w

  TF1 *doublegaus_w01w = new TF1("doublegaus_w01w",doublegausf,-.1,.1,6);

  doublegaus_w01w->SetParameters(20,w01w->GetMean(),w01w->GetRMS(),5,w01w->GetMean(),w01w->GetRMS());
  w01w->Fit("doublegaus_w01w","R");

  TF1 *w01w_f1 = new TF1("w01w_f1","gaus",-0.1,0.1);
  TF1 *w01w_f2 = new TF1("w01w_f2","gaus",-0.1,0.1);

  w01w_f1->SetParameter(0,doublegaus_w01w->GetParameter(0));
  w01w_f1->SetParameter(1,doublegaus_w01w->GetParameter(1));
  w01w_f1->SetParameter(2,doublegaus_w01w->GetParameter(2));
  w01w_f1->SetParError(2,doublegaus_w01w->GetParError(2));
  w01w_f1->SetLineColor(kRed);
  w01w_f2->SetParameter(0,doublegaus_w01w->GetParameter(3));
  w01w_f2->SetParameter(1,doublegaus_w01w->GetParameter(4));
  w01w_f2->SetParameter(2,doublegaus_w01w->GetParameter(5));
  w01w_f2->SetParError(2,doublegaus_w01w->GetParError(5));
  w01w_f2->SetLineColor(kBlue);
  
  Double_t intw01w_1 = w01w_f1->Integral(-.1,.1);
  Double_t intw01w_2 = w01w_f2->Integral(-.1,.1);
  
  wres_01w = (intw01w_1*fabs(w01w_f1->GetParameter(2))+ intw01w_2*fabs(w01w_f2->GetParameter(2)))/(intw01w_1 + intw01w_2);

  err_wres_01w  = (1./(intw01w_1+intw01w_2));
  err_wres_01w *= sqrt(intw01w_1*intw01w_1*pow((w01w_f1->GetParError(2)),2)+intw01w_2*intw01w_2*pow(w01w_f2->GetParError(2),2));

  cout<<endl<<"resolution ww01: "<<wres_01w<<" +- " <<err_wres_01w<<endl<<endl;

  //w02w

  TF1 *doublegaus_w02w = new TF1("doublegaus_w02w",doublegausf,-.1,.1,6);

  doublegaus_w02w->SetParameters(20,w02w->GetMean(),w02w->GetRMS(),5,w02w->GetMean(),w02w->GetRMS());
  w02w->Fit("doublegaus_w02w","R");

  TF1 *w02w_f1 = new TF1("w02w_f1","gaus",-0.1,0.1);
  TF1 *w02w_f2 = new TF1("w02w_f2","gaus",-0.1,0.1);

  w02w_f1->SetParameter(0,doublegaus_w02w->GetParameter(0));
  w02w_f1->SetParameter(1,doublegaus_w02w->GetParameter(1));
  w02w_f1->SetParameter(2,doublegaus_w02w->GetParameter(2));
  w02w_f1->SetParError(2,doublegaus_w02w->GetParError(2));
  w02w_f1->SetLineColor(kRed);
  w02w_f2->SetParameter(0,doublegaus_w02w->GetParameter(3));
  w02w_f2->SetParameter(1,doublegaus_w02w->GetParameter(4));
  w02w_f2->SetParameter(2,doublegaus_w02w->GetParameter(5));
  w02w_f2->SetParError(2,doublegaus_w02w->GetParError(5));
  w02w_f2->SetLineColor(kBlue);
  
  Double_t intw02w_1 = w02w_f1->Integral(-.1,.1);
  Double_t intw02w_2 = w02w_f2->Integral(-.1,.1);
  
  wres_02w = (intw02w_1*fabs(w02w_f1->GetParameter(2))+ intw02w_2*fabs(w02w_f2->GetParameter(2)))/(intw02w_1 + intw02w_2);

  err_wres_02w  = (1./(intw02w_1+intw02w_2));
  err_wres_02w *= sqrt(intw02w_1*intw02w_1*pow((w02w_f1->GetParError(2)),2)+intw02w_2*intw02w_2*pow(w02w_f2->GetParError(2),2));

  cout<<endl<<"resolution ww12: "<<wres_02w<<" +- " <<err_wres_02w<<endl<<endl;

  //w03w

  TF1 *doublegaus_w03w = new TF1("doublegaus_w03w",doublegausf,-.1,.1,6);

   doublegaus_w03w->SetParameters(20,w03w->GetMean(),w03w->GetRMS(),5,w03w->GetMean(),w03w->GetRMS());
  w03w->Fit("doublegaus_w03w","R");

  TF1 *w03w_f1 = new TF1("w03w_f1","gaus",-0.1,0.1);
  TF1 *w03w_f2 = new TF1("w03w_f2","gaus",-0.1,0.1);

  w03w_f1->SetParameter(0,doublegaus_w03w->GetParameter(0));
  w03w_f1->SetParameter(1,doublegaus_w03w->GetParameter(1));
  w03w_f1->SetParameter(2,doublegaus_w03w->GetParameter(2));
  w03w_f1->SetParError(2,doublegaus_w03w->GetParError(2));
  w03w_f1->SetLineColor(kRed);
  w03w_f2->SetParameter(0,doublegaus_w03w->GetParameter(3));
  w03w_f2->SetParameter(1,doublegaus_w03w->GetParameter(4));
  w03w_f2->SetParameter(2,doublegaus_w03w->GetParameter(5));
  w03w_f2->SetParError(2,doublegaus_w03w->GetParError(5));
  w03w_f2->SetLineColor(kBlue);
  
  Double_t intw03w_1 = w03w_f1->Integral(-.1,.1);
  Double_t intw03w_2 = w03w_f2->Integral(-.1,.1);
  
  wres_03w = (intw03w_1*fabs(w03w_f1->GetParameter(2))+ intw03w_2*fabs(w03w_f2->GetParameter(2)))/(intw03w_1 + intw03w_2);

  err_wres_03w  = (1./(intw03w_1+intw03w_2));
  err_wres_03w *= sqrt(intw03w_1*intw03w_1*pow((w03w_f1->GetParError(2)),2)+intw03w_2*intw03w_2*pow(w03w_f2->GetParError(2),2));

  cout<<endl<<"resolution ww23: "<<wres_03w<<" +- " <<err_wres_03w<<endl<<endl;

  //w04w

  TF1 *doublegaus_w04w = new TF1("doublegaus_w04w",doublegausf,-.1,.1,6);

  doublegaus_w04w->SetParameters(20,w04w->GetMean(),w04w->GetRMS(),5,w04w->GetMean(),w04w->GetRMS());
  w04w->Fit("doublegaus_w04w","R");

  TF1 *w04w_f1 = new TF1("w04w_f1","gaus",-0.1,0.1);
  TF1 *w04w_f2 = new TF1("w04w_f2","gaus",-0.1,0.1);

  w04w_f1->SetParameter(0,doublegaus_w04w->GetParameter(0));
  w04w_f1->SetParameter(1,doublegaus_w04w->GetParameter(1));
  w04w_f1->SetParameter(2,doublegaus_w04w->GetParameter(2));
  w04w_f1->SetParError(2,doublegaus_w04w->GetParError(2));
  w04w_f1->SetLineColor(kRed);
  w04w_f2->SetParameter(0,doublegaus_w04w->GetParameter(3));
  w04w_f2->SetParameter(1,doublegaus_w04w->GetParameter(4));
  w04w_f2->SetParameter(2,doublegaus_w04w->GetParameter(5));
  w04w_f2->SetParError(2,doublegaus_w04w->GetParError(5));
  w04w_f2->SetLineColor(kBlue);
  
  Double_t intw04w_1 = w04w_f1->Integral(-.1,.1);
  Double_t intw04w_2 = w04w_f2->Integral(-.1,.1);
  
  wres_04w = (intw04w_1*fabs(w04w_f1->GetParameter(2))+ intw04w_2*fabs(w04w_f2->GetParameter(2)))/(intw04w_1 + intw04w_2);

  err_wres_04w  = (1./(intw04w_1+intw04w_2));
  err_wres_04w *= sqrt(intw04w_1*intw04w_1*pow((w04w_f1->GetParError(2)),2)+intw04w_2*intw04w_2*pow(w04w_f2->GetParError(2),2));

  cout<<endl<<"resolution ww34: "<<wres_04w<<" +- " <<err_wres_04w<<endl<<endl;

  //w05w

  TF1 *doublegaus_w05w = new TF1("doublegaus_w05w",doublegausf,-.1,.1,6);

  doublegaus_w05w->SetParameters(20,w05w->GetMean(),w05w->GetRMS(),5,w05w->GetMean(),w05w->GetRMS());
  w05w->Fit("doublegaus_w05w","R");

  TF1 *w05w_f1 = new TF1("w05w_f1","gaus",-0.1,0.1);
  TF1 *w05w_f2 = new TF1("w05w_f2","gaus",-0.1,0.1);

  w05w_f1->SetParameter(0,doublegaus_w05w->GetParameter(0));
  w05w_f1->SetParameter(1,doublegaus_w05w->GetParameter(1));
  w05w_f1->SetParameter(2,doublegaus_w05w->GetParameter(2));
  w05w_f1->SetParError(2,doublegaus_w05w->GetParError(2));
  w05w_f1->SetLineColor(kRed);
  w05w_f2->SetParameter(0,doublegaus_w05w->GetParameter(3));
  w05w_f2->SetParameter(1,doublegaus_w05w->GetParameter(4));
  w05w_f2->SetParameter(2,doublegaus_w05w->GetParameter(5));
  w05w_f2->SetParError(2,doublegaus_w05w->GetParError(5));
  w05w_f2->SetLineColor(kBlue);
  
  Double_t intw05w_1 = w05w_f1->Integral(-.1,.1);
  Double_t intw05w_2 = w05w_f2->Integral(-.1,.1);
  
  wres_05w = (intw05w_1*fabs(w05w_f1->GetParameter(2))+ intw05w_2*fabs(w05w_f2->GetParameter(2)))/(intw05w_1 + intw05w_2);

  err_wres_05w  = (1./(intw05w_1+intw05w_2));
  err_wres_05w *= sqrt(intw05w_1*intw05w_1*pow((w05w_f1->GetParError(2)),2)+intw05w_2*intw05w_2*pow(w05w_f2->GetParError(2),2));

  cout<<endl<<"resolution ww45: "<<wres_05w<<" +- " <<err_wres_05w<<endl<<endl;

  //w06w

  TF1 *doublegaus_w06w = new TF1("doublegaus_w06w",doublegausf,-.1,.1,6);

  doublegaus_w06w->SetParameters(20,w06w->GetMean(),w06w->GetRMS(),5,w06w->GetMean(),w06w->GetRMS());
  w06w->Fit("doublegaus_w06w","R");

  TF1 *w06w_f1 = new TF1("w06w_f1","gaus",-0.1,0.1);
  TF1 *w06w_f2 = new TF1("w06w_f2","gaus",-0.1,0.1);

  w06w_f1->SetParameter(0,doublegaus_w06w->GetParameter(0));
  w06w_f1->SetParameter(1,doublegaus_w06w->GetParameter(1));
  w06w_f1->SetParameter(2,doublegaus_w06w->GetParameter(2));
  w06w_f1->SetParError(2,doublegaus_w06w->GetParError(2));
  w06w_f1->SetLineColor(kRed);
  w06w_f2->SetParameter(0,doublegaus_w06w->GetParameter(3));
  w06w_f2->SetParameter(1,doublegaus_w06w->GetParameter(4));
  w06w_f2->SetParameter(2,doublegaus_w06w->GetParameter(5));
  w06w_f2->SetParError(2,doublegaus_w06w->GetParError(5));
  w06w_f2->SetLineColor(kBlue);
  
  Double_t intw06w_1 = w06w_f1->Integral(-.1,.1);
  Double_t intw06w_2 = w06w_f2->Integral(-.1,.1);
  
  wres_06w = (intw06w_1*fabs(w06w_f1->GetParameter(2))+ intw06w_2*fabs(w06w_f2->GetParameter(2)))/(intw06w_1 + intw06w_2);

  err_wres_06w  = (1./(intw06w_1+intw06w_2));
  err_wres_06w *= sqrt(intw06w_1*intw06w_1*pow((w06w_f1->GetParError(2)),2)+intw06w_2*intw06w_2*pow(w06w_f2->GetParError(2),2));

  cout<<endl<<"resolution ww56: "<<wres_06w<<" +- " <<err_wres_06w<<endl<<endl;

  //w07w

  TF1 *doublegaus_w07w = new TF1("doublegaus_w07w",doublegausf,-.1,.1,6);

  doublegaus_w07w->SetParameters(20,w07w->GetMean(),w07w->GetRMS(),5,w07w->GetMean(),w07w->GetRMS());
  w07w->Fit("doublegaus_w07w","R");

  TF1 *w07w_f1 = new TF1("w07w_f1","gaus",-0.1,0.1);
  TF1 *w07w_f2 = new TF1("w07w_f2","gaus",-0.1,0.1);

  w07w_f1->SetParameter(0,doublegaus_w07w->GetParameter(0));
  w07w_f1->SetParameter(1,doublegaus_w07w->GetParameter(1));
  w07w_f1->SetParameter(2,doublegaus_w07w->GetParameter(2));
  w07w_f1->SetParError(2,doublegaus_w07w->GetParError(2));
  w07w_f1->SetLineColor(kRed);
  w07w_f2->SetParameter(0,doublegaus_w07w->GetParameter(3));
  w07w_f2->SetParameter(1,doublegaus_w07w->GetParameter(4));
  w07w_f2->SetParameter(2,doublegaus_w07w->GetParameter(5));
  w07w_f2->SetParError(2,doublegaus_w07w->GetParError(5));
  w07w_f2->SetLineColor(kBlue);
  
  Double_t intw07w_1 = w07w_f1->Integral(-.1,.1);
  Double_t intw07w_2 = w07w_f2->Integral(-.1,.1);
  
  wres_07w = (intw07w_1*fabs(w07w_f1->GetParameter(2))+ intw07w_2*fabs(w07w_f2->GetParameter(2)))/(intw07w_1 + intw07w_2);

  err_wres_07w  = (1./(intw07w_1+intw07w_2));
  err_wres_07w *= sqrt(intw07w_1*intw07w_1*pow((w07w_f1->GetParError(2)),2)+intw07w_2*intw07w_2*pow(w07w_f2->GetParError(2),2));

  cout<<endl<<"resolution ww67: "<<wres_07w<<" +- " <<err_wres_07w<<endl<<endl;

  //w08w

  TF1 *doublegaus_w08w = new TF1("doublegaus_w08w",doublegausf,-.1,.1,6);

  doublegaus_w08w->SetParameters(20,w08w->GetMean(),w08w->GetRMS(),5,w08w->GetMean(),w08w->GetRMS());
  w08w->Fit("doublegaus_w08w","R");

  TF1 *w08w_f1 = new TF1("w08w_f1","gaus",-0.1,0.1);
  TF1 *w08w_f2 = new TF1("w08w_f2","gaus",-0.1,0.1);

  w08w_f1->SetParameter(0,doublegaus_w08w->GetParameter(0));
  w08w_f1->SetParameter(1,doublegaus_w08w->GetParameter(1));
  w08w_f1->SetParameter(2,doublegaus_w08w->GetParameter(2));
  w08w_f1->SetParError(2,doublegaus_w08w->GetParError(2));
  w08w_f1->SetLineColor(kRed);
  w08w_f2->SetParameter(0,doublegaus_w08w->GetParameter(3));
  w08w_f2->SetParameter(1,doublegaus_w08w->GetParameter(4));
  w08w_f2->SetParameter(2,doublegaus_w08w->GetParameter(5));
  w08w_f2->SetParError(2,doublegaus_w08w->GetParError(5));
  w08w_f2->SetLineColor(kBlue);
  
  Double_t intw08w_1 = w08w_f1->Integral(-.1,.1);
  Double_t intw08w_2 = w08w_f2->Integral(-.1,.1);
  
  wres_08w = (intw08w_1*fabs(w08w_f1->GetParameter(2))+ intw08w_2*fabs(w08w_f2->GetParameter(2)))/(intw08w_1 + intw08w_2);

  err_wres_08w  = (1./(intw08w_1+intw08w_2));
  err_wres_08w *= sqrt(intw08w_1*intw08w_1*pow((w08w_f1->GetParError(2)),2)+intw08w_2*intw08w_2*pow(w08w_f2->GetParError(2),2));

  cout<<endl<<"resolution ww78: "<<wres_08w<<" +- " <<err_wres_08w<<endl<<endl;


  //Draw

  //v without

  TCanvas *canvas = new TCanvas();
  vall->Draw();
  doublegaus_vall->Draw("same");
  vall_f1->Draw("same");
  vall_f2->Draw("same");
  canvas = new TCanvas();
  v01->Draw();
  doublegaus_v01->Draw("same");
  v01_f1->Draw("same");
  v01_f2->Draw("same");
  canvas = new TCanvas();
  v02->Draw();
  doublegaus_v02->Draw("same");
  v02_f1->Draw("same");
  v02_f2->Draw("same");
  canvas = new TCanvas();
  v03->Draw();
  doublegaus_v03->Draw("same");
  v03_f1->Draw("same");
  v03_f2->Draw("same");
  canvas = new TCanvas();
  v04->Draw();
  doublegaus_v04->Draw("same");
  v04_f1->Draw("same");
  v04_f2->Draw("same");
  canvas = new TCanvas();
  v05->Draw();
  doublegaus_v05->Draw("same");
  v05_f1->Draw("same");
  v05_f2->Draw("same");
  canvas = new TCanvas();
  v06->Draw();
  doublegaus_v06->Draw("same");
  v06_f1->Draw("same");
  v06_f2->Draw("same");
  canvas = new TCanvas();
  v07->Draw();
  doublegaus_v07->Draw("same");
  v07_f1->Draw("same");
  v07_f2->Draw("same");
  canvas = new TCanvas();
  v08->Draw();
  doublegaus_v08->Draw("same");
  v08_f1->Draw("same");
  v08_f2->Draw("same");

  //w without

  canvas = new TCanvas();
  wall->Draw();
  doublegaus_wall->Draw("same");
  wall_f1->Draw("same");
  wall_f2->Draw("same");
  canvas = new TCanvas();
  w01->Draw();
  doublegaus_w01->Draw("same");
  w01_f1->Draw("same");
  w01_f2->Draw("same");
  canvas = new TCanvas();
  w02->Draw();
  doublegaus_w02->Draw("same");
  w02_f1->Draw("same");
  w02_f2->Draw("same");
  canvas = new TCanvas();
  w03->Draw();
  doublegaus_w03->Draw("same");
  w03_f1->Draw("same");
  w03_f2->Draw("same");
  canvas = new TCanvas();
  w04->Draw();
  doublegaus_w04->Draw("same");
  w04_f1->Draw("same");
  w04_f2->Draw("same");
  canvas = new TCanvas();
  w05->Draw();
  doublegaus_w05->Draw("same");
  w05_f1->Draw("same");
  w05_f2->Draw("same");
  canvas = new TCanvas();
  w06->Draw();
  doublegaus_w06->Draw("same");
  w06_f1->Draw("same");
  w06_f2->Draw("same");
  canvas = new TCanvas();
  w07->Draw();
  doublegaus_w07->Draw("same");
  w07_f1->Draw("same");
  w07_f2->Draw("same");
  canvas = new TCanvas();
  w08->Draw();
  doublegaus_w08->Draw("same");
  w08_f1->Draw("same");
  w08_f2->Draw("same");

  //v using clusterSplit1.C

  canvas = new TCanvas();
  vallw->Draw();
  doublegaus_vallw->Draw("same");
  vallw_f1->Draw("same");
  vallw_f2->Draw("same");
  canvas = new TCanvas();
  v01w->Draw();
  doublegaus_v01w->Draw("same");
  v01w_f1->Draw("same");
  v01w_f2->Draw("same");
  canvas = new TCanvas();
  v02w->Draw();
  doublegaus_v02w->Draw("same");
  v02w_f1->Draw("same");
  v02w_f2->Draw("same");
  canvas = new TCanvas();
  v03w->Draw();
  doublegaus_v03w->Draw("same");
  v03w_f1->Draw("same");
  v03w_f2->Draw("same");
  canvas = new TCanvas();
  v04w->Draw();
  doublegaus_v04w->Draw("same");
  v04w_f1->Draw("same");
  v04w_f2->Draw("same");
  canvas = new TCanvas();
  v05w->Draw();
  doublegaus_v05w->Draw("same");
  v05w_f1->Draw("same");
  v05w_f2->Draw("same");
  canvas = new TCanvas();
  v06w->Draw();
  doublegaus_v06w->Draw("same");
  v06w_f1->Draw("same");
  v06w_f2->Draw("same");
  canvas = new TCanvas();
  v07w->Draw();
  doublegaus_v07w->Draw("same");
  v07w_f1->Draw("same");
  v07w_f2->Draw("same");
  canvas = new TCanvas();
  v08w->Draw();
  doublegaus_v08w->Draw("same");
  v08w_f1->Draw("same");
  v08w_f2->Draw("same");

  //w using clusterSplit1.C

  canvas = new TCanvas();
  wallw->Draw();
  doublegaus_wallw->Draw("same");
  wallw_f1->Draw("same");
  wallw_f2->Draw("same");
  canvas = new TCanvas();
  w01w->Draw();
  doublegaus_w01w->Draw("same");
  w01w_f1->Draw("same");
  w01w_f2->Draw("same");
  canvas = new TCanvas();
  w02w->Draw();
  doublegaus_w02w->Draw("same");
  w02w_f1->Draw("same");
  w02w_f2->Draw("same");
  canvas = new TCanvas();
  w03w->Draw();
  doublegaus_w03w->Draw("same");
  w03w_f1->Draw("same");
  w03w_f2->Draw("same");
  canvas = new TCanvas();
  w04w->Draw();
  doublegaus_w04w->Draw("same");
  w04w_f1->Draw("same");
  w04w_f2->Draw("same");
  canvas = new TCanvas();
  w05w->Draw();
  doublegaus_w05w->Draw("same");
  w05w_f1->Draw("same");
  w05w_f2->Draw("same");
  canvas = new TCanvas();
  w06w->Draw();
  doublegaus_w06w->Draw("same");
  w06w_f1->Draw("same");
  w06w_f2->Draw("same");
  canvas = new TCanvas();
  w07w->Draw();
  doublegaus_w07w->Draw("same");
  w07w_f1->Draw("same");
  w07w_f2->Draw("same");
  canvas = new TCanvas();
  w08w->Draw();
  doublegaus_w08w->Draw("same");
  w08w_f1->Draw("same");
  w08w_f2->Draw("same");


  //overview graphs

  //v without

  double vx[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  double vxx[8] = {5, 15, 25, 35, 45, 55, 65, 73.5};
  double vex[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  double vexx[8] = {5, 5, 5, 5, 5, 5, 5, 3.5};
  double vy[8] = {vres_01*10000, vres_02*10000, vres_03*10000, vres_04*10000, 
                  vres_05*10000, vres_06*10000, vres_07*10000, vres_08*10000};
  double vyy[8] = {vres_01*10, vres_02*10, vres_03*10, vres_04*10, 
                   vres_05*10, vres_06*10, vres_07*10, vres_08*10};
  double vey[8] = {err_vres_01*10000, err_vres_02*10000, 
                   err_vres_03*10000, err_vres_04*10000, 
                   err_vres_05*10000, err_vres_06*10000, 
                   err_vres_07*10000, err_vres_08*10000};
  double veyy[8] = {err_vres_01*10, err_vres_02*10, 
                    err_vres_03*10, err_vres_04*10, 
                    err_vres_05*10, err_vres_06*10, 
                    err_vres_07*10, err_vres_08*10};

  TH2D *hr1v = new TH2D("hr1v","hr1v",8,0.5,8.5, 10,0,500);
  hr1v->SetStats(kFALSE);
  hr1v->SetTitle("");
  hr1v->SetYTitle("Spatial Resolution v (#mum)");
  hr1v->SetLabelOffset(0.01, "X");
  hr1v->SetLabelSize(0.05, "X");

  TAxis *v_xaxis = hr1v->GetXaxis();
  v_xaxis->SetBinLabel(1," 0 < w < 10mm");
  v_xaxis->SetBinLabel(2,"10 < w < 20mm");
  v_xaxis->SetBinLabel(3,"20 < w < 30mm");
  v_xaxis->SetBinLabel(4,"30 < w < 40mm");
  v_xaxis->SetBinLabel(5,"40 < w < 50mm");
  v_xaxis->SetBinLabel(6,"50 < w < 60mm");
  v_xaxis->SetBinLabel(7,"60 < w < 70mm");
  v_xaxis->SetBinLabel(8,"70 < w < 80mm");

  TGraphErrors *vres_vs_w = new TGraphErrors(8, vx, vy, vex, vey);
  vres_vs_w->SetTitle("");
  vres_vs_w->SetMarkerStyle(21);
  vres_vs_w->SetMarkerColor(2);
  vres_vs_w->SetMarkerSize(1);
  canvas = new TCanvas();
  hr1v->Draw();
  vres_vs_w->Draw("p");

  TGraphErrors *vres_vs_w2 = new TGraphErrors(8, vxx, vyy, vexx, veyy);
  vres_vs_w2->SetTitle("");
  vres_vs_w2->SetMarkerStyle(21);
  vres_vs_w2->SetMarkerColor(2);
  vres_vs_w2->SetMarkerSize(1);
  canvas = new TCanvas();
  vres_vs_w2->Draw("ap");


  //w without

  double wx[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  double wxx[8] = {5, 15, 25, 35, 45, 55, 65, 73.5};
  double wex[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  double wexx[8] = {5, 5, 5, 5, 5, 5, 5, 3.5};
  double wy[8] = {wres_01*10000, wres_02*10000, wres_03*10000, wres_04*10000, 
                  wres_05*10000, wres_06*10000, wres_07*10000, wres_08*10000};
  double wyy[8] = {wres_01*10, wres_02*10, wres_03*10, wres_04*10, 
                   wres_05*10, wres_06*10, wres_07*10, wres_08*10};
  double wey[8] = {err_wres_01*10000, err_wres_02*10000,  
                   err_wres_03*10000, err_wres_04*10000, 
                   err_wres_05*10000, err_wres_06*10000, 
                   err_wres_07*10000, err_wres_08*10000};
  double weyy[8] = {err_wres_01*10, err_wres_02*10, 
                    err_wres_03*10, err_wres_04*10, 
                    err_wres_05*10, err_wres_06*10, 
                    err_wres_07*10, err_wres_08*10};

  TH2D *hr1w = new TH2D("hr1w","hr1w",8,0.5,8.5, 10,0,500);
  hr1w->SetStats(kFALSE);
  hr1w->SetTitle("");
  hr1w->SetYTitle("Spatial Resolution w (#mum)");
  hr1w->SetLabelOffset(0.01, "X");
  hr1w->SetLabelSize(0.05, "X");

  TAxis *w_xaxis = hr1w->GetXaxis();
  w_xaxis->SetBinLabel(1," 0 < w < 10mm");
  w_xaxis->SetBinLabel(2,"10 < w < 20mm");
  w_xaxis->SetBinLabel(3,"20 < w < 30mm");
  w_xaxis->SetBinLabel(4,"30 < w < 40mm");
  w_xaxis->SetBinLabel(5,"40 < w < 50mm");
  w_xaxis->SetBinLabel(6,"50 < w < 60mm");
  w_xaxis->SetBinLabel(7,"60 < w < 70mm");
  w_xaxis->SetBinLabel(8,"70 < w < 80mm");

  TGraphErrors *wres_vs_w = new TGraphErrors(8, wx, wy, wex, wey);
  wres_vs_w->SetTitle("");
  wres_vs_w->SetMarkerStyle(21);
  wres_vs_w->SetMarkerColor(2);
  wres_vs_w->SetMarkerSize(1);
  canvas = new TCanvas();
  hr1w->Draw();
  wres_vs_w->Draw("p");

  TGraphErrors *wres_vs_w2 = new TGraphErrors(8, wxx, wyy, wexx, weyy);
  wres_vs_w2->SetTitle("");
  wres_vs_w2->SetMarkerStyle(21);
  wres_vs_w2->SetMarkerColor(2);
  wres_vs_w2->SetMarkerSize(1);
  canvas = new TCanvas();
  wres_vs_w2->Draw("ap");


  //v using clusterSplit1.C

  double vwx[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  double vwxx[8] = {5, 15, 25, 35, 45, 55, 65, 73.5};
  double vwex[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  double vwexx[8] = {5, 5, 5, 5, 5, 5, 5, 3.5};
  double vwy[8] = {vres_01w*10000, vres_02w*10000, vres_03w*10000, vres_04w*10000,
                   vres_05w*10000, vres_06w*10000, vres_07w*10000, vres_08w*10000};
  double vwyy[8] = {vres_01w*10, vres_02w*10, vres_03w*10, vres_04w*10, 
		    vres_05w*10, vres_06w*10, vres_07w*10, vres_08w*10};
  double vwey[8] = {err_vres_01w*10000, err_vres_02w*10000,
		    err_vres_03w*10000, err_vres_04w*10000,
		    err_vres_05w*10000, err_vres_06w*10000,
		    err_vres_07w*10000, err_vres_08w*10000};
  double vweyy[8] = {err_vres_01w*10, err_vres_02w*10,
		     err_vres_03w*10, err_vres_04w*10,
		     err_vres_05w*10, err_vres_06w*10,
		     err_vres_07w*10, err_vres_08w*10};

  TH2D *hr1vw = new TH2D("hr1vw","hr1vw",8,0.5,8.5, 10,0,500);
  hr1vw->SetStats(kFALSE);
  hr1vw->SetTitle("");
  hr1vw->SetYTitle("Spatial Resolution v using clusterSplit1.C (#mum)");
  hr1vw->SetLabelOffset(0.01, "X");
  hr1vw->SetLabelSize(0.05, "X");

  TAxis *vw_xaxis = hr1vw->GetXaxis();
  vw_xaxis->SetBinLabel(1," 0 < w < 10mm");
  vw_xaxis->SetBinLabel(2,"10 < w < 20mm");
  vw_xaxis->SetBinLabel(3,"20 < w < 30mm");
  vw_xaxis->SetBinLabel(4,"30 < w < 40mm");
  vw_xaxis->SetBinLabel(5,"40 < w < 50mm");
  vw_xaxis->SetBinLabel(6,"50 < w < 60mm");
  vw_xaxis->SetBinLabel(7,"60 < w < 70mm");
  vw_xaxis->SetBinLabel(8,"70 < w < 80mm");

  TGraphErrors *vresw_vs_w = new TGraphErrors(8, vwx,vwy, vwex, vwey);
  vresw_vs_w->SetTitle("");
  vresw_vs_w->SetMarkerStyle(21);
  vresw_vs_w->SetMarkerColor(2);
  vresw_vs_w->SetMarkerSize(1);
  canvas = new TCanvas();
  hr1vw->Draw();
  vresw_vs_w->Draw("p");

  TGraphErrors *vresw_vs_w2 = new TGraphErrors(8, vwxx, vwyy, vwexx, vweyy);
  vresw_vs_w2->SetTitle("");
  vresw_vs_w2->SetMarkerStyle(21);
  vresw_vs_w2->SetMarkerColor(2);
  vresw_vs_w2->SetMarkerSize(1);
  canvas = new TCanvas();
  vresw_vs_w2->Draw("ap");


  //w using clusterSplit1.C

  double wwx[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  double wwxx[8] = {5, 15, 25, 35, 45, 55, 65, 73.5};
  double wwex[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  double wwexx[8] = {5, 5, 5, 5, 5, 5, 5, 3.5};
  double wwy[8] = {wres_01w*10000, wres_02w*10000, wres_03w*10000, wres_04w*10000,
                   wres_05w*10000, wres_06w*10000, wres_07w*10000, wres_08w*10000};
  double wwyy[8] = {wres_01w*10, wres_02w*10, wres_03w*10, wres_04w*10, 
		    wres_05w*10, wres_06w*10, wres_07w*10, wres_08w*10};
  double wwey[8] = {err_wres_01w*10000, err_wres_02w*10000,
		    err_wres_03w*10000, err_wres_04w*10000,
		    err_wres_05w*10000, err_wres_06w*10000,
		    err_wres_07w*10000, err_wres_08w*10000};
  double wweyy[8] = {err_wres_01w*10, err_wres_02w*10,
		     err_wres_03w*10, err_wres_04w*10,
		     err_wres_05w*10, err_wres_06w*10,
		     err_wres_07w*10, err_wres_08w*10};

  TH2D *hr1ww = new TH2D("hr1ww","hr1ww",8,0.5,8.5, 10,0,500);
  hr1ww->SetStats(kFALSE);
  hr1ww->SetTitle("");
  hr1ww->SetYTitle("Spatial Resolution w using clusterSplit1.C (#mum)");
  hr1ww->SetLabelOffset(0.01, "X");
  hr1ww->SetLabelSize(0.05, "X");

  TAxis *ww_xaxis = hr1ww->GetXaxis();
  ww_xaxis->SetBinLabel(1," 0 < w < 10mm");
  ww_xaxis->SetBinLabel(2,"10 < w < 20mm");
  ww_xaxis->SetBinLabel(3,"20 < w < 30mm");
  ww_xaxis->SetBinLabel(4,"30 < w < 40mm");
  ww_xaxis->SetBinLabel(5,"40 < w < 50mm");
  ww_xaxis->SetBinLabel(6,"50 < w < 60mm");
  ww_xaxis->SetBinLabel(7,"60 < w < 70mm");
  ww_xaxis->SetBinLabel(8,"70 < w < 80mm");

  TGraphErrors *wresw_vs_w = new TGraphErrors(8, wwx,wwy, wwex, wwey);
  wresw_vs_w->SetTitle("");
  wresw_vs_w->SetMarkerStyle(21);
  wresw_vs_w->SetMarkerColor(2);
  wresw_vs_w->SetMarkerSize(1);
  canvas = new TCanvas();
  hr1ww->Draw();
  wresw_vs_w->Draw("p");

  TGraphErrors *wresw_vs_w2 = new TGraphErrors(8, wwxx, wwyy, wwexx, wweyy);
  wresw_vs_w2->SetTitle("");
  wresw_vs_w2->SetMarkerStyle(21);
  wresw_vs_w2->SetMarkerColor(2);
  wresw_vs_w2->SetMarkerSize(1);
  canvas = new TCanvas();
  wresw_vs_w2->Draw("ap");

}

