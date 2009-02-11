
#include "PndMCTrack.h"
#include "PndTpcPoint.h"
//#include "TString.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TH1D.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


void plot(std::string filename) {

  std::cout<<"\n\nreading root file (MC output) "
	   <<filename.c_str()<<"  "<<std::endl;
  
  TFile* inFile = new TFile(filename.c_str());
  TTree* tree = (TTree*)inFile->Get("cbmsim");

  TClonesArray* tracks = new TClonesArray("PndMCTrack");
  TClonesArray* points = new TClonesArray("PndTpcPoint");

  TH1D* hist = new TH1D("bla", "dE/dx", 300, 0, 10e-6);
  TH1D* hist_all = new TH1D("bla2", "dE/dx all", 300, 0, 10e-6);
  TH1D* hist_length = new TH1D("bla3", "distribution of step-lengths for Pions", 100, 0, 0.6);

  int nE=0;
  if(tree!=0) {
    tree->SetBranchAddress("MCTrack", &tracks);
    tree->SetBranchAddress("PndTpcPoint", &points);
    nE = tree->GetEntries();
  }

  for(int ev=0; ev<nE; ev++) {
    tree->GetEvent(ev);
    int nPoints = points->GetEntriesFast();
    if(ev%500==0) {
      std::cout<<". ";
      std::cout.flush();
    }
      

    for (int p=0; p<nPoints; p++) {

      int trackID = ((PndTpcPoint*)points->At(p))->GetTrackID();
      ///if(trackID<0)
      //std::cout<<"TrackID: "<<trackID<<std::endl;
      int ntracks = tracks->GetEntriesFast();
      //std::cout<<"# tracks: "<<ntracks<<std::endl;
      int pdg = ((PndMCTrack*)tracks->At(trackID))->GetPdgCode();

   
      double eLoss = ((PndTpcPoint*)points->At(p))->GetEnergyLoss();
      double length = ((PndTpcPoint*)points->At(p))->GetLength();

      TVector3 mom;
      ((PndTpcPoint*)points->At(p))->Momentum(mom);
      double p = mom.Mag();

      if(p<0.3 || p>0.4)
	continue;

      hist_all->Fill(eLoss/length);
               
      // cut on PIONS
      if(pdg == 211) {
	//std::cout<<pdg<<std::endl;
      	hist->Fill(eLoss/length);
	hist_length->Fill(length);
      }
            
     
    }
  }

  TCanvas* canv = new TCanvas();
  hist->SetStats(kFALSE);
  hist->Draw();
  double smallmax = hist->GetMaximum();
  hist_all->SetLineColor(kRed);
  double largemax = hist_all->GetMaximum();
  hist_all->Scale(smallmax/largemax);
  //hist_all->Draw("same");

  TCanvas* canv2 = new TCanvas();
  hist_length->Draw();
}
      
      
//for plotting large root files that have been split
void plotChain(std::string filename1, std::string filename2) {

  
  TChain* tree = new TChain("cbmsim");
  tree->AddFile(filename1.c_str());
  tree->AddFile(filename2.c_str());

  TClonesArray* tracks = new TClonesArray("PndMCTrack");
  TClonesArray* points = new TClonesArray("PndTpcPoint");

  TH1D* hist = new TH1D("bla", "dE/dx", 300, 0, 10e-6);
  TH1D* hist_all = new TH1D("bla2", "dE/dx all", 300, 0, 10e-6);
  TH1D* hist_length = new TH1D("bla3", "distribution of step-lengths for Pions", 100, 0, 0.6);

  int nE=0;
  if(tree!=0) {
    tree->SetBranchAddress("MCTrack", &tracks);
    tree->SetBranchAddress("PndTpcPoint", &points);
    nE = tree->GetEntries();
  }

  std::cout<<"\nFound "<<nE<<" events in the Chain... "<<std::endl;
  int counter=0;

  for(int ev=0; ev<nE; ev++) {
    tree->GetEvent(ev);
    int nPoints = points->GetEntriesFast();
    if(ev%500==0) {
      std::cout<<". ";
      std::cout.flush();
    }
      

    for (int p=0; p<nPoints; p++) {

      int trackID = ((PndTpcPoint*)points->At(p))->GetTrackID();
      ///if(trackID<0)
      //std::cout<<"TrackID: "<<trackID<<std::endl;
      int ntracks = tracks->GetEntriesFast();
      //std::cout<<"# tracks: "<<ntracks<<std::endl;
      int pdg = ((PndMCTrack*)tracks->At(trackID))->GetPdgCode();

   
      double eLoss = ((PndTpcPoint*)points->At(p))->GetEnergyLoss();
      double length = ((PndTpcPoint*)points->At(p))->GetLength();

      TVector3 mom;
      ((PndTpcPoint*)points->At(p))->Momentum(mom);
      double p = mom.Mag();

      if(p<0.3 || p>0.4)
	continue;

      counter++;

      hist_all->Fill(eLoss/length);
      
      // cut on PIONS
      //if(pdg == 211) {
      	hist->Fill(eLoss/length);
	hist_length->Fill(length);
	//	counter++;
	//}
            
     
    }
  }
  
  std::cout<<"\n\ncounter: "<<counter<<std::endl;
  TCanvas* canv = new TCanvas();
  hist->SetStats(kFALSE);
  hist->Draw();
  double smallmax = hist->GetMaximum();
  hist_all->SetLineColor(kRed);
  double largemax = hist_all->GetMaximum();
  hist_all->Scale(smallmax/largemax);
  //hist_all->Draw("same");

  TCanvas* canv2 = new TCanvas();
  hist_length->Draw();
}
