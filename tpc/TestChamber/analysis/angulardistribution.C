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


  //gROOT->Macro("../macro/christian_style.C"); 

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

   TH1D *theta_2clust = new TH1D("theta_2clust","",500,-100,100);
   theta_2clust->SetXTitle("theta");
   theta_2clust->SetLineColor(2);
   TH1D *thetax_2clust = new TH1D("thetax_2clust","",500,-100,100);
   thetax_2clust->SetXTitle("theta x");
   thetax_2clust->SetLineColor(2);
   TH1D *thetay_2clust = new TH1D("thetay_2clust","",500,-100,100);
   thetay_2clust->SetXTitle("theta y ");
   thetay_2clust->SetLineColor(2);

   TH1D *theta_3clust = new TH1D("theta_3clust","",500,-100,100);
   theta_3clust->SetXTitle("theta");
   theta_3clust->SetLineColor(3);
   TH1D *thetax_3clust = new TH1D("thetax_3clust","",500,-100,100);
   thetax_3clust->SetXTitle("theta x");
   thetax_3clust->SetLineColor(3);
   TH1D *thetay_3clust = new TH1D("thetay_3clust","",500,-100,100);
   thetay_3clust->SetXTitle("theta y ");
   thetay_3clust->SetLineColor(3);

   TH1D *theta_4clust = new TH1D("theta_4clust","",500,-100,100);
   theta_4clust->SetXTitle("theta");
   theta_4clust->SetLineColor(4);
   TH1D *thetax_4clust = new TH1D("thetax_4clust","",500,-100,100);
   thetax_4clust->SetXTitle("theta x");
   thetax_4clust->SetLineColor(4);
   TH1D *thetay_4clust = new TH1D("thetay_4clust","",500,-100,100);
   thetay_4clust->SetXTitle("theta y ");
   thetay_4clust->SetLineColor(4);

  myChain.SetBranchAddress("track", &intr);

  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);
    TCtrack tr(*intr);

     if (fabs(tr.getAx()) > 1.E3) continue;
     if (fabs(tr.getAy()) > 1.E3) continue;
     //if (tr.getChi2()/tr.getNDF()>4) continue;
     //if (tr.getChi2()/tr.getNDF()<0.1) continue;
     //if (tr.nClFit()<2) continue;

	  thetay->Fill(tr.getThY());
	  thetax->Fill(tr.getThX());
          theta->Fill(tr.getTh());

  }

  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);
    TCtrack tr(*intr);

     if (fabs(tr.getAx()) > 1.E3) continue;
     if (fabs(tr.getAy()) > 1.E3) continue;
     //if (tr.getChi2()/tr.getNDF()>4) continue;
     //if (tr.getChi2()/tr.getNDF()<0.1) continue;
     if (tr.nClFit()<2) continue;

	  thetay_2clust->Fill(tr.getThY());
	  thetax_2clust->Fill(tr.getThX());
          theta_2clust->Fill(tr.getTh());

  }

  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);
    TCtrack tr(*intr);

     if (fabs(tr.getAx()) > 1.E3) continue;
     if (fabs(tr.getAy()) > 1.E3) continue;
     //if (tr.getChi2()/tr.getNDF()>4) continue;
     //if (tr.getChi2()/tr.getNDF()<0.1) continue;
     if (tr.nClFit()<3) continue;

	  thetay_3clust->Fill(tr.getThY());
	  thetax_3clust->Fill(tr.getThX());
          theta_3clust->Fill(tr.getTh());

  }

  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);
    TCtrack tr(*intr);

     if (fabs(tr.getAx()) > 1.E3) continue;
     if (fabs(tr.getAy()) > 1.E3) continue;
     //if (tr.getChi2()/tr.getNDF()>4) continue;
     //if (tr.getChi2()/tr.getNDF()<0.1) continue;
     if (tr.nClFit()<4) continue;

	  thetay_4clust->Fill(tr.getThY());
	  thetax_4clust->Fill(tr.getThX());
          theta_4clust->Fill(tr.getTh());

  }

  TCanvas *th = new TCanvas();
  theta->Draw();
  TCanvas *thy = new TCanvas();
  thetay->Draw();
  TCanvas *thx = new TCanvas();
  thetax->Draw();

  th = new TCanvas();
  theta_2clust->Draw();
  thy = new TCanvas();
  thetay_2clust->Draw();
  thx = new TCanvas();
  thetax_2clust->Draw();

  th = new TCanvas();
  theta_3clust->Draw();
  thy = new TCanvas();
  thetay_3clust->Draw();
  thx = new TCanvas();
  thetax_3clust->Draw();

  th = new TCanvas();
  theta_4clust->Draw();
  thy = new TCanvas();
  thetay_4clust->Draw();
  thx = new TCanvas();
  thetax_4clust->Draw();


  th = new TCanvas();
  theta->Draw();
  theta_2clust->Draw("same");
  theta_3clust->Draw("same");
  theta_4clust->Draw("same");
  thy = new TCanvas();
  thetay->Draw();
  thetay_2clust->Draw("same");
  thetay_3clust->Draw("same");
  thetay_4clust->Draw("same");
  thx = new TCanvas();
  thetax->Draw();
  thetax_2clust->Draw("same");
  thetax_3clust->Draw("same");
  thetax_4clust->Draw("same");

}

