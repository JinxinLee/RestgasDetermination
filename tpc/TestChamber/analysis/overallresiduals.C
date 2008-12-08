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

#include "cuts.C"
#include "corrections.C"

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


  TH1D *uresidp = new TH1D("uresidp","",500,-1,1);
  uresidp->SetXTitle("residual u [cm]");
  uresidp->SetYTitle("Number of clusters");
  //uresidp->SetStats(kFALSE);
  //uresidp->SetFillColor(2);

  TH1D *vresidp = new TH1D("vresidp","",500,-0.1,0.1);
  vresidp->SetXTitle("residual v [cm]");
  vresidp->SetYTitle("Number of tracks");
  //vresidp->SetStats(kFALSE);
  //vresidp->SetFillColor(2);

  TH1D *wresidp = new TH1D("wresidp","",500,-0.1,0.1);
  wresidp->SetXTitle("residual w [cm]");
  wresidp->SetYTitle("Number of tracks");
  //wresidp->SetStats(kFALSE);
  //wresidp->SetFillColor(2);




  myChain.SetBranchAddress("track", &intr);

  time_t rawtime;
  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;
   
  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);

		TCtrack ctr = without_end_pads(tr);      

	// if(!IEEE(tr)) continue;
	  if(!IEEE(ctr)) continue;

	  //  for(int i=0;i<tr.nCl();++i){
		 //   TCcluster c = tr.getCl(i);
		   for(int i=0;i<ctr.nCl();++i){
		 TCcluster c = ctr.getCl(i);
      if(c.getFit()){//was used in fit

	uresidp->Fill(c.getRes().X());
	vresidp->Fill(c.getRes().Y());
	wresidp->Fill(c.getRes().Z());
      }
    }
  }

  //end of event loop

  //fits

  //definitions

  Double_t vres_all = -1;
  Double_t wres_all = -1;

  Double_t err_vres_all = -1;
  Double_t err_wres_all = -1;

  //v without

  TF1 *doublegaus_vall = new TF1("doublegaus_vall",doublegausf,-.1,.1,6);

  doublegaus_vall->SetParameters(200,vresidp->GetMean(),vresidp->GetRMS(),50,vresidp->GetMean(),vresidp->GetRMS());

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
  
  Double_t intvall_1 = vall_f1->Integral(-0.1,0.1);
  Double_t intvall_2 = vall_f2->Integral(-0.1,0.1);
  
  vres_all = (intvall_1*fabs(vall_f1->GetParameter(2))+ intvall_2*fabs(vall_f2->GetParameter(2)))/(intvall_1 + intvall_2);

  err_vres_all  = (1./(intvall_1+intvall_2));
  err_vres_all *= sqrt(intvall_1*intvall_1*pow((vall_f1->GetParError(2)),2)+intvall_2*intvall_2*pow(vall_f2->GetParError(2),2));

  cout<<endl<<"Overall resolution v "<<vres_all<<" +- " <<err_vres_all<<endl<<endl;

  // w without

  TF1 *doublegaus_wall = new TF1("doublegaus_wall",doublegausf,-.1,.1,6);

  doublegaus_wall->SetParameters(200,wresidp->GetMean(),wresidp->GetRMS(),50,wresidp->GetMean(),wresidp->GetRMS());

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
  
  Double_t intwall_1 = wall_f1->Integral(-0.1,0.1);
  Double_t intwall_2 = wall_f2->Integral(-0.1,0.1);
  
  wres_all = (intwall_1*fabs(wall_f1->GetParameter(2))+ intwall_2*fabs(wall_f2->GetParameter(2)))/(intwall_1 + intwall_2);

  err_wres_all  = (1./(intwall_1+intwall_2));
  err_wres_all *= sqrt(intwall_1*intwall_1*pow((wall_f1->GetParError(2)),2)+intwall_2*intwall_2*pow(wall_f2->GetParError(2),2));

  cout<<endl<<"Overall resolution w "<<wres_all<<" +- " <<err_wres_all<<endl<<endl;


  //plot time needed

  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;

  //plots

  //u

  TCanvas *x = new TCanvas("canv2","");   
   uresidp->Draw();

  TCanvas *y = new TCanvas("canv4","");   
  vresidp->Draw();
  doublegaus_vall->Draw("same");
   vall_f1->Draw("same");
   vall_f2->Draw("same");

  /*  TText *t1 = new TText(0.125, 0.87, "Overall resolution v "<<vres_all<<" +- " <<err_vres_all);
  t1->SetNDC();
  t1->SetTextSize(0.038);
  t1->SetTextFont(102);
  t1->Draw();
  */

  TCanvas *z = new TCanvas("canv6","");   
  wresidp->Draw();
   doublegaus_wall->Draw("same");
   wall_f1->Draw("same");
   wall_f2->Draw("same");
   
}

