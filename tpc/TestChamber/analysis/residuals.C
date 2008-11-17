
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
#include "TH3.h"
#include "TF1.h"
#include "TProfile.h"
#include "TGraph.h"
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
#include "corrections.C"

void residual(TString files){

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  TCtrack *intr=0;

  TH2D *vres_vs_wres = new TH2D("vres_vs_wres","",100,-0.1,0.1,100,-0.1,0.1);

  TH2D *vres_vs_ures = new TH2D("vres_vs_ures","",100,-0.1,0.1,100,-0.1,0.1);

  TH2D *ures_vs_wres = new TH2D("ures_vs_wres","",100,-0.1,0.1,100,-0.1,0.1);

  TH3D *res = new TH3D("res","",100,-0.001,0.001,100,-0.001,0.001,100,-0.001,0.001);

  myChain.SetBranchAddress("track", &intr);

  for (Int_t iev=0;iev<nevent;iev++){
	
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
	TCtrack corrTrack = without_end_pads(tr);

    if(!IEEE(corrTrack)) continue;

   	for(int i=0;i<corrTrack.nCl();++i){ 

	  TCcluster c = corrTrack.getCl(i);

	  //	  if(c.getFit()){
		if(c.getRes().Y()>-0.001&&c.getRes().Y()<0.001&&c.getRes().X()>-0.001&&c.getRes().X()<0.001&&c.getRes().Z()>-0.001&&c.getRes().Z()<0.001){

	  vres_vs_wres->Fill(c.getRes().Y(),c.getRes().Z());
	  vres_vs_ures->Fill(c.getRes().Y(),c.getRes().X());
	  ures_vs_wres->Fill(c.getRes().X(),c.getRes().Z());
	  res->Fill(c.getRes().X(),c.getRes().Y(),c.getRes().Z());
		}
		//	  }
	}
  }

  TCanvas *canvas = new TCanvas();
  vres_vs_wres->Draw("colz");
  canvas = new TCanvas();
  vres_vs_ures->Draw("colz");
  canvas = new TCanvas();
  ures_vs_wres->Draw("colz");
  canvas = new TCanvas();
  res->Draw();

}
