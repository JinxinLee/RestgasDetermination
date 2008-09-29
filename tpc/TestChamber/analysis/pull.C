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

void pull(TString files){

  TChain myChain("at");


  TH1D *pullu = new TH1D("pullu","",500,0,1000);
  pullu->SetXTitle("pull u");

  TH1D *pullv = new TH1D("pullv","",500,0,1000);
  pullv->SetXTitle("pull v");

  TH1D *pullw = new TH1D("pullw","",500,0,100000);
  pullw->SetXTitle("pull w");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;


  Int_t nevent = myChain.GetEntries();

  TCtrack *intr=0;

  myChain.SetBranchAddress("track", &intr);

  //event loop
  for (Int_t iev=0;iev<nevent;iev++){

    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    if (tr.nClFit()<4) continue;
    if (tr.getThX()<1) continue;

    for(Int_t i=0;i<tr.nCl();i++){

      TCcluster c = tr.getCl(i);
      if(c.getFit()){ 

	pullu->Fill(((c.posUVW().X()-c.getRes().X())*(c.posUVW().X()-c.getRes().X()))/(c.getErr().X()*c.getErr().X()));
	pullv->Fill(((c.posUVW().Y()-c.getRes().Y())*(c.posUVW().Y()-c.getRes().Y()))/(c.getErr().Y()*c.getErr().Y()));
	pullw->Fill(((c.posUVW().Z()-c.getRes().Z())*(c.posUVW().Z()-c.getRes().Z()))/(c.getErr().Z()*c.getErr().Z()));

      }
    }
  }
  TCanvas *canvas = new TCanvas();
  pullv->Draw();
  canvas = new TCanvas();
  pullw->Draw();
  canvas = new TCanvas();
  pullu->Draw();

}
