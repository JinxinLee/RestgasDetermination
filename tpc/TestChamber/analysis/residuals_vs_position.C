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

/*
//v correction function from fit to profile
double f1(double x){
return -0.0096+0.024*x;
}
double f2(double x){
return -0.0154+0.042*x;
}
double f3(double x){
return -0.0154+0.039*x;
}
double f4(double x){
return -0.0597+0.152*x;
}
*/

void position(TString files){

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  TCtrack *intr=0;

  //define the histograms

  TH2D *uresid_vs_u = new TH2D("u_vs_uresid","",500,0,10,500,-1,1);
  uresid_vs_u->SetXTitle("Position u [cm]");
  uresid_vs_u->SetYTitle("Residual u [cm]");

  TProfile *prof_u = new TProfile("prof_u","Profile of u residual vs u",100,0,10,-1,1);
  prof_u->SetXTitle("Position u [cm]");
  prof_u->SetYTitle("Residual u [cm]");

  TH2D *vresid_vs_v = new TH2D("vresid_vs_v","",500,0,1,500,-0.1,0.1);
  vresid_vs_v->SetXTitle("Position v [cm]");
  vresid_vs_v->SetYTitle("Residual v [cm]");
  vresid_vs_v->SetStats(kFALSE);

  TProfile *prof_v = new TProfile("prof_v","Profile of v residual vs v",100,0,1,-0.1,0.1);
  prof_v->SetXTitle("Position v [cm]");
  prof_v->SetYTitle("Residual v [cm]");
  prof_v->SetStats(kFALSE);

  TProfile *prof_vU1 = new TProfile("prof_vU1","Profile of v residual vs v",50,0,1,-0.1,0.1);
  TProfile *prof_vU2 = new TProfile("prof_vU2","Profile of v residual vs v",50,0,1,-0.1,0.1);
  TProfile *prof_vU3 = new TProfile("prof_vU3","Profile of v residual vs v",50,0,1,-0.1,0.1);
  TProfile *prof_vU4 = new TProfile("prof_vU4","Profile of v residual vs v",50,0,1,-0.1,0.1);
 
  TProfile *prof_vV1 = new TProfile("prof_vV1","Profile of v residual vs v",500,0,1,-0.1,0.1);
  TProfile *prof_vV2 = new TProfile("prof_vV2","Profile of v residual vs v",500,0,1,-0.1,0.1);
  TProfile *prof_vV3 = new TProfile("prof_vV3","Profile of v residual vs v",500,0,1,-0.1,0.1);
  TProfile *prof_vV4 = new TProfile("prof_vV4","Profile of v residual vs v",500,0,1,-0.1,0.1);
  TProfile *prof_vV5 = new TProfile("prof_vV5","Profile of v residual vs v",500,0,1,-0.1,0.1);
  TProfile *prof_vV6 = new TProfile("prof_vV6","Profile of v residual vs v",500,0,1,-0.1,0.1);
  TProfile *prof_vV7 = new TProfile("prof_vV7","Profile of v residual vs v",500,0,1,-0.1,0.1);
  TProfile *prof_vV8 = new TProfile("prof_vV8","Profile of v residual vs v",500,0,1,-0.1,0.1);
 
  TH2D *wresid_vs_w = new TH2D("wresid_vs_w","",500,0,10,500,-0.1,0.1);
  wresid_vs_w->SetXTitle("Position w [cm]");
  wresid_vs_w->SetYTitle("Residual w [cm]");

  TH2D *wresid_vs_v = new TH2D("wresid_vs_v","",100,0,1,100,-0.1,0.1);
  wresid_vs_v->SetXTitle("Position v [cm]");
  wresid_vs_v->SetYTitle("Residual w [cm]");

  TProfile *prof_w = new TProfile("prof_w","Profile of w residual vs w",100,0,10,-0.1,0.1);
  prof_w->SetXTitle("Position w [cm]");
  prof_w->SetYTitle("Residual w [cm]");

  TProfile *prof_w_v = new TProfile("prof_w_v","Profile of w residual vs v",100,0,1,-0.1,0.1);

  TH1D *test = new TH1D("test","",500,-0.1,0.1);

  myChain.SetBranchAddress("track", &intr);

  //event loop

  for (Int_t iev=0;iev<nevent;iev++){
	
    myChain.GetEntry(iev);
    TCtrack tr(*intr);

	TCtrack corrTrack = without_end_pads(tr);
	
	if(!IEEE(corrTrack)) continue;
	//	  if(!IEEE(tr)) continue;
    
	/*
	  bool passLow=false;
	  bool passHigh=false;
	  for(int i=0;i<tr.nCl();++i){
	  if(tr.getCl(i).posUVW().Y()<0.1) passLow=true;
	  if(tr.getCl(i).posUVW().Y()>0.7) passHigh=true;
	  }
	  if(!(passLow&&passHigh))continue;
	*/

	/*
	  std::vector<TCcluster> corrClusters;
	  int detId;
	*/

	//loop over clusters in the event

	//	for(int i=0;i<tr.nCl();++i){
    // TCcluster c = tr.getCl(i);
	for(int i=0;i<corrTrack.nCl();++i){ 
	  TCcluster c = corrTrack.getCl(i);
	  if (c.getFit()){

		//	if(c.getRes().Y()<0.0002&&c.getRes().Y()>0.0001)continue;
		// if (true){

		//simple residual in v
		test->Fill(c.getRes().Y());
		
		//profile of v positon vs v residual in v bins (delta v = 1mm)
		if(c.posUVW().Y()<0.1&&c.posUVW().Y()>0.06)prof_vV1->Fill(c.posUVW().Y(),c.getRes().Y());
		if(c.posUVW().Y()<0.2&&c.posUVW().Y()>0.1)prof_vV2->Fill(c.posUVW().Y(),c.getRes().Y());
		if(c.posUVW().Y()<0.3&&c.posUVW().Y()>0.2)prof_vV3->Fill(c.posUVW().Y(),c.getRes().Y());
		if(c.posUVW().Y()<0.4&&c.posUVW().Y()>0.3)prof_vV4->Fill(c.posUVW().Y(),c.getRes().Y());
		if(c.posUVW().Y()<0.5&&c.posUVW().Y()>0.4)prof_vV5->Fill(c.posUVW().Y(),c.getRes().Y());
		if(c.posUVW().Y()<0.6&&c.posUVW().Y()>0.5)prof_vV6->Fill(c.posUVW().Y(),c.getRes().Y());
		if(c.posUVW().Y()<0.7&&c.posUVW().Y()>0.6)prof_vV7->Fill(c.posUVW().Y(),c.getRes().Y());
		if(c.posUVW().Y()<0.74&&c.posUVW().Y()>0.7)prof_vV8->Fill(c.posUVW().Y(),c.getRes().Y());

		/*				
		//profile of v positon vs v residual in v bins (delta v = 1mm)
		if(c.posUVW().Y()<0.1)prof_v->Fill(c.posUVW().Y(),c.getRes().Y()-prof_vV1->GetMean(2));
		if(c.posUVW().Y()<0.2&&c.posUVW().Y()>0.1)prof_v->Fill(c.posUVW().Y(),c.getRes().Y()-prof_vV2->GetMean(2));
		if(c.posUVW().Y()<0.3&&c.posUVW().Y()>0.2)prof_v->Fill(c.posUVW().Y(),c.getRes().Y()-prof_vV3->GetMean(2));
		if(c.posUVW().Y()<0.4&&c.posUVW().Y()>0.3)prof_v->Fill(c.posUVW().Y(),c.getRes().Y()-prof_vV4->GetMean(2));
		if(c.posUVW().Y()<0.5&&c.posUVW().Y()>0.4)prof_v->Fill(c.posUVW().Y(),c.getRes().Y()-prof_vV5->GetMean(2));
		if(c.posUVW().Y()<0.6&&c.posUVW().Y()>0.5)prof_v->Fill(c.posUVW().Y(),c.getRes().Y()-prof_vV6->GetMean(2));
		if(c.posUVW().Y()<0.7&&c.posUVW().Y()>0.6)prof_v->Fill(c.posUVW().Y(),c.getRes().Y()-prof_vV7->GetMean(2));
		if(c.posUVW().Y()>0.7)prof_v->Fill(c.posUVW().Y(),c.getRes().Y()-prof_vV8->GetMean(2));
		*/

		//w residual versus v position
		wresid_vs_v->Fill(c.posUVW().Y(),c.getRes().Z());

		/*
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
		  */
		//u,v,w residual versus u,v,w position
		uresid_vs_u->Fill(c.posUVW().X(),c.getRes().X());
		vresid_vs_v->Fill(c.posUVW().Y(),c.getRes().Y());
		wresid_vs_w->Fill(c.posUVW().Z(),c.getRes().Z());

		//profile of u,v,w residual versus u,v,w position
		prof_u->Fill(c.posUVW().X(),c.getRes().X());
		prof_v->Fill(c.posUVW().Y(),c.getRes().Y());
		prof_w->Fill(c.posUVW().Z(),c.getRes().Z());
	
		//profile of v residuals versus v position for different angle settings
		// 	if(tr.getThX()<0.&&tr.getThY()<0.)prof_vU1->Fill(d.posUVW().Y(),d.getRes().Y());
		// 	if(tr.getThX()<0.&&tr.getThY()>0.)prof_vU2->Fill(d.posUVW().Y(),d.getRes().Y());
		// 	if(tr.getThX()>0.&&tr.getThY()<0.)prof_vU3->Fill(d.posUVW().Y(),d.getRes().Y());
		// 	if(tr.getThX()>0.&&tr.getThY()>0.)prof_vU4->Fill(d.posUVW().Y(),d.getRes().Y());

		//profile of v residuals versus v position in u bins (4 pads, u = 4*0.62 cm)
		// 	if(d.posUVW().X()<4.*0.62)prof_vU1->Fill(d.posUVW().Y(),d.getRes().Y());
		// 	if(d.posUVW().X()>4.*0.62&&d.posUVW().X()<8.*0.62)prof_vU2->Fill(d.posUVW().Y(),d.getRes().Y());
		// 	if(d.posUVW().X()>8.*0.62&&d.posUVW().X()<12.*0.62)prof_vU3->Fill(d.posUVW().Y(),d.getRes().Y());
		// 	if(d.posUVW().X()>12*0.62)prof_vU4->Fill(d.posUVW().Y(),d.getRes().Y());
		//	if(d.nTime()!=1)continue;

		//profile of v residuals versus v position for different nPadY
		if(c.nPadY()==1)prof_vU1->Fill(c.posUVW().Y(),c.getRes().Y());
		if(c.nPadY()==2)prof_vU2->Fill(c.posUVW().Y(),c.getRes().Y());
		if(c.nPadY()==3)prof_vU3->Fill(c.posUVW().Y(),c.getRes().Y());
		if(c.nPadY()>3)prof_vU4->Fill(c.posUVW().Y(),c.getRes().Y());

		//profile of w residuals versus v position
		prof_w_v->Fill(c.posUVW().Y(),c.getRes().Z());
	  }
    }
  }

  //end of event loop

  //overview graph for slope in profile of v residual versus v position

  double v [8] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8};
  double mean [8] = {prof_vV1->GetMean(2), prof_vV2->GetMean(2), prof_vV3->GetMean(2), prof_vV4->GetMean(2), prof_vV5->GetMean(2), prof_vV6->GetMean(2), prof_vV7->GetMean(2), prof_vV8->GetMean(2)};

  TGraph *vmean = new TGraph(8, v, mean);
  vmean->SetTitle("");
  vmean->SetMarkerStyle(21);
  vmean->SetMarkerColor(2);
  vmean->SetMarkerSize(1);

  TH2D *hr1v_mean = new TH2D("hr1v_mean","hr1v_mean",10,0,1, 10,-0.02,0.02);
  hr1v_mean->SetStats(kFALSE);
  hr1v_mean->SetTitle("");
  hr1v_mean->SetXTitle("v [cm]");
  hr1v_mean->SetYTitle("v residual [cm]");

  //draw overview graph and fit
  TCanvas *canvas = new TCanvas();
  hr1v_mean->Draw();
  vmean->Draw("p");
  vmean->Fit("pol1");

  //Draw all the distributions and fitting some

  canvas = new TCanvas();
  uresid_vs_u->Draw("colz");
  canvas = new TCanvas();
  vresid_vs_v->Draw("colz");
  canvas = new TCanvas();
  wresid_vs_w->Draw("colz");
  canvas = new TCanvas();
  wresid_vs_v->Draw("colz");

  canvas = new TCanvas();
  prof_u->Draw("");
  canvas = new TCanvas();
  // prof_v->Fit("pol1");
  prof_v->Draw("");
  canvas = new TCanvas();
  prof_w->Draw("");
  canvas = new TCanvas();
  prof_w_v->Draw("");

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


  canvas = new TCanvas();
  canvas->Divide(3,3);
  canvas->cd(1);
  prof_vV1->Draw("");
  //  prof_vV1->Fit("pol0");
  canvas->cd(2);
  prof_vV2->Draw("");
  // prof_vV2->Fit("pol0");
  canvas->cd(3);
  prof_vV3->Draw("");
  // prof_vV3->Fit("pol0");
  canvas->cd(4);
  prof_vV4->Draw("");
  // prof_vV4->Fit("pol0");
  canvas->cd(5);
  prof_vV5->Draw("");
  // prof_vV5->Fit("pol0");
  canvas->cd(6);
  prof_vV6->Draw("");
  // prof_vV6->Fit("pol0");
  canvas->cd(7);
  prof_vV7->Draw("");
  //prof_vV7->Fit("pol0");
  canvas->cd(8);
  prof_vV8->Draw("");
  //prof_vV8->Fit("pol0");

  canvas = new TCanvas();
  test->Draw();

}
