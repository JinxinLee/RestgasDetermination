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


  TH2D *amp_u = new TH2D("apm_u","",10,0,10,100,0,5000);
  TH2D *amp_v = new TH2D("amp_v","",10,0,1,100,0,5000);
  TH2D *amp_w = new TH2D("amp_w","",100,0,10,100,0,5000);

  myChain.SetBranchAddress("track", &intr);

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);

    if(!IEEE(tr)) continue;
	
	for(int i=0;i<tr.nCl();++i){
	  TCcluster c = tr.getCl(i);
	   if(!c.getFit()) continue;
	  //if(c.nPadY()!=2) continue;
	  //	  if(c.nTime()!=1) continue;

	   // if(c.nRaw()!=2)continue;

	  for(int j=0;j<c.nRaw();++j){

	  amp_u->Fill(c.getRaw(j).posUVW().X(),c.getRaw(j).getAmp());
	  amp_v->Fill(c.getRaw(j).posUVW().Y(),c.getRaw(j).getAmp());
	  amp_w->Fill(c.getRaw(j).posUVW().Z(),c.getRaw(j).getAmp());

	}
	}
  }

  //end of event loop

  TCanvas *canvas = new TCanvas();
  amp_u->Draw("lego");
  canvas = new TCanvas();
  amp_v->Draw("lego");
  canvas = new TCanvas();
  amp_w->Draw("lego");

}
