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


void plots(TString files){

  //gSystem->Load("libtestChamber"); 


  //gROOT->Macro("macro/christian_style.C"); 

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  

  TCtrack *intr=0;


   TH1D *yresid = new TH1D("yresid","",500,-1,1);
   yresid->SetXTitle("residual y [cm]");
   //yresid->SetFillColor(2);

   TH1D *yresidw = new TH1D("yresidw","",500,-1,1);
   yresidw->SetXTitle("residual y with cut [cm]");
   yresidw->SetFillColor(4);

   TCanvas *y = new TCanvas();

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
	  yresid->Fill(c.getRes().X());
 	}
      }


      for(int i=0;i<tr.nCl();++i){
          //if(tr.nClFit()<5) continue;
          if(!consecCut(tr)) continue;
          TCcluster c = tr.getCl(i);
	  if(c.getFit()){//was used in fit
	    yresidw->Fill(c.getRes().X());
          }
       }
  }

  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;

  yresid->Draw();
  yresidw->Draw("same");

  y->Update();

}

