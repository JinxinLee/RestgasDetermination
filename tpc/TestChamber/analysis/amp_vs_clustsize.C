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
#include "TLegend.h"

#include "../src/TCtrack.h"
#include "../src/TCcluster.h"
#include "../src/TCclusterSort.h"
#include <algorithm>
#include <vector>
#include <set>

//include macros to recalculate or cut the data

//cuts.C includes cuts on different Trackparameter
#include "cuts.C"
//corrections.C recalculates the clusterposition according to
//the fit done in residuals_vs_position.C
//mark: included here but not used in this macro.
#include "corrections.C"

void amplitude(TString files){

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  TCtrack *intr=0;

  TH2D *amp_cl_w = new TH2D("amp_cl_w","",100,0,2000,20,-0.5,19.5);
  amp_cl_w->SetYTitle("clustersize");
  amp_cl_w->SetXTitle("amplitude");
  amp_cl_w->SetStats(kFALSE);

  TH2D *amp_cl_w_1 = new TH2D("amp_cl_w_1","",100,0,2000,20,-0.5,29.5);
  amp_cl_w_1->SetXTitle("amplitude");
  amp_cl_w_1->SetYTitle("clustersize");
  TH2D *amp_cl_w_2 = new TH2D("amp_cl_w_2","",100,0,2000,20,-0.5,19.5);
  TH2D *amp_cl_w_3 = new TH2D("amp_cl_w_3","",100,0,2000,20,-0.5,19.5);
  TH2D *amp_cl_w_4 = new TH2D("amp_cl_w_4","",100,0,2000,20,-0.5,19.5);
  TH2D *amp_cl_w_5 = new TH2D("amp_cl_w_5","",100,0,2000,20,-0.5,19.5);
  TH2D *amp_cl_w_6 = new TH2D("amp_cl_w_6","",100,0,2000,20,-0.5,19.5);
  TH2D *amp_cl_w_7 = new TH2D("amp_cl_w_7","",100,0,2000,20,-0.5,19.5);
  TH2D *amp_cl_w_8 = new TH2D("amp_cl_w_8","",100,0,2000,20,-0.5,19.5);

  //Profiles of the TH2Ds above

  myChain.SetBranchAddress("track", &intr);

  //event loop 
  //loop over events

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);

	//definition of IEEE cuts see cuts.C
    if(!IEEE(tr)) continue;
	
	//loop over clusters in event

	for(int i=0;i<tr.nCl();++i){
	  //just shorter than typing tr.getCl(i) every time
	  TCcluster c = tr.getCl(i);
	  //make sure the cluster was used to fit the track
	  if(!c.getFit()) continue;
	  //some other cuts that are not included in cuts.C
	  //if(c.nPadY()!=1) continue;
	  //if(c.nTime()!=1) continue;
	  //if(c.nRaw()!=2)continue;

	  //just fill the histograms

	  amp_cl_w->Fill(c.getAmp(),c.nRaw());

	  if(c.posUVW().Z()<1) amp_cl_w_1->Fill(c.getAmp(),c.nRaw());
	  if(c.posUVW().Z()>1&&c.posUVW().Z()<2) amp_cl_w_2->Fill(c.getAmp(),c.nRaw());
	  if(c.posUVW().Z()>2&&c.posUVW().Z()<3) amp_cl_w_3->Fill(c.getAmp(),c.nRaw());
	  if(c.posUVW().Z()>3&&c.posUVW().Z()<4) amp_cl_w_4->Fill(c.getAmp(),c.nRaw());
	  if(c.posUVW().Z()>4&&c.posUVW().Z()<5) amp_cl_w_5->Fill(c.getAmp(),c.nRaw());
	  if(c.posUVW().Z()>5&&c.posUVW().Z()<6) amp_cl_w_6->Fill(c.getAmp(),c.nRaw());
	  if(c.posUVW().Z()>6&&c.posUVW().Z()<7) amp_cl_w_7->Fill(c.getAmp(),c.nRaw());
	  if(c.posUVW().Z()>7) amp_cl_w_8->Fill(c.getAmp(),c.nRaw());


	}
  }
  //end of event loop

  //Draw all that stuff

  TCanvas *canvas = new TCanvas();
  amp_cl_w->Draw("colz");

  canvas = new TCanvas();
  amp_cl_w_1->Draw("colz");
  canvas = new TCanvas();
  amp_cl_w_2->Draw("colz");
  canvas = new TCanvas();
  amp_cl_w_3->Draw("colz");
  canvas = new TCanvas();
  amp_cl_w_4->Draw("colz");
  canvas = new TCanvas();
  amp_cl_w_5->Draw("colz");
  canvas = new TCanvas();
  amp_cl_w_6->Draw("colz");
  canvas = new TCanvas();
  amp_cl_w_7->Draw("colz");
  canvas = new TCanvas();
  amp_cl_w_8->Draw("colz");

}
