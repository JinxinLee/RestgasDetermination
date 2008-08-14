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


void Hits(TString files){

  //gSystem->Load("libtestChamber"); 


  gROOT->Macro("../macro/christian_style.C"); 

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  

  TCtrack *intr=0;


   TH1D *nHits = new TH1D("nHits","",31,0,30);
   nHits->SetXTitle("Number of Clusters");
   //xresid->SetFillColor(2);

   TH1D *nSelHits = new TH1D("nSelHits","",31,0,30);
   nSelHits->SetXTitle("Number of Clusters on track");
   nSelHits->SetLineColor(2);
   //nSelHits->SetFillColor(6);

   TH1D *difference = new TH1D("nSelHits-nHits","",31,0,30);
   difference->SetXTitle("# Clusters - # Clusters on track");
   //nSelHits->SetFillColor(6);

  myChain.SetBranchAddress("track", &intr);

  time_t rawtime;
  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;

  for (Int_t iev=0;iev<nevent;iev++){

    myChain.GetEntry(iev);
    TCtrack tr(*intr);

    nSelHits->Fill(tr.nClFit());
    nHits->Fill(tr.nCl());
    difference->Fill(tr.nCl() - tr.nClFit());
  }

  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;

  TCanvas *both = new TCanvas();
  nSelHits->Draw();
  nHits->Draw("same");
  TCanvas *selhits = new TCanvas();
  nSelHits->Draw();
  TCanvas *hits = new TCanvas();
  nHits->Draw();
  TCanvas *diff = new TCanvas();
  difference->Draw();

}

