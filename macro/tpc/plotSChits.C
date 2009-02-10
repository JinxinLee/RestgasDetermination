
#include "PndTpcPoint.h"
#include "CbmMCTrack.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TH2D.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


void XYplot(std::string filename) {

  //std::cout<<"\nPlease enter filename to process: "<<std::endl;

  //std::string filename;
  //getline(std::cin, filename);
  std::cout<<"\n\nreading root file (MC output) "
	   <<filename.c_str()<<"  \n"<<std::endl;
  
  TFile* inFile = new TFile(filename.c_str());
  TTree* tree = (TTree*)inFile->Get("cbmsim");
  
  TClonesArray* points = new TClonesArray("PndTpcPoint");
  TClonesArray* tracks = new TClonesArray("CbmMCTrack");
  
  
  TH2D* hits = new TH2D("hits", "hits in the chamber",200,-45,45,200,-45,45);
  
  TH2D* hitsE = new TH2D("hitsE", "hits weighted by energy loss",200,-45,45,200,-45,45);

  int nE=0;
if(tree!=0) {
  tree->SetBranchAddress("PndTpcPoint", &points);
  tree->SetBranchAddress("MCTrack", &tracks);
  nE = tree->GetEntries();
}

std::cout<<"found "<<nE<<" entries in tree\n"<<std::endl;
  
  //loop over events
  int totPoints = 0;
  
  for(int ev=0; ev<nE; ev++) {
    if(ev%(nE/10)==0) {
      std::cout<<".  ";
      std::cout.flush();
    }
    tree->GetEvent(ev);
    int nPoints = points->GetEntriesFast();
    totPoints+=nPoints;
    
    //loop over points
    for (int p=0; p<nPoints; p++) {

      int trackID = ((PndTpcPoint*)points->At(p))->GetTrackID();
      int motherID = ((CbmMCTrack*)tracks->At(trackID))->GetMotherID();

      //cut on trackID

      //cut on motherID
      if(motherID!=-1)
	continue;      
      
      double x = ((PndTpcPoint*)points->At(p))->GetX();
      double y = ((PndTpcPoint*)points->At(p))->GetY();
      
      double eLoss = ((PndTpcPoint*)points->At(p))->GetEnergyLoss();
      
      hits->Fill(x,y);
      hitsE->Fill(x,y,eLoss);
    }
     
  }

  TCanvas* canv = new TCanvas();
  canv->Divide(2,1);
  canv->cd(1);
  hits->Draw("COLZ");
  canv->cd(2);
  hitsE->Draw("COLZ");

}


void plotTree(TTree* tree, int pdg_cut=-1, int eventcut=-1, double p_low_cut=0, double p_high_cut=2) {

  
  TClonesArray* points = new TClonesArray("PndTpcPoint");
  TClonesArray* tracks = new TClonesArray("CbmMCTrack");
  
  
  TH2D* hits = new TH2D("hits", "electron hits in the chamber",200,-40,110,150,15.5,41.5);

  TH1D* E_hist = new TH1D("energy", "energy distribution of tracks [GeV]",1000, 0, 0.001);
  TH2D* hitsE = new TH2D("hitsE", "electron hits weighted by energy loss",200,-40,110,150,15.5,41.5);
  TH1D* pdghist = new TH1D("pdgs","actual PDG IDs in the data", 10000, 0, 10000);
  TH1D* trackIDs = new TH1D("tids", "Track IDs", 1000,0,1000);

  int nE=0;
  if(tree!=0) {
    tree->SetBranchAddress("PndTpcPoint", &points);
    tree->SetBranchAddress("MCTrack", &tracks);
    nE = tree->GetEntries();
  }

  int evtcut=nE;
  if(eventcut!=-1)
    evtcut = eventcut;

  std::cout<<"found "<<nE<<" entries in tree\n"<<std::endl;
  
  //loop over events
  int totPoints = 0;

  std::vector<int> track_map(1000,0); //keep track of the "used" track IDs
  
  for(int ev=0; ev<evtcut; ev++) {
    if(ev%(evtcut/10)==0) {
      std::cout<<".  ";
      std::cout.flush();
    }
    tree->GetEvent(ev);
    int nPoints = points->GetEntriesFast();
    totPoints+=nPoints;
    
    //loop over points
    for (int p=0; p<nPoints; p++) {

      int trackID = ((PndTpcPoint*)points->At(p))->GetTrackID();
      int motherID = ((CbmMCTrack*)tracks->At(trackID))->GetMotherID();
      int pdg = ((CbmMCTrack*)tracks->At(trackID))->GetPdgCode();
      TLorentzVector lmom = ((CbmMCTrack*)tracks->At(trackID))->Get4Momentum();

      //cut on particle type
       if(pdg_cut!=-1)
 	if(pdg != pdg_cut) 
 	  continue; 

      if(pdg>100000)
	std::cout<<pdg<<std::endl;
        
      if(trackID>track_map.size()+1)
	track_map.resize(trackID+1,0);
      
      if(track_map[trackID]==0) {
	if(pdg_cut==11)
	  E_hist->Fill(lmom.Energy()-0.000511);  //MINUS E- REST MASS
	pdghist->Fill(pdg);
	track_map[trackID]=1;           //count every energy only once per track
      }
      //cut on trackID

      //cut on motherID
      //if(motherID!=-1)
      //continue;  

     
      double x = ((PndTpcPoint*)points->At(p))->GetX();
      double y = ((PndTpcPoint*)points->At(p))->GetY();
      double z = ((PndTpcPoint*)points->At(p))->GetZ();
      
      double r = sqrt(x*x + y*y);
      if(r < 15.4 || r>41.6)
	std::cout<<"Radius out of bounds! (r="<<r<<"); something is wrong!"<<std::endl;
      double eLoss = ((PndTpcPoint*)points->At(p))->GetEnergyLoss();
      
      hits->Fill(z,r);
      hitsE->Fill(z,r,eLoss);
    }
     
  }

  TCanvas* canv = new TCanvas();
  canv->Divide(1,2);
  canv->cd(1);
  hits->Draw();
  canv->cd(2);
  hitsE->Draw();
  TCanvas* canv2 =  new TCanvas();
  //canv2->Divide(2,1);
  //canv2->cd(1);
  //pdghist->Draw();
  //canv2->cd(2);
  E_hist->Draw();
  TCanvas* canv3 = new TCanvas();
  pdghist->Draw();
  

  std::cout<<"\n\nPrinting used trackIDs:"<<std::endl;
  int ntracks=0;
  for(int i=0; i<track_map.size(); i++) 
    if(track_map[i]==1) {
      std::cout<<i<<std::endl;
      ntracks++;
    }

  std::cout<<"\nProcessed "<<totPoints<<" MC points in total"<<std::endl;
  std::cout<<"There have been "<<ntracks<<" tracks in the data"<<std::endl;

    
}



void plot(std::string filename, int pdg_cut=-1, int event_cut=-1, double p_low_cut=0., double p_high_cut=2.) {

  std::cout<<"\n\nreading root file (MC output) "
	   <<filename.c_str()<<"  \n"<<std::endl;
  
  TFile* inFile = new TFile(filename.c_str());
  TTree* tree = (TTree*)inFile->Get("cbmsim");

  plotTree(tree,pdg_cut,event_cut,p_low_cut,p_high_cut);
  
  
}
