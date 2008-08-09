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


void clustersize(TString files){

  //gSystem->Load("libtestChamber"); 


  gROOT->Macro("../macro/christian_style.C"); 

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  

  TCtrack *intr=0;


   TH1D *clustersize = new TH1D("clustersize","",10,0,10);
   clustersize->SetXTitle("clustersize");
   //clustersize->SetFillColor(2);


  TF1 *fitcurve1 = new TF1("fitcurve1","gaus",-1,1);
  //  fitcurve->SetLineColor(kRed);



   myChain.SetBranchAddress("track", &intr);

   
   for (Int_t iev=0;iev<nevent;iev++){
     
     myChain.GetEntry(iev);
     TCtrack tr(*intr);
     
     
     for(int i=0;i<tr.nCl();++i){
       TCcluster c = tr.getCl(i);
       if(c.getFit()){//was used in fit
	 clustersize->Fill(c.nRaw());
       }
     }
   }


   TCanvas *size = new TCanvas("canv1","");   
   clustersize->Draw();
   
}

