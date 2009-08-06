// first try

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1F.h"
#include "PndTpcDigi.h"
#include "TClonesArray.h"
#include "TCanvas.h"

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
 
  float m_Max = 1.;
  float m_Min = -1.;
  float t_Max = 5.;
  float t_Min = -5.;
  float phi_Min = 0;
  float phi_Max = 180;
  float theta_Min = 65;
  float theta_Max = 100;
  float c_Min = -0.1;
  float c_Max = 0.1;

  float mins[5] = {phi_Min, theta_Min, c_Min, m_Min, t_Min};
  float maxs[5] = {phi_Max, theta_Max, c_Max, m_Max, t_Max};
  

  while ((c = getopt(argc, argv, "t:d:")) != -1)
    switch (c) {
    case 't':
      THRESHOLD = atoi(optarg);
      break;
    case 'd':
      TREE_DEPTH = atoi(optarg);
      break;
    }


  

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
  
  
  int BIN_m = 500;
  int BIN_t = 500;
  
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

  
   
  
  // FAST HOUGH SEARCH --------------------------------------------------

  //instantiate interface object:

  fastHoughGPU_IFC* IFC = new fastHoughGPU_IFC();
  int nClusters = riemannListRZ.size();
  
  //set up the IFC
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
  IFC->testIntersect(*nodelist,0,THRESHOLD);
  votes = IFC->getVotes();

  std::cout<<"\nroot node received "<<votes[0]
	   <<" of "<<nClusters<<" votes"<<std::endl;
  
  if(votes[0]<THRESHOLD) {
    std::cout<<"not enough votes for root! Something's wrong, aborting . . ."<<std::endl;
    return 0;
  }
  
    
  // made it through root, begin oct-tree search ------------------------
  
   
  for(int l=1; l<TREE_DEPTH; ++l) {
    
    std::vector<Hough5DNode*>* new_nodes = new std::vector<Hough5DNode*>();
    //create new nodes
    for(int n=0; n<nodelist->size(); ++n) {
      Hough5DNode* the_node=nodelist->at(n);
      float* sons = the_node->getSonArray();
      if(votes[n]>=THRESHOLD) {
	for(int s=0; s<32; ++s) 
	  new_nodes->push_back(new Hough5DNode(sons+5*s,l,nClusters));
      }
      delete nodelist->at(n);
    }
    nodelist->clear();
    nodelist=new_nodes;    

    //test root node only for intersection
    IFC->testIntersect(*nodelist,l,THRESHOLD);
    votes = IFC->getVotes();

    int count=0;
    for(int k=0; k<nodelist->size(); ++k)
      if(votes[k]>=THRESHOLD)
	count++;
    std::cout<<"LEVEL "<<l<<":  "<<count<<" of "<<nodelist->size()<<" checked the test"<<std::endl;
    
  }
    
  
}
