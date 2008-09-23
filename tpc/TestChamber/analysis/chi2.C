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

  TH1D *chi2NDF = new TH1D("chi2NDF","",500,-2,10);
  chi2NDF->SetLineColor(6);

  TH1D *chi2NDF_2clust = new TH1D("chi2NDF_2clust","",500,-2,10);
  chi2NDF_2clust->SetLineColor(2);

  TH1D *chi2NDF_3clust = new TH1D("chi2NDF_3clust","",500,-2,10);
  chi2NDF_3clust->SetLineColor(3);

  TH1D *chi2NDF_4clust = new TH1D("chi2NDF_4clust","",500,-2,10);
  chi2NDF_4clust->SetLineColor(4);


  myChain.SetBranchAddress("track", &intr);

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);

    chi2NDF->Fill(tr.getChi2()/tr.getNDF());

  }

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    if (tr.nCl()<2) continue;


    chi2NDF_2clust->Fill(tr.getChi2()/tr.getNDF());

  }

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    if (tr.nCl()<3) continue;


    chi2NDF_3clust->Fill(tr.getChi2()/tr.getNDF());

  }
  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    if (tr.nCl()<4) continue;


    chi2NDF_4clust->Fill(tr.getChi2()/tr.getNDF());


  }

  TCanvas *chi = new TCanvas("chi","");
  chi2NDF->Draw();
  TCanvas *chi2 = new TCanvas("chi2","");
  chi2NDF_2clust->Draw();
  TCanvas *chi3 = new TCanvas("chi3","");
  chi2NDF_3clust->Draw();
  TCanvas *chi4 = new TCanvas("chi4","");
  chi2NDF_4clust->Draw();
  TCanvas *chiall = new TCanvas("chiall","");
  chi2NDF->Draw();
  chi2NDF_2clust->Draw("same");
  chi2NDF_3clust->Draw("same");
  chi2NDF_4clust->Draw("same");

}

