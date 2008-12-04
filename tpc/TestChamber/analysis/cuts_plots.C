#include <iostream>
#include <fstream>
#include "TString.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "TF2.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPaveText.h"
#include "TROOT.h"
#include "TLegend.h"

#include "../src/TCtrack.h"
#include "../src/TCcluster.h"

#include "consecCut.C"

void numbers(TString files){

  //gSystem->Load("libtestChamber"); 


  //gROOT->Macro("../macro/christian_style.C"); 

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();


  TCtrack *intr=0;

   TH1D *theta = new TH1D("theta","",500,-100,100);
   theta->SetXTitle("angle theta");
   theta->SetYTitle("number of tracks");
   theta->SetStats(kFALSE);
   TH1D *clusters = new TH1D("clusters","",40,-0.5,39.5);
   clusters->SetXTitle("number of clusters on track");
   clusters->SetYTitle("number of tracks");
   clusters->SetStats(kFALSE);
   TH1D *theta_Thx = new TH1D("theta_Thx","",500,-100,100);
   theta_Thx->SetXTitle("angle theta x");
   theta_Thx->SetYTitle("number of tracks");
   theta_Thx->SetStats(kFALSE);
   TH1D *theta_Thy = new TH1D("theta_Thy","",500,-100,100);
   theta_Thy->SetXTitle("angle theta y");
   theta_Thy->SetYTitle("number of tracks");
   theta_Thy->SetStats(kFALSE);

  myChain.SetBranchAddress("track", &intr);

  cout << "nevent" <<  nevent << "good triggers" << endl;

  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);
    TCtrack tr(*intr);

     if (fabs(tr.getAx()) > 1.E3) continue;
     if (fabs(tr.getAy()) > 1.E3) continue;
     if (fabs(tr.getThY()) < 1) continue;
     if (fabs(tr.getThX()) < 1) continue;
     if (tr.nClFit()<5) continue;

      theta->Fill(tr.getTh());

  }

  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);
    TCtrack tr(*intr);

     if (fabs(tr.getTh()) < 1) continue;
     if (fabs(tr.getTh()) > 30) continue;
     if (fabs(tr.getThY()) < 1) continue;
     if (fabs(tr.getThX()) < 1) continue;

     clusters->Fill(tr.nClFit());

  }

  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);
    TCtrack tr(*intr);

     if (fabs(tr.getAx()) > 1.E3) continue;
     if (fabs(tr.getAy()) > 1.E3) continue;
     if (fabs(tr.getTh()) < 1) continue;
     if (fabs(tr.getTh()) > 30) continue;
     if (fabs(tr.getThY()) < 1) continue;
     if (tr.nClFit()<5) continue;

      theta_Thx->Fill(tr.getThX());

  }

  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);
    TCtrack tr(*intr);

     if (fabs(tr.getAx()) > 1.E3) continue;
     if (fabs(tr.getAy()) > 1.E3) continue;
     if (fabs(tr.getTh()) < 1) continue;
     if (fabs(tr.getTh()) > 30) continue;
     if (fabs(tr.getThX()) < 1) continue;
     if (tr.nClFit()<5) continue;

      theta_Thy->Fill(tr.getThY());

  }


  TCanvas *th = new TCanvas();
  theta->Draw();

  th = new TCanvas();
  clusters->Draw();

  th = new TCanvas();
  theta_Thx->Draw();

  th = new TCanvas();
  theta_Thy->Draw();

}

