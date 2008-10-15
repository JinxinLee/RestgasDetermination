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

#include "cuts.C"

Double_t doublegausf(Double_t *x, Double_t *par){
  Double_t arg1 = 0;
  Double_t arg2 = 0;
  if (fabs(par[2]) > 1.E-10) arg1 = (x[0] - par[1])/par[2];
  if (fabs(par[4]) > 1.E-10) arg2 = (x[0] - par[1])/par[4];
  Double_t fitval = par[0]*TMath::Exp(-0.5*arg1*arg1) + par[3]*TMath::Exp(-0.5*arg2*arg2);
  return fitval;
}

void resolution(TString files){

  TChain myChain("at");

  //define the histograms
  //v

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

  //w

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

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;


  Int_t nevent = myChain.GetEntries();

  TCtrack *intr=0;

  myChain.SetBranchAddress("track", &intr);

  //event loop
  for (Int_t iev=0;iev<nevent;iev++){

    myChain.GetEntry(iev);
    TCtrack tr(*intr);

    if(!IEEE(tr)) continue;

    for(Int_t i=0;i<tr.nCl();i++){

      TCcluster c = tr.getCl(i);
      if(c.getFit()){ 

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
  }

  //end of event loop
  
  //v

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

  //w

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
 

  //v
  //vall

  TF1 *doublegaus_vall = new TF1("doublegaus_vall",doublegausf,-.1,.1,5);

  doublegaus_vall->SetParameters(20,vall->GetMean(),vall->GetRMS(),5,vall->GetRMS());

  doublegaus_vall->SetParLimits(2,0.01,0.045);
  doublegaus_vall->SetParLimits(4,0.01,0.045);

  vall->Fit("doublegaus_vall","R");

  TF1 *vall_f1 = new TF1("vall_f1","gaus",-0.1,0.1);
  TF1 *vall_f2 = new TF1("vall_f2","gaus",-0.1,0.1);

  vall_f1->SetParameter(0,doublegaus_vall->GetParameter(0));
  vall_f1->SetParameter(1,doublegaus_vall->GetParameter(1));
  vall_f1->SetParameter(2,doublegaus_vall->GetParameter(2));
  vall_f1->SetParError(2,doublegaus_vall->GetParError(2));
  vall_f1->SetLineColor(kRed);
  vall_f2->SetParameter(0,doublegaus_vall->GetParameter(3));
  vall_f2->SetParameter(1,doublegaus_vall->GetParameter(1));
  vall_f2->SetParameter(2,doublegaus_vall->GetParameter(4));
  vall_f2->SetParError(2,doublegaus_vall->GetParError(4));
  vall_f2->SetLineColor(kBlue);
  
  Double_t intvall_1 = vall_f1->Integral(-.1,.1);
  Double_t intvall_2 = vall_f2->Integral(-.1,.1);
  
  vres_all = (intvall_1*fabs(vall_f1->GetParameter(2))+ intvall_2*fabs(vall_f2->GetParameter(2)))/(intvall_1 + intvall_2);

  err_vres_all  = (1./(intvall_1+intvall_2));
  err_vres_all *= sqrt(intvall_1*intvall_1*pow((vall_f1->GetParError(2)),2)+intvall_2*intvall_2*pow(vall_f2->GetParError(2),2));

  cout<<endl<<"Overall resolution v without: "<<vres_all<<" +- " <<err_vres_all<<endl<<endl;


  //v01

  TF1 *doublegaus_v01 = new TF1("doublegaus_v01",doublegausf,-.1,.1,5);

  doublegaus_v01->SetParameters(20,v01->GetMean(),v01->GetRMS(),5,v01->GetRMS());

  doublegaus_v01->SetParLimits(2,0.01,0.045);
  doublegaus_v01->SetParLimits(4,0.01,0.045);

  v01->Fit("doublegaus_v01","R");

  TF1 *v01_f1 = new TF1("v01_f1","gaus",-0.1,0.1);
  TF1 *v01_f2 = new TF1("v01_f2","gaus",-0.1,0.1);

  v01_f1->SetParameter(0,doublegaus_v01->GetParameter(0));
  v01_f1->SetParameter(1,doublegaus_v01->GetParameter(1));
  v01_f1->SetParameter(2,doublegaus_v01->GetParameter(2));
  v01_f1->SetParError(2,doublegaus_v01->GetParError(2));
  v01_f1->SetLineColor(kRed);
  v01_f2->SetParameter(0,doublegaus_v01->GetParameter(3));
  v01_f2->SetParameter(1,doublegaus_v01->GetParameter(1));
  v01_f2->SetParameter(2,doublegaus_v01->GetParameter(4));
  v01_f2->SetParError(2,doublegaus_v01->GetParError(4));
  v01_f2->SetLineColor(kBlue);
  
  Double_t intv01_1 = v01_f1->Integral(-.1,.1);
  Double_t intv01_2 = v01_f2->Integral(-.1,.1);
  
  vres_01 = (intv01_1*fabs(v01_f1->GetParameter(2))+ intv01_2*fabs(v01_f2->GetParameter(2)))/(intv01_1 + intv01_2);

  err_vres_01  = (1./(intv01_1+intv01_2));
  err_vres_01 *= sqrt(intv01_1*intv01_1*pow((v01_f1->GetParError(2)),2)+intv01_2*intv01_2*pow(v01_f2->GetParError(2),2));

  cout<<endl<<"resolution v01: "<<vres_01<<" +- " <<err_vres_01<<endl<<endl;


  //v02

  TF1 *doublegaus_v02 = new TF1("doublegaus_v02",doublegausf,-.1,.1,5);

  doublegaus_v02->SetParameters(20,v02->GetMean(),v02->GetRMS(),5,v02->GetRMS());

  doublegaus_v02->SetParLimits(2,0.01,0.045);
  doublegaus_v02->SetParLimits(4,0.01,0.045);

  v02->Fit("doublegaus_v02","R");

  TF1 *v02_f1 = new TF1("v02_f1","gaus",-0.1,0.1);
  TF1 *v02_f2 = new TF1("v02_f2","gaus",-0.1,0.1);

  v02_f1->SetParameter(0,doublegaus_v02->GetParameter(0));
  v02_f1->SetParameter(1,doublegaus_v02->GetParameter(1));
  v02_f1->SetParameter(2,doublegaus_v02->GetParameter(2));
  v02_f1->SetParError(2,doublegaus_v02->GetParError(2));
  v02_f1->SetLineColor(kRed);
  v02_f2->SetParameter(0,doublegaus_v02->GetParameter(3));
  v02_f2->SetParameter(1,doublegaus_v02->GetParameter(1));
  v02_f2->SetParameter(2,doublegaus_v02->GetParameter(4));
  v02_f2->SetParError(2,doublegaus_v02->GetParError(4));
  v02_f2->SetLineColor(kBlue);
  
  Double_t intv02_1 = v02_f1->Integral(-.1,.1);
  Double_t intv02_2 = v02_f2->Integral(-.1,.1);
  
  vres_02 = (intv02_1*fabs(v02_f1->GetParameter(2))+ intv02_2*fabs(v02_f2->GetParameter(2)))/(intv02_1 + intv02_2);

  err_vres_02  = (1./(intv02_1+intv02_2));
  err_vres_02 *= sqrt(intv02_1*intv02_1*pow((v02_f1->GetParError(2)),2)+intv02_2*intv02_2*pow(v02_f2->GetParError(2),2));

  cout<<endl<<"resolution v12: "<<vres_02<<" +- " <<err_vres_02<<endl<<endl;

  //v03

  TF1 *doublegaus_v03 = new TF1("doublegaus_v03",doublegausf,-.1,.1,5);

  doublegaus_v03->SetParameters(20,v03->GetMean(),v03->GetRMS(),5,v03->GetRMS());

  doublegaus_v03->SetParLimits(2,0.01,0.045);
  doublegaus_v03->SetParLimits(4,0.01,0.045);

  v03->Fit("doublegaus_v03","R");

  TF1 *v03_f1 = new TF1("v03_f1","gaus",-0.1,0.1);
  TF1 *v03_f2 = new TF1("v03_f2","gaus",-0.1,0.1);

  v03_f1->SetParameter(0,doublegaus_v03->GetParameter(0));
  v03_f1->SetParameter(1,doublegaus_v03->GetParameter(1));
  v03_f1->SetParameter(2,doublegaus_v03->GetParameter(2));
  v03_f1->SetParError(2,doublegaus_v03->GetParError(2));
  v03_f1->SetLineColor(kRed);
  v03_f2->SetParameter(0,doublegaus_v03->GetParameter(3));
  v03_f2->SetParameter(1,doublegaus_v03->GetParameter(1));
  v03_f2->SetParameter(2,doublegaus_v03->GetParameter(4));
  v03_f2->SetParError(2,doublegaus_v03->GetParError(4));
  v03_f2->SetLineColor(kBlue);
  
  Double_t intv03_1 = v03_f1->Integral(-.1,.1);
  Double_t intv03_2 = v03_f2->Integral(-.1,.1);
  
  vres_03 = (intv03_1*fabs(v03_f1->GetParameter(2))+ intv03_2*fabs(v03_f2->GetParameter(2)))/(intv03_1 + intv03_2);

  err_vres_03  = (1./(intv03_1+intv03_2));
  err_vres_03 *= sqrt(intv03_1*intv03_1*pow((v03_f1->GetParError(2)),2)+intv03_2*intv03_2*pow(v03_f2->GetParError(2),2));

  cout<<endl<<"resolution v23: "<<vres_03<<" +- " <<err_vres_03<<endl<<endl;

  //v04

  TF1 *doublegaus_v04 = new TF1("doublegaus_v04",doublegausf,-.1,.1,5);

  doublegaus_v04->SetParameters(20,v04->GetMean(),v04->GetRMS(),5,v04->GetRMS());

  doublegaus_v04->SetParLimits(2,0.01,0.045);
  doublegaus_v04->SetParLimits(4,0.01,0.045);

  v04->Fit("doublegaus_v04","R");

  TF1 *v04_f1 = new TF1("v04_f1","gaus",-0.1,0.1);
  TF1 *v04_f2 = new TF1("v04_f2","gaus",-0.1,0.1);

  v04_f1->SetParameter(0,doublegaus_v04->GetParameter(0));
  v04_f1->SetParameter(1,doublegaus_v04->GetParameter(1));
  v04_f1->SetParameter(2,doublegaus_v04->GetParameter(2));
  v04_f1->SetParError(2,doublegaus_v04->GetParError(2));
  v04_f1->SetLineColor(kRed);
  v04_f2->SetParameter(0,doublegaus_v04->GetParameter(3));
  v04_f2->SetParameter(1,doublegaus_v04->GetParameter(1));
  v04_f2->SetParameter(2,doublegaus_v04->GetParameter(4));
  v04_f2->SetParError(2,doublegaus_v04->GetParError(4));
  v04_f2->SetLineColor(kBlue);
  
  Double_t intv04_1 = v04_f1->Integral(-.1,.1);
  Double_t intv04_2 = v04_f2->Integral(-.1,.1);
  
  vres_04 = (intv04_1*fabs(v04_f1->GetParameter(2))+ intv04_2*fabs(v04_f2->GetParameter(2)))/(intv04_1 + intv04_2);

  err_vres_04  = (1./(intv04_1+intv04_2));
  err_vres_04 *= sqrt(intv04_1*intv04_1*pow((v04_f1->GetParError(2)),2)+intv04_2*intv04_2*pow(v04_f2->GetParError(2),2));

  cout<<endl<<"resolution v34: "<<vres_04<<" +- " <<err_vres_04<<endl<<endl;

  //v05

  TF1 *doublegaus_v05 = new TF1("doublegaus_v05",doublegausf,-.1,.1,5);

  doublegaus_v05->SetParameters(20,v05->GetMean(),v05->GetRMS(),5,v05->GetRMS());

  doublegaus_v05->SetParLimits(2,0.01,0.045);
  doublegaus_v05->SetParLimits(4,0.01,0.045);

  v05->Fit("doublegaus_v05","R");

  TF1 *v05_f1 = new TF1("v05_f1","gaus",-0.1,0.1);
  TF1 *v05_f2 = new TF1("v05_f2","gaus",-0.1,0.1);

  v05_f1->SetParameter(0,doublegaus_v05->GetParameter(0));
  v05_f1->SetParameter(1,doublegaus_v05->GetParameter(1));
  v05_f1->SetParameter(2,doublegaus_v05->GetParameter(2));
  v05_f1->SetParError(2,doublegaus_v05->GetParError(2));
  v05_f1->SetLineColor(kRed);
  v05_f2->SetParameter(0,doublegaus_v05->GetParameter(3));
  v05_f2->SetParameter(1,doublegaus_v05->GetParameter(1));
  v05_f2->SetParameter(2,doublegaus_v05->GetParameter(4));
  v05_f2->SetParError(2,doublegaus_v05->GetParError(4));
  v05_f2->SetLineColor(kBlue);
  
  Double_t intv05_1 = v05_f1->Integral(-.1,.1);
  Double_t intv05_2 = v05_f2->Integral(-.1,.1);
  
  vres_05 = (intv05_1*fabs(v05_f1->GetParameter(2))+ intv05_2*fabs(v05_f2->GetParameter(2)))/(intv05_1 + intv05_2);

  err_vres_05 = (1./(intv05_1+intv05_2));
  err_vres_05 *= sqrt(intv05_1*intv05_1*pow((v05_f1->GetParError(2)),2)+intv05_2*intv05_2*pow(v05_f2->GetParError(2),2));

  cout<<endl<<"resolution v45: "<<vres_05<<" +- " <<err_vres_05<<endl<<endl;

  //v06

  TF1 *doublegaus_v06 = new TF1("doublegaus_v06",doublegausf,-.1,.1,5);

  doublegaus_v06->SetParameters(20,v06->GetMean(),v06->GetRMS(),5,v06->GetRMS());

  doublegaus_v06->SetParLimits(2,0.01,0.045);
  doublegaus_v06->SetParLimits(4,0.01,0.045);

  v06->Fit("doublegaus_v06","R");

  TF1 *v06_f1 = new TF1("v06_f1","gaus",-0.1,0.1);
  TF1 *v06_f2 = new TF1("v06_f2","gaus",-0.1,0.1);

  v06_f1->SetParameter(0,doublegaus_v06->GetParameter(0));
  v06_f1->SetParameter(1,doublegaus_v06->GetParameter(1));
  v06_f1->SetParameter(2,doublegaus_v06->GetParameter(2));
  v06_f1->SetParError(2,doublegaus_v06->GetParError(2));
  v06_f1->SetLineColor(kRed);
  v06_f2->SetParameter(0,doublegaus_v06->GetParameter(3));
  v06_f2->SetParameter(1,doublegaus_v06->GetParameter(1));
  v06_f2->SetParameter(2,doublegaus_v06->GetParameter(4));
  v06_f2->SetParError(2,doublegaus_v06->GetParError(4));
  v06_f2->SetLineColor(kBlue);
  
  Double_t intv06_1 = v06_f1->Integral(-.1,.1);
  Double_t intv06_2 = v06_f2->Integral(-.1,.1);
  
  vres_06 = (intv06_1*fabs(v06_f1->GetParameter(2))+ intv06_2*fabs(v06_f2->GetParameter(2)))/(intv06_1 + intv06_2);

  err_vres_06  = (1./(intv06_1+intv06_2));
  err_vres_06 *= sqrt(intv06_1*intv06_1*pow((v06_f1->GetParError(2)),2)+intv06_2*intv06_2*pow(v06_f2->GetParError(2),2));

  cout<<endl<<"resolution v56: "<<vres_06<<" +- " <<err_vres_06<<endl<<endl;

  //v07

  TF1 *doublegaus_v07 = new TF1("doublegaus_v07",doublegausf,-.1,.1,5);

  doublegaus_v07->SetParameters(20,v07->GetMean(),v07->GetRMS(),5,v07->GetRMS());

  doublegaus_v07->SetParLimits(2,0.01,0.045);
  doublegaus_v07->SetParLimits(4,0.01,0.045);

  v07->Fit("doublegaus_v07","R");

  TF1 *v07_f1 = new TF1("v07_f1","gaus",-0.1,0.1);
  TF1 *v07_f2 = new TF1("v07_f2","gaus",-0.1,0.1);

  v07_f1->SetParameter(0,doublegaus_v07->GetParameter(0));
  v07_f1->SetParameter(1,doublegaus_v07->GetParameter(1));
  v07_f1->SetParameter(2,doublegaus_v07->GetParameter(2));
  v07_f1->SetParError(2,doublegaus_v07->GetParError(2));
  v07_f1->SetLineColor(kRed);
  v07_f2->SetParameter(0,doublegaus_v07->GetParameter(3));
  v07_f2->SetParameter(1,doublegaus_v07->GetParameter(1));
  v07_f2->SetParameter(2,doublegaus_v07->GetParameter(4));
  v07_f2->SetParError(2,doublegaus_v07->GetParError(4));
  v07_f2->SetLineColor(kBlue);
  
  Double_t intv07_1 = v07_f1->Integral(-.1,.1);
  Double_t intv07_2 = v07_f2->Integral(-.1,.1);
  
  vres_07 = (intv07_1*fabs(v07_f1->GetParameter(2))+ intv07_2*fabs(v07_f2->GetParameter(2)))/(intv07_1 + intv07_2);

  err_vres_07  = (1./(intv07_1+intv07_2));
  err_vres_07 *= sqrt(intv07_1*intv07_1*pow((v07_f1->GetParError(2)),2)+intv07_2*intv07_2*pow(v07_f2->GetParError(2),2));

  cout<<endl<<"resolution v67: "<<vres_07<<" +- " <<err_vres_07<<endl<<endl;

  //v08

  TF1 *doublegaus_v08 = new TF1("doublegaus_v08",doublegausf,-.1,.1,5);

  doublegaus_v08->SetParameters(20,v08->GetMean(),v08->GetRMS(),5,v08->GetRMS());

  doublegaus_v08->SetParLimits(2,0.01,0.045);
  doublegaus_v08->SetParLimits(4,0.01,0.045);

  v08->Fit("doublegaus_v08","R");

  TF1 *v08_f1 = new TF1("v08_f1","gaus",-0.1,0.1);
  TF1 *v08_f2 = new TF1("v08_f2","gaus",-0.1,0.1);

  v08_f1->SetParameter(0,doublegaus_v08->GetParameter(0));
  v08_f1->SetParameter(1,doublegaus_v08->GetParameter(1));
  v08_f1->SetParameter(2,doublegaus_v08->GetParameter(2));
  v08_f1->SetParError(2,doublegaus_v08->GetParError(2));
  v08_f1->SetLineColor(kRed);
  v08_f2->SetParameter(0,doublegaus_v08->GetParameter(3));
  v08_f2->SetParameter(1,doublegaus_v08->GetParameter(1));
  v08_f2->SetParameter(2,doublegaus_v08->GetParameter(4));
  v08_f2->SetParError(2,doublegaus_v08->GetParError(4));
  v08_f2->SetLineColor(kBlue);
  
  Double_t intv08_1 = v08_f1->Integral(-.1,.1);
  Double_t intv08_2 = v08_f2->Integral(-.1,.1);
  
  vres_08 = (intv08_1*fabs(v08_f1->GetParameter(2))+ intv08_2*fabs(v08_f2->GetParameter(2)))/(intv08_1 + intv08_2);

  err_vres_08  = (1./(intv08_1+intv08_2));
  err_vres_08 *= sqrt(intv08_1*intv08_1*pow((v08_f1->GetParError(2)),2)+intv08_2*intv08_2*pow(v08_f2->GetParError(2),2));

  cout<<endl<<"resolution v78: "<<vres_08<<" +- " <<err_vres_08<<endl<<endl;

  //w
  //wall


  TF1 *doublegaus_wall = new TF1("doublegaus_wall",doublegausf,-.1,.1,5);

  doublegaus_wall->SetParameters(200,wall->GetMean(),wall->GetRMS(),50,wall->GetRMS());

  doublegaus_wall->SetParLimits(2,0.01,0.045);
  doublegaus_wall->SetParLimits(4,0.01,0.045);

  wall->Fit("doublegaus_wall","R");

  TF1 *wall_f1 = new TF1("wall_f1","gaus",-0.1,0.1);
  TF1 *wall_f2 = new TF1("wall_f2","gaus",-0.1,0.1);

  wall_f1->SetParameter(0,doublegaus_wall->GetParameter(0));
  wall_f1->SetParameter(1,doublegaus_wall->GetParameter(1));
  wall_f1->SetParameter(2,doublegaus_wall->GetParameter(2));
  wall_f1->SetParError(2,doublegaus_wall->GetParError(2));
  wall_f1->SetLineColor(kRed);
  wall_f2->SetParameter(0,doublegaus_wall->GetParameter(3));
  wall_f2->SetParameter(1,doublegaus_wall->GetParameter(1));
  wall_f2->SetParameter(2,doublegaus_wall->GetParameter(4));
  wall_f2->SetParError(2,doublegaus_wall->GetParError(4));
  wall_f2->SetLineColor(kBlue);
  
  Double_t intwall_1 = wall_f1->Integral(-.1,.1);
  Double_t intwall_2 = wall_f2->Integral(-.1,.1);
  
  wres_all = (intwall_1*fabs(wall_f1->GetParameter(2))+ intwall_2*fabs(wall_f2->GetParameter(2)))/(intwall_1 + intwall_2);

  err_wres_all  = (1./(intwall_1+intwall_2));
  err_wres_all *= sqrt(intwall_1*intwall_1*pow((wall_f1->GetParError(2)),2)+intwall_2*intwall_2*pow(wall_f2->GetParError(2),2));

  cout<<endl<<"Overall resolution w without: "<<wres_all<<" +- " <<err_wres_all<<endl<<endl;


  //w01

  TF1 *doublegaus_w01 = new TF1("doublegaus_w01",doublegausf,-.1,.1,5);

  doublegaus_w01->SetParameters(20,w01->GetMean(),w01->GetRMS(),5,w01->GetRMS());

  doublegaus_w01->SetParLimits(2,0.01,0.045);
  doublegaus_w01->SetParLimits(4,0.01,0.045);

  w01->Fit("doublegaus_w01","R");

  TF1 *w01_f1 = new TF1("w01_f1","gaus",-0.1,0.1);
  TF1 *w01_f2 = new TF1("w01_f2","gaus",-0.1,0.1);

  w01_f1->SetParameter(0,doublegaus_w01->GetParameter(0));
  w01_f1->SetParameter(1,doublegaus_w01->GetParameter(1));
  w01_f1->SetParameter(2,doublegaus_w01->GetParameter(2));
  w01_f1->SetParError(2,doublegaus_w01->GetParError(2));
  w01_f1->SetLineColor(kRed);
  w01_f2->SetParameter(0,doublegaus_w01->GetParameter(3));
  w01_f2->SetParameter(1,doublegaus_w01->GetParameter(1));
  w01_f2->SetParameter(2,doublegaus_w01->GetParameter(4));
  w01_f2->SetParError(2,doublegaus_w01->GetParError(4));
  w01_f2->SetLineColor(kBlue);
  
  Double_t intw01_1 = w01_f1->Integral(-.1,.1);
  Double_t intw01_2 = w01_f2->Integral(-.1,.1);
  
  wres_01 = (intw01_1*fabs(w01_f1->GetParameter(2))+ intw01_2*fabs(w01_f2->GetParameter(2)))/(intw01_1 + intw01_2);

  err_wres_01  = (1./(intw01_1+intw01_2));
  err_wres_01 *= sqrt(intw01_1*intw01_1*pow((w01_f1->GetParError(2)),2)+intw01_2*intw01_2*pow(w01_f2->GetParError(2),2));

  cout<<endl<<"resolution w01: "<<wres_01<<" +- " <<err_wres_01<<endl<<endl;


  //w02

  TF1 *doublegaus_w02 = new TF1("doublegaus_w02",doublegausf,-.1,.1,5);

  doublegaus_w02->SetParameters(20,w02->GetMean(),w02->GetRMS(),5,w02->GetRMS());

  doublegaus_w02->SetParLimits(2,0.01,0.045);
  doublegaus_w02->SetParLimits(4,0.01,0.045);

  w02->Fit("doublegaus_w02","R");

  TF1 *w02_f1 = new TF1("w02_f1","gaus",-0.1,0.1);
  TF1 *w02_f2 = new TF1("w02_f2","gaus",-0.1,0.1);

  w02_f1->SetParameter(0,doublegaus_w02->GetParameter(0));
  w02_f1->SetParameter(1,doublegaus_w02->GetParameter(1));
  w02_f1->SetParameter(2,doublegaus_w02->GetParameter(2));
  w02_f1->SetParError(2,doublegaus_w02->GetParError(2));
  w02_f1->SetLineColor(kRed);
  w02_f2->SetParameter(0,doublegaus_w02->GetParameter(3));
  w02_f2->SetParameter(1,doublegaus_w02->GetParameter(1));
  w02_f2->SetParameter(2,doublegaus_w02->GetParameter(4));
  w02_f2->SetParError(2,doublegaus_w02->GetParError(4));
  w02_f2->SetLineColor(kBlue);
  
  Double_t intw02_1 = w02_f1->Integral(-.1,.1);
  Double_t intw02_2 = w02_f2->Integral(-.1,.1);
  
  wres_02 = (intw02_1*fabs(w02_f1->GetParameter(2))+ intw02_2*fabs(w02_f2->GetParameter(2)))/(intw02_1 + intw02_2);

  err_wres_02  = (1./(intw02_1+intw02_2));
  err_wres_02 *= sqrt(intw02_1*intw02_1*pow((w02_f1->GetParError(2)),2)+intw02_2*intw02_2*pow(w02_f2->GetParError(2),2));

  cout<<endl<<"resolution w12: "<<wres_02<<" +- " <<err_wres_02<<endl<<endl;

  //w03

  TF1 *doublegaus_w03 = new TF1("doublegaus_w03",doublegausf,-.1,.1,5);

  doublegaus_w03->SetParameters(20,w03->GetMean(),w03->GetRMS(),5,w03->GetRMS());

  doublegaus_w03->SetParLimits(2,0.01,0.045);
  doublegaus_w03->SetParLimits(4,0.01,0.045);

  w03->Fit("doublegaus_w03","R");

  TF1 *w03_f1 = new TF1("w03_f1","gaus",-0.1,0.1);
  TF1 *w03_f2 = new TF1("w03_f2","gaus",-0.1,0.1);

  w03_f1->SetParameter(0,doublegaus_w03->GetParameter(0));
  w03_f1->SetParameter(1,doublegaus_w03->GetParameter(1));
  w03_f1->SetParameter(2,doublegaus_w03->GetParameter(2));
  w03_f1->SetParError(2,doublegaus_w03->GetParError(2));
  w03_f1->SetLineColor(kRed);
  w03_f2->SetParameter(0,doublegaus_w03->GetParameter(3));
  w03_f2->SetParameter(1,doublegaus_w03->GetParameter(1));
  w03_f2->SetParameter(2,doublegaus_w03->GetParameter(4));
  w03_f2->SetParError(2,doublegaus_w03->GetParError(4));
  w03_f2->SetLineColor(kBlue);
  
  Double_t intw03_1 = w03_f1->Integral(-.1,.1);
  Double_t intw03_2 = w03_f2->Integral(-.1,.1);
  
  wres_03 = (intw03_1*fabs(w03_f1->GetParameter(2))+ intw03_2*fabs(w03_f2->GetParameter(2)))/(intw03_1 + intw03_2);

  err_wres_03  = (1./(intw03_1+intw03_2));
  err_wres_03 *= sqrt(intw03_1*intw03_1*pow((w03_f1->GetParError(2)),2)+intw03_2*intw03_2*pow(w03_f2->GetParError(2),2));

  cout<<endl<<"resolution w23: "<<wres_03<<" +- " <<err_wres_03<<endl<<endl;

  //w04

  TF1 *doublegaus_w04 = new TF1("doublegaus_w04",doublegausf,-.1,.1,5);

  doublegaus_w04->SetParameters(200,w04->GetMean(),w04->GetRMS(),5,w04->GetRMS());

  doublegaus_w04->SetParLimits(2,0.01,0.045);
  doublegaus_w04->SetParLimits(4,0.01,0.045);

  w04->Fit("doublegaus_w04","R");

  TF1 *w04_f1 = new TF1("w04_f1","gaus",-0.1,0.1);
  TF1 *w04_f2 = new TF1("w04_f2","gaus",-0.1,0.1);

  w04_f1->SetParameter(0,doublegaus_w04->GetParameter(0));
  w04_f1->SetParameter(1,doublegaus_w04->GetParameter(1));
  w04_f1->SetParameter(2,doublegaus_w04->GetParameter(2));
  w04_f1->SetParError(2,doublegaus_w04->GetParError(2));
  w04_f1->SetLineColor(kRed);
  w04_f2->SetParameter(0,doublegaus_w04->GetParameter(3));
  w04_f2->SetParameter(1,doublegaus_w04->GetParameter(1));
  w04_f2->SetParameter(2,doublegaus_w04->GetParameter(4));
  w04_f2->SetParError(2,doublegaus_w04->GetParError(4));
  w04_f2->SetLineColor(kBlue);
  
  Double_t intw04_1 = w04_f1->Integral(-.1,.1);
  Double_t intw04_2 = w04_f2->Integral(-.1,.1);
  
  wres_04 = (intw04_1*fabs(w04_f1->GetParameter(2))+ intw04_2*fabs(w04_f2->GetParameter(2)))/(intw04_1 + intw04_2);

  err_wres_04  = (1./(intw04_1+intw04_2));
  err_wres_04 *= sqrt(intw04_1*intw04_1*pow((w04_f1->GetParError(2)),2)+intw04_2*intw04_2*pow(w04_f2->GetParError(2),2));

  cout<<endl<<"resolution w34: "<<wres_04<<" +- " <<err_wres_04<<endl<<endl;

  //w05

  TF1 *doublegaus_w05 = new TF1("doublegaus_w05",doublegausf,-.1,.1,5);

  doublegaus_w05->SetParameters(200,w05->GetMean(),w05->GetRMS(),50,w05->GetRMS());

  doublegaus_w05->SetParLimits(2,0.01,0.045);
  doublegaus_w05->SetParLimits(4,0.01,0.045);

  w05->Fit("doublegaus_w05","R");

  TF1 *w05_f1 = new TF1("w05_f1","gaus",-0.1,0.1);
  TF1 *w05_f2 = new TF1("w05_f2","gaus",-0.1,0.1);

  w05_f1->SetParameter(0,doublegaus_w05->GetParameter(0));
  w05_f1->SetParameter(1,doublegaus_w05->GetParameter(1));
  w05_f1->SetParameter(2,doublegaus_w05->GetParameter(2));
  w05_f1->SetParError(2,doublegaus_w05->GetParError(2));
  w05_f1->SetLineColor(kRed);
  w05_f2->SetParameter(0,doublegaus_w05->GetParameter(3));
  w05_f2->SetParameter(1,doublegaus_w05->GetParameter(1));
  w05_f2->SetParameter(2,doublegaus_w05->GetParameter(4));
  w05_f2->SetParError(2,doublegaus_w05->GetParError(4));
  w05_f2->SetLineColor(kBlue);
  
  Double_t intw05_1 = w05_f1->Integral(-.1,.1);
  Double_t intw05_2 = w05_f2->Integral(-.1,.1);
  
  wres_05 = (intw05_1*fabs(w05_f1->GetParameter(2))+ intw05_2*fabs(w05_f2->GetParameter(2)))/(intw05_1 + intw05_2);

  err_wres_05  = (1./(intw05_1+intw05_2));
  err_wres_05 *= sqrt(intw05_1*intw05_1*pow((w05_f1->GetParError(2)),2)+intw05_2*intw05_2*pow(w05_f2->GetParError(2),2));

  cout<<endl<<"resolution w45: "<<wres_05<<" +- " <<err_wres_05<<endl<<endl;

  //w06

  TF1 *doublegaus_w06 = new TF1("doublegaus_w06",doublegausf,-.1,.1,5);

  doublegaus_w06->SetParameters(20,w06->GetMean(),w06->GetRMS(),5,w06->GetRMS());

  doublegaus_w06->SetParLimits(2,0.01,0.045);
  doublegaus_w06->SetParLimits(4,0.01,0.045);

  w06->Fit("doublegaus_w06","R");

  TF1 *w06_f1 = new TF1("w06_f1","gaus",-0.1,0.1);
  TF1 *w06_f2 = new TF1("w06_f2","gaus",-0.1,0.1);

  w06_f1->SetParameter(0,doublegaus_w06->GetParameter(0));
  w06_f1->SetParameter(1,doublegaus_w06->GetParameter(1));
  w06_f1->SetParameter(2,doublegaus_w06->GetParameter(2));
  w06_f1->SetParError(2,doublegaus_w06->GetParError(2));
  w06_f1->SetLineColor(kRed);
  w06_f2->SetParameter(0,doublegaus_w06->GetParameter(3));
  w06_f2->SetParameter(1,doublegaus_w06->GetParameter(1));
  w06_f2->SetParameter(2,doublegaus_w06->GetParameter(4));
  w06_f2->SetParError(2,doublegaus_w06->GetParError(4));
  w06_f2->SetLineColor(kBlue);
  
  Double_t intw06_1 = w06_f1->Integral(-.1,.1);
  Double_t intw06_2 = w06_f2->Integral(-.1,.1);
  
  wres_06 = (intw06_1*fabs(w06_f1->GetParameter(2))+ intw06_2*fabs(w06_f2->GetParameter(2)))/(intw06_1 + intw06_2);

  err_wres_06  = (1./(intw06_1+intw06_2));
  err_wres_06 *= sqrt(intw06_1*intw06_1*pow((w06_f1->GetParError(2)),2)+intw06_2*intw06_2*pow(w06_f2->GetParError(2),2));

  cout<<endl<<"resolution w56: "<<wres_06<<" +- " <<err_wres_06<<endl<<endl;

  //w07

  TF1 *doublegaus_w07 = new TF1("doublegaus_w07",doublegausf,-.1,.1,5);

  doublegaus_w07->SetParameters(20,w07->GetMean(),w07->GetRMS(),5,w07->GetRMS());

  doublegaus_w07->SetParLimits(2,0.01,0.045);
  doublegaus_w07->SetParLimits(4,0.01,0.045);

  w07->Fit("doublegaus_w07","R");

  TF1 *w07_f1 = new TF1("w07_f1","gaus",-0.1,0.1);
  TF1 *w07_f2 = new TF1("w07_f2","gaus",-0.1,0.1);

  w07_f1->SetParameter(0,doublegaus_w07->GetParameter(0));
  w07_f1->SetParameter(1,doublegaus_w07->GetParameter(1));
  w07_f1->SetParameter(2,doublegaus_w07->GetParameter(2));
  w07_f1->SetParError(2,doublegaus_w07->GetParError(2));
  w07_f1->SetLineColor(kRed);
  w07_f2->SetParameter(0,doublegaus_w07->GetParameter(3));
  w07_f2->SetParameter(1,doublegaus_w07->GetParameter(1));
  w07_f2->SetParameter(2,doublegaus_w07->GetParameter(4));
  w07_f2->SetParError(2,doublegaus_w07->GetParError(4));
  w07_f2->SetLineColor(kBlue);
  
  Double_t intw07_1 = w07_f1->Integral(-.1,.1);
  Double_t intw07_2 = w07_f2->Integral(-.1,.1);
  
  wres_07 = (intw07_1*fabs(w07_f1->GetParameter(2))+ intw07_2*fabs(w07_f2->GetParameter(2)))/(intw07_1 + intw07_2);

  err_wres_07  = (1./(intw07_1+intw07_2));
  err_wres_07 *= sqrt(intw07_1*intw07_1*pow((w07_f1->GetParError(2)),2)+intw07_2*intw07_2*pow(w07_f2->GetParError(2),2));

  cout<<endl<<"resolution w67: "<<wres_07<<" +- " <<err_wres_07<<endl<<endl;

  //w08

  TF1 *doublegaus_w08 = new TF1("doublegaus_w08",doublegausf,-.1,.1,5);

  doublegaus_w08->SetParameters(2000,w08->GetMean(),w08->GetRMS(),50,w08->GetRMS());

  doublegaus_w08->SetParLimits(2,0.01,0.045);
  doublegaus_w08->SetParLimits(4,0.01,0.045);

  w08->Fit("doublegaus_w08","R");

  TF1 *w08_f1 = new TF1("w08_f1","gaus",-0.1,0.1);
  TF1 *w08_f2 = new TF1("w08_f2","gaus",-0.1,0.1);

  w08_f1->SetParameter(0,doublegaus_w08->GetParameter(0));
  w08_f1->SetParameter(1,doublegaus_w08->GetParameter(1));
  w08_f1->SetParameter(2,doublegaus_w08->GetParameter(2));
  w08_f1->SetParError(2,doublegaus_w08->GetParError(2));
  w08_f1->SetLineColor(kRed);
  w08_f2->SetParameter(0,doublegaus_w08->GetParameter(3));
  w08_f2->SetParameter(1,doublegaus_w08->GetParameter(1));
  w08_f2->SetParameter(2,doublegaus_w08->GetParameter(4));
  w08_f2->SetParError(2,doublegaus_w08->GetParError(4));
  w08_f2->SetLineColor(kBlue);
  
  Double_t intw08_1 = w08_f1->Integral(-.1,.1);
  Double_t intw08_2 = w08_f2->Integral(-.1,.1);
  
  wres_08 = (intw08_1*fabs(w08_f1->GetParameter(2))+ intw08_2*fabs(w08_f2->GetParameter(2)))/(intw08_1 + intw08_2);

  err_wres_08  = (1./(intw08_1+intw08_2));
  err_wres_08 *= sqrt(intw08_1*intw08_1*pow((w08_f1->GetParError(2)),2)+intw08_2*intw08_2*pow(w08_f2->GetParError(2),2));

  cout<<endl<<"resolution w78: "<<wres_08<<" +- " <<err_wres_08<<endl<<endl;


  //Draw

  //v

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

  //w

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


  //overview graphs

  //v

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


  //w

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

}

