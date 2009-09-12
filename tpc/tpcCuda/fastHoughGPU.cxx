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
#include "TStopwatch.h"

#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <cstdlib>
#include <sstream>
#include <string>

#include "Hough5DNode.h"
#include "fastHoughGPU_IFC.h"


bool getBit(char* c, int n) {
  int i = (int) (c[n>>3] & (1 << (n & 7)));
  return (bool) i;
}


void clearBit(char* c, int n) {
  c[n>>3] = c[n>>3] & ~(1 << (n & 7 ));
}

//helper functor for node sorting
bool compareNodes (Hough5DNode* n1, Hough5DNode* n2) { 
  return (n1->getVote() > n2->getVote()); 
}



int main(int argc, char** argv) {

  extern char *optarg;
  int c;
  
  int TREE_DEPTH = 6;  //number of space divisions
  int THRESHOLD = 40;

  int THREADS = 320;
  float SCALE =0.90f;

  uint cutoffDec = 7;
  uint cutoffLevel = 4;
  

  int minCL = 5;
  
  int dynLevel = 5;
  bool tracking = false;

 
  float m_Max = 1.f;
  float m_Min = -1.f;
  float t_Max = 5.f;
  float t_Min = -5.f;
  float phi_Min = 0.f;
  float phi_Max = 180.f;
  float theta_Min = 20.f;
  float theta_Max = 160.f;
  float c_Min = -0.5f;
  float c_Max = 0.5f;

//    float m_Max = 10.f;
//    float m_Min = -10.f;
//    float t_Max = 200.f;
//    float t_Min = -200.f;
//    float phi_Min = 0.f;
//    float phi_Max = 180.f;
//    float theta_Min = 20.f;
//    float theta_Max = 160.f;
//    float c_Min = -1.f;
//    float c_Max = 1.f;


  float mins[5] = {phi_Min, theta_Min, c_Min, m_Min, t_Min};
  float maxs[5] = {phi_Max, theta_Max, c_Max, m_Max, t_Max};
  

  while ((c = getopt(argc, argv, "t:d:T:l:s:c:cl")) != -1)
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
    case 'l':
      dynLevel = atoi(optarg);
      break;
    case 's':
      tracking = true;
      break;
    case 'c':
      cutoffDec = atoi(optarg);
      break;
    case 'cl':
      cutoffLevel = atoi(optarg);
      break;
    default :
      std::cout<<"\n\nFast Hough Transformation on the GPU -------\n\n"
	       <<"Options:\n\n"
	       <<"     -d:  Tree Depth - when to abort the algorithm\n"
	       <<"     -t:  Threshold - Minimal number of votes required"
	       <<" unti dynamic thresholding kicks in\n"
	       <<"     -l:  Starting level for dynamic thresholding\n"
	       <<"     -T:  Number of threads per kernel block (default:"
	       <<" 128)\n"
	       <<"     -s:  Search for tracks after FHT search is finished\n"
	       <<"     -c:  First decimal of fraction of nodes to kill in "
	       <<" cutoff\n"
	       <<"     -cl:  Level at which to start cutoff\n"
	       <<std::endl;
      return 0;
    }


  TApplication* app = new TApplication("blub", NULL, NULL);
  

  //READ data and CREATE histograms and data containers -----------------



  unsigned int EVENT=6;
  

  
  TString dir = "../../DATA/";
  
  TString project = "Test10";  //with event 6!
  //TString project = "Test20";  //with event 7!

  //TString project = "EvtMixExample";
  

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

  
  TStopwatch timer;
  timer.Start();
  


  // FAST HOUGH SEARCH --------------------------------------------------


  //instantiate interface object:

  fastHoughGPU_IFC* IFC = new fastHoughGPU_IFC(40, 10000000);
  int nClusters = riemannListRZ.size();

  
  char* root_hitlist = (char*) malloc(nClusters/sizeof(char) + 1);
  
  //size of one node-hitlist in units of char
  int chunk = nClusters/(sizeof(char)*8) + 1;

  //initialize virgin hitlist to 1-bits only
  memset(root_hitlist,0xFF,chunk);

    
  //set up the IFC
  IFC->setKernelPars(THREADS);
  IFC->initClusters(clusterList);
  IFC->initParameterSpace(mins, maxs);
  IFC->setCutoff((float)cutoffDec/10);
  IFC->setCutoffLevel(cutoffLevel);
  
  std::vector<Hough5DNode*>* nodelist= new std::vector<Hough5DNode*>();
  unsigned int* votes;


  IFC->setHitList(root_hitlist,1);
  
  //init rood node
  std::cout<<"Init root node: "<<std::endl;
  float center[5] = {0.f,0.f,0.f,0.f,0.f};
  Hough5DNode* root = new Hough5DNode(center, 0); 
   

  //test root node only for intersection
  nodelist->push_back(root);
  IFC->testIntersection(nodelist,0,THRESHOLD);
  votes = IFC->getVotes();

  std::cout<<"\nroot node received "<<votes[0]
	   <<" of "<<nClusters<<" votes"<<std::endl;
  
  if(votes[0]<THRESHOLD) {
    std::cout<<"not enough votes for root! Something's wrong, aborting . . ."
	     <<std::endl;
    return 0;
  }
  
  free(root_hitlist);
    
  // made it through root, begin oct-tree search ------------------------
  
   
  
  std::vector<Hough5DNode*>* last_nodes = new std::vector<Hough5DNode*>();


  // float thresh_min=35;
