#include <iostream>
#include <fstream>
#include <ctime>
#include "TString.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TPad.h"
#include "TH1.h"
#include "TF1.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPaveText.h"
#include "TROOT.h"

#include "../src/TCtrack.h"
#include "../src/TCcluster.h"
#include "../src/TCclusterSort.h"
#include <algorithm>
#include <vector>
#include <set>

#include "consecCut.C"
#include "clusterSplit1.C"


void plots(TString files){

  //gSystem->Load("libtestChamber"); 


  gROOT->Macro("../macro/christian_style.C"); 

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  

  TCtrack *intr=0;

   TH1D *nHits = new TH1D("nHits","",500,0,30);
   nHits->SetXTitle("Distance between two clusters [cm]");
   //nHits->SetFillColor(2);

   TH1D *nHits1 = new TH1D("nHits1","",500,0,30);
   nHits1->SetXTitle("Distance between two clusters [cm]");
   nHits1->SetLineColor(6);
   //nHits1->SetFillColor(2);

   TH1D *nHits2 = new TH1D("nHits2","",500,0,30);
   nHits2->SetXTitle("distance between two clusters [cm]");
   nHits2->SetLineColor(7);
   //nHits2->SetFillColor(2);

   TH1D *nSelHits = new TH1D("nSelHits","",30,0,30);
   nSelHits->SetXTitle("Number of Clusters on track");
   //nSelHits->SetFillColor(6);

   TH1D *clustersperlength = new TH1D("clustersperlength","",250,0,100);
   clustersperlength->SetXTitle("Number of Clusters per unit length [1/cm]");
   clustersperlength->SetFillColor(6);

  myChain.SetBranchAddress("track", &intr);

  //displays time before starting the event loop

  time_t rawtime;
  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;

  //event loop

  for (Int_t iev=0;iev<nevent;iev++){

    myChain.GetEntry(iev);
    TCtrack tr(*intr);

    double n=0;

    for(int i=0;i<tr.nCl();++i){
	TCcluster c = tr.getCl(i);
	if(c.getFit()){//was used in fit
         for(int j=0;j<tr.nCl();++j){
         TCcluster d = tr.getCl(j);
	 if(d.getFit()){//was used in fit

         //calculate distances in X,Y and Z.
         //Fill this information in one histogram.

         nHits->Fill(fabs(c.posUVW().X()-d.posUVW().X()));
         nHits1->Fill(fabs(c.posUVW().Y()-d.posUVW().Y()));
         nHits2->Fill(fabs(c.posUVW().Z()-d.posUVW().Z()));

        //for the tracklength the X information only is used.

         double a = (fabs(c.posUVW().X()-d.posUVW().X()));

        //calculate maximum distance between two clusters in one Event

          for(int m=1;m<tr.nCl()-1;++m){
          TCcluster k = tr.getCl(m);
          if(k.getFit()){//was used in fit
          double l = (fabs(c.posUVW().X()-k.posUVW().X()));

       //define n as maximum distance

         if(a<l){
          n = (fabs(c.posUVW().X()-k.posUVW().X()));
          }
         else{
          n = (fabs(c.posUVW().X()-d.posUVW().X()));
          }
            }
           }
          }
         }
 	}
      }

    //Fill the two missing histograms

    clustersperlength->Fill(tr.nClFit()/n);
    nSelHits->Fill(tr.nClFit());

  }

  //displays time after finishing the event loop

  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;

  //draw the histograms

  TCanvas *x = new TCanvas();
  nHits1->Draw();
  nHits2->Draw("same");
  nHits->Draw("same");
  TCanvas *y = new TCanvas();
  nSelHits->Draw();
  TCanvas *z = new TCanvas();
  clustersperlength->Draw();

}

