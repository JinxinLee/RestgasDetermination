/*
 * LmdPoint.C * *  Created on: May 15, 2009 *      Author: huagen
 */

{
#include "TCanvas.h"
	std::cout;
	 gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	 rootlogon();
	 gSystem->Load("libLmd");
 //    gROOT->Macro("Libs.C");

  TStopwatch timer;
  timer.Start();

  Int_t nEvents = 1e5;

  TFile* MCPoint = new TFile("/private/huagen/simdata/Lmd_DPM_elastic_6.2_1.9mrad_5M_1.root","READ");
  TTree* tree    = (TTree*)MCPoint->Get("cbmsim");

  TClonesArray* point = new TClonesArray("PndSdsMCPoint");
  tree->SetBranchAddress("LMDPoint", &point);

  TH2F *h1 = new TH2F("c1","Point on X versus Y",100, -10, 10, 100, -10, 10);
  h1->SetFillColor(kBlue);
  TH3F *h2 = new TH3F("c2","Point on X,Y and Z", 100, -10, 10, 100, -10, 10, 100, 1070,1150);
  h2->SetTitle("the MC point; #X cm;#Y cm;# Z cm");
  h2->SetFillColor(kRed);
  TH1F *h3 = new TH1F("c3","Energy Loss", 1000, 0, 1000);
 // h3->SetFillColor(kRed);
  h3->SetTitle("the EnergyLoss: #X keV;");

  double X=0, Y=0,Z=0;
  double eloss;

	  for (int i=0; i<nEvents&&i<tree->GetEntriesFast();i++){
	  tree->GetEntry(i);
	  for(int j=0;j<point->GetEntriesFast();j++){
		  PndSdsMCPoint* MyPoint = (PndSdsMCPoint*)point->At(j);
		  X = (MyPoint->GetPosition().X()+MyPoint->GetPositionOut().X())/2.;
		  Y = (MyPoint->GetPosition().Y()+MyPoint->GetPositionOut().Y())/2;
		  Z = (MyPoint->GetPosition().Z()+MyPoint->GetPositionOut().Z())/2;
		  eloss = MyPoint->GetEnergyLoss()*1000000;
		 // cout<<"the eloss is:"<<eloss<<endl;
		  h1->Fill(X,Y);
		  h1->SetFillColor(kBlue);
		  h2->Fill(X,Y,Z);
		  h2->SetFillColor(kRed);
		  h3->Fill(eloss);
	  }
  }
       TCanvas *c;
       h1->DrawCopy();
	   c = new TCanvas("c2");
	   h2->DrawCopy();
	   c = new TCanvas("C3");
	   h3->DrawCopy();
}
