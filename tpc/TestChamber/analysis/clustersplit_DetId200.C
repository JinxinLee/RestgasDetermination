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


void plots(TString files){

  //gSystem->Load("libtestChamber"); 


  //gROOT->Macro("macro/christian_style.C"); 

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  

  TCtrack *intr=0;


   TH1D *xclust = new TH1D("xclust","",500,-1,1);
   xclust->SetXTitle("cluster x [cm]");
   //xclust->SetFillColor(2);

   TH1D *xclustw = new TH1D("xclustw","",500,-1,1);
   xclustw->SetXTitle("cluster x using ClusterSplit1.C [cm]");
   //xclustw->SetFillColor(2);

   TH1D *yclust = new TH1D("yclust","",500,-1,1);
   yclust->SetXTitle("cluster y [cm]");
   //yclust->SetFillColor(2);

   TH1D *yclustw = new TH1D("yclustw","",500,-1,1);
   yclustw->SetXTitle("cluster y using ClusterSplit1.C [cm]");
   //yclustw->SetFillColor(2);

   TH1D *zclust = new TH1D("zclust","",500,-1,1);
   zclust->SetXTitle("cluster z [cm]");
   //zclust->SetFillColor(2);

   TH1D *zclustw = new TH1D("zclustw","",500,-1,1);
   zclustw->SetXTitle("cluster z using ClusterSplit.C [cm]");
   //zclustw->SetFillColor(2);

  TF1 *fitcurve1 = new TF1("fitcurve1","gaus",-1,1);
  //  fitcurve->SetLineColor(kRed);
  TF1 *fitcurve2 = new TF1("fitcurve2","gaus",-1,1);
  //  fitcurve->SetLineColor(kRed);



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
	 xclust->Fill(c.getRes().X());
	 yclust->Fill(c.getRes().Y());
	 zclust->Fill(c.getRes().Z());
	 
       }
     }
   }
     yclust->Fit("fitcurve1","R");
     zclust->Fit("fitcurve2","R");
   
   for (Int_t iev=0;iev<nevent;iev++){
     
     myChain.GetEntry(iev);
     TCtrack tr(*intr);
     TCtrack trSplit = clusterSplit1(tr);
     trSplit.fit(200);
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
     yclustw->Fit("fitcurve1","R");
     zclustw->Fit("fitcurve2","R");

  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;


   TCanvas *xw = new TCanvas("canv1","");   
   xclustw->Draw();
   TCanvas *x = new TCanvas("canv2","");   
   xclust->Draw();
   TCanvas *yw = new TCanvas("canv3","");   
   yclustw->Draw();
   TCanvas *y = new TCanvas("canv4","");   
   yclust->Draw();
   TCanvas *zw = new TCanvas("canv5","");   
   zclustw->Draw();
   TCanvas *z = new TCanvas("canv6","");   
   zclust->Draw();
   //chi2->Draw();
   
}

