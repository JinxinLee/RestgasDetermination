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


  TH2D *amps = new TH2D("amps","",100,0,200,100,0,200);


  myChain.SetBranchAddress("track", &intr);

  

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);

    if(!IEEE(tr)) continue;
	
	for(int i=0;i<tr.nCl();++i){
	  TCcluster c = tr.getCl(i);
	  if(!c.getFit()) continue;
	  if(c.nPadY()!=2) continue;
	  //	  if(c.nTime()!=1) continue;
	  amps->Fill(c.getRaw(0).getAmp(),c.getRaw(1).getAmp());
	}

  }

  //end of event loop

  TCanvas *canvas = new TCanvas();
  amps->Draw("colz");

}
