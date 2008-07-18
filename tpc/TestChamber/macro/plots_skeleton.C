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

#include "../src/AnalysisEvent.h"
#include "../src/AnalysisCluster.h"
#define WITH_CUTS


Double_t doublegausf(Double_t *x,Double_t *par)
{
  Double_t arg1 = 0;
  Double_t arg2 = 0;
  if (fabs(par[2]) > 1.E-10) arg1 = (x[0] - par[1])/par[2];
  if (fabs(par[5]) > 1.E-10) arg2 = (x[0] - par[4])/par[5];
  Double_t fitval = par[0]*TMath::Exp(-0.5*arg1*arg1)+
	par[3]*TMath::Exp(-0.5*arg2*arg2);
  return fitval;
}

void plots(TString files){

  gSystem->Load("../libanalysis.so"); 


  gROOT->Macro("christian_style.C"); 

  TChain myChain("at");

  //histo booking
  //  TH1D *zpos = new TH1D("zresidall","",200,0,80);
  //  zpos->SetXTitle("position z [mm]");


  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  

  AnalysisEvent   *event;

  myChain.SetBranchAddress("EventBranch", &event);
 
  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);
    

    //global cuts
    //    if(event->th()>30 || event->thX()<1) continue;
    
    for(unsigned int j=0;j<event->nClusters();++j) {


      //zpos->Fill(event->getCluster(j)->pos().z() );

    }
  }

  //fits, etc...  


  //histo drwaing ans saving
  //   canvas = new TCanvas();//"canvas","",0,0,1200,800);
  //   zpos->Draw();
  //   canvas->SaveAs("output/zpos.eps");

}

