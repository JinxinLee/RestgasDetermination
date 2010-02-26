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

#include "PndTpcCluster.h"
#include "Hypersurface2D.h"
#include "Hough2DNode.h"

#include <vector>
#include <iostream>
#include <algorithm>

//helper functor for node sorting
bool compareNodes (Hough2DNode* n1, Hough2DNode* n2) { 
  return (n1->getVote() > n2->getVote()); 
}



void plotHough(TString filename, int evLo, int evHi, int DEPTH, int THRESH) {

  //param space -----------------------------
  double theta_min = -3.f;
  double theta_max = 3.f;
  double r_min = 0.f;
  double r_max = 15.f;
  
  double mins[2] = {theta_min, r_min};
  double maxs[2] = {theta_max, r_max};
    
  // ----------------------------------------

  double x_OFF = 5.f;
  
  TFile* recofile = new TFile(filename);
  if(recofile->IsZombie()) {
    std::cerr<<"Reco file not existing! Aborting."<<std::endl;
  }

  TCanvas* canv =  new TCanvas();
  canv->Divide(3,1);

  TH2F* real = new TH2F("bla", "Real Space",100,0,10,100,0,8);
  real->GetXaxis()->SetTitle("x + 5 (cm)");
  real->GetYaxis()->SetTitle("z (cm)");  
  TH2F* hough = new TH2F("bldsaa", "Hough Space",100,-3,3,100,0,15);
  hough->GetXaxis()->SetTitle("Theta (radians)");
  hough->GetYaxis()->SetTitle("Distance r (cm)");

  TTree* recotree = (TTree*)recofile->Get("cbmsim");
  
  TClonesArray* clArr = new TClonesArray("PndTpcCluster");
  recotree->SetBranchAddress("PndTpcCluster", &clArr);
  
  unsigned int nEv = evHi-evLo;

  TObjArray* clusters = new TObjArray();
  TObjArray* surfs = new TObjArray();
  
  std::vector<Hypersurface2D*> hitreps;
  std::vector<PndTpcCluster*> totClusters;

  TF1* rep = new TF1("blu","[0]*cos(x)+[1]*sin(x)",-3,3);

  int totCl =0;
  
  for(unsigned int k=0; k<=nEv; k++) {
    recotree->GetEvent(evLo+k);
    
    //loop over clusters
    int nCl = clArr->GetEntriesFast();
    
    TPolyMarker* marker = new TPolyMarker(nCl);
    marker->SetMarkerStyle(4);    

    for(int c=0; c<nCl; c++) {
      PndTpcCluster* cl = (PndTpcCluster*)clArr->At(c);
      totClusters.push_back(new PndTpcCluster(*cl));
      TVector3 pos = cl->pos();
      double x = pos.X()+x_OFF; //offset to avoid theta-symmetry and numerical problems
      double z = pos.Z();
      marker->SetPoint(c,x,z); 
            
      //transformed representation
      TVector3* vec = new TVector3(x,z,0);
      double r = vec->Mag();
      double theta = vec->Phi();
      
      std::cout<<"theta: "<<theta<<", x: "<<x<<", z:"<<z<<std::endl;
      
      TF1* surf = new TF1("blu","[0]*cos(x)+[1]*sin(x)",-3,3);
      surf->SetParameter(0,x);
      surf->SetParameter(1,z);
      surf->SetLineWidth(1);
      surfs->Add(surf);
      
      hitreps.push_back(new Hypersurface2D(x,z,*surf,c+totCl));
      hitreps.back()->setParamSpace(mins, maxs);
    }
    
    totCl+=nCl;
    clusters->Add(marker);
    
  }
  
  canv->cd(1);
  real->Draw();
  for(int n=0; n<clusters->GetEntries(); n++)
    ((TPolyMarker*)clusters->At(n))->Draw("same");

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
      double* son_arr = the_node->getSonArray();
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
    double* center = (survivors->at(l))->getCenter();
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
  colors.push_back(kOrange-2);
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
    bool* bestHitList = survivors->front()->getHitList();
    
    std::cout<<survivors->front()->getVote()<<std::endl;
    for(int b=0; b<totCl; b++)
      std::cout<<(bool)bestHitList[b]<<" ";
    std::cout<<std::endl;
    
    std::vector<PndTpcCluster*>* sol = new std::vector<PndTpcCluster*>();
    
    for(int c=0; c<totCl; c++) 
      if(bestHitList[c])
	sol->push_back(totClusters[c]);
    
    if(sol->size()<5) {
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

  for(int i=0; i<solutions.size(); i++) {
    TPolyMarker* mark = new TPolyMarker((solutions[i])->size());
    
    //fill hist with positions;
    for(int p=0; p<(solutions[i])->size(); p++) {
      TVector3 pos = (solutions[i])->at(p)->pos();
      mark->SetPoint(p,pos.X()+5,pos.Z());
    }
        
    mark->SetMarkerColor(colors[i]);
    mark->SetMarkerStyle(4);
    
    clusters->Add(mark);
    mark->Draw("same");

    Hough2DNode* cand = cand_nodes[i];
    double* center = cand->getCenter();
    
    double theta = (center[0] + 0.5f)*(theta_max-theta_min)+theta_min;
    double r_shift = (center[1] + 0.5f)*(r_max-r_min)+r_min;

    double r = r_shift - x_OFF*cos(theta);
    
    double m = -1.f/(tan(theta));
    double t = r/(sin(theta));
    
    double y1 = 0.f;
    double x1 = (y1-t)/m;
    
    double y2 = 8.f;
    double x2 = (y2-t)/m;
    
    TPolyLine* line = new TPolyLine(2);
    line->SetPoint(0,x1+x_OFF,y1);
    line->SetPoint(1,x2+x_OFF,y2);
    
    lines->Add(line);
    line->Draw("same");
    
  }

  
  
  
}
