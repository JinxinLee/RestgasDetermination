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
#include "TLegend.h"

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

  TH1D *u = new TH1D("u","",50,0,10);
  TH1D *v = new TH1D("v","",100,0,1);
  TH1D *w = new TH1D("w","",100,0,10);

  TH1D *u_raw = new TH1D("u_raw","",50,0,10);
  TH1D *v_raw = new TH1D("v_raw","",50,0,1);
  v_raw->SetStats(kFALSE);
  TH1D *w_raw = new TH1D("w_raw","",100,0,10);

  TH1D *v_raw_1 = new TH1D("v_raw_1","",50,0,1);
  v_raw_1->SetLineColor(5);
  v_raw->SetStats(kFALSE);
  TH1D *v_raw_2 = new TH1D("v_raw_2","",50,0,1);
  v_raw_2->SetLineColor(6);
  v_raw_2->SetStats(kFALSE);
  TH1D *v_raw_3 = new TH1D("v_raw_3","",50,0,1);
  v_raw_3->SetLineColor(3);
  v_raw_3->SetStats(kFALSE);
  TH1D *v_raw_4 = new TH1D("v_raw_4","",50,0,1);
  v_raw_4->SetLineColor(4);
  v_raw_4->SetStats(kFALSE);

  TH2D *w_vs_v = new TH2D("w_vs_v","",100,0,10,100,0,1);

  TH2D *w_vs_v_raw = new TH2D("w_vs_v_raw","",100,0,10,10,0,1);

  TH2D *w_vs_v_1 = new TH2D("w_vs_v_1","",100,0,10,10,0,1);
  TH2D *w_vs_v_2 = new TH2D("w_vs_v_2","",100,0,10,10,0,1);
  TH2D *w_vs_v_3 = new TH2D("w_vs_v_3","",100,0,10,10,0,1);
  TH2D *w_vs_v_4 = new TH2D("w_vs_v_4","",100,0,10,10,0,1);

  TH2D *v_vs_v = new TH2D("v_vs_v","",10,0,1,100,0,1);

  TH2D *v_vs_v_1 = new TH2D("v_vs_v_1","",10,0,1,100,0,1);
  TH2D *v_vs_v_2 = new TH2D("v_vs_v_2","",10,0,1,100,0,1);
  TH2D *v_vs_v_3 = new TH2D("v_vs_v_3","",10,0,1,100,0,1);
  TH2D *v_vs_v_4 = new TH2D("v_vs_v_4","",10,0,1,100,0,1);

  TH2D *u_vs_u = new TH2D("u_vs_u","",10,0,10,10,0,10);

  TH2D *u_vs_u_1 = new TH2D("u_vs_u_1","",10,0,10,10,0,10);
  TH2D *u_vs_u_2 = new TH2D("u_vs_u_2","",10,0,10,10,0,10);
  TH2D *u_vs_u_3 = new TH2D("u_vs_u_3","",10,0,10,10,0,10);
  TH2D *u_vs_u_4 = new TH2D("u_vs_u_4","",10,0,10,10,0,10);

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

	  u->Fill(c.posUVW().X());
	  v->Fill(c.posUVW().Y());
	  w->Fill(c.posUVW().Z());

	  w_vs_v->Fill(c.posUVW().Z(),c.posUVW().Y());

	  for(int j=0;j<c.nRaw();++j){

		u_raw->Fill(c.getRaw(j).posUVW().X());
		v_raw->Fill(c.getRaw(j).posUVW().Y());
		w_raw->Fill(c.getRaw(j).posUVW().Z());

		if(c.nPadY()==1)v_raw_1->Fill(c.getRaw(j).posUVW().Y());
		if(c.nPadY()==2)v_raw_2->Fill(c.getRaw(j).posUVW().Y());
		if(c.nPadY()==3)v_raw_3->Fill(c.getRaw(j).posUVW().Y());
		if(c.nPadY()>3)v_raw_4->Fill(c.getRaw(j).posUVW().Y());

		w_vs_v_raw->Fill(c.getRaw(j).posUVW().Z(),c.getRaw(j).posUVW().Y());

		if(c.nPadY()==1) w_vs_v_1->Fill(c.getRaw(j).posUVW().Z(),c.getRaw(j).posUVW().Y());
		if(c.nPadY()==2) w_vs_v_2->Fill(c.getRaw(j).posUVW().Z(),c.getRaw(j).posUVW().Y());
		if(c.nPadY()==3) w_vs_v_3->Fill(c.getRaw(j).posUVW().Z(),c.getRaw(j).posUVW().Y());
		if(c.nPadY()>3) w_vs_v_4->Fill(c.getRaw(j).posUVW().Z(),c.getRaw(j).posUVW().Y());

		v_vs_v->Fill(c.getRaw(j).posUVW().Y(),c.posUVW().Y());

		if(c.nPadY()==1) v_vs_v_1->Fill(c.getRaw(j).posUVW().Y(),c.posUVW().Y());
		if(c.nPadY()==2) v_vs_v_2->Fill(c.getRaw(j).posUVW().Y(),c.posUVW().Y());
		if(c.nPadY()==3) v_vs_v_3->Fill(c.getRaw(j).posUVW().Y(),c.posUVW().Y());
		if(c.nPadY()>3) v_vs_v_4->Fill(c.getRaw(j).posUVW().Y(),c.posUVW().Y());

		u_vs_u->Fill(c.getRaw(j).posUVW().X(),c.posUVW().X());

		if(c.nPadY()==1) u_vs_u_1->Fill(c.getRaw(j).posUVW().X(),c.posUVW().X());
		if(c.nPadY()==2) u_vs_u_2->Fill(c.getRaw(j).posUVW().X(),c.posUVW().X());
		if(c.nPadY()==3) u_vs_u_3->Fill(c.getRaw(j).posUVW().X(),c.posUVW().X());
		if(c.nPadY()>3) u_vs_u_4->Fill(c.getRaw(j).posUVW().X(),c.posUVW().X());

	  }
	}
  }

  //end of event loop

  TLegend *leg = new TLegend(0.7,0.8,0.95,0.99);
  leg->AddEntry(v_raw,"total","l");
  leg->AddEntry(v_raw_1,"nPadY = 1","l");
  leg->AddEntry(v_raw_2,"nPadY = 2","l");
  leg->AddEntry(v_raw_3,"nPadY = 3","l");
  leg->AddEntry(v_raw_4,"nPadY > 3","l");
  leg->SetFillColor(0);

  TCanvas *canvas = new TCanvas();
  u->Draw();
  canvas = new TCanvas();
  v->Draw();
  canvas = new TCanvas();
  w->Draw();

  canvas = new TCanvas();
  w_vs_v->Draw("colz");

  canvas = new TCanvas();
  u_raw->Draw();
  canvas = new TCanvas();
  v_raw->Draw();
  v_raw_2->Draw("same");
  v_raw_1->Draw("same");
  v_raw_3->Draw("same");
  v_raw_4->Draw("same");
  leg->Draw();
  canvas = new TCanvas();
  w_raw->Draw();

  canvas = new TCanvas();
  w_vs_v_raw->Draw("colz");
  canvas = new TCanvas();
  w_vs_v_1->Draw("lego");
  canvas = new TCanvas();
  w_vs_v_2->Draw("lego");
  canvas = new TCanvas();
  w_vs_v_3->Draw("lego");
  canvas = new TCanvas();
  w_vs_v_4->Draw("lego");

  canvas = new TCanvas();
  v_vs_v->Draw("colz");
  canvas = new TCanvas();
  v_vs_v_1->Draw("lego");
  canvas = new TCanvas();
  v_vs_v_2->Draw("lego");
  canvas = new TCanvas();
  v_vs_v_3->Draw("lego");
  canvas = new TCanvas();
  v_vs_v_4->Draw("lego");

  canvas = new TCanvas();
  u_vs_u->Draw("colz");
  canvas = new TCanvas();
  u_vs_u_1->Draw("lego");
  canvas = new TCanvas();
  u_vs_u_2->Draw("lego");
  canvas = new TCanvas();
  u_vs_u_3->Draw("lego");
  canvas = new TCanvas();
  u_vs_u_4->Draw("lego");

}
