
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

void clust(){

  double voltagev [8] = {103, 102, 101, 100, 99, 98, 97, 96};
  double errorv [8] = {0, 0, 0, 0, 0, 0, 0, 0};
  double vertical1 [8] = {1.831, 1.697, 1.718, 1.595, 1.592, 1.463, 1.462, 1.358};
  double RMSv1 [8] = {0.7647, 0.7207, 0.6994, 0.6678, 0.6329, 0.5779, 0.6251, 0.5521};

  double voltageh [6] = {106, 103, 100, 98, 97.5, 97};
  double errorh [6] = {0, 0, 0, 0, 0, 0};
  double horizontal1 [6] = {2.717, 2.219, 1.907, 1.714, 1.687, 1.648};
  double RMSh1 [6] = {1.524, 1.167, 0.91, 0.8013, 0.7947, 0.7611};

  TGraphErrors *v1 = new TGraphErrors(8, voltagev, vertical1, errorv, RMSv1);
  v1->SetTitle("");
  v1->SetMarkerStyle(21);
  v1->SetMarkerColor(2);
  v1->SetMarkerSize(1);

  TH2D *hrv1 = new TH2D("hrv1","hrv1",10,94.5,104.5, 100,0,3);
  hrv1->SetStats(kFALSE);
  hrv1->SetTitle("");
  hrv1->SetXTitle("detector voltage %");
  hrv1->SetYTitle("clustersize");

  TGraphErrors *h1 = new TGraphErrors(6, voltageh, horizontal1, errorh, RMSh1);
  h1->SetTitle("");
  h1->SetMarkerStyle(21);
  h1->SetMarkerColor(2);
  h1->SetMarkerSize(1);

  TH2D *hrh1 = new TH2D("hrh1","hrh1",20,96.5,106.5, 100,0,5);
  hrh1->SetStats(kFALSE);
  hrh1->SetTitle("");
  hrh1->SetXTitle("detector voltage %");
  hrh1->SetYTitle("clustersize");

  TCanvas *canvas = new TCanvas();
  hrv1->Draw();
  v1->Draw("p");

  canvas = new TCanvas();
  hrh1->Draw();
  h1->Draw("p");

}
