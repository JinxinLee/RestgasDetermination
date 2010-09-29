//Plotting macro for TestBench data. 
//Needs to be compiled!

//Author: Felix Boehmer



#include "TCanvas.h"
#include "TFile.h"
#include "TH2F.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TPolyMarker.h"
#include "TPolyLine.h"
#include "TVector3.h"
#include "TBox.h"
#include "TSystem.h"
#include "TMath.h"
#include "TStyle.h"


#include "PndTpcCluster.h"
#include "Hypersurface2D.h"
#include "Hough2DNode.h"
#include "PndTpcPadPlane.h"
#include "PndTpcPadShapePool.h"
#include "PndTpcPad.h"

#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <map>
#include <string>
#include <exception>

//helper functor for node sorting
bool compareNodes (Hough2DNode* n1, Hough2DNode* n2) { 
  return (n1->getVote() > n2->getVote()); 
}



void plotHough(TString filename, TString type, int evLo, int evHi, 
	       int DEPTH, int THRESH) {


  gStyle->SetOptStat(0);

  //param space -----------------------------
  double theta_min = -3.f;
  double theta_max = 3.f;
  double r_min = 0.f;
  double r_max = 15.f;
  
  double mins[2] = {theta_min, r_min};
  double maxs[2] = {theta_max, r_max};
    
  // ----------------------------------------

  TString cosm = "COSMIC";   //steers the param space
  TString elsa = "ELSA";

  if(!(type==cosm || type==elsa)) {
    std::cout<<"Wrong run type specifier! Expected 'ELSA' or 'COSMIC'"<<std::endl;
    return;
  }
  
  double x_OFF = 0.;
  if(type==elsa)
    x_OFF=5.;
  
  int MINCANDSIZE = 10;
  
  TFile* recofile = new TFile(filename);
  if(recofile->IsZombie()) {
    std::cerr<<"Reco file not existing! Aborting."<<std::endl;
  }

  std::string vmc = gSystem->Getenv("VMCWORKDIR");
  vmc.append("/tpc/TestBench/");
  std::string copy = vmc;
  
  PndTpcPadShapePool* pool=new PndTpcPadShapePool(copy.append("TBhexa_pads.dat").c_str());
  copy=vmc;
  PndTpcPadPlane* plane = new PndTpcPadPlane(copy.append("newPadTC.dat").c_str(),
					     pool);

  std::cout<<"Plane has "<<plane->GetNPads()<<" pads."<<std::endl;

  TCanvas* canv =  new TCanvas("dasd","sadasf",1000,1000);
  canv->Divide(2,2);

  TH2F* real;
  if(type==cosm) {
    real= new TH2F("bla", "Real Space z-y",100,0,10,100,-5,5);
    real->GetXaxis()->SetTitle("z (cm)");
    real->GetYaxis()->SetTitle("y (cm)");  
  }
  else {
    real = new TH2F("bla", "Real Space",100,-5,5,100,0,8);
    real->GetXaxis()->SetTitle("x (cm)");
    real->GetYaxis()->SetTitle("z (cm)"); 
  }

  TH2F* real2;
  real2 = new TH2F("bla2", "Real Space x-y",100,-5,5,100,-5,5);
  real2->GetXaxis()->SetTitle("x (cm)");
  real2->GetYaxis()->SetTitle("y (cm)");  
  
  TH2F* hough = new TH2F("bldsaa", "Hough Space",100,-3,3,100,0,15);
  hough->GetXaxis()->SetTitle("Theta (radians)");
  hough->GetYaxis()->SetTitle("Distance r (cm)");

  TTree* recotree = (TTree*)recofile->Get("cbmsim");
  
  TClonesArray* clArr = new TClonesArray("PndTpcCluster");
  recotree->SetBranchAddress("PndTpcCluster", &clArr);
  
  unsigned int nEv = evHi-evLo;

  //TObjArray* clusters = new TObjArray();
  TObjArray* digis = new TObjArray();
  TObjArray* surfs = new TObjArray();

  std::map<int,TPolyMarker*> clusters;
  std::vector<std::map<int,TPolyMarker*>* > clusters2;
  std::map<int,TPolyMarker*> clustersXY;
  
  std::map<int,TPolyMarker*>::iterator it;
  
  std::vector<Hypersurface2D*> hitreps;
  std::vector<PndTpcCluster*> totClusters;

  TF1* rep = new TF1("blu","[0]*cos(x)+[1]*sin(x)",-3,3);

  int totCl =0;

  TPolyMarker* digimarker = new TPolyMarker(0);
  digimarker->SetMarkerStyle(5);
  TPolyMarker* marker=NULL;
  TPolyMarker* marker2D=NULL;

  //Draw padplane
  canv->cd(4);
  real2->Draw();
  int npads=plane->GetNPads();
  
  for(int i=0;i<npads;++i){
    PndTpcPad* apad=0;
    try{
      apad=plane->GetPad(i);
    }
    catch (std::exception &e){
      cout << e.what() << endl;
      cout.flush();
      continue;
    }
    apad->Draw(kBlack);
    
  }
  canv->Update();


    
  for(unsigned int k=0; k<=nEv; k++) {
    
    recotree->GetEvent(evLo+k);
    
    //loop over clusters
    int nCl = clArr->GetEntriesFast();
    
        
    for(int c=0; c<nCl; c++) {
      PndTpcCluster* cl = (PndTpcCluster*)clArr->At(c);
      totClusters.push_back(new PndTpcCluster(*cl));
      TVector3 pos = cl->pos();
      
      int size = cl->nDigi();
      int size2d = cl->get2DSize();
      //plot cluster in the right size     

      if(clusters.count(size)==0) {
	marker =  new TPolyMarker(0);
	clusters[size]=marker;
	marker->SetMarkerStyle(4);
	marker->SetMarkerSize((double)size/2);
	if(type==cosm)
	  marker->SetPoint(0,pos.Z(),pos.Y()); 
	else
	  marker->SetPoint(0,pos.X(),pos.Z()); 
      }
      else {
	it = clusters.find(size);
	marker=it->second;
	if(type==cosm)
	  marker->SetPoint(marker->GetLastPoint()+1,pos.Z(),pos.Y()); 
	else
	  marker->SetPoint(marker->GetLastPoint()+1,pos.X(),pos.Z()); 
      }
      
      if(clustersXY.count(size2d)==0) {
	marker2D =  new TPolyMarker(0);
	clustersXY[size2d] = marker2D;
	marker2D->SetMarkerStyle(4);
	marker2D->SetMarkerSize((double)size2d/2);
	marker2D->SetPoint(0,pos.X(),pos.Y()); 
	marker2D->SetMarkerColor(kBlue+1);
      }
      else {
      	marker2D=clustersXY[size2d];
	marker2D->SetPoint(marker2D->GetLastPoint()+1,pos.X(),pos.Y()); 
      }

      
      //std::cout<<"Getting digis ... "<<std::endl;
      //plot digis
      for(int d=0; d<size; d++) {
      	PndTpcDigi dig;
	dig = cl->getDigi(d);
	unsigned int padID = dig.padId();
	double x,y;
	
	PndTpcPad* apad=0;
	
	try{
	  plane->GetPadXY(padID,x,y);
	  apad=plane->GetPad(padID);
	}
	catch(...){
	  std::cout<<"Unknown padID: "<<padID<<std::endl;
	  continue;
	}
	apad->Draw(kOrange+10);
	digimarker->SetPoint(digimarker->GetLastPoint()+1,x,y);
      }
      
      
      //transformed representation
      TVector3* vec;
      if(type==cosm)
	vec = new TVector3(pos.Z(),pos.Y(),0);
      else
	vec = new TVector3(pos.X()+x_OFF,pos.Z(),0);
      
      double r = vec->Mag();
      double theta = vec->Phi();

      std::cout<<"theta: "<<theta<<", x: "<<pos.X() + x_OFF
      	       <<", z:"<<pos.Z()<<std::endl;
      
      TF1* surf = new TF1("blu","[0]*cos(x)+[1]*sin(x)",-3,3);
      if(type==cosm) {
	surf->SetParameter(0,pos.Z()+x_OFF);
	surf->SetParameter(1,pos.Y());
      }
      else {
	surf->SetParameter(0,pos.X()+x_OFF);
	surf->SetParameter(1,pos.Z());
      }
      surf->SetLineWidth(1);
      surfs->Add(surf);
      if(type==cosm) 
	hitreps.push_back(new Hypersurface2D(pos.Z()+x_OFF,pos.Y(),
					     *surf,c+totCl));
      else
	hitreps.push_back(new Hypersurface2D(pos.X()+x_OFF,pos.Z(),
					     *surf,c+totCl));
      hitreps.back()->setParamSpace(mins, maxs);
    }
    
    totCl+=nCl;
    
  }
  
  canv->cd(1);
  real->Draw();
  for(it=clusters.begin(); it!=clusters.end(); it++) 
    it->second->Draw("same");
  
  canv->cd(4);
    //digimarker->Draw("same");
  std::cout<<"XY clusters size: "<<clustersXY.size()<<std::endl;
  for(it=clustersXY.begin(); it!=clustersXY.end(); it++) 
    it->second->Draw("same");
  
 

   
  canv->cd(2);
  hough->Draw();
  for(int n=0; n<surfs->GetEntries(); n++)
    ((TF1*)surfs->At(n))->Draw("same");

  
    //hough search
 
  double center[2] = {0.f,0.f};
  Hough2DNode* root = new Hough2DNode(center,0,totCl);
  for(int i=0; i<hitreps.size(); i++)
    (hitreps[i])->testIntersect(root);
      
  std::cout<<"Number of votes: "<<root->getVote()<<std::endl;

  std::vector<Hough2DNode*>* survivors = new std::vector<Hough2DNode*>();
  survivors->push_back(root);

  for(int t=1; t<DEPTH; t++) {
    std::vector<Hough2DNode*>* sons = new std::vector<Hough2DNode*>();
    
    for(int n=0; n<survivors->size(); n++) { //loop over survivors
      //create sons
      Hough2DNode* the_node = survivors->at(n);
      const double* son_arr = the_node->getSonArray();
      for (int s=0; s<4; s++) {
	sons->push_back(new Hough2DNode(son_arr+2*s,the_node->getLevel()+1,totCl));
      }

      delete survivors->at(n);
    } //end loop over survivors
    
    survivors->clear();

    for(int s=0; s<sons->size(); s++) {
      Hough2DNode* node = sons->at(s);
      for(int i=0; i<hitreps.size(); i++)
	(hitreps[i])->testIntersect(node);
      if(node->getVote()>=THRESH)
	survivors->push_back(node);
      else
	delete sons->at(s);
    }
    std::cout<<"At level "<<t<<" we have "<<survivors->size()
	     <<" survivors"<<std::endl;
    
  }
  
  std::vector<TBox*> boxlist;
  for(int l=0; l<survivors->size(); l++) {
    const double* center = (survivors->at(l))->getCenter();
    double length = (survivors->at(l))->getSideLength();
    double x1 = (center[0] - 0.5*length + 0.5)*(theta_max-theta_min)+theta_min;
    double x2 = (center[0] + 0.5*length + 0.5)*(theta_max-theta_min)+theta_min;
    double y1 = (center[1] - 0.5*length + 0.5)*(r_max-r_min)+r_min;
    double y2 = (center[1] + 0.5*length + 0.5)*(r_max-r_min)+r_min;
    boxlist.push_back(new TBox(x1,y1,x2,y2));
    boxlist.back()->SetLineColor(kPink+10);
    boxlist.back()->SetFillStyle(0);
    boxlist.back()->Draw("l");
  }
   
  std::vector<Color_t> colors;
  colors.push_back(kSpring-6);
  colors.push_back(kOrange+5);
  colors.push_back(kRed+3);
  colors.push_back(kMagenta+2);
  colors.push_back(kBlue-6);
  colors.push_back(kAzure-3);


  //list of clusters for each found track
  std::vector<std::vector<PndTpcCluster*>*> solutions;
  std::vector<Hough2DNode*> cand_nodes;
 
 
  bool cont;
  //extract tracks until solutions have less clusters than minCL
  while(cont) {
    
    //sort nodes by final votes
    sort((*survivors).begin(), (*survivors).end(), compareNodes);
    
    //extract clusters from best node
    const bool* bestHitList = survivors->front()->getHitList();
    
    std::cout<<survivors->front()->getVote()<<std::endl;
    for(int b=0; b<totCl; b++)
      std::cout<<(bool)bestHitList[b]<<" ";
    std::cout<<std::endl;
    
    std::vector<PndTpcCluster*>* sol = new std::vector<PndTpcCluster*>();
    
    for(int c=0; c<totCl; c++) 
      if(bestHitList[c])
	sol->push_back(totClusters[c]);
    
    if(sol->size()<MINCANDSIZE) {
      cont=false;
      continue;
    }
    
    cand_nodes.push_back(survivors->front());  //remember cand. nodes
    
    //remove hits for first node from all others
    for(int p=0; p<totCl; p++) {
      if(!bestHitList[p])
	continue;
      for(int n=0; n<survivors->size(); n++)
	if((*survivors)[n]->checkHit(p))
	  (*survivors)[n]->removeHit(p);
    }
    
    solutions.push_back(sol);
    
  }


  //PURGE and draw track candidates
    
  canv->cd(3);
  real->Draw();

  TObjArray* lines = new TObjArray();
  
  TPolyMarker* mark=NULL;
  
  for(int i=0; i<solutions.size(); i++) {
      
    clusters2.push_back(new std::map<int,TPolyMarker*>());
    std::map<int,TPolyMarker*>* mmap = clusters2.back();


    //fill hist with positions;
    for(int p=0; p<(solutions[i])->size(); p++) {
      int size = (solutions[i])->at(p)->nDigi();

    //plot cluster in the right size     
      
      if(mmap->count(size)==0) {
	mark = new TPolyMarker(0);
	(*mmap)[size] = mark;
	mark->SetMarkerStyle(4);
	mark->SetMarkerSize((double)size/2);
	mark->SetMarkerColor(colors[i]);
      }
      else{
	mark=(*mmap)[size];
      }
      
      TVector3 pos = (solutions[i])->at(p)->pos();
      if(type==cosm)
	mark->SetPoint(mark->GetLastPoint()+1,pos.Z(),pos.Y());
      else
	mark->SetPoint(mark->GetLastPoint()+1,pos.X(),pos.Z());
    }
        
    Hough2DNode* cand = cand_nodes[i];
    const double* center = cand->getCenter();
    
    double theta = (center[0] + 0.5)*(theta_max-theta_min)+theta_min;
    double r_shift = (center[1] + 0.5)*(r_max-r_min)+r_min;
    
    double r = r_shift - x_OFF*cos(theta);
    
    double m = -1./(tan(theta));
    double t = r/(sin(theta));

        
    double y1;
    if(type==cosm)
      y1 = -5.;
    else
      y1 = 0.;
    double x1 = (y1-t)/m - x_OFF;
    
    double y2;
    if(type==cosm)
      y2 = 5.;
    else
      y2 = 8.;
    
    double x2 = (y2-t)/m - x_OFF;

    //std::cout<<"x1: "<<x1<<";  x2: "<<x2<<std::endl;
    
    TPolyLine* line = new TPolyLine(2);
    line->SetPoint(0,x1+x_OFF,y1);
    line->SetPoint(1,x2+x_OFF,y2);
    
    lines->Add(line);
    line->Draw("same");
    
    for(it=mmap->begin(); it!=mmap->end(); it++)
    it->second->Draw("same");
 

  }
  
  
   
}
