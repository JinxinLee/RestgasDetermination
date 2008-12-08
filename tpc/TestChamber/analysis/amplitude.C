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

  //define the histograms

  //TH2D to plot amplitude vs position of the clusters

  TH2D *amp_cl_u = new TH2D("amp_cl_u","",10,0,10,100,0,5000);
  TH2D *amp_cl_v = new TH2D("amp_cl_v","",500,0,1,100,0,5000);
  TH2D *amp_cl_w = new TH2D("amp_cl_w","",100,0,10,100,0,2000);
  amp_cl_w->SetXTitle("position w [cm]");
  amp_cl_w->SetYTitle("amplitude");
  amp_cl_w->SetStats(kFALSE);

  //Profiles of the distributions above

  TProfile *prof_amp_cl_u = new TProfile("prof_amp_cl_u","",100,0,10,0,2000);
  TProfile *prof_amp_cl_v = new TProfile("prof_amp_cl_v","",500,0,1,0,2000);
  TProfile *prof_amp_cl_w = new TProfile("prof_amp_cl_w","",100,0,10,0,2000);

  //TH1D to plot distributions of the amplitudes according to the 
  //cluster-position in v. (v ranges see in event loop)

  TH1D *amp_cl_v_1 = new TH1D("amp_cl_v_1","",100,0,2000);
  amp_cl_v_1->SetLineColor(1);
  amp_cl_v_1->SetXTitle("amplitude");
  amp_cl_v_1->SetYTitle("Number of tracks");
  TH1D *amp_cl_v_2 = new TH1D("amp_cl_v_2","",100,0,2000);
  amp_cl_v_2->SetLineColor(2);
  TH1D *amp_cl_v_3 = new TH1D("amp_cl_v_3","",100,0,2000);
  amp_cl_v_3->SetLineColor(3);
  TH1D *amp_cl_v_4 = new TH1D("amp_cl_v_4","",100,0,2000);
  amp_cl_v_4->SetLineColor(4);
  TH1D *amp_cl_v_5 = new TH1D("amp_cl_v_5","",100,0,2000);
  amp_cl_v_5->SetLineColor(5);
  TH1D *amp_cl_v_6 = new TH1D("amp_cl_v_6","",100,0,2000);
  amp_cl_v_6->SetLineColor(6);
  TH1D *amp_cl_v_7 = new TH1D("amp_cl_v_7","",100,0,2000);
  amp_cl_v_7->SetLineColor(7);
  TH1D *amp_cl_v_8 = new TH1D("amp_cl_v_8","",100,0,2000);
  amp_cl_v_8->SetLineColor(8);

  TH1D *amp_cl_w_1 = new TH1D("amp_cl_w_1","",100,0,2000);
  amp_cl_w_1->SetLineColor(1);
  amp_cl_w_1->SetXTitle("amplitude");
  amp_cl_w_1->SetYTitle("Number of clusters");
  TH1D *amp_cl_w_2 = new TH1D("amp_cl_w_2","",100,0,2000);
  amp_cl_w_2->SetLineColor(2);
  TH1D *amp_cl_w_3 = new TH1D("amp_cl_w_3","",100,0,2000);
  amp_cl_w_3->SetLineColor(3);
  TH1D *amp_cl_w_4 = new TH1D("amp_cl_w_4","",100,0,2000);
  amp_cl_w_4->SetLineColor(4);
  TH1D *amp_cl_w_5 = new TH1D("amp_cl_w_5","",100,0,2000);
  amp_cl_w_5->SetLineColor(5);
  TH1D *amp_cl_w_6 = new TH1D("amp_cl_w_6","",100,0,2000);
  amp_cl_w_6->SetLineColor(6);
  TH1D *amp_cl_w_7 = new TH1D("amp_cl_w_7","",100,0,2000);
  amp_cl_w_7->SetLineColor(7);
  TH1D *amp_cl_w_8 = new TH1D("amp_cl_w_8","",100,0,2000);
  amp_cl_w_8->SetLineColor(8);

  //TH2D to plot amplitude vs position for the raw hits 

  TH2D *amp_u = new TH2D("amp_u","",10,0,10,100,0,5000);
  TH2D *amp_v = new TH2D("amp_v","",10,0,1,100,0,5000);
  TH2D *amp_w = new TH2D("amp_w","",100,0,10,100,0,2000);

  //Profiles of the TH2Ds above

  TProfile *prof_amp_u = new TProfile("prof_amp_u","",100,0,10,0,2000);
  TProfile *prof_amp_v = new TProfile("prof_amp_v","",100,0,1,0,2000);
  TProfile *prof_amp_w = new TProfile("prof_amp_w","",100,0,10,0,2000);

  //TH1D to plot distributions of the raw- amplitudes according to the 
  //raw-hit-position in v. (v ranges see in event loop)

  TH1D *amp_v_1 = new TH1D("amp_v_1","",100,0,2000);
  amp_v_1->SetLineColor(1);
  TH1D *amp_v_2 = new TH1D("amp_v_2","",100,0,2000);
  amp_v_2->SetLineColor(2);
  amp_v_2->SetXTitle("amplitude");
  amp_v_2->SetYTitle("Number of clusters");
  TH1D *amp_v_3 = new TH1D("amp_v_3","",100,0,2000);
  amp_v_3->SetLineColor(2);
  TH1D *amp_v_4 = new TH1D("amp_v_4","",100,0,2000);
  amp_v_4->SetLineColor(2);
  TH1D *amp_v_5 = new TH1D("amp_v_5","",100,0,2000);
  amp_v_5->SetLineColor(2);
  TH1D *amp_v_6 = new TH1D("amp_v_6","",100,0,2000);
  amp_v_6->SetLineColor(2);
  TH1D *amp_v_7 = new TH1D("amp_v_7","",100,0,2000);
  amp_v_7->SetLineColor(2);
  TH1D *amp_v_8 = new TH1D("amp_v_8","",100,0,2000);
  amp_v_8->SetLineColor(1);

  TH1D *amp = new TH1D("amp","",100,0,2000);
  amp->SetXTitle("amplitude");
  amp->SetYTitle("Number of tracks");
  amp->SetStats(kFALSE);

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

	  amp_cl_u->Fill(c.posUVW().X(),c.getAmp());
	  amp_cl_v->Fill(c.posUVW().Y(),c.getAmp());
	  amp_cl_w->Fill(c.posUVW().Z(),c.getAmp());

	  prof_amp_cl_u->Fill(c.posUVW().X(),c.getAmp());
	  prof_amp_cl_v->Fill(c.posUVW().Y(),c.getAmp());
	  prof_amp_cl_w->Fill(c.posUVW().Z(),c.getAmp());

	  if(c.posUVW().Y()<0.1) amp_cl_v_1->Fill(c.getAmp());
	  if(c.posUVW().Y()>0.1&&c.posUVW().Y()<0.2) amp_cl_v_2->Fill(c.getAmp());
	  if(c.posUVW().Y()>0.2&&c.posUVW().Y()<0.3) amp_cl_v_3->Fill(c.getAmp());
	  if(c.posUVW().Y()>0.3&&c.posUVW().Y()<0.4) amp_cl_v_4->Fill(c.getAmp());
	  if(c.posUVW().Y()>0.4&&c.posUVW().Y()<0.5) amp_cl_v_5->Fill(c.getAmp());
	  if(c.posUVW().Y()>0.5&&c.posUVW().Y()<0.6) amp_cl_v_6->Fill(c.getAmp());
	  if(c.posUVW().Y()>0.6&&c.posUVW().Y()<0.7) amp_cl_v_7->Fill(c.getAmp());
	  if(c.posUVW().Y()>0.7) amp_cl_v_8->Fill(c.getAmp());

	  if(c.posUVW().Z()<1) amp_cl_w_1->Fill(c.getAmp());
	  if(c.posUVW().Z()>1&&c.posUVW().Z()<2) amp_cl_w_2->Fill(c.getAmp());
	  if(c.posUVW().Z()>2&&c.posUVW().Z()<3) amp_cl_w_3->Fill(c.getAmp());
	  if(c.posUVW().Z()>3&&c.posUVW().Z()<4) amp_cl_w_4->Fill(c.getAmp());
	  if(c.posUVW().Z()>4&&c.posUVW().Z()<5) amp_cl_w_5->Fill(c.getAmp());
	  if(c.posUVW().Z()>5&&c.posUVW().Z()<6) amp_cl_w_6->Fill(c.getAmp());
	  if(c.posUVW().Z()>6&&c.posUVW().Z()<7) amp_cl_w_7->Fill(c.getAmp());
	  if(c.posUVW().Z()>7) amp_cl_w_8->Fill(c.getAmp());

	  amp->Fill(c.getAmp());

	  //loop over Raw-hits inside the cluster

	  for(int j=0;j<c.nRaw();++j){

		//and again fill histograms

		amp_u->Fill(c.getRaw(j).posUVW().X(),c.getRaw(j).getAmp());
		amp_v->Fill(c.getRaw(j).posUVW().Y(),c.getRaw(j).getAmp());
		amp_w->Fill(c.getRaw(j).posUVW().Z(),c.getRaw(j).getAmp());

		prof_amp_u->Fill(c.getRaw(j).posUVW().X(),c.getRaw(j).getAmp());
		prof_amp_v->Fill(c.getRaw(j).posUVW().Y(),c.getRaw(j).getAmp());
		prof_amp_w->Fill(c.getRaw(j).posUVW().Z(),c.getRaw(j).getAmp());

		if(c.getRaw(j).posUVW().Y()<0.1) amp_v_1->Fill(c.getRaw(j).getAmp());
		if(c.getRaw(j).posUVW().Y()>0.1&&c.getRaw(j).posUVW().Y()<0.2) amp_v_2->Fill(c.getRaw(j).getAmp());
		if(c.getRaw(j).posUVW().Y()>0.2&&c.getRaw(j).posUVW().Y()<0.3) amp_v_3->Fill(c.getRaw(j).getAmp());
		if(c.getRaw(j).posUVW().Y()>0.3&&c.getRaw(j).posUVW().Y()<0.4) amp_v_4->Fill(c.getRaw(j).getAmp());
		if(c.getRaw(j).posUVW().Y()>0.4&&c.getRaw(j).posUVW().Y()<0.5) amp_v_5->Fill(c.getRaw(j).getAmp());
		if(c.getRaw(j).posUVW().Y()>0.5&&c.getRaw(j).posUVW().Y()<0.6) amp_v_6->Fill(c.getRaw(j).getAmp());
		if(c.getRaw(j).posUVW().Y()>0.6&&c.getRaw(j).posUVW().Y()<0.7) amp_v_7->Fill(c.getRaw(j).getAmp());
		if(c.getRaw(j).posUVW().Y()>0.7) amp_v_8->Fill(c.getRaw(j).getAmp());
	  }
	}
  }

  //end of event loop

  //Draw all that stuff

  //legend for the plots with a lot of histograms inside

  TLegend *leg = new TLegend(0.5,0.7,0.999,0.999);
  leg->AddEntry(amp_cl_v_1,"v < 0.1mm","l");
  leg->AddEntry(amp_cl_v_2,"0.1mm < v < 0.2mm","l");
  leg->AddEntry(amp_cl_v_3,"0.2mm < v < 0.3mm","l");
  leg->AddEntry(amp_cl_v_4,"0.3mm < v < 0.4mm","l");
  leg->AddEntry(amp_cl_v_5,"0.4mm < v < 0.5mm","l");
  leg->AddEntry(amp_cl_v_6,"0.5mm < v < 0.6mm","l");
  leg->AddEntry(amp_cl_v_7,"0.6mm < v < 0.7mm","l");
  leg->AddEntry(amp_cl_v_8,"v > 0.7mm","l");
  leg->SetFillColor(0);

  TLegend *leg1 = new TLegend(0.5,0.7,0.999,0.999);
  leg1->AddEntry(amp_cl_v_1,"first and last v pad","l");
  leg1->AddEntry(amp_cl_v_2,"v pads in between","l");
  leg1->SetFillColor(0);

  TLegend *legw = new TLegend(0.5,0.7,0.999,0.999);
  legw->AddEntry(amp_cl_w_1,"w < 1cm","l");
  legw->AddEntry(amp_cl_w_2,"1cm < w < 2cm","l");
  legw->AddEntry(amp_cl_w_3,"2cm < w < 3cm","l");
  legw->AddEntry(amp_cl_w_4,"3cm < w < 4cm","l");
  legw->AddEntry(amp_cl_w_5,"4cm < w < 5cm","l");
  legw->AddEntry(amp_cl_w_6,"5cm < w < 6cm","l");
  legw->AddEntry(amp_cl_w_7,"6cm < w < 7cm","l");
  legw->AddEntry(amp_cl_w_8,"w > 7cm","l");
  legw->SetFillColor(0);

  //define canvases and draw with according options

  TCanvas *canvas = new TCanvas();
  amp_u->Draw("lego");
  canvas = new TCanvas();
  amp_v->Draw("lego");
  canvas = new TCanvas();
  amp_w->Draw("lego");

  canvas = new TCanvas();
  prof_amp_u->Draw();
  canvas = new TCanvas();
  prof_amp_v->Draw();
  canvas = new TCanvas();
  prof_amp_w->Draw();

  canvas = new TCanvas();
  amp_v_1->Draw();
  canvas = new TCanvas();
  amp_v_2->Draw();
  canvas = new TCanvas();
  amp_v_3->Draw();
  canvas = new TCanvas();
  amp_v_4->Draw();
  canvas = new TCanvas();
  amp_v_5->Draw();
  canvas = new TCanvas();
  amp_v_6->Draw();
  canvas = new TCanvas();
  amp_v_7->Draw();
  canvas = new TCanvas();
  amp_v_8->Draw();
  canvas = new TCanvas();
  amp_v_2->Draw();
  amp_v_1->Draw("same");
  amp_v_3->Draw("same");
  amp_v_4->Draw("same");
  amp_v_5->Draw("same");
  amp_v_6->Draw("same");
  amp_v_7->Draw("same");
  amp_v_8->Draw("same");
  leg1->Draw();

  canvas = new TCanvas();
  amp_cl_u->Draw("lego");
  canvas = new TCanvas();
  amp_cl_v->Draw("lego");
  canvas = new TCanvas();
  amp_cl_w->Draw("colz");
  canvas = new TCanvas();
  amp_cl_v->Draw("colz");

  canvas = new TCanvas();
  prof_amp_cl_u->Draw();
  canvas = new TCanvas();
  prof_amp_cl_v->Draw();
  canvas = new TCanvas();
  prof_amp_cl_w->Draw();

  canvas = new TCanvas();
  amp_cl_v_1->Draw();
  canvas = new TCanvas();
  amp_cl_v_2->Draw();
  canvas = new TCanvas();
  amp_cl_v_3->Draw();
  canvas = new TCanvas();
  amp_cl_v_4->Draw();
  canvas = new TCanvas();
  amp_cl_v_5->Draw();
  canvas = new TCanvas();
  amp_cl_v_6->Draw();
  canvas = new TCanvas();
  amp_cl_v_7->Draw();
  canvas = new TCanvas();
  amp_cl_v_8->Draw();
  canvas = new TCanvas();
  amp_cl_v_1->Draw();
  amp_cl_v_2->Draw("same");
  amp_cl_v_3->Draw("same");
  amp_cl_v_4->Draw("same");
  amp_cl_v_5->Draw("same");
  amp_cl_v_6->Draw("same");
  amp_cl_v_7->Draw("same");
  amp_cl_v_8->Draw("same");
  leg->Draw();

  canvas = new TCanvas();
  amp_cl_w_1->Draw();
  amp_cl_w_2->Draw("same");
  amp_cl_w_3->Draw("same");
  amp_cl_w_4->Draw("same");
  amp_cl_w_5->Draw("same");
  amp_cl_w_6->Draw("same");
  amp_cl_w_7->Draw("same");
  amp_cl_w_8->Draw("same");
  legw->Draw();

  canvas = new TCanvas();
  amp->Draw();

}
