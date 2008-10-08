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


  gROOT->Macro("../macro/christian_style.C"); 

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  

  TCtrack *intr=0;


   TH1D *wresid = new TH1D("wresid","",500,-0.2,0.2);
   wresid->SetXTitle("residual w [cm]");
   //wresid->SetFillColor(2);

   TH1D *wresidw = new TH1D("wresidw","",500,-0.2,0.2);
   wresidw->SetXTitle("residual w with cut [cm]");
   wresidw->SetFillColor(4);
   wresidw->SetLineColor(4);

   TCanvas *z = new TCanvas();

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
	  wresid->Fill(c.getRes().Z());
 	}
      }


      for(int i=0;i<tr.nCl();++i){
          //if(tr.nClFit()<5) continue;
          if(!consecCut(tr)) continue;
          TCcluster c = tr.getCl(i);
	  if(c.getFit()){//was used in fit
	    wresidw->Fill(c.getRes().Z());
          }

    }
  }

  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;

  wresid->Draw();
  wresidw->Draw("same");
  z->Update();

}

