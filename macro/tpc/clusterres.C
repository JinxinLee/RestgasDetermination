#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include <assert.h>
#include <iostream>
#include <sstream>
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TF1.h"

void clusterres(TString filename){
  
  TFile* file=new TFile(filename,"READ");
  TTree* tr=(TTree*)file->Get("cbmsim");
  assert(tr!=0);

  gStyle->SetOptFit();

  TCanvas* c=new TCanvas("Clusters","Clusters",10,10,500,500); 
  c->Divide(3,3);

  TCanvas* ca=new TCanvas("Pulls","Pulls",15,15,500,500); 
  ca->Divide(3,3);

  TCanvas* c1=new TCanvas("ClusterRes","ClusterRes",20,20,400,400); 

  TGraph* g=new TGraph(9);

  for(int i=1; i<10; ++i){
    c->cd(i);
    std::ostringstream histo;
    histo  <<"hRX"<<i;
    std::ostringstream select;
    select << "TrackFitStat._PndTpcResX>>"<<histo.str()<<"(60,-0.4,0.4)";
    std::ostringstream cut;
    cut<<"TrackFitStat._PndTpcClusterSize=="<<i;
    tr->Draw(select.str().c_str(),cut.str().c_str());
    TH1F* hX = (TH1F*)gDirectory->Get(histo.str().c_str());
    hX->SetLineColor(kBlue);
    hX->Fit("gaus");
    g->SetPoint(i-1,i,hX->GetFunction("gaus")->GetParameter(2));

    ca->cd(i);
    histo.clear();
    histo<<"hPullX"<<i;
    select.clear();
    select << "TrackFitStat._PndTpcResX/TrackFitStat._>>"<<histo.str()<<"(60,-0.4,0.4)";

  }
  c1->cd();
  g->Draw("AP");
  g->SetTitle("Cluster resolution vs size");
  g->SetName("clres");
  g->SetMarkerStyle(20);
  g->SetMarkerSize(2);
  g->GetHistogram()->GetXaxis()->SetTitle("cluster size");
  g->GetHistogram()->GetYaxis()->SetTitle("#sigma[cm]");
  g->Draw("AP");
  
  
  TFile* out=new TFile("clRes.root","RECREATE");
  g->Write("clres");
  out->Close();
}
