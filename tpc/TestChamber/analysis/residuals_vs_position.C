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

//v correction function from fit to profile
double f1(double x){
  return -0.0096+0.024*x;
}
double f2(double x){
  return -0.0091+0.023*x;
}
double f3(double x){
  return -0.0154+0.039*x;
}
double f4(double x){
  return -0.0597+0.152*x;
}


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

  TProfile *prof_v = new TProfile("prof_v","Profile of v residual vs v",50,0,1,-0.1,0.1);

  TProfile *prof_vU1 = new TProfile("prof_vU1","Profile of v residual vs v",50,0,1,-0.1,0.1);
  TProfile *prof_vU2 = new TProfile("prof_vU2","Profile of v residual vs v",50,0,1,-0.1,0.1);
  TProfile *prof_vU3 = new TProfile("prof_vU3","Profile of v residual vs v",50,0,1,-0.1,0.1);
  TProfile *prof_vU4 = new TProfile("prof_vU4","Profile of v residual vs v",50,0,1,-0.1,0.1);

  TH2D *wresid_vs_w = new TH2D("wresid_vs_w","",500,0,10,500,-0.1,0.1);
  wresid_vs_w->SetXTitle("Position w [cm]");
  wresid_vs_w->SetYTitle("Residual w [cm]");

  TProfile *prof_w = new TProfile("prof_w","Profile of w residual vs w",500,0,10,-0.1,0.1);

  myChain.SetBranchAddress("track", &intr);

  for (Int_t iev=0;iev<nevent;iev++){
	
    myChain.GetEntry(iev);
    TCtrack tr(*intr);

    if(!IEEE(tr)) continue;
     
	/*
	bool passLow=false;
	bool passHigh=false;
	for(int i=0;i<tr.nCl();++i){
	  if(tr.getCl(i).posUVW().Y()<0.1) passLow=true;
	  if(tr.getCl(i).posUVW().Y()>0.7) passHigh=true;
	}
	if(!(passLow&&passHigh))continue;
	*/

	
	std::vector<TCcluster> corrClusters;
	int detId;
	for(int i=0;i<tr.nCl();++i){
	  
	  TCcluster c = tr.getCl(i);

	  detId=c.getId();
	  if(!c.getFit()) continue;
	  TVector3 pos = c.posUVW();

	  if(c.nPadY()==1){
		if(c.posUVW().Y()<0.050001)continue;
		if(c.posUVW().Y()>0.7499999)continue;
	  }
	  if(c.nPadY()==2){
		if(c.posUVW().Y()<0.1499999)continue;
		if(c.posUVW().Y()>0.6500001)continue;
	  }


	  if(c.nPadY()==1)
		//pos.SetY(pos.Y()-f1(pos.Y()));
	  if(c.nPadY()==2)
		pos.SetY(pos.Y()-f2(pos.Y()));
	  if(c.nPadY()==3)
		//pos.SetY(pos.Y()-f3(pos.Y()));
	  if(c.nPadY()>3)
		//pos.SetY(pos.Y()-f4(pos.Y()));
	  
	  //pos.SetZ(pos.X());
	  //pos.SetY(0.4);
	  c.posUVW(pos);
	  
	  corrClusters.push_back(c);
	  
	}
	TCtrack corrTrack;
	corrTrack.addClusters(corrClusters);
	TCalign::getInstance();
	corrTrack.fit(detId);
	
    for(int i=0;i<corrTrack.nCl();++i){
      TCcluster d = corrTrack.getCl(i);
      if(d.getFit()){//was used in fit
		//if(d.nPadY()<2)continue;
	uresid_vs_u->Fill(d.posUVW().X(),d.getRes().X());
	vresid_vs_v->Fill(d.posUVW().Y(),d.getRes().Y());
	wresid_vs_w->Fill(d.posUVW().Z(),d.getRes().Z());

	prof_u->Fill(d.posUVW().X(),d.getRes().X());
	prof_v->Fill(d.posUVW().Y(),d.getRes().Y());

// 	if(tr.getThX()<0.&&tr.getThY()<0.)prof_vU1->Fill(d.posUVW().Y(),d.getRes().Y());
// 	if(tr.getThX()<0.&&tr.getThY()>0.)prof_vU2->Fill(d.posUVW().Y(),d.getRes().Y());
// 	if(tr.getThX()>0.&&tr.getThY()<0.)prof_vU3->Fill(d.posUVW().Y(),d.getRes().Y());
// 	if(tr.getThX()>0.&&tr.getThY()>0.)prof_vU4->Fill(d.posUVW().Y(),d.getRes().Y());

// 	if(d.posUVW().X()<4.*0.62)prof_vU1->Fill(d.posUVW().Y(),d.getRes().Y());
// 	if(d.posUVW().X()>4.*0.62&&d.posUVW().X()<8.*0.62)prof_vU2->Fill(d.posUVW().Y(),d.getRes().Y());
// 	if(d.posUVW().X()>8.*0.62&&d.posUVW().X()<12.*0.62)prof_vU3->Fill(d.posUVW().Y(),d.getRes().Y());
// 	if(d.posUVW().X()>12*0.62)prof_vU4->Fill(d.posUVW().Y(),d.getRes().Y());
//	if(d.nTime()!=1)continue;
 	if(d.nPadY()==1)prof_vU1->Fill(d.posUVW().Y(),d.getRes().Y());
 	if(d.nPadY()==2)prof_vU2->Fill(d.posUVW().Y(),d.getRes().Y());
 	if(d.nPadY()==3)prof_vU3->Fill(d.posUVW().Y(),d.getRes().Y());
 	if(d.nPadY()>3)prof_vU4->Fill(d.posUVW().Y(),d.getRes().Y());



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

  canvas = new TCanvas();
  canvas->Divide(2,2);
  canvas->cd(1);
  prof_vU1->Fit("pol1");
  prof_vU1->Draw("");
  canvas->cd(2);
  prof_vU2->Fit("pol1");
  prof_vU2->Draw("");
  canvas->cd(3);
  prof_vU3->Fit("pol1");
  prof_vU3->Draw("");
  canvas->cd(4);
  prof_vU4->Fit("pol1");
  prof_vU4->Draw("");


}
