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
#include "TProfile.h"
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


  TH2D *uresid_vs_u = new TH2D("u_vs_uresid","",500,0,10,500,-1,1);
  uresid_vs_u->SetXTitle("Position u [cm]");
  uresid_vs_u->SetYTitle("Residual u [cm]");

  TProfile *prof_u = new TProfile("prof_u","Profile of u residual vs u",500,0,10,-1,1);

  TH2D *vresid_vs_v = new TH2D("vresid_vs_v","",500,0,1,500,-0.1,0.1);
  vresid_vs_v->SetXTitle("Position v [cm]");
  vresid_vs_v->SetYTitle("Residual v [cm]");

  TProfile *prof_v = new TProfile("prof_v","Profile of v residual vs v",500,0,1,-0.1,0.1);

  TH2D *wresid_vs_w = new TH2D("wresid_vs_w","",500,0,10,500,-0.1,0.1);
  wresid_vs_w->SetXTitle("Position w [cm]");
  wresid_vs_w->SetYTitle("Residual w [cm]");

  TProfile *prof_w = new TProfile("prof_w","Profile of w residual vs w",500,0,10,-0.1,0.1);

  myChain.SetBranchAddress("track", &intr);

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);

    if(!IEEE(tr)) continue;
     
    for(int i=0;i<tr.nCl();++i){
      TCcluster d = tr.getCl(i);
      if(d.getFit()){//was used in fit

	uresid_vs_u->Fill(d.posUVW().X(),d.getRes().X());
	vresid_vs_v->Fill(d.posUVW().Y(),d.getRes().Y());
	wresid_vs_w->Fill(d.posUVW().Z(),d.getRes().Z());

	prof_u->Fill(d.posUVW().X(),d.getRes().X());
	prof_v->Fill(d.posUVW().Y(),d.getRes().Y());
	prof_w->Fill(d.posUVW().Z(),d.getRes().Z());
      }
    }
  }

  //end of event loop

  TCanvas *canvas = new TCanvas();
  uresid_vs_u->Draw("colz");
  canvas = new TCanvas();
  vresid_vs_v->Draw("colz");
  canvas = new TCanvas();
  wresid_vs_w->Draw("colz");

  canvas = new TCanvas();
  prof_u->Draw("");
  canvas = new TCanvas();
  prof_v->Draw("");
  canvas = new TCanvas();
  prof_w->Draw("");

}
