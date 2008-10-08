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


   TH1D *uresidp = new TH1D("uresidp","",500,-1,1);
   uresidp->SetXTitle("residual u [cm]");
   //uresidp->SetFillColor(2);

   TH1D *uresid = new TH1D("uresid","",500,-1,1);
   uresid->SetXTitle("residual u [cm]");
   //uresid->SetFillColor(2);
   uresid->SetLineColor(2);

   TH1D *uresidw = new TH1D("uresidw","",500,-1,1);
   uresidw->SetXTitle("residual u [cm]");
   //uresidw->SetFillColor(2);

   TH1D *vresidp = new TH1D("vresidp","",500,-0.1,0.1);
   vresidp->SetXTitle("residual v [cm]");
   //vresidp->SetFillColor(2);

   TH1D *vresid = new TH1D("vresid","",500,-0.2,0.2);
   vresid->SetXTitle("residual v [cm]");
   //vresid->SetFillColor(2);
   vresid->SetLineColor(2);

   TH1D *vresidw = new TH1D("vresidw","",500,-0.2,0.2);
   vresidw->SetXTitle("residual v [cm]");
   //vresidw->SetFillColor(2);

   TH1D *wresidp = new TH1D("wresidp","",500,-0.1,0.1);
   wresidp->SetXTitle("residual w [cm]");
   //wresidp->SetFillColor(2);

   TH1D *wresid = new TH1D("wresid","",500,-0.2,0.2);
   wresid->SetXTitle("residual w [cm]");
   //wresid->SetFillColor(2);
   wresid->SetLineColor(2);

   TH1D *wresidw = new TH1D("wresidw","",500,-0.2,0.2);
   wresidw->SetXTitle("residual w [cm]");
   //wresidw->SetFillColor(2);



   myChain.SetBranchAddress("track", &intr);

  time_t rawtime;
  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;
   
   for (Int_t iev=0;iev<nevent;iev++){
     
     myChain.GetEntry(iev);
     TCtrack tr(*intr);
     if (fabs(tr.getAx()) > 1.E3) continue;
     if (fabs(tr.getAy()) > 1.E3) continue;
     if (tr.getChi2()/tr.getNDF()>2) continue;
     if (tr.getChi2()/tr.getNDF()<0.01) continue;
     if (tr.nCl()<2) continue;
     
     for(int i=0;i<tr.nCl();++i){
       TCcluster c = tr.getCl(i);
       if(c.getFit()){//was used in fit
	 uresid->Fill(c.getRes().X());
	 vresid->Fill(c.getRes().Y());
	 wresid->Fill(c.getRes().Z());
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
     if (trSplit.nCl()<2) continue;;


     for(int i=0;i<trSplit.nCl();++i){
       //if(tr.nClFit()<5) continue;
       //std::cout << clusterSplit1(tr) << std::endl;
       TCcluster d = trSplit.getCl(i);
       
       if(d.getFit()){//was used in fit
	 uresidw->Fill(d.getRes().X());
	 vresidw->Fill(d.getRes().Y());
	 wresidw->Fill(d.getRes().Z());
	 
	 
       }
     }

   }

  TF1 *doublegaus_f = new TF1("doublegaus_f",doublegausf,-1,1,6);
  doublegaus_f->SetParameters(200,vresidp->GetMean(),vresidp->GetRMS(),
							 50,vresidp->GetMean(),vresidp->GetRMS());
  
  vresidp->Fit("doublegaus_f","R");

  TF1 *zdoublegaus_f = new TF1("zdoublegaus_f",doublegausf,-1,1,6);
  zdoublegaus_f->SetParameters(200,wresidp->GetMean(),wresidp->GetRMS(),
							 50,wresidp->GetMean(),wresidp->GetRMS());
  
  wresidp->Fit("zdoublegaus_f","R");

  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;


   TCanvas *xw = new TCanvas("canv1","");   
   uresidw->Draw();
   uresid->Draw("same");
   TCanvas *x = new TCanvas("canv2","");   
   uresidp->Draw();
   TCanvas *yw = new TCanvas("canv3","");   
   vresidw->Draw();
   vresid->Draw("same");
   TCanvas *y = new TCanvas("canv4","");   
   vresidp->Draw();
   doublegaus_f->Draw("same");
   TCanvas *zw = new TCanvas("canv5","");   
   wresid->Draw();
   wresidw->Draw("same");
   TCanvas *z = new TCanvas("canv6","");   
   wresidp->Draw();
   zdoublegaus_f->Draw("same");
   
}

