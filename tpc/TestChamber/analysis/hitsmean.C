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
  double vertical1 [8] = {6.93, 6.68, 6.917, 6.65, 6.752, 6.329, 6.325, 5.748};
  double RMSv1 [8] = {3.97, 3.94, 4.041, 4.005, 3.973, 3.781, 3.994, 3.445};
  double vertical2 [8] = {5.05, 4.946, 5.114, 4.96, 5.063, 4.824, 4.742, 4.298};
  double RMSv2 [8] = {3.61, 3.633, 3.746, 3.76, 3.823, 3.704, 3.689, 3.216};

  double voltageh [6] = {106, 103, 100, 98, 97.5, 97};
  double errorh [6] = {0, 0, 0, 0, 0, 0};
  double horizontal1 [6] = {13.91, 12.85, 12.92, 11.53, 11.22, 11.04};
  double RMSh1 [6] = {8.179, 7.824, 7.91, 7.017, 6.965, 6.929};
  double horizontal2 [6] = {11.57, 11.71, 11.36, 9.366, 9.09, 8.915};
  double RMSh2 [6] = {8.004, 8.36, 8.125, 6.654, 6.61, 6.52};

  TGraphErrors *v1 = new TGraphErrors(8, voltagev, vertical1, errorv, RMSv1);
  v1->SetTitle("");
  v1->SetMarkerStyle(21);
  v1->SetMarkerColor(2);
  v1->SetMarkerSize(1);

  TH2D *hrv1 = new TH2D("hrv1","hrv1",10,94.5,104.5, 10,0,10);
  hrv1->SetStats(kFALSE);
  hrv1->SetTitle("number of hits");
  hrv1->SetXTitle("detector voltage %");
  hrv1->SetYTitle("number of hits");

  TGraphErrors *v2 = new TGraphErrors(8, voltagev, vertical2, errorv, RMSv2);
  v2->SetTitle("");
  v2->SetMarkerStyle(21);
  v2->SetMarkerColor(2);
  v2->SetMarkerSize(1);

  TH2D *hrv2 = new TH2D("hrv2","hrv2",10,94.5,104.5, 10,0,10);
  hrv2->SetStats(kFALSE);
  hrv2->SetTitle("number of hits on track");
  hrv2->SetXTitle("detector voltage %");
  hrv2->SetYTitle("number of hits");

  TGraphErrors *h1 = new TGraphErrors(6, voltageh, horizontal1, errorh, RMSh1);
  h1->SetTitle("");
  h1->SetMarkerStyle(21);
  h1->SetMarkerColor(2);
  h1->SetMarkerSize(1);

  TH2D *hrh1 = new TH2D("hrh1","hrh1",20,96.5,106.5, 10,0,25);
  hrh1->SetStats(kFALSE);
  hrh1->SetTitle("number of hits");
  hrh1->SetXTitle("detector voltage %");
  hrh1->SetYTitle("number of hits");

  TGraphErrors *h2 = new TGraphErrors(6, voltageh, horizontal2, errorh, RMSh2);
  h2->SetTitle("");
  h2->SetMarkerStyle(21);
  h2->SetMarkerColor(2);
  h2->SetMarkerSize(1);

  TH2D *hrh2 = new TH2D("hrh2","hrh2",20,96.5,106.5, 10,0,25);
  hrh2->SetStats(kFALSE);
  hrh2->SetTitle("number of hits on track");
  hrh2->SetXTitle("detector voltage %");
  hrh2->SetYTitle("number of hits");

  TCanvas *canvas = new TCanvas();
  hrv1->Draw();
  v1->Draw("p");

  canvas = new TCanvas();
  hrv2->Draw();
  v2->Draw("p");

  canvas = new TCanvas();
  hrh1->Draw();
  h1->Draw("p");

  canvas = new TCanvas();
  hrh2->Draw();
  h2->Draw("p");
}
