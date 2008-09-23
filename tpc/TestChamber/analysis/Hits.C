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
#include "clusterSplit1.C"

void plots(TString files){

  //gSystem->Load("libtestChamber"); 


  gROOT->Macro("../macro/christian_style.C"); 

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  

  TCtrack *intr=0;


  TH1D *nHits = new TH1D("nHits","",30,0,30);
  nHits->SetXTitle("Number of Clusters");
  //xresid->SetFillColor(2);

  TH1D *nSelHits = new TH1D("nSelHits","",30,0,30);
  nSelHits->SetXTitle("Number of Clusters on track");
  nSelHits->SetLineColor(6);

  TH1D *diffnhits = new TH1D("diffnhits","",30,0,30);
  diffnhits->SetXTitle("Number of Hits - Number of Hits on Track");

  TH1D *diffnhits3 = new TH1D("diffnhits3","",30,0,30);
  diffnhits3->SetXTitle("");
  diffnhits3->SetLineColor(5);

  myChain.SetBranchAddress("track", &intr);

  time_t rawtime;
  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;

  for (Int_t iev=0;iev<nevent;iev++){

    myChain.GetEntry(iev);
    TCtrack tr(*intr);

    nSelHits->Fill(tr.nClFit());
    nHits->Fill(tr.nCl());
    diffnhits->Add(nHits, nSelHits, 1, -1);
    diffnhits3->Add(nHits, nSelHits, 1, -1);
  }


  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;

  TCanvas *x = new TCanvas();
  nHits->Draw();
  nSelHits->Draw("same");
  TCanvas *diff = new TCanvas();
  diffnhits->Draw();
  TCanvas *diff3 = new TCanvas();
  nHits->Draw();
  nSelHits->Draw("same");
  diffnhits3->Draw("same");


}

