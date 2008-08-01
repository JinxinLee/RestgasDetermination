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

void angulars(TString files){

  //gSystem->Load("libtestChamber"); 


  //gROOT->Macro("macro/christian_style.C"); 

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();


  TCtrack *intr=0;

   TH1D *theta = new TH1D("theta","",500,-100,100);
   theta->SetXTitle("theta");
   TH1D *thetax = new TH1D("thetax","",500,-100,100);
   thetax->SetXTitle("theta x");
   TH1D *thetay = new TH1D("thetay","",500,-100,100);
   thetay->SetXTitle("theta y ");


  myChain.SetBranchAddress("track", &intr);

  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);
    TCtrack tr(*intr);

     if (fabs(tr.getAx()) > 1.E3) continue;
     //if (tr.getAy() < 1.E6) continue;

	  thetay->Fill(tr.getThY());
	  thetax->Fill(tr.getThX());
          theta->Fill(tr.getTh());

  }

  TCanvas *th = new TCanvas();
  theta->Draw();
  TCanvas *thy = new TCanvas();
  thetay->Draw();
  TCanvas *thx = new TCanvas();
  thetax->Draw();

}

