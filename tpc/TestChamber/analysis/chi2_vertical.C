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

  TH1D *chi2 = new TH1D("chi2","",200,-2,10);

  TH1D *chi2NDFw = new TH1D("chi2NDFw","",200,-2,10);
  chi2NDFw->SetLineColor(6);

  TH1D *chi2w = new TH1D("chi2w","",200,-2,10);

  myChain.SetBranchAddress("track", &intr);

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    TCtrack trSplit = clusterSplit1(tr);
    trSplit.fit(260);


    chi2NDF->Fill(tr.getChi2()/tr.getNDF());
    chi2->Fill(tr.getChi2());

    chi2NDFw->Fill(trSplit.getChi2()/trSplit.getNDF());
    chi2w->Fill(trSplit.getChi2());


  }

  TCanvas *chi = new TCanvas("chi","");
  chi2NDF->Draw();
  chi2->Draw("same");

  TCanvas *chiw = new TCanvas("chiw","");
  chi2NDFw->Draw();
  chi2w->Draw("same");

}