//   float thresh_step = (THRESHOLD-thresh_min)/TREE_DEPTH;
//   std::cout<<"thresh_step: "<<thresh_step<<std::endl;
  
  
  char* new_hitlist;
  char* old_hitlist = (char*) malloc(nClusters/sizeof(char) + 1);
  
  
  //initialize virgin hitlist to 1-bits only
  memset(old_hitlist,0xFF,chunk);
  
  std::cout<<"\nROOT NODE HITLIST:"<<std::endl;
  for(int l=0; l<nClusters; l++) 
    std::cout<<getBit(old_hitlist, l);
  std::cout<<"\n\n"<<std::endl;

  int count=1;
  uint MAXSIZE = 10000000;

  for(int l=1; l<TREE_DEPTH; ++l) {
    
    std::vector<Hough5DNode*>* new_nodes = new std::vector<Hough5DNode*>();
    //avoid resizing
    new_nodes->reserve(MAXSIZE);
      
    std::cout << nodelist->size();

    std::cout.flush();
    
    new_hitlist = (char*) malloc(count*32*chunk);
    std::cout << " " << (void*) new_hitlist << std::endl;
    
    if(l>1)
      old_hitlist = IFC->getHitList();
        
    int counter=0;
    //create new nodes
    for(int n=0; n<nodelist->size(); ++n) {
     //  for(int c=0; c<nClusters; c++)
//  	std::cout<<getBit(old_hitlist+chunk*n, c);
//       std::cout<<"\n"<<votes[n]<<"\n"<<std::endl;
      Hough5DNode* the_node=(*nodelist)[n];
      float* sons = the_node->getSonArray();
      if(l<dynLevel) {
	if(votes[n]>=THRESHOLD) {
	  
	  the_node->setVotes(votes[n]);
	  //copy this nodes' hitlist to the new one
	  //son hitlist duplication happens in the IFC
	  
	  memcpy(new_hitlist+chunk*counter, 
		 old_hitlist+n*chunk, chunk);
	  for(int s=0; s<32; s++) {
	    new_nodes->push_back(new Hough5DNode(sons+5*s,l,nClusters));
	  }
	  counter++;
	  
	}
	else {
	  delete (*nodelist)[n];
	  (*nodelist)[n] = NULL;
	}
      }
      //dynamic thresholding based on last generation's vote
      else {
	if(votes[n] >= ((*last_nodes)[(int)n/32])->getVote()*SCALE) {
	  memcpy(new_hitlist+chunk*counter, 
		 old_hitlist+n*chunk, chunk);
	  counter++;
	  the_node->setVotes(votes[n]);
	  for(int s=0; s<32; ++s) {
	    new_nodes->push_back(new Hough5DNode(sons+5*s,l,nClusters));
	  }
	} 
	else {
	  delete (*nodelist)[n];
	  (*nodelist)[n] = NULL;
	}
      }
      //not necessary?
      free(sons);
    }
   

    for(int x=0; x<last_nodes->size(); x++)
      delete (*last_nodes)[x];

    if(l<=TREE_DEPTH-1)
      last_nodes->clear();
    
    int lcount=0;
    for(int n=0; n<nodelist->size(); n++)
      if((*nodelist)[n] != NULL) {
	lcount++;
	last_nodes->push_back((*nodelist)[n]);
      }
    
    std::cout<<"setting hitlist"<<std::endl;
    IFC->setHitList(new_hitlist,counter);
    free(new_hitlist);

    
    std::cout<<"Added "<<last_nodes->size()<<" last_nodes"
	     <<"     (count="<<count<<")"<<std::endl;
    nodelist->clear();
    nodelist=new_nodes;    
	
    
    //for(int i=0; i<last_nodes->size(); i++)
    //  last_nodes->at(i)->print();
    
    //std::cout<<"Calling Intersect-Kernel with THRESHOLD: "
    //<<THRESHOLD-l*thresh_step<<std::endl;
    //if(l<6)
    IFC->testIntersection(nodelist,l,THRESHOLD);
    //else
    //IFC->testIntersection(*nodelist,l,35);
    //IFC->testIntersection(*nodelist,l,THRESHOLD-l*thresh_step);
    votes = IFC->getVotes();
    
    count=0;
        
    for(int k=0; k<nodelist->size(); ++k) {
      if(l<5)  {
	if(votes[k] >= THRESHOLD)
	  count++; }
      else
	if(votes[k] >= last_nodes->at((int)k/32)->getVote()*SCALE) 
	  count++;
    }
    std::cout<<"LEVEL "<<l<<":  "<<count<<" of "
         <<nodelist->size()<<" checked the test"<<std::endl;
      
  }
  
  for(int x=0; x<nodelist->size(); x++){
    (nodelist->at(x))->setVotes(votes[x]);
  }

 
  

  // --------------------- END FHT ------------------------------------------------

  timer.Stop();
  

  TFile* file = new TFile("plots.root");
  TH2D* phic = (TH2D*)file->Get("phic_80");
  
    
  
  std::vector<TBox*> boxlist;
  
  for(int n=0; n<nodelist->size(); n++) {
    //(solution_list[s])->print();
    //if((*nodelist)[n]->getVote() > last_nodes->at((int)n/32)->getVote()*SCALE) {
    if((*nodelist)[n]->getVote() >= THRESHOLD*0.85) {
      float* center = ((*nodelist)[n])->getCenter();
      float length = ((*nodelist)[n])->getSideLength();
      float x1 = (center[3] - 0.5*length)*(m_Max-m_Min);
      float x2 = (center[3] + 0.5*length)*(m_Max-m_Min);
      float y1 = (center[4] - 0.5*length)*(t_Max-t_Min);
      float y2 = (center[4] + 0.5*length)*(t_Max-t_Min);
      boxlist.push_back(new TBox(x1,y1,x2,y2));
    }
  }
  
  gStyle->SetPalette(1);
  gROOT->SetStyle("Plain");
  
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

  

 //PURGE NODES AND EXTRACT CLUSTERS --------------------------
  
  if(tracking) {


    //make new clusterList
    std::vector<PndTpcCluster*> finalClist;
    for(int i=0; i<clusterList.size(); i++) {
      TVector3 pos = (clusterList[i])->pos();
      if(pos.X()>0)
	finalClist.push_back(clusterList[i]);
    }
    assert(finalClist.size() == nClusters);
    
    
    bool cont=true;
    char* hitlist = IFC->getHitList();
    
    //list of clusters for each found track
    std::vector<std::vector<PndTpcCluster*>*> solutions;
    
    std::vector<TH2D*> hists;
    
    std::vector<Color_t> colors;
    colors.push_back(kGreen);
    colors.push_back(kGreen-9);
    colors.push_back(kSpring+8);
    colors.push_back(kOrange-2);
    colors.push_back(kOrange+7);
    colors.push_back(kRed+3);
    colors.push_back(kRed);
    colors.push_back(kMagenta+2);
    colors.push_back(kBlue-6);
    colors.push_back(kBlue+1);
    colors.push_back(kAzure-3);
    
    //set votes in node objects
    for(int n=0; n<nodelist->size(); n++) 
      if((*nodelist)[n]->getVote()>0) {
	int c=0;
	for(int v=0; v<nClusters; v++) 
	  if(getBit(hitlist+n*chunk,v)) {
	    (*nodelist)[n]->setHit(v);
	    c++;
	  }
	assert(c==(*nodelist)[n]->getVote());
      }
    
    
    
    
    
    //extract tracks until solutions have less clusters than minCL
    while(cont) {
      
      //sort nodes by final votes
      sort((*nodelist).begin(), (*nodelist).end(), compareNodes);
      
      //extract clusters from best node
      bool* bestHitList = nodelist->front()->getHitList();
      
      std::cout<<nodelist->front()->getVote()<<std::endl;
      for(int b=0; b<nClusters; b++)
	std::cout<<(bool)bestHitList[b]<<" ";
      std::cout<<std::endl;
      
      std::vector<PndTpcCluster*>* sol = new std::vector<PndTpcCluster*>();
      
      for(int c=0; c<nClusters; c++) 
	if(bestHitList[c])
	  sol->push_back(finalClist[c]);
      
      if(sol->size()<minCL) {
	cont=false;
	continue;
      }
      
      //remove hits for first node from all others
      for(int p=0; p<nClusters; p++) {
	if(!bestHitList[p])
	  continue;
	for(int n=0; n<nodelist->size(); n++)
	  if((*nodelist)[n]->checkHit(p))
	    (*nodelist)[n]->removeHit(p);
      }
      
      solutions.push_back(sol);
      
      
    }
    
    TCanvas* blub = new TCanvas();
    //blub->Divide(3,2);
        
    
    for(int i=0; i<solutions.size(); i++) {
      std::string tr = "track_";
      std::stringstream ss;
      ss<<i;
      tr.append(ss.str());
      hists.push_back(new TH2D(tr.c_str(), "", 
			       100,-42,42,100,-42,42));
      //fill hist with positions;
      for(int p=0; p<(solutions[i])->size(); p++) {
      TVector3 pos = (solutions[i])->at(p)->pos();
      hists.back()->Fill(pos.X(), pos.Y());
      }
      std::cout<<"\nFound Track with "<<(solutions[i])->size()
	       <<" clusters"<<std::endl;
      
      hists.back()->SetMarkerColor(colors[i]);
      hists.back()->SetMarkerStyle(20);
      hists.back()->SetMarkerSize(0.5);
     
      //blub->cd(i+1);
      
      //hists.back()->SetLineWidth(3);
      if(i>0)
	hists.back()->Draw("same");
      else
	hists.back()->Draw();
    }

  }

    
  std::cout<<"Track qualification: more than "<<minCL
	   <<" (minCL) hits\n\n"<<std::endl;
  
  std::cout<<"\n\nFHT took "<<timer.RealTime()<<" seconds to process"
	   <<std::endl;
  
  
  gApplication->SetReturnFromRun(true);
  //gApplication->Terminate();
  gSystem->Run();
  
}
