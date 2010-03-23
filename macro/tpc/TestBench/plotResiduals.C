
#include <map>
#include <iostream>
#include <string>
#include <sstream>
//plot 2D residual distributions as function of the clustersize


#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TVector3.h"
#include "TrackFitStat.h"
#include "TClonesArray.h"


void plotResiduals(TString filename, bool cut_small=false,
		   bool cut_large=false) {

  TFile* infile = new TFile(filename);
  if(infile->IsZombie()) {
    std::cerr<<"Infile not existing!"<<std::endl;
    return;
  }

  TTree* tree = (TTree*)infile->Get("cbmsim");
  int nEv = tree->GetEntries();
  std::cout<<"Found "<<nEv<<" entries in the tree .."<<std::endl;

  TClonesArray* statArr = new TClonesArray("TrackFitStat");
  tree->SetBranchAddress("TrackFitStat", &statArr);

  std::map<unsigned int,TH2D*> histmap;
  
  TH2D* res2D;

  for(int ev=0; ev<nEv; ev++) {
    tree->GetEvent(ev);
    if (statArr->GetEntriesFast()<1)
      continue;
    TrackFitStat* stat = (TrackFitStat*) statArr->At(0);
    const std::vector<double>* resX = stat->GetResX();
    const std::vector<double>* resY = stat->GetResY();
    const std::vector<unsigned int>* sizes = stat->Get2DClSizes();
    const std::vector<TVector3>* pps = stat->GetProjectionPoints();;
    
    if(!(resX->size() == resY->size() && sizes->size()==resX->size())) {
      std::cerr<<"SIZES DONT MATCH!!!!"<<std::endl;
      continue;
    }


    for(int r=0; r<resX->size(); r++){
      unsigned int clsize = sizes->at(r);
      //TVector3 pp = pps->at(r);
      //double y = pp.Y();
      if(!(histmap.count(clsize))) {
	std::string name = "ClusterSize";
	std::stringstream ss;
	ss<<clsize;
	name.append(ss.str());
	res2D = new TH2D(name.c_str(),name.c_str(),200,-1,1,200,-1,1);
	res2D->GetXaxis()->SetTitle("Residual x (cm)");
	res2D->GetYaxis()->SetTitle("Residual y (cm)");	
	histmap[clsize] = res2D;
      }
      else
	res2D = histmap[clsize];
      
      //if(cut_small && y>0.6)
      //	continue;
      //if(cut_large && y<0.6)
      //	continue;
	  
      res2D->Fill(resX->at(r),resY->at(r));
    }
  }

  TCanvas* canv = new TCanvas();
  canv->Divide(2,2);
  
  for(int s=0; s<4; s++) {
    canv->cd(s+1);
    gPad->SetLogz();
    TH2D* blub = histmap[s+1];
    if(blub!=NULL)
      blub->Draw("COLZ");
  }
  


}
