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


   TH1D *xresid = new TH1D("xresid","",500,-1,1);
   xresid->SetXTitle("residual x [cm]");
   //xresid->SetFillColor(2);

   TH1D *xresidw = new TH1D("xresidw","",500,-1,1);
   xresidw->SetXTitle("residual x with cut [cm]");
   xresidw->SetFillColor(4);

   TCanvas *x = new TCanvas();

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
	  xresid->Fill(c.getRes().X());
 	}
      }


      for(int i=0;i<tr.nCl();++i){
          //if(tr.nClFit()<5) continue;
          if(!consecCut(tr)) continue;
          TCcluster c = tr.getCl(i);
	  if(c.getFit()){//was used in fit
	    xresidw->Fill(c.getRes().X());
          }

    }
  }

  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;

  xresid->Draw();
  xresidw->Draw("same");
  x->Update();

}

