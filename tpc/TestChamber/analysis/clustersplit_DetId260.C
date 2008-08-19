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


   TH1D *xclustp = new TH1D("xclustp","",500,-1,1);
   xclustp->SetXTitle("residual x [cm]");
   //xclustp->SetFillColor(2);

   TH1D *xclust = new TH1D("xclust","",500,-1,1);
   xclust->SetXTitle("residual x [cm]");
   xclust->SetFillColor(2);

   TH1D *xclustw = new TH1D("xclustw","",500,-1,1);
   xclustw->SetXTitle("residual x [cm]");
   //xclustw->SetFillColor(2);

   TH1D *yclustp = new TH1D("yclustp","",500,-1,1);
   yclustp->SetXTitle("residual y [cm]");
   //yclustp->SetFillColor(2);

   TH1D *yclust = new TH1D("yclust","",500,-1,1);
   yclust->SetXTitle("residual y [cm]");
   yclust->SetFillColor(2);

   TH1D *yclustw = new TH1D("yclustw","",500,-1,1);
   yclustw->SetXTitle("residual y [cm]");
   //yclustw->SetFillColor(2);

   TH1D *zclustp = new TH1D("zclustp","",500,-1,1);
   zclustp->SetXTitle("residual z [cm]");
   //zclustp->SetFillColor(2);

   TH1D *zclust = new TH1D("zclust","",500,-1,1);
   zclust->SetXTitle("residual z [cm]");
   zclust->SetFillColor(2);

   TH1D *zclustw = new TH1D("zclustw","",500,-1,1);
   zclustw->SetXTitle("residual z [cm]");
   //zclustw->SetFillColor(2);



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
	 xclust->Fill(c.getRes().X());
	 yclust->Fill(c.getRes().Y());
	 zclust->Fill(c.getRes().Z());
	 xclustp->Fill(c.getRes().X());
	 yclustp->Fill(c.getRes().Y());
	 zclustp->Fill(c.getRes().Z());
       }
     }
   }
   
   for (Int_t iev=0;iev<nevent;iev++){
     
     myChain.GetEntry(iev);
     TCtrack tr(*intr);
     TCtrack trSplit = clusterSplit1(tr);
     trSplit.fit(260);    
     if (fabs(tr.getAx()) > 1.E3) continue;
     if (fabs(tr.getAy()) > 1.E3) continue;
     if (tr.getChi2()/tr.getNDF()>2) continue;
     if (tr.getChi2()/tr.getNDF()<0.01) continue;
     if (tr.nCl()<2) continue;;


     for(int i=0;i<trSplit.nCl();++i){
       //if(tr.nClFit()<5) continue;
       //std::cout << clusterSplit1(tr) << std::endl;
       TCcluster d = trSplit.getCl(i);
       
       if(d.getFit()){//was used in fit
	 xclustw->Fill(d.getRes().X());
	 yclustw->Fill(d.getRes().Y());
	 zclustw->Fill(d.getRes().Z());
	 
	 
       }
     }

   }

  TF1 *doublegaus_f = new TF1("doublegaus_f",doublegausf,-1,1,6);
  doublegaus_f->SetParameters(200,yclustp->GetMean(),yclustp->GetRMS(),
							 50,yclustp->GetMean(),yclustp->GetRMS());
  
  yclustp->Fit("doublegaus_f","R");

  TF1 *zdoublegaus_f = new TF1("zdoublegaus_f",doublegausf,-1,1,6);
  zdoublegaus_f->SetParameters(200,zclustp->GetMean(),zclustp->GetRMS(),
							 50,zclustp->GetMean(),zclustp->GetRMS());
  
  zclustp->Fit("zdoublegaus_f","R");

  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;


   TCanvas *xw = new TCanvas("canv1","");   
   xclustw->Draw();
   xclust->Draw("same");
   TCanvas *x = new TCanvas("canv2","");   
   xclustp->Draw();
   TCanvas *yw = new TCanvas("canv3","");   
   yclustw->Draw();
   yclust->Draw("same");
   TCanvas *y = new TCanvas("canv4","");   
   yclustp->Draw();
   doublegaus_f->Draw("same");
   TCanvas *zw = new TCanvas("canv5","");   
   zclustw->Draw();
   zclust->Draw("same");
   TCanvas *z = new TCanvas("canv6","");   
   zclustp->Draw();
   zdoublegaus_f->Draw("same");
   
}

