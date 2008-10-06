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


  TH1D *pullu = new TH1D("pullu","",500,-10,10);
  pullu->SetXTitle("pull u");

  TH1D *pullv = new TH1D("pullv","",500,-10,10);
  pullv->SetXTitle("pull v");

  TH1D *pullw = new TH1D("pullw","",500,-10,10);
  pullw->SetXTitle("pull w");

  TH1D *resu = new TH1D("resu","",500,-0.1,0.1);
  resu->SetXTitle("residual u [cm]");

  TH1D *resv = new TH1D("resv","",500,-0.1,0.1);
  resv->SetXTitle("residual v [cm]");

  TH1D *resw = new TH1D("resw","",500,-0.1,0.1);
  resw->SetXTitle("residual w [cm]");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;


  Int_t nevent = myChain.GetEntries();

  TCtrack *intr=0;

  myChain.SetBranchAddress("track", &intr);

  //event loop
  for (Int_t iev=0;iev<nevent;iev++){

    myChain.GetEntry(iev);
    TCtrack tr(*intr);
    if (tr.nClFit()<5) continue;
    if (fabs(tr.getThX())<1) continue;
    if (fabs(tr.getThY())<1) continue;
    if (fabs(tr.getTh())<1) continue;
    if (fabs(tr.getTh())>30) continue;
    if (tr.getChi2()/tr.getNDF()<0.1) continue;

    for(Int_t i=0;i<tr.nCl();i++){

      TCcluster c = tr.getCl(i);
      if(c.getFit()){ 

	pullu->Fill(c.getRes().X()/c.getErr().X());
	pullv->Fill(c.getRes().Y()/c.getErr().Y());
	pullw->Fill(c.getRes().Z()/c.getErr().Z());

	resu->Fill(c.getRes().X());
	resv->Fill(c.getRes().Y());
	resw->Fill(c.getRes().Z());

      }
    }
  }

  TCanvas *canvas = new TCanvas();
  pullv->Draw();
  canvas = new TCanvas();
  pullw->Draw();
  canvas = new TCanvas();
  pullu->Draw();

  TCanvas *canvasr = new TCanvas();
  resv->Draw();
  canvasr = new TCanvas();
  resw->Draw();
  canvasr = new TCanvas();
  resu->Draw();
}
