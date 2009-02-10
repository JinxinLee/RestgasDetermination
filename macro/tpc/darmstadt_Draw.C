//drawing macro for the worst talk of all time
#include "recotasks/TrackFitStat.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TF1.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLine.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


void draw(std::string filename, std::string opt = "SURF1")
{

  std::ifstream infile(filename.c_str(), std::fstream::in);

  std::vector<Color_t> colors;
  colors.push_back(kGreen);
  colors.push_back(kGreen-9);
  colors.push_back(kSpring+8);
  colors.push_back(kOrange-2);
  colors.push_back(kOrange+7);
  colors.push_back(kRed+3);
  colors.push_back(kRed);
  
  TH2D* histo = new TH2D("histo", "blub", 7, 0, 1.5, 15, 15, 150);
  TH2D* clustnumbers = new TH2D("cln", "cln", 7, 0, 1.5, 15, 15, 150);
  
  double mom, angle, dump, p, sigma, value;
  int nClust;

  double x[15] = {15,20,30,40,50,60,70,80,90,100,110,120,130,140,150};
  std::vector<TGraph*> graphs;
  double values[15];
  
  for(int line=0; line<105; line++) {
    infile>>mom>>angle>>dump>>p>>sigma>>nClust;
    //std::cout<<mom<<"  "<<angle<<"  "<<p<<"  "<<sigma;
    if(p!=0) {
      value=sigma/p;
      values[line%15] = value;
    }
    else
      value=0;
    //dirty correction
   //  if(value>0.05)
//       value=0.05
	
    int bin_x = (line/15)+1;
    int bin_y = (line%15)+1;
    histo->SetBinContent(bin_x, bin_y, value);
    clustnumbers->SetBinContent(bin_x, bin_y, nClust);

    if(line%15==0)
      graphs.push_back(new TGraph(15));
    int g = graphs.size();
    (graphs[g-1])->SetPoint(line%15,x[line%15],value);
  }
  
  gStyle->SetOptStat(0);
  TCanvas* c1 = new TCanvas();
  c1->Divide(1,2);
  c1->cd(1);
  histo->Draw(opt.c_str());
  c1->cd(2);
  clustnumbers->Draw("SURF1");
  TCanvas* c2 = new TCanvas();
  int g = 0;
  (graphs[g])->Draw("APL");
  (graphs[g])->SetMarkerStyle(21);
  (graphs[g])->SetMarkerSize(1);
  (graphs[g])->SetMarkerColor(colors[0]);
  (graphs[g])->SetLineColor(colors[0]);
  (graphs[g])->SetLineWidth(1);
 
  
  //TH1F* h = (graphs[0])->GetHistogram();
  //h->Draw();
  for(int i=1; i<7; i++) {
    (graphs[i])->Draw("PL");
    (graphs[i])->SetMarkerStyle(21);
    (graphs[i])->SetMarkerSize(1);
    (graphs[i])->SetMarkerColor(colors[i]);
    (graphs[i])->SetLineColor(colors[i]);
    (graphs[i])->SetLineWidth(1);
  }

  TLegend* leg = new TLegend(0.8,0.55,0.9,0.9);
  std::vector<TLine*> lines;
  std::vector<std::string*> moms;
  moms.push_back(new std::string("0.2 GeV"));
  moms.push_back(new std::string("0.4 GeV"));
  moms.push_back(new std::string("0.6 GeV"));
  moms.push_back(new std::string("0.8 GeV"));
  moms.push_back(new std::string("1.0 GeV"));
  moms.push_back(new std::string("1.2 GeV"));
  moms.push_back(new std::string("1.5 GeV"));
  

  for(int i=0; i<7; i++) {
    lines.push_back(new TLine());
    (lines[i])->SetLineColor(colors[i]);
    (lines[i])->SetLineWidth(2);
    leg->AddEntry(lines[i], (moms[i])->c_str(), "l");
  }
  leg->Draw();
      

   

}



   

void compare(std::string filename, std::string filename2)
{

  std::ifstream infile(filename.c_str(), std::fstream::in);
  std::ifstream infile2(filename2.c_str(), std::fstream::in);

  std::vector<Color_t> colors;
  colors.push_back(kGreen);
  colors.push_back(kGreen-9);
  colors.push_back(kSpring+8);
  colors.push_back(kOrange-2);
  colors.push_back(kOrange+7);
  colors.push_back(kRed+3);
  colors.push_back(kRed);
  
   
  double mom, angle, dump, p, sigma, value, mom2, angle2, p2, sigma2, value2;
  
  double x[15] = {15,20,30,40,50,60,70,80,90,100,110,120,130,140,150};
  std::vector<TGraph*> graphs;

  double values[15];
  
  for(int line=0; line<105; line++) {
    infile>>mom>>angle>>dump>>p>>sigma;
    infile2>>mom2>>angle2>>dump>>p2>>sigma2;
    if(p!=0) {
      value=sigma/p;
      value2=sigma2/p2;
      values[line%15] = value/value2;
    }
    else
      value=0;
    //dirty correction
   //  if(value>0.05)
//       value=0.05
	
     
    if(line%15==0)
      graphs.push_back(new TGraph(15));
    int g = graphs.size();
    (graphs[g-1])->SetPoint(line%15,x[line%15],values[line%15]);
  }
  
  gStyle->SetOptStat(0);
  TCanvas* c1 = new TCanvas();
  int g = 0;
  (graphs[g])->Draw("APL");
  (graphs[g])->SetMarkerStyle(21);
  (graphs[g])->SetMarkerSize(1);
  (graphs[g])->SetMarkerColor(colors[0]);
  (graphs[g])->SetLineColor(colors[0]);
  (graphs[g])->SetLineWidth(1);
 
  
  //TH1F* h = (graphs[0])->GetHistogram();
  //h->Draw();
  for(int i=1; i<4; i++) {
    (graphs[i])->Draw("PL");
    (graphs[i])->SetMarkerStyle(21);
    (graphs[i])->SetMarkerSize(1);
    (graphs[i])->SetMarkerColor(colors[i]);
    (graphs[i])->SetLineColor(colors[i]);
    (graphs[i])->SetLineWidth(1);
  }

  TLegend* leg = new TLegend(0.8,0.55,0.9,0.9);
  std::vector<TLine*> lines;
  std::vector<std::string*> moms;
  moms.push_back(new std::string("0.2 GeV"));
  moms.push_back(new std::string("0.4 GeV"));
  moms.push_back(new std::string("0.6 GeV"));
  moms.push_back(new std::string("0.8 GeV"));
  moms.push_back(new std::string("1.0 GeV"));
  moms.push_back(new std::string("1.2 GeV"));
  moms.push_back(new std::string("1.5 GeV"));
  

  for(int i=0; i<4; i++) {
    lines.push_back(new TLine());
    (lines[i])->SetLineColor(colors[i]);
    (lines[i])->SetLineWidth(2);
    leg->AddEntry(lines[i], (moms[i])->c_str(), "l");
  }
  leg->Draw();
      

   

}


