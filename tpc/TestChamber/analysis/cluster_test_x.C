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
   xclust->SetXTitle("cluster x");
   //xclust->SetFillColor(2);

   TH1D *xclustw = new TH1D("xclustw","",500,-1,1);
   xclustw->SetXTitle("cluster x with cut");
   //xclustw->SetFillColor(2);



   myChain.SetBranchAddress("track", &intr);
   
   for (Int_t iev=0;iev<nevent;iev++){
     
     myChain.GetEntry(iev);
     TCtrack tr(*intr);
     
     
     for(int i=0;i<tr.nCl();++i){
       TCcluster c = tr.getCl(i);
       if(c.getFit()){//was used in fit
	 xclust->Fill(c.getRes().X());
	 
       }
     }
   }
   
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
	 
	 
       }
     }
     
   }
   

   TCanvas *x = new TCanvas("canv1","");   
   xclustw->Draw();
   TCanvas *c3 = new TCanvas("canv2","");   
   xclust->Draw();
   x->Update();
   //chi2->Draw();
   
}

