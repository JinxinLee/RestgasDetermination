#include <iostream>
#include <fstream>
#include <ctime>
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
#include "cuts.C"

void plots(TString files){

  //gSystem->Load("libtestChamber"); 


  gROOT->Macro("../macro/christian_style.C"); 

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  

  TCtrack *intr=0;


   TH1D *vresid = new TH1D("vresid","",500,-0.2,0.2);
   vresid->SetXTitle("residual v [cm]");
   vresid->SetStats(kFALSE);
   //vresid->SetFillColor(2);

   TH1D *vresidw = new TH1D("vresidw","",500,-0.2,0.2);
   vresidw->SetXTitle("residual v with cut [cm]");
   vresidw->SetFillColor(4);
   vresidw->SetLineColor(4);

   TCanvas *y = new TCanvas();

   myChain.SetBranchAddress("track", &intr);

   time_t rawtime;
   time ( &rawtime );
   std::cout<< "The current time is " << ctime(&rawtime) << std::endl;

   for (Int_t iev=0;iev<nevent;iev++){

     myChain.GetEntry(iev);
     TCtrack tr(*intr);
     if (!IEEE(tr)) continue;


      for(int i=0;i<tr.nCl();++i){
	TCcluster c = tr.getCl(i);
	if(c.getFit()){//was used in fit
	  vresid->Fill(c.getRes().Y());
 	}
      }


      for(int i=0;i<tr.nCl();++i){
          //if(tr.nClFit()<5) continue;
          if(!consecCut(tr)) continue;
          TCcluster c = tr.getCl(i);
	  if(c.getFit()){//was used in fit
	    vresidw->Fill(c.getRes().Y());
          }
       }
  }

  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;

  vresid->Draw();
  //  vresidw->Draw("same");
  y->Update();

}

