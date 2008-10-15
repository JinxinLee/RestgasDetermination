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


void clusterperlength(TString files){

  //gSystem->Load("libtestChamber"); 


  gROOT->Macro("../macro/christian_style.C"); 

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  

  TCtrack *intr=0;

  TH1D *Distancex = new TH1D("Distancex","",500,0,30);
  Distancex->SetXTitle("Distance between two clusters [cm]");
  //Distancex->SetFillColor(2);

  TH1D *Distancey = new TH1D("Distancey","",500,0,30);
  Distancey->SetXTitle("Distance between two clusters [cm]");
  Distancey->SetLineColor(6);
  //Distancey->SetFillColor(2);

  TH1D *Distancez = new TH1D("Distancez","",500,0,30);
  Distancez->SetXTitle("Distance between two clusters [cm]");
  Distancez->SetLineColor(7);
  //Distancez->SetFillColor(2);

  TH1D *nSelHits = new TH1D("nSelHits","",30,0,30);
  nSelHits->SetXTitle("Number of Clusters on track");
  //nSelHits->SetFillColor(6);

  TH1D *clustersperlength = new TH1D("clustersperlength","",250,0,30);
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

    if(!IEEE(tr)) continue;

    double maxDist=-1;
    double maxDistI=-1;
    double maxDistJ=-1;

    for(int i=0;i<tr.nCl();++i){
      TCcluster c = tr.getCl(i);
      if(c.getFit()){//was used in fit
	for(int j=i+1;j<tr.nCl();++j){
	  TCcluster d = tr.getCl(j);
	  if(d.getFit()){//was used in fit

	    //calculate distances in X,Y and Z.
	    //Fill this information in one histogram.

	    Distancex->Fill(fabs(c.posUVW().X()-d.posUVW().X()));
	    Distancey->Fill(fabs(c.posUVW().Y()-d.posUVW().Y()));
	    Distancez->Fill(fabs(c.posUVW().Z()-d.posUVW().Z()));

	    //calculate distance between two Clusters using TVector3.

	    TVector3 distanceVector = c.posUVW()-d.posUVW();
	    double a = distanceVector.Mag();
	    if(a>maxDist){
	      maxDist=a;
	      maxDistI = i;
	      maxDistJ = j;
	    }
	  }
	}
      }
    }

    //Fill the two missing histograms

    clustersperlength->Fill(tr.nClFit()/maxDist);
    nSelHits->Fill(tr.nClFit());

  }

  //displays time after finishing the event loop

  time ( &rawtime );
  std::cout<< "The current time is " << ctime(&rawtime) << std::endl;

  //draw the histograms

  TCanvas *x = new TCanvas();
  Distancey->Draw();
  Distancex->Draw("same");
  Distancez->Draw("same");
  TCanvas *y = new TCanvas();
  nSelHits->Draw();
  TCanvas *z = new TCanvas();
  clustersperlength->Draw();

}

