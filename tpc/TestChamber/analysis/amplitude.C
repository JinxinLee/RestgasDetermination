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

#include "cuts.C"

void position(TString files){

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  TCtrack *intr=0;

  //define the histograms
  //without using clusterSplit1.C

  TH2D *amp_cl_u = new TH2D("amp_cl_u","",10,0,10,100,0,5000);
  TH2D *amp_cl_v = new TH2D("amp_cl_v","",500,0,1,100,0,5000);
  TH2D *amp_cl_w = new TH2D("amp_cl_w","",100,0,10,100,0,5000);

  TProfile *prof_amp_cl_u = new TProfile("prof_amp_cl_u","",100,0,10,0,2000);
  TProfile *prof_amp_cl_v = new TProfile("prof_amp_cl_v","",500,0,1,0,2000);
  TProfile *prof_amp_cl_w = new TProfile("prof_amp_cl_w","",100,0,10,0,2000);

  TH1D *amp_cl_v_1 = new TH1D("amp_cl_v_1","",100,0,2000);
  amp_cl_v_1->SetLineColor(1);
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

  TH2D *amp_u = new TH2D("amp_u","",10,0,10,100,0,5000);
  TH2D *amp_v = new TH2D("amp_v","",10,0,1,100,0,5000);
  TH2D *amp_w = new TH2D("amp_w","",100,0,10,100,0,5000);

  TProfile *prof_amp_u = new TProfile("prof_amp_u","",100,0,10,0,2000);
  TProfile *prof_amp_v = new TProfile("prof_amp_v","",100,0,1,0,2000);
  TProfile *prof_amp_w = new TProfile("prof_amp_w","",100,0,10,0,2000);

  TH1D *amp_v_1 = new TH1D("amp_v_1","",100,0,2000);
  amp_v_1->SetLineColor(1);
  TH1D *amp_v_2 = new TH1D("amp_v_2","",100,0,2000);
  amp_v_2->SetLineColor(2);
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

	  for(int j=0;j<c.nRaw();++j){

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

  TLegend *leg = new TLegend(0.5,0.7,0.999,0.999);
  leg->AddEntry(amp_cl_v_1,"v < 0.1mm","l");
  leg->AddEntry(amp_cl_v_2,"0.1mm < v < 0.2mm","l");
  leg->AddEntry(amp_cl_v_3,"0.2mm < v < 0.3mm","l");
  leg->AddEntry(amp_cl_v_4,"0.3mm < v < 0.4mm","l");
  leg->AddEntry(amp_cl_v_5,"0.4mm < v < 0.5mm","l");
  leg->AddEntry(amp_cl_v_6,"0.5mm < v < 0.8mm","l");
  leg->AddEntry(amp_cl_v_7,"0.6mm < v < 0.7mm","l");
  leg->AddEntry(amp_cl_v_8,"v > 0.7mm","l");
  leg->SetFillColor(0);

  TLegend *leg1 = new TLegend(0.5,0.7,0.999,0.999);
  leg1->AddEntry(amp_cl_v_1,"first and last v pad","l");
  leg1->AddEntry(amp_cl_v_2,"v pads in between","l");
  leg1->SetFillColor(0);

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
  amp_v_1->Draw();
  amp_v_2->Draw("same");
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
  amp_cl_w->Draw("lego");
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

}
