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

void angulars(TString files){

  //gSystem->Load("libtestChamber"); 


  //gROOT->Macro("../macro/christian_style.C"); 

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();


  TCtrack *intr=0;

   TH2D *theta_vs_thetaX = new TH2D("theta_vs_thetaX","",500,-100,100,500,-100,100);
   theta_vs_thetaX->SetXTitle("theta");
   theta_vs_thetaX->SetYTitle("theta x");
   TH2D *theta_vs_thetaY = new TH2D("theta_vs_thetaY","",500,-100,100,500,-100,100);
   theta_vs_thetaY->SetXTitle("theta");
   theta_vs_thetaY->SetYTitle("theta y");
   TH2D *thetaY_vs_thetaX = new TH2D("thetaY_vs_thetaX","",500,-100,100,500,-100,100);
   theta_vs_thetaX->SetXTitle("theta y");
   theta_vs_thetaX->SetYTitle("theta x");

   TH1D *theta_4clust = new TH1D("theta_4clust","",500,-100,100);
   theta_4clust->SetXTitle("theta");
   theta_4clust->SetYTitle("Number of tracks");
   theta_4clust->SetLineColor(2);
   TH1D *thetax_4clust = new TH1D("thetax_4clust","",500,-100,100);
   thetax_4clust->SetXTitle("theta x");
   thetax_4clust->SetYTitle("Number of tracks");
   thetax_4clust->SetLineColor(2);
   TH1D *thetay_4clust = new TH1D("thetay_4clust","",500,-100,100);
   thetay_4clust->SetXTitle("theta y ");
   thetay_4clust->SetYTitle("Number of tracks");
   thetay_4clust->SetLineColor(2);

   TH1D *theta_6clust = new TH1D("theta_6clust","",500,-100,100);
   theta_6clust->SetXTitle("theta");
   theta_6clust->SetLineColor(3);
   TH1D *thetax_6clust = new TH1D("thetax_6clust","",500,-100,100);
   thetax_6clust->SetXTitle("theta x");
   thetax_6clust->SetLineColor(3);
   TH1D *thetay_6clust = new TH1D("thetay_6clust","",500,-100,100);
   thetay_6clust->SetXTitle("theta y ");
   thetay_6clust->SetLineColor(3);

   TH1D *theta_8clust = new TH1D("theta_8clust","",500,-100,100);
   theta_8clust->SetXTitle("theta");
   theta_8clust->SetLineColor(4);
   TH1D *thetax_8clust = new TH1D("thetax_8clust","",500,-100,100);
   thetax_8clust->SetXTitle("theta x");
   thetax_8clust->SetLineColor(4);
   TH1D *thetay_8clust = new TH1D("thetay_8clust","",500,-100,100);
   thetay_8clust->SetXTitle("theta y ");
   thetay_8clust->SetLineColor(4);

  myChain.SetBranchAddress("track", &intr);

  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);
    TCtrack tr(*intr);

     if (fabs(tr.getAx()) > 1.E3) continue;
     if (fabs(tr.getAy()) > 1.E3) continue;
     //if (tr.getChi2()/tr.getNDF()>4) continue;
     //if (tr.getChi2()/tr.getNDF()<0.1) continue;
     if (tr.nClFit()<5) continue;

	  thetay_4clust->Fill(tr.getThY());
	  thetax_4clust->Fill(tr.getThX());
          theta_4clust->Fill(tr.getTh());
	  theta_vs_thetaX->Fill(tr.getTh(),tr.getThX());
	  theta_vs_thetaY->Fill(tr.getTh(),tr.getThY());
	  thetaY_vs_thetaX->Fill(tr.getThY(),tr.getThX());

  }

  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);
    TCtrack tr(*intr);

     if (fabs(tr.getAx()) > 1.E3) continue;
     if (fabs(tr.getAy()) > 1.E3) continue;
     //if (tr.getChi2()/tr.getNDF()>4) continue;
     //if (tr.getChi2()/tr.getNDF()<0.1) continue;
     if (tr.nClFit()<7) continue;

	  thetay_6clust->Fill(tr.getThY());
	  thetax_6clust->Fill(tr.getThX());
          theta_6clust->Fill(tr.getTh());

  }

  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);
    TCtrack tr(*intr);

     if (fabs(tr.getAx()) > 1.E3) continue;
     if (fabs(tr.getAy()) > 1.E3) continue;
     //if (tr.getChi2()/tr.getNDF()>4) continue;
     //if (tr.getChi2()/tr.getNDF()<0.1) continue;
     if (tr.nClFit()<9) continue;

	  thetay_8clust->Fill(tr.getThY());
	  thetax_8clust->Fill(tr.getThX());
          theta_8clust->Fill(tr.getTh());

  }

  TLegend *leg1 = new TLegend(0.6,0.7,0.999,0.999);
  leg1->AddEntry(theta_4clust,"more than 4 clusters on track","l");
  leg1->AddEntry(theta_6clust,"more than 6 clusters on track","l");
  leg1->AddEntry(theta_8clust,"more than 8 clusters on track","l");
  leg1->SetFillColor(0);

  TLegend *leg2 = new TLegend(0.6,0.7,0.999,0.999);
  leg2->AddEntry(thetay_4clust,"more than 4 clusters on track","l");
  leg2->AddEntry(thetay_6clust,"more than 6 clusters on track","l");
  leg2->AddEntry(thetay_8clust,"more than 8 clusters on track","l");
  leg2->SetFillColor(0);

  TLegend *leg3 = new TLegend(0.6,0.7,0.999,0.999);
  leg3->AddEntry(thetax_4clust,"more than 4 clusters on track","l");
  leg3->AddEntry(thetax_6clust,"more than 6 clusters on track","l");
  leg3->AddEntry(thetax_8clust,"more than 8 clusters on track","l");
  leg3->SetFillColor(0);

  TCanvas *th = new TCanvas();
  theta_vs_thetaX->Draw("colz");
  TCanvas *thy = new TCanvas();
  theta_vs_thetaY->Draw("colz");
  TCanvas *thx = new TCanvas();
  thetaY_vs_thetaX->Draw("colz");

  th = new TCanvas();
  theta_4clust->Draw();
  thy = new TCanvas();
  thetay_4clust->Draw();
  thx = new TCanvas();
  thetax_4clust->Draw();

  th = new TCanvas();
  theta_6clust->Draw();
  thy = new TCanvas();
  thetay_6clust->Draw();
  thx = new TCanvas();
  thetax_6clust->Draw();

  th = new TCanvas();
  theta_8clust->Draw();
  thy = new TCanvas();
  thetay_8clust->Draw();
  thx = new TCanvas();
  thetax_8clust->Draw();

  th = new TCanvas();
  theta_4clust->Draw();
  theta_6clust->Draw("same");
  theta_8clust->Draw("same");
  leg1->Draw();
  thy = new TCanvas();
  thetay_4clust->Draw();
  thetay_6clust->Draw("same");
  thetay_8clust->Draw("same");
  leg2->Draw();
  thx = new TCanvas();
  thetax_4clust->Draw();
  thetax_6clust->Draw("same");
  thetax_8clust->Draw("same");
  leg3->Draw();

}

