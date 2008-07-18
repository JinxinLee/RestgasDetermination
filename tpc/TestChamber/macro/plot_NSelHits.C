#include <iostream>
#include <fstream>
#include "TString.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPaveText.h"
#include "TGraphErrors.h"

#include "../src/AnalysisEvent.h"
#include "../src/AnalysisCluster.h"
#define WITH_CUTS

void plot_NSelHits(TString files){

  gSystem->Load("../libanalysis.so"); 
  gROOT->Macro("christian_style.C"); 

  TChain myChain("at");

  TH1D *nselhits = new TH1D("nselhits","",30,0.5,30.5);
  nselhits->SetXTitle("Number of Hits on Track");
  nselhits->SetYTitle("Number of Tracks");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;


  Int_t nevent = myChain.GetEntries();

  AnalysisEvent   *event;

  myChain.SetBranchAddress("EventBranch", &event);

 
  //event loop
  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);

    //global cuts
    
    //if(event->th()>30 || fabs(event->thX())<1) continue;
    //if( (event->th()>30) || (event->th()<1) ) continue;
    if(event->th()>30) continue;
    if(fabs(event->thY())<15) continue;
    if( (event->getNselhits()) < 4 ) continue;

    nselhits->Fill(event->getNselhits());

  }//end of event loop
    
  TCanvas *canvas1 = new TCanvas();
  nselhits->Draw("");
  canvas1->SaveAs("output/nselhits.eps");
  

}

