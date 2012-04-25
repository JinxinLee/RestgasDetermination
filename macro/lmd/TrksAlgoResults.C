#include "TString.h"
#include "TStopwatch.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TFile.h"
using namespace std;
void TrksAlgoResults()
{
  double x[8] = {1,2,3,4,5,10,15,20};
  double speedC[8] = {0.446,0.501,0.504,0.507,0.507,0.789,1.598,3.18};
  double speedF[8] = {0.397,0.469,0.529,0.578,0.634,0.766,0.869,1.14};
  TGraph *grCA = new TGraph(8,x,speedC);
  grCA->SetMarkerStyle(20);
  grCA->SetMarkerColor(2);
  TGraph *grF = new TGraph(8,x,speedF);
  grF->SetMarkerStyle(25);
  grF->SetMarkerColor(3);
  TMultiGraph *mg = new TMultiGraph();
  mg->Add(grCA);
  mg->Add(grF);
  mg->Draw("AP");
}
