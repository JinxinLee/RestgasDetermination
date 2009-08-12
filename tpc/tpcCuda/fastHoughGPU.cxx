//-----------------------------------------------------------
//
// Description:
//      Driver programm for the pandaroot-CUDA interface
//      
//      
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer      TU Munich       (original author)
//
//
//-----------------------------------------------------------

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1F.h"
#include "PndTpcDigi.h"
#include "TClonesArray.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TH2D.h"
#include "TBox.h"

#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>

#include "Hough5DNode.h"
#include "fastHoughGPU_IFC.h"



int main(int argc, char** argv) {

  extern char *optarg;
  int c;
  
  int TREE_DEPTH = 6;  //number of space divisions
  int THRESHOLD = 40;

  int THREADS = 64;
  float SCALE =0.90;

 
  float m_Max = 1.f;
  float m_Min = -1.f;
  float t_Max = 5.f;
  float t_Min = -5.f;
  float phi_Min = 0.f;
  float phi_Max = 180.f;
  float theta_Min = 0.f;
  float theta_Max = 180.f;
  float c_Min = -0.5f;
  float c_Max = 0.5f;

  float mins[5] = {phi_Min, theta_Min, c_Min, m_Min, t_Min};
  float maxs[5] = {phi_Max, theta_Max, c_Max, m_Max, t_Max};
  

  while ((c = getopt(argc, argv, "t:d:T:")) != -1)
    switch (c) {
    case 't':
      THRESHOLD = atoi(optarg);
      break;
    case 'd':
      TREE_DEPTH = atoi(optarg);
      break;
    case 'T':
      THREADS = atoi(optarg);
      break;
      
    }


  TApplication* app = new TApplication("blub", NULL, NULL);
  

  //READ data and CREATE histograms and data containers -----------------



  unsigned int EVENT=6;
  

  
  TString dir = "../../DATA/";
  TString project = "Test10";

  project=dir+project;
  //TString mc_filename = project+".mc.root";
  TString reco_filename = project+".reco.root";

    
  TFile* reco_file =  new TFile(reco_filename);
  TTree* reco_tree = (TTree*)reco_file->Get("cbmsim");

    
  TClonesArray* _clusters = new TClonesArray("PndTpcCluster");
  reco_tree->SetBranchAddress("PndTpcCluster", &_clusters);
  reco_tree->GetEntry(EVENT);
 
    
  int size = _clusters->GetEntriesFast();
      
  std::vector<PndTpcCluster*> clusterList;
  std::vector<TVector3> riemannListRZ;

  //loop over clusters --------------------------------------------------
  for(int c=0; c<size; ++c) {
    PndTpcCluster* cl = ((PndTpcCluster*)_clusters->At(c));
    clusterList.push_back(cl);    
    TVector3 pos = cl->pos();
    
    if(pos.X() < 0.)
      continue;
    
    riemannListRZ.push_back(TVector3(pos.Perp(), 0., pos.Z()));
  } 


  // PLOT RZ Hough Histogram --------------------------------------------
  
  TH2D* houghRZ = new TH2D("vfg", "RZ hough space", 
			   500, m_Min, m_Max, 500, t_Min, t_Max);

  double tBinWidth = (t_Max - t_Min)/500;
  
  for(unsigned int rp=0; rp<riemannListRZ.size(); ++rp) {
    
    TVector3 pointRZ = riemannListRZ[rp];
    double perp = pointRZ.X();
    double z = pointRZ.Z();
            
    for(unsigned int t=0; t<500; ++t) {
      
      double T = (t+0.5)*tBinWidth + t_Min;
      double M = (T-z) / (perp*(-1.));
      houghRZ->Fill(M,T);
    }
  }

  
 

  
  
  // FAST HOUGH SEARCH --------------------------------------------------

  //instantiate interface object:

  fastHoughGPU_IFC* IFC = new fastHoughGPU_IFC(40, 10000000);
  int nClusters = riemannListRZ.size();
  
  //set up the IFC
  IFC->setKernelPars(THREADS);
  IFC->initClusters(clusterList);
  IFC->initParameterSpace(mins, maxs);
  
  std::vector<Hough5DNode*>* nodelist= new std::vector<Hough5DNode*>();
  unsigned int* votes;

  
  //init rood node
  std::cout<<"Init root node: "<<std::endl;
  float center[5] = {0.f,0.f,0.f,0.f,0.f};
  Hough5DNode* root = new Hough5DNode(center, 0); 
   

  //test root node only for intersection
  nodelist->push_back(root);
  IFC->testIntersection(*nodelist,0,THRESHOLD);
  votes = IFC->getVotes();

  std::cout<<"\nroot node received "<<votes[0]
	   <<" of "<<nClusters<<" votes"<<std::endl;
  
  if(votes[0]<THRESHOLD) {
    std::cout<<"not enough votes for root! Something's wrong, aborting . . ."<<std::endl;
    return 0;
  }
  
    
  // made it through root, begin oct-tree search ------------------------
  
   
  
  std::vector<Hough5DNode*>* last_nodes = new std::vector<Hough5DNode*>();


  float thresh_min=35;
  float thresh_step = (THRESHOLD-thresh_min)/TREE_DEPTH;
  std::cout<<"thresh_step: "<<thresh_step<<std::endl;
  

  for(int l=1; l<TREE_DEPTH; ++l) {
    
    std::vector<Hough5DNode*>* new_nodes = new std::vector<Hough5DNode*>();
    
    
    //create new nodes
    for(int n=0; n<nodelist->size(); ++n) {
      Hough5DNode* the_node=nodelist->at(n);
      float* sons = the_node->getSonArray();
      
      if(l<5) {
	if(votes[n]>=THRESHOLD) {
	  for(int s=0; s<32; ++s) {
	    the_node->setVotes(votes[n]);
	    new_nodes->push_back(new Hough5DNode(sons+5*s,l,nClusters));
	  }
	}
	else {
	  delete nodelist->at(n);
	  nodelist->at(n) = NULL;
	}
      }
      else {
	//working, but not fitting with fixed THR of testIntersect
	if(votes[n] >= last_nodes->at((int)n/32)->getVote()*SCALE) {
	  for(int s=0; s<32; ++s) {
	    the_node->setVotes(votes[n]);
	    new_nodes->push_back(new Hough5DNode(sons+5*s,l,nClusters));
	  }
	} 
	else {
	  delete nodelist->at(n);
	  nodelist->at(n) = NULL;
	}
      }
    }

    for(int x=0; x<last_nodes->size(); x++)
      delete last_nodes->at(x);

    if(l<TREE_DEPTH-1)
      last_nodes->clear();
    
    int lcount=0;
    for(int n=0; n<nodelist->size(); n++)
      if(nodelist->at(n) != NULL) {
	lcount++;
	last_nodes->push_back(nodelist->at(n));
      }
    
    
    std::cout<<"Added "<<last_nodes->size()<<" last_nodes"<<std::endl;
    nodelist->clear();
    nodelist=new_nodes;    

    //for(int i=0; i<last_nodes->size(); i++)
    //  last_nodes->at(i)->print();
    
    

    //std::cout<<"Calling Intersect-Kernel with THRESHOLD: "
    //<<THRESHOLD-l*thresh_step<<std::endl;
    //if(l<6)
      IFC->testIntersection(*nodelist,l,THRESHOLD);
      //else
      //IFC->testIntersection(*nodelist,l,35);
    //IFC->testIntersection(*nodelist,l,THRESHOLD-l*thresh_step);
    votes = IFC->getVotes();
    
    int count=0;
    for(int k=0; k<nodelist->size(); ++k)
      if(votes[k]>=THRESHOLD)
	count++;
    std::cout<<"LEVEL "<<l<<":  "<<count<<" of "
	     <<nodelist->size()<<" checked the test"<<std::endl;
    
  }
  
  for(int n=0; n<nodelist->size(); n++){
    (nodelist->at(n))->setVotes(votes[n]);
  }


  // --------------------- END FHT ------------------------------------------------


  

  TFile* file = new TFile("plots.root");
  TH2D* phic = (TH2D*)file->Get("phic_80");
  
    
  
  std::vector<TBox*> boxlist;
  
  for(int n=0; n<nodelist->size(); n++) {
    //(solution_list[s])->print();
    //if(nodelist->at(n)->getVote() > last_nodes->at((int)n/32)->getVote()*SCALE) {
    if(nodelist->at(n)->getVote() >= THRESHOLD*0.85) {
      float* center = (nodelist->at(n))->getCenter();
      float length = (nodelist->at(n))->getSideLength();
      float x1 = (center[3] - 0.5*length)*(m_Max-m_Min);
      float x2 = (center[3] + 0.5*length)*(m_Max-m_Min);
      float y1 = (center[4] - 0.5*length)*(t_Max-t_Min);
      float y2 = (center[4] + 0.5*length)*(t_Max-t_Min);
      boxlist.push_back(new TBox(x1,y1,x2,y2));
    }
  }
  
  gStyle->SetPalette(1);
  
  TCanvas* canv = new TCanvas();
  //canv->SetGrayscale();
  houghRZ->Draw();
  
  
  for(int b=0; b<boxlist.size(); ++b) {
    (boxlist[b])->SetLineColor(kPink+10);
    (boxlist[b])->SetFillStyle(0);
    (boxlist[b])->Draw("l");
  }

  std::vector<TBox*> boxlist2;
    
  for(int s=0; s<nodelist->size(); s++) {
    //if(nodelist->at(s)->getVote() > last_nodes->at((int)s/32)->getVote()*SCALE) {
    if(nodelist->at(s)->getVote() >= THRESHOLD*0.85) {
      float* center = (nodelist->at(s))->getCenter();
      float length = (nodelist->at(s))->getSideLength();
      float x1 = (center[0] - 0.5*length)*(phi_Max-phi_Min) +90;
      float x2 = (center[0] + 0.5*length)*(phi_Max-phi_Min) +90;
      float y1 = (center[2] - 0.5*length)*(c_Max-c_Min);
      float y2 = (center[2] + 0.5*length)*(c_Max-c_Min);
      boxlist2.push_back(new TBox(x1,y1,x2,y2));
    }
  }

  TCanvas* canv2 = new TCanvas();
  phic->Draw("COLZ");
  
  for(int b=0; b<boxlist2.size(); ++b) {
    (boxlist2[b])->SetLineColor(kPink+10);
    (boxlist2[b])->SetFillStyle(0);
    (boxlist2[b])->Draw("l");
  }
  
  
  TH2D* sebastian_stinkt = new TH2D("seb", "Sebastian riecht streng",
				    100,phi_Min,phi_Max, 100, m_Min, m_Max);
  TCanvas* canv3 = new TCanvas();


  std::vector<TBox*> boxlist3;
  //sparse->Projection(0,3)->Draw("COLZ");
  sebastian_stinkt->Draw();
  for(int s=0; s<nodelist->size(); s++) {
    if(nodelist->at(s)->getVote() >= THRESHOLD*0.85) {
      float* center = (nodelist->at(s))->getCenter();
      float length = (nodelist->at(s))->getSideLength();
      float x1 = (center[0] - 0.5*length)*(phi_Max-phi_Min) +90;
      float x2 = (center[0] + 0.5*length)*(phi_Max-phi_Min) +90;
      float y1 = (center[3] - 0.5*length)*(m_Max-m_Min);
      float y2 = (center[3] + 0.5*length)*(m_Max-m_Min);
      boxlist3.push_back(new TBox(x1,y1,x2,y2));
    }
  }

  for(int b=0; b<boxlist3.size(); ++b) {
    (boxlist3[b])->SetLineColor(kPink+10);
    (boxlist3[b])->SetFillStyle(0);
    (boxlist3[b])->Draw("l");
  }


  
  gApplication->SetReturnFromRun(true);
  gSystem->Run();
    
}
