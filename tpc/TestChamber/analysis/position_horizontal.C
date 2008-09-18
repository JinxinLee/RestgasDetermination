#include <iostream>
#include <fstream>
#include <ctime>
#include "TString.h"
#include "TChain.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TPad.h"
#include "TH1.h"
#include "TH2.h"
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

void position(TString files){

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  TCtrack *intr=0;

  //define the histograms
  //without using clusterSplit1.C

  TH1D *upos = new TH1D("upos","",500,0,11);
  upos->SetXTitle("Position u [cm]");
  TH1D *vpos = new TH1D("vpos","",500,0,1);
  vpos->SetXTitle("Position v [cm]");
  TH1D *wpos = new TH1D("wpos","",500,0,9);
  wpos->SetXTitle("Position w [cm]");

  //without using clusterSplit1.C

  TH1D *uposw = new TH1D("uposw","",500,0,11);
  uposw->SetXTitle("Position u [cm] using clusterSplit1.C");
  TH1D *vposw = new TH1D("vposw","",500,0,1);
  vposw->SetXTitle("Position v [cm] using clusterSplit1.C");
  TH1D *wposw = new TH1D("wposw","",500,0,9);
  wposw->SetXTitle("Position w [cm] using clusterSplit1.C");
 
  myChain.SetBranchAddress("track", &intr);

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
     
    for(int i=0;i<tr.nCl();++i){
      TCcluster d = tr.getCl(i);
      if(d.getFit()){//was used in fit

	upos->Fill(d.posUVW().X());
	vpos->Fill(d.posUVW().Y());
	wpos->Fill(d.posUVW().Z());
      }
    }
  }
   
  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    TCtrack trSplit = clusterSplit1(tr);
    trSplit.fit(200);
     
    for(int i=0;i<trSplit.nCl();++i){
      TCcluster c = trSplit.getCl(i);
      if(c.getFit()){//was used in fit

	uposw->Fill(c.posUVW().X());
	vposw->Fill(c.posUVW().Y());
	wposw->Fill(c.posUVW().Z());

      }
    }
  }

  //end of event loop

  TCanvas *canvas = new TCanvas();
  xpos->Draw();
  canvas = new TCanvas();
  ypos->Draw();
  canvas = new TCanvas();
  zpos->Draw();
  canvas = new TCanvas();
  xposw->Draw();
  canvas = new TCanvas();
  yposw->Draw();
  canvas = new TCanvas();
  zposw->Draw();

}
