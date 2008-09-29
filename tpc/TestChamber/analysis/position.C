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

  TH1D *upos = new TH1D("upos","",500,0,10);
  upos->SetXTitle("Position u [cm]");
  TH1D *vpos = new TH1D("vpos","",500,0,1);
  vpos->SetXTitle("Position v [cm]");
  TH1D *wpos = new TH1D("wpos","",500,0,9);
  wpos->SetXTitle("Position w [cm]");
  TH2D *v_vs_w = new TH2D("v_vs_w","",500,0,1,500,0,9);
  v_vs_w->SetXTitle("Position v [cm]");
  v_vs_w->SetYTitle("Position w [cm]");

  TH1D *upos_2clust = new TH1D("upos_2clust","",500,0,10);
  upos_2clust->SetXTitle("Position u [cm]");
  upos_2clust->SetLineColor(2);
  TH1D *vpos_2clust = new TH1D("vpos_2clust","",500,0,1);
  vpos_2clust->SetXTitle("Position v [cm]");
  vpos_2clust->SetLineColor(2);
  TH1D *wpos_2clust = new TH1D("wpos_2clust","",500,0,9);
  wpos_2clust->SetXTitle("Position w [cm]");
  wpos_2clust->SetLineColor(2);
  TH2D *v_vs_w_2clust = new TH2D("v_vs_w_2clust","",500,0,1,500,0,9);
  v_vs_w_2clust->SetXTitle("Position v [cm]");
  v_vs_w_2clust->SetYTitle("Position w [cm]");
 
  TH1D *upos_3clust = new TH1D("upos_3clust","",500,0,10);
  upos_3clust->SetXTitle("Position u [cm]");
  upos_3clust->SetLineColor(3);
  TH1D *vpos_3clust = new TH1D("vpos_3clust","",500,0,1);
  vpos_3clust->SetXTitle("Position v [cm]");
  vpos_3clust->SetLineColor(3);
  TH1D *wpos_3clust = new TH1D("wpos_3clust","",500,0,9);
  wpos_3clust->SetXTitle("Position w [cm]");
  wpos_3clust->SetLineColor(3);
  TH2D *v_vs_w_3clust = new TH2D("v_vs_w_3clust","",500,0,1,500,0,9);
  v_vs_w_3clust->SetXTitle("Position v [cm]");
  v_vs_w_3clust->SetYTitle("Position w [cm]");

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
	v_vs_w->Fill(d.posUVW().Y(),d.posUVW().Z());
      }
    }
  }
   
  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
     if (fabs(tr.getAx()) > 1.E3) continue;
     if (fabs(tr.getAy()) > 1.E3) continue;
     //if (tr.getChi2()/tr.getNDF()>4) continue;
     //if (tr.getChi2()/tr.getNDF()<0.1) continue;
     if (tr.nClFit()<2) continue;
     
    for(int i=0;i<tr.nCl();++i){
      TCcluster d = tr.getCl(i);
      if(d.getFit()){//was used in fit

	upos_2clust->Fill(d.posUVW().X());
	vpos_2clust->Fill(d.posUVW().Y());
	wpos_2clust->Fill(d.posUVW().Z());
	v_vs_w_2clust->Fill(d.posUVW().Y(),d.posUVW().Z());
      }
    }
  }
  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
     if (fabs(tr.getAx()) > 1.E3) continue;
     if (fabs(tr.getAy()) > 1.E3) continue;
     //if (tr.getChi2()/tr.getNDF()>4) continue;
     //if (tr.getChi2()/tr.getNDF()<0.1) continue;
     if (tr.nClFit()<3) continue;
     
    for(int i=0;i<tr.nCl();++i){
      TCcluster d = tr.getCl(i);
      if(d.getFit()){//was used in fit

	upos_3clust->Fill(d.posUVW().X());
	vpos_3clust->Fill(d.posUVW().Y());
	wpos_3clust->Fill(d.posUVW().Z());
	v_vs_w_3clust->Fill(d.posUVW().Y(),d.posUVW().Z());
      }
    }
  }

  for (Int_t iev=0;iev<nevent;iev++){
     
    myChain.GetEntry(iev);
    TCtrack tr(*intr);
     if (fabs(tr.getAx()) > 1.E3) continue;
     if (fabs(tr.getAy()) > 1.E3) continue;
     //if (tr.getChi2()/tr.getNDF()>4) continue;
     //if (tr.getChi2()/tr.getNDF()<0.1) continue;
     if (tr.nClFit()<4) continue;
     
    for(int i=0;i<tr.nCl();++i){
      TCcluster d = tr.getCl(i);
      if(d.getFit()){//was used in fit

	upos_4clust->Fill(d.posUVW().X());
	vpos_4clust->Fill(d.posUVW().Y());
	wpos_4clust->Fill(d.posUVW().Z());
	v_vs_w_4clust->Fill(d.posUVW().Y(),d.posUVW().Z());
      }
    }
  }


  //end of event loop

  TCanvas *canvas = new TCanvas();
  upos->Draw();
  canvas = new TCanvas();
  vpos->Draw();
  canvas = new TCanvas();
  wpos->Draw();
  canvas = new TCanvas();
  v_vs_w->Draw();
  canvas = new TCanvas();
  v_vs_w_2clust->Draw();
  canvas = new TCanvas();
  v_vs_w_3clust->Draw();
  canvas = new TCanvas();
  v_vs_w_4clust->Draw();

  canvas = new TCanvas();
  upos->Draw();
  upos_2clust->Draw("same");
  upos_3clust->Draw("same");
  upos_4clust->Draw("same");
  canvas = new TCanvas();
  vpos->Draw();
  vpos_2clust->Draw("same");
  vpos_3clust->Draw("same");
  vpos_4clust->Draw("same");
  canvas = new TCanvas();
  wpos->Draw();
  wpos_2clust->Draw("same");
  wpos_3clust->Draw("same");
  wpos_4clust->Draw("same");

}
