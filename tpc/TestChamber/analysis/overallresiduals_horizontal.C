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
#include "TROOT.h"

#include "../src/TCtrack.h"
#include "../src/TCcluster.h"


#include "consecCut.C"
#include "clusterSplit1.C"

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

  //gSystem->Load("libtestChamber"); 


  gROOT->Macro("../macro/christian_style.C"); 

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  

  TCtrack *intr=0;


  TH1D *uresidp = new TH1D("uresidp","",500,-0.2,0.2);
  uresidp->SetXTitle("residual u [cm]");
  //uresidp->SetFillColor(2);

  TH1D *uresidw = new TH1D("uresidw","",500,-0.2,0.2);
  uresidw->SetXTitle("residual u [cm]");
  //uresidw->SetFillColor(2);

  TH1D *vresidp = new TH1D("vresidp","",500,-0.1,0.1);
  vresidp->SetXTitle("residual v [cm]");
  //vresidp->SetFillColor(2);

  TH1D *vresidw = new TH1D("vresidw","",500,-0.1,0.1);
  vresidw->SetXTitle("residual v [cm]");
  //vresidw->SetFillColor(2);

  TH1D *wresidp = new TH1D("wresidp","",500,-0.1,0.1);
  wresidp->SetXTitle("residual w [cm]");
  //wresidp->SetFillColor(2);

  TH1D *wresidw = new TH1D("wresidw","",500,-0.1,0.1);
  wresidw->SetXTitle("residual w [cm]");
  //wresidw->SetFillColor(2);



  myChain.SetBranchAddress("track", &intr);

  time_t rawtime;
  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;
   
  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
     
    for(int i=0;i<tr.nCl();++i){
      TCcluster c = tr.getCl(i);
      if(c.getFit()){//was used in fit

	if (fabs(tr.getAx()) > 1.E3) continue;
	if (fabs(tr.getAy()) > 1.E3) continue;
	if (tr.getChi2()/tr.getNDF()>2) continue;
	if (tr.getChi2()/tr.getNDF()<0.01) continue;
	if (tr.nClFit()<2) continue;

	uresidp->Fill(c.getRes().X());
	vresidp->Fill(c.getRes().Y());
	wresidp->Fill(c.getRes().Z());
      }
    }
  }
   
  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    TCtrack trSplit = clusterSplit1(tr);
    trSplit.fit(200);  

    if (fabs(trSplit.getAx()) > 1.E3) continue;
    if (fabs(trSplit.getAy()) > 1.E3) continue;
    if (trSplit.getChi2()/trSplit.getNDF()>2) continue;
    if (trSplit.getChi2()/trSplit.getNDF()<0.01) continue;
    if (trSplit.nClFit()<2) continue;

    for(int i=0;i<trSplit.nCl();++i){

      TCcluster d = trSplit.getCl(i);
       
      if(d.getFit()){//was used in fit
	uresidw->Fill(d.getRes().X());
	vresidw->Fill(d.getRes().Y());
	wresidw->Fill(d.getRes().Z());
	 
	 
      }
    }

  }

  //end of event loop

  //fits

  //definitions

  Double_t vres_all = -1;
  Double_t vres_allw = -1;
  Double_t wres_all = -1;
  Double_t wres_allw = -1;

  Double_t err_vres_all = -1;
  Double_t err_vres_allw = -1;
  Double_t err_wres_all = -1;
  Double_t err_wres_allw = -1;

  //v without

  TF1 *doublegaus_vall = new TF1("doublegaus_vall",doublegausf,-.1,.1,6);

  doublegaus_vall->SetParameters(20,vresidp->GetMean(),vresidp->GetRMS(),5,vresidp->GetMean(),vresidp->GetRMS());

  vresidp->Fit("doublegaus_vall","R");

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

  // w without

  TF1 *doublegaus_wall = new TF1("doublegaus_wall",doublegausf,-.1,.1,6);

  doublegaus_wall->SetParameters(20,wresidp->GetMean(),wresidp->GetRMS(),5,wresidp->GetMean(),wresidp->GetRMS());

  wresidp->Fit("doublegaus_wall","R");

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

  //v using clusterSplit1.C

  TF1 *doublegaus_vallw = new TF1("doublegaus_vallw",doublegausf,-.1,.1,6);

  doublegaus_vallw->SetParameters(20,vresidw->GetMean(),vresidw->GetRMS(),5,vresidw->GetMean(),vresidw->GetRMS());

  vresidw->Fit("doublegaus_vallw","R");

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

  // w using clusterSplit1.C

  TF1 *doublegaus_wallw = new TF1("doublegaus_wallw",doublegausf,-.1,.1,6);

  doublegaus_wallw->SetParameters(20,wresidw->GetMean(),wresidw->GetRMS(),5,wresidw->GetMean(),wresidw->GetRMS());

  wresidw->Fit("doublegaus_wallw","R");

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

  //plot time needed

  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;

  //plots

  //u

  TCanvas *xw = new TCanvas("canv1","");   
  uresidw->Draw();
  TCanvas *x = new TCanvas("canv2","");   
  uresidp->Draw();

  //v

  TCanvas *yw = new TCanvas("canv3","");   
  vresidw->Draw();
  doublegaus_vallw->Draw("same");
  vallw_f1->Draw("same");
  vallw_f2->Draw("same");
  
  TText *t1 = new TText(0.125, 0.87, endl<<"Overall resolution v "<<vres_all<<" +- " <<err_vres_all<<endl);
  t1->SetNDC();
  t1->SetTextSize(0.038);
  t1->SetTextFont(102);
  t1->Draw();

  TCanvas *y = new TCanvas("canv4","");   
  vresidp->Draw();
  doublegaus_vall->Draw("same");
  vall_f1->Draw("same");
  vall_f2->Draw("same");
  TCanvas *zw = new TCanvas("canv5","");   
  wresidw->Draw();
  doublegaus_wallw->Draw("same");
  wallw_f1->Draw("same");
  wallw_f2->Draw("same");
  TCanvas *z = new TCanvas("canv6","");   
  wresidp->Draw();
  doublegaus_wall->Draw("same");
  wall_f1->Draw("same");
  wall_f2->Draw("same");
   
}

