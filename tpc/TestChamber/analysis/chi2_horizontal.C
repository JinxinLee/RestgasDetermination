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
#include "clusterSplit1.C"


void plots(TString files){

  //gSystem->Load("libtestChamber"); 


  gROOT->Macro("../macro/christian_style.C"); 

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();


  TCtrack *intr=0;

  TH1D *chi2NDF = new TH1D("chi2NDF","",200,-2,10);
  chi2NDF->SetLineColor(6);

  TH1D *chi2NDF_2clust = new TH1D("chi2NDF_2clust","",200,-2,10);
  chi2NDF_2clust->SetLineColor(2);

  TH1D *chi2NDF_3clust = new TH1D("chi2NDF_3clust","",200,-2,10);
  chi2NDF_3clust->SetLineColor(3);

  TH1D *chi2NDF_4clust = new TH1D("chi2NDF_4clust","",200,-2,10);
  chi2NDF_4clust->SetLineColor(4);

  TH1D *chi2NDFw = new TH1D("chi2NDFw","",200,-2,10);
  chi2NDFw->SetLineColor(6);

  TH1D *chi2NDF_2clustw = new TH1D("chi2NDF_2clustw","",200,-2,10);
  chi2NDF_2clustw->SetLineColor(2);

  TH1D *chi2NDF_3clustw = new TH1D("chi2NDF_3clustw","",200,-2,10);
  chi2NDF_3clustw->SetLineColor(3);

  TH1D *chi2NDF_4clustw = new TH1D("chi2NDF_4clustw","",200,-2,10);
  chi2NDF_4clustw->SetLineColor(4);


  myChain.SetBranchAddress("track", &intr);

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    TCtrack trSplit = clusterSplit1(tr);
    trSplit.fit(200);


    chi2NDF->Fill(tr.getChi2()/tr.getNDF());

    chi2NDFw->Fill(trSplit.getChi2()/trSplit.getNDF());

  }

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    TCtrack trSplit = clusterSplit1(tr);
    trSplit.fit(200);
    if (tr.nCl()<2) continue;


    chi2NDF_2clust->Fill(tr.getChi2()/tr.getNDF());

    chi2NDF_2clustw->Fill(trSplit.getChi2()/trSplit.getNDF());

  }

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    TCtrack trSplit = clusterSplit1(tr);
    trSplit.fit(200);
    if (tr.nCl()<3) continue;


    chi2NDF_3clust->Fill(tr.getChi2()/tr.getNDF());

    chi2NDF_3clustw->Fill(trSplit.getChi2()/trSplit.getNDF());

  }
  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    TCtrack trSplit = clusterSplit1(tr);
    trSplit.fit(200);
    if (tr.nCl()<4) continue;


    chi2NDF_4clust->Fill(tr.getChi2()/tr.getNDF());

    chi2NDF_4clustw->Fill(trSplit.getChi2()/trSplit.getNDF());

  }

  TCanvas *chi = new TCanvas("chi","");
  chi2NDF->Draw();
  chi2NDF_2clust->Draw("same");
  chi2NDF_3clust->Draw("same");
  chi2NDF_4clust->Draw("same");

  TCanvas *chiw = new TCanvas("chiw","");
  chi2NDFw->Draw();
  chi2NDF_2clustw->Draw("same");
  chi2NDF_3clustw->Draw("same");
  chi2NDF_4clustw->Draw("same");

}

