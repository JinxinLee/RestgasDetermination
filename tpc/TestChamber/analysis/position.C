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


  TH1D *upos_4clust = new TH1D("upos_4clust","",500,0,10);
  upos_4clust->SetXTitle("Position u [cm]");
  upos_4clust->SetLineColor(4);
  TH1D *vpos_4clust = new TH1D("vpos_4clust","",500,0,1);
  vpos_4clust->SetXTitle("Position v [cm]");
  vpos_4clust->SetLineColor(4);
  TH1D *wpos_4clust = new TH1D("wpos_4clust","",500,0,9);
  wpos_4clust->SetXTitle("Position w [cm]");
  wpos_4clust->SetLineColor(4);
  TH2D *v_vs_w_4clust = new TH2D("v_vs_w_4clust","",500,0,1,500,0,9);
  v_vs_w_4clust->SetXTitle("Position v [cm]");
  v_vs_w_4clust->SetYTitle("Position w [cm]");


  TH1D *upos_4clust_2 = new TH1D("upos_4clust_2","",500,0,10);
  upos_4clust_2->SetXTitle("Position u [cm]");
  upos_4clust_2->SetLineColor(6);
  TH1D *vpos_4clust_2 = new TH1D("vpos_4clust_2","",500,0,1);
  vpos_4clust_2->SetXTitle("Position v [cm]");
  vpos_4clust_2->SetLineColor(6);
  TH1D *wpos_4clust_2 = new TH1D("wpos_4clust_2","",500,0,9);
  wpos_4clust_2->SetXTitle("Position w [cm]");
  wpos_4clust_2->SetLineColor(6);
  TH2D *v_vs_w_4clust_2 = new TH2D("v_vs_w_4clust_2","",500,0,1,500,0,9);
  v_vs_w_4clust_2->SetXTitle("Position v [cm]");
  v_vs_w_4clust_2->SetYTitle("Position w [cm]");

  TH1D *upos_6clust = new TH1D("upos_6clust","",500,0,10);
  upos_6clust->SetXTitle("Position u [cm]");
  upos_6clust->SetLineColor(2);
  TH1D *vpos_6clust = new TH1D("vpos_6clust","",500,0,1);
  vpos_6clust->SetXTitle("Position v [cm]");
  vpos_6clust->SetLineColor(2);
  TH1D *wpos_6clust = new TH1D("wpos_6clust","",500,0,9);
  wpos_6clust->SetXTitle("Position w [cm]");
  wpos_6clust->SetLineColor(2);
  TH2D *v_vs_w_6clust = new TH2D("v_vs_w_6clust","",500,0,1,500,0,9);
  v_vs_w_6clust->SetXTitle("Position v [cm]");
  v_vs_w_6clust->SetYTitle("Position w [cm]");
 
  TH1D *upos_8clust = new TH1D("upos_8clust","",500,0,10);
  upos_8clust->SetXTitle("Position u [cm]");
  upos_8clust->SetLineColor(3);
  TH1D *vpos_8clust = new TH1D("vpos_8clust","",500,0,1);
  vpos_8clust->SetXTitle("Position v [cm]");
  vpos_8clust->SetLineColor(3);
  TH1D *wpos_8clust = new TH1D("wpos_8clust","",500,0,9);
  wpos_8clust->SetXTitle("Position w [cm]");
  wpos_8clust->SetLineColor(3);
  TH2D *v_vs_w_8clust = new TH2D("v_vs_w_8clust","",500,0,1,500,0,9);
  v_vs_w_8clust->SetXTitle("Position v [cm]");
  v_vs_w_8clust->SetYTitle("Position w [cm]");


  myChain.SetBranchAddress("track", &intr);

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);

    if(!IEEE(tr)) continue;

    for(int i=0;i<tr.nCl();++i){
      TCcluster d = tr.getCl(i);
      if(d.getFit()){//was used in fit
		//if(d.nPadY()!=2) continue;
		//	   	if(d.nTime()==1) continue;
	   
		  if(d.nPadY()==1) upos_4clust->Fill(d.posUVW().X());
		  if(d.nPadY()==1) vpos_4clust->Fill(d.posUVW().Y());
		  if(d.nPadY()==1) wpos_4clust->Fill(d.posUVW().Z());
		  if(d.nPadY()==1) v_vs_w_4clust->Fill(d.posUVW().Y(),d.posUVW().Z());
		  /*
		upos_4clust->Fill(d.posUVW().X());
		vpos_4clust->Fill(d.posUVW().Y());
		wpos_4clust->Fill(d.posUVW().Z());
		v_vs_w_4clust->Fill(d.posUVW().Y(),d.posUVW().Z());
		  */
		if(d.nPadY()==2) upos_4clust_2->Fill(d.posUVW().X());
		if(d.nPadY()==2) vpos_4clust_2->Fill(d.posUVW().Y());
		if(d.nPadY()==2) wpos_4clust_2->Fill(d.posUVW().Z());
		if(d.nPadY()==2) v_vs_w_4clust_2->Fill(d.posUVW().Y(),d.posUVW().Z());
      }
    }
  }

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);

    if(!IEEE(tr)) continue;
    if (tr.nClFit()<7) continue;
     
    for(int i=0;i<tr.nCl();++i){
      TCcluster d = tr.getCl(i);
      if(d.getFit()){//was used in fit

	upos_6clust->Fill(d.posUVW().X());
	vpos_6clust->Fill(d.posUVW().Y());
	wpos_6clust->Fill(d.posUVW().Z());
	v_vs_w_6clust->Fill(d.posUVW().Y(),d.posUVW().Z());
      }
    }
  }
  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);

    if(!IEEE(tr)) continue;
    if (tr.nClFit()<9) continue;
     
    for(int i=0;i<tr.nCl();++i){
      TCcluster d = tr.getCl(i);
      if(d.getFit()){//was used in fit

	upos_8clust->Fill(d.posUVW().X());
	vpos_8clust->Fill(d.posUVW().Y());
	wpos_8clust->Fill(d.posUVW().Z());
	v_vs_w_8clust->Fill(d.posUVW().Y(),d.posUVW().Z());
      }
    }
  }

  //end of event loop

  TCanvas *canvas = new TCanvas();
  upos_4clust->Draw();
  //upos_4clust_2->Draw("same");
  upos_6clust->Draw("same");
  upos_8clust->Draw("same");
  canvas = new TCanvas();
  vpos_4clust->Draw();
  vpos_4clust_2->Draw("same");
  //vpos_6clust->Draw("same");
  //vpos_8clust->Draw("same");
  canvas = new TCanvas();
  wpos_4clust->Draw();
  //wpos_4clust_2->Draw("same");
  wpos_6clust->Draw("same");
  wpos_8clust->Draw("same");

  canvas = new TCanvas();
  v_vs_w_4clust->Draw("colz");
  canvas = new TCanvas();
  v_vs_w_4clust_2->Draw("colz");
  canvas = new TCanvas();
  v_vs_w_6clust->Draw("colz");
  canvas = new TCanvas();
  v_vs_w_8clust->Draw("colz");

}
