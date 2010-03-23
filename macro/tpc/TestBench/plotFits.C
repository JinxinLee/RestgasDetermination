//ROOT macro for visualising obtained genfit straight line fits 
//author: Felix Boehmer

#include <map>
#include <iostream>

#include "TString.h"
#include "TFile.h"
#include "TPolyMarker.h"
#include "TH3D.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TVector3.h"
#include "TPolyMarker3D.h"
#include "TPolyLine3D.h"


#include "PndTpcDriftedElectron.h"
#include "PndTpcCluster.h"
#include "GFTrack.h"



void plotFits(TString reco_file, int evHi, int evLo=0, int markerstyle=2, 
	      bool different_sizes=true) {

  TFile* recofile = new TFile(reco_file);
  int MARKERSTYLE=markerstyle;
  
  double driftvel = 0.00055192;

  if(recofile->IsZombie()) {
    std::cerr<<"Reco-file is not existing! Aborting."<<std::endl;
    return;
  }
  
  TString digi_file = reco_file;
  digi_file.ReplaceAll("reco.root", "raw.root");
  TFile* digifile = new TFile(digi_file);

  if(digifile->IsZombie()) {
    std::cerr<<"Digi-file is not existing! Aborting."<<std::endl;
    return;
  }
  
  TH3D* hist = new TH3D("bla", "", 100,-5,5,100,-5,5,100,0,10);
  hist->GetXaxis()->SetTitle("x (cm)");
  hist->GetYaxis()->SetTitle("y (cm)");
  hist->GetZaxis()->SetTitle("z (cm)");
  
  TCanvas* canv = new TCanvas();
  hist->Draw();


  TTree* recotree = (TTree*)recofile->Get("cbmsim");
  TTree* digitree = (TTree*)digifile->Get("cbmsim");
  
  TClonesArray* trackArr = new TClonesArray("GFTrack");
  TClonesArray* drelArr = new TClonesArray("PndTpcDriftedElectron");
  TClonesArray* clustArr = new TClonesArray("PndTpcCluster");
  
  recotree->SetBranchAddress("TrackPostFit", &trackArr);
  recotree->SetBranchAddress("PndTpcCluster", &clustArr);
  digitree->SetBranchAddress("PndTpcDriftedElectron", &drelArr);


  int nEvents = evHi-evLo;
  
  std::map<int,TPolyMarker3D*> mmap;
  
  TObjArray* markers = new TObjArray();
  TObjArray* lines = new TObjArray();
  
  
  for(unsigned int ev=0; ev<nEvents; ev++) {
    
    recotree->GetEvent(evLo+ev);
    digitree->GetEvent(evLo+ev);
    
    unsigned int nEl = drelArr->GetEntriesFast();
    unsigned int nCl = clustArr->GetEntriesFast();
    
    TPolyMarker3D* de_m = new TPolyMarker3D(nEl, 1);
    TPolyMarker3D* cl_m;
    //cl_m->SetMarkerColor(kOrange+2);
    //de_m->Print();
    markers->Add(de_m);
    //markers->Add(cl_m);
    
    //markers->Print();

    for(unsigned int k=0; k<nEl; k++) {
      PndTpcDriftedElectron* el = (PndTpcDriftedElectron*)drelArr->At(k);
      double z = el->t()*driftvel;
      de_m->SetPoint(k,el->x(),el->y(),z);
    }
    
    for(unsigned int l=0; l<nCl; l++) {
      PndTpcCluster* cl = (PndTpcCluster*)clustArr->At(l);
      TVector3 clpos = cl->pos();
      int size = cl->nDigi();
      if(!(mmap.count(size))){
	cl_m = new TPolyMarker3D();
	cl_m->SetMarkerStyle(MARKERSTYLE);
	if(different_sizes)
	  cl_m->SetMarkerSize((double)size/2);
	else
	  cl_m->SetMarkerSize(1.);
	cl_m->SetMarkerColor(kOrange+2);
	mmap[size] = cl_m;
	std::cout<<"creating new PolyMarker3D"<<std::endl;
	cl_m->SetPoint(0,clpos.X(),clpos.Y(),clpos.Z());
	std::cout<<cl_m->GetLastPoint()<<std::endl;

      }
      else {
	cl_m = mmap[size];
	cl_m->SetPoint(cl_m->GetLastPoint()+1,clpos.X(),clpos.Y(),clpos.Z());
      }
      
    }
    

    GFTrack* tr = (GFTrack*)trackArr->At(0);
    TVector3 pos = tr->getPos();
    TVector3 mom = tr->getMom();
    
    TPolyLine3D* line =  new TPolyLine3D(2);
    mom = mom*(1/mom.Mag());
    TVector3 pos2 = mom*9+pos;
    line->SetPoint(0,pos.X(),pos.Y(),pos.Z());
    line->SetPoint(1,pos2.X(),pos2.Y(),pos2.Z());
    
    lines->Add(line);
    line->Draw("same");
    
    de_m->Draw("same");

  }
  
  std::map<int,TPolyMarker3D*>::iterator it;

  for(it=mmap.begin(); it!=mmap.end(); it++) {
    it->second->Draw("same");
  }
}
