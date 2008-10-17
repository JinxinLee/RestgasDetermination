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

#include "cuts.C"

void position(TString files){

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  TCtrack *intr=0;

  //define the histograms
  //without using clusterSplit1.C

  double cuts[3]={200.,400.,600.};
  int colors[3]={kRed,kGreen,kBlue};

  TH1D *vpos = new TH1D("vpos","",100,0,1);
  vpos->SetXTitle("Position v [cm]");
  TH1D *vpos0 = new TH1D("vpos0","",100,0,1);
  vpos0->SetLineColor(colors[0]);
  TH1D *vpos1 = new TH1D("vpos1","",100,0,1);
  vpos1->SetLineColor(colors[1]);
  TH1D *vpos2 = new TH1D("vpos2","",100,0,1);
  vpos2->SetLineColor(colors[2]);

  TH1D *amp = new TH1D("amp","",100,0.,3000.);

  myChain.SetBranchAddress("track", &intr);

  

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);

    if(!IEEE(tr)) continue;
	
	for(int i=0;i<tr.nCl();++i){
	  TCcluster c = tr.getCl(i);
	  if(!c.getFit()) continue;
	  if(c.nPadY()!=1) continue;
	  if(c.nTime()!=1) continue;
	  amp->Fill(c.getAmp());
	  vpos->Fill(c.posUVW().Y());
	  if(c.getAmp()>cuts[0]) vpos0->Fill(c.posUVW().Y());
	  if(c.getAmp()>cuts[1]) vpos1->Fill(c.posUVW().Y());
	  if(c.getAmp()>cuts[2]) vpos2->Fill(c.posUVW().Y());
	}

  }

  //end of event loop

  TCanvas *canvas = new TCanvas();
  vpos->Draw();
  vpos0->Draw("same");
  vpos1->Draw("same");
  vpos2->Draw("same");
 
  canvas = new TCanvas();
  amp->Draw();

}
