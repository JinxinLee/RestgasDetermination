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
#include "TROOT.h"

#include "../src/AnalysisEvent.h"
#include "../src/AnalysisCluster.h"
#define WITH_CUTS

void plot_clustsize(TString files){

  gSystem->Load("../libanalysis.so"); 
  gROOT->Macro("christian_style.C"); 

  TChain myChain("at");

  TH1D *clustsize = new TH1D("clustsize","",15,0.5,15.5);
  clustsize->SetXTitle("Cluster Size");
  clustsize->SetYTitle("Number of Clusters");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;


  Int_t nevent = myChain.GetEntries();

  AnalysisEvent *event = new AnalysisEvent();

  myChain.SetBranchAddress("EventBranch", &event);

 
  //event loop
  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);

    //global cuts
    
    //if(event->th()>30 || fabs(event->thX())<1) continue;
    //if( (event->th()>30) || (event->th()<1) ) continue;
    if(event->th()>30) continue;
    if( (event->getNselhits()) < 4 ) continue;

    for(unsigned int j=0;j<event->nClusters();++j) {

      clustsize->Fill(event->getCluster(j)->clustersize());

    }

  }//end of event loop
    
  TCanvas *canvas1 = new TCanvas();
  clustsize->Draw("");
  canvas1->SaveAs("output/clustsize.eps");
  

}

