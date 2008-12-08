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


   TH1D *uresid = new TH1D("uresid","",500,-1,1);
   uresid->SetXTitle("residual u [cm]");
   uresid->SetStats(kFALSE);
   //uresid->SetFillColor(2);

   TH1D *uresidw = new TH1D("uresidw","",500,-1,1);
   uresidw->SetXTitle("residual u with cut [cm]");
   //uresidw->SetFillColor(4);   
   //uresidw->SetLineColor(4);
   //uresidw->SetStats(kFALSE);

   TCanvas *x = new TCanvas();

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
	  uresid->Fill(c.getRes().X());
 	}
      }


      for(int i=0;i<tr.nCl();++i){
          //if(tr.nClFit()<5) continue;
		// if(!consecCut(tr)) continue;
          TCcluster c = tr.getCl(i);
	  if(c.getFit()){//was used in fit
	    uresidw->Fill(c.getRes().X());
          }

    }
  }

  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;

  uresid->Draw();
  x->Update();

}

