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
#include "TLegend.h"

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

  TH1D *chi2NDF_4clust = new TH1D("chi2NDF_4clust","",500,-2,10);
  chi2NDF_4clust->SetXTitle("chi^2 / number of degrees of freedom");
  chi2NDF_4clust->SetLineColor(4);

  TH1D *chi2NDF_6clust = new TH1D("chi2NDF_6clust","",500,-2,10);
  chi2NDF_6clust->SetXTitle("chi^2 / number of degrees of freedom");
  chi2NDF_6clust->SetLineColor(6);

  TH1D *chi2NDF_8clust = new TH1D("chi2NDF_8clust","",500,-2,10);
  chi2NDF_8clust->SetXTitle("chi^2 / number of degrees of freedom");
  chi2NDF_8clust->SetLineColor(5);

  TH1D *chi2NDF_10clust = new TH1D("chi2NDF_10clust","",500,-2,10);
  chi2NDF_10clust->SetXTitle("chi^2 / number of degrees of freedom");
  chi2NDF_10clust->SetLineColor(3);

  TH1D *chi2NDF_12clust = new TH1D("chi2NDF_12clust","",500,-2,10);
  chi2NDF_12clust->SetXTitle("chi^2 / number of degrees of freedom");
  chi2NDF_12clust->SetLineColor(7);

  myChain.SetBranchAddress("track", &intr);

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    if (tr.nClFit()<4) continue;
    if(tr.getThX()<1)continue;

    chi2NDF_4clust->Fill(tr.getChi2()/tr.getNDF());


  }

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    if (tr.nClFit()<6) continue;
    if(tr.getThX()<1)continue;

    chi2NDF_6clust->Fill(tr.getChi2()/tr.getNDF());


  }
  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    if (tr.nClFit()<8) continue;
    if(tr.getThX()<1)continue;

    chi2NDF_8clust->Fill(tr.getChi2()/tr.getNDF());


  }

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    if (tr.nClFit()<10) continue;
    if(tr.getThX()<1)continue;

    chi2NDF_10clust->Fill(tr.getChi2()/tr.getNDF());


  }

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    if (tr.nClFit()<12) continue;
    if(tr.getThX()<1)continue;

    chi2NDF_12clust->Fill(tr.getChi2()/tr.getNDF());


  }

  TLegend *leg = new TLegend(0.5,0.7,0.999,0.999);
  leg->AddEntry(chi2NDF_4clust,"more than 4 clusters on track","l");
  leg->AddEntry(chi2NDF_6clust,"more than 6 clusters on track","l");
  leg->AddEntry(chi2NDF_8clust,"more than 8 clusters on track","l");
  leg->AddEntry(chi2NDF_10clust,"more than 10 clusters on track","l");
  leg->AddEntry(chi2NDF_12clust,"more than 12 clusters on track","l");
  leg->SetFillColor(0);

  TCanvas *chi4 = new TCanvas("chi4","");
  chi2NDF_4clust->Draw();
  TCanvas *chi6 = new TCanvas("chi6","");
  chi2NDF_6clust->Draw();
  TCanvas *chi8 = new TCanvas("chi8","");
  chi2NDF_8clust->Draw();
  TCanvas *chi10 = new TCanvas("chi10","");
  chi2NDF_10clust->Draw();
  TCanvas *chi12 = new TCanvas("chi12","");
  chi2NDF_12clust->Draw();
  TCanvas *chiall = new TCanvas("chiall","");
  chi2NDF_4clust->Draw();
  chi2NDF_6clust->Draw("same");
  chi2NDF_8clust->Draw("same");
  chi2NDF_10clust->Draw("same");
  chi2NDF_12clust->Draw("same");
  leg->Draw();

}

