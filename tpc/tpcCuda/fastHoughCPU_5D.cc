//-----------------------------------------------------------
//
// Description:
//      Driver program for CPU-based FHT algorithm in 
//      5 dimensions
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



#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"
#include "PndTpcCluster.h"
#include "TClonesArray.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TString.h"
#include "TVectorD.h"
#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TH2D.h"
#include "TBox.h"
#include "THnSparse.h"

#include <cmath>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <cstdlib>

#include "Hough5DNode.h"
#include "Hyperplane5D.h"

#include "TStopwatch.h"


int main(int argc, char** argv) {

  extern char *optarg;
  int c;
  
  int TREE_DEPTH = 6;  //number of space divisions
  int THRESHOLD = 40;
  float SCALE = 0.95f;

 
  float m_Max = 2.;
  float m_Min = -2.;
  float t_Max = 5.;
  float t_Min = -5.;
  float phi_Min = 0;
  float phi_Max = 180;
  float theta_Min = 20;
  float theta_Max = 160;
  float c_Min = -0.5;
  float c_Max = 0.5;

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


  TApplication* app = new TApplication("blub", NULL, NULL);


  //READ data and CREATE histograms and data containers -----------------


  bool CUT_CHAMBER=true;   //only collect hits with x>0;
    
  unsigned int EVENT=7;
  
  TString dir = "../../DATA/";
  //TString project = "Test10";
  TString project = "Test20";

  project=dir+project;
  //TString mc_filename = project+".mc.root";
  TString reco_filename = project+".reco.root";

  TString sparse_file = project+"outputSHORT.root";
  
  TFile* reco_file =  new TFile(reco_filename);
  TTree* reco_tree = (TTree*)reco_file->Get("cbmsim");

  TFile* sparseF = new TFile(sparse_file);
  THnSparse* sparse = (THnSparseF*) sparseF->Get("fullHough");

  //TFile* mc_file =  new TFile(mc_filename);
  //TTree* mc_tree = (TTree*)mc_file->Get("cbmsim");

  TClonesArray* _clusters = new TClonesArray("PndTpcCluster");
  reco_tree->SetBranchAddress("PndTpcCluster", &_clusters);
  reco_tree->GetEntry(EVENT);
 
  // TClonesArray* _points = new TClonesArray("PndTpcPoint");
//   mc_tree->SetBranchAddress("PndTpcPoint", &_points);
//   mc_tree->GetEntry(EVENT);
  
  int nClusters = _clusters->GetEntriesFast();
  //int nPoints = _points->GetEntriesFast();
  
  int BIN_m = 500;
  int BIN_t = 500;
  
  std::vector<TVector3> riemannListRZ;
  std::vector<Hyperplane5D> hyperplanes;

  
  //loop over clusters --------------------------------------------------
  int count=0;
  for(int c=0; c<nClusters; ++c) {
    PndTpcCluster* cl = ((PndTpcCluster*)_clusters->At(c));
    TVector3 pos = cl->pos();
    
    if(CUT_CHAMBER && pos.X() < 0.)
      continue;
    
    riemannListRZ.push_back(TVector3(pos.Perp(), 0., pos.Z()));
    hyperplanes.push_back(Hyperplane5D(cl,count));
    (hyperplanes.back()).setParamSpace(mins, maxs);
    //float* coords = (hyperplanes.back()).getCoords();
    //for(int i=0; i<5; i++)
    //  std::cout<<coords[i]<<"  ";
    std::cout<<std::endl;
    count++;
    
  }


  // PLOT RZ Hough Histogram --------------------------------------------
  
  TH2D* houghRZ = new TH2D("vfg", "RZ hough space", 
			   BIN_m, m_Min, m_Max, BIN_t, t_Min, t_Max);

  double tBinWidth = (t_Max - t_Min)/BIN_t;
  
  for(unsigned int rp=0; rp<riemannListRZ.size(); ++rp) {
    
    TVector3 pointRZ = riemannListRZ[rp];
    double perp = pointRZ.X();
    double z = pointRZ.Z();
            
    for(unsigned int t=0; t<BIN_t; ++t) {
      
      double T = (t+0.5)*tBinWidth + t_Min;
      double M = (T-z) / (perp*(-1.));
      houghRZ->Fill(M,T);
    }
  }

  
 
  
  // FAST HOUGH SEARCH --------------------------------------------------

  unsigned int points = riemannListRZ.size();

  //init rood node
  std::cout<<"Init root node: "<<std::endl;
  float center[5] = {0.f,0.f,0.f,0.f,0.f};
  Hough5DNode* root = new Hough5DNode(center, 0, points); 
  
  //setting up parameter space
  //root->setParamSpace(mins, maxs);
  
  std::vector<Hough5DNode*>* parent_list = new std::vector<Hough5DNode*>();
    
  float* corners = root->getCorners();
  for(int i=0; i<160; i++) {
    if(i%5==0)
      std::cout<<std::endl;
    std::cout<<corners[i]<<"   ";
  }
  
  for(int i=0; i<points; i++) {
    Hyperplane5D plane = hyperplanes[i];
    plane.testIntersect(*root);
  }

  std::cout << "\nThere have been "<<root->getVote()
	    <<" of "<<points<<" hyperplane crossings in root"
	    <<std::endl;
  if(root->getVote() < THRESHOLD)
    return 0;
  
    
  TStopwatch timer;
  timer.Start();

  
  // made it through root, begin oct-tree search ------------------------
 
  
  parent_list->push_back(root);
  std::vector<Hough5DNode*>* temp_sons = new std::vector<Hough5DNode*>();
  
  uint MAXSIZE = 10000000;

  for(int l=0; l<TREE_DEPTH; ++l) {
  
    std::vector<Hough5DNode*>* new_nodes = new std::vector<Hough5DNode*>();
    new_nodes->reserve(MAXSIZE);
      
    std::cout<<"level: "<<l<<std::endl;
    std::cout<<"parent_list size: "<<parent_list->size()<<std::endl;
        
    int counter=0;
    //create new nodes
    for(int n=0; n<parent_list->size(); ++n) {
    
      Hough5DNode* the_node=parent_list->at(n);
      float* sons = the_node->getSonArray();
      
      //hitlist of the mother
      bool* hitList = the_node->getHitList();
      
      //create sons
      for(int s=0; s<32; s++) {
	temp_sons->push_back(new Hough5DNode(sons+5*s, 
					    the_node->getLevel()+1,
					    points));

	Hough5DNode* the_son = temp_sons->at(temp_sons->size()-1);
	//now loop over points for this son and do hit check
	for(int i=0; i<points; i++) {
	  //we don't need to check if mother wasn't hit
	  if(!hitList[i])
	    continue;
	  //std::cout<<"intersection test for son "<<s<<std::endl;
	  Hyperplane5D plane =  hyperplanes[i];
	  plane.testIntersect(the_son);
	}	
	
	if(the_son->getLevel() < 4) {
	  if(the_son->getVote() < THRESHOLD) {
	    //TODO: optimize
	    delete temp_sons->at(temp_sons->size()-1);
	    temp_sons->at(temp_sons->size()-1) = NULL;
	  }
	}
	//dynamic thresholding
	else{
	  if(the_son->getVote() < the_node->getVote()*SCALE) {
	    delete temp_sons->at(temp_sons->size()-1);
	    temp_sons->at(temp_sons->size()-1) = NULL;
	  }
	}
      } //end loop over sons

      //keep sons that passed the test
      for(int s=0; s<32; s++) {
	if(temp_sons->at(s)!=NULL)
	  new_nodes->push_back(temp_sons->at(s));
      }
      temp_sons->clear();
      //erase parent (no longer needed)
      if(l!=TREE_DEPTH-1)
	delete parent_list->at(n);

    } //end loop over parent_list
	  
    if(l!=TREE_DEPTH-1)
      parent_list = new_nodes;
  }

 
  /*  parent_list.push_back(root);
  std::cout<<"Starting Oct-Tree search ..."<<std::endl;

  //int counter = 0;
  while(parent_list.size()>0) {
    Hough5DNode* the_node = (*parent_list.begin());
    //std::cout<<"Starting with mother:"<<std::endl;
    //the_node->print();
    if(the_node->getLevel() >= TREE_DEPTH) {
      solution_list.push_back(the_node);
      parent_list.pop_front();
      continue;
    }
    float* sons = the_node->getSonArray();
    bool* hitList = the_node->getHitList();
    for(int s=0; s<32; s++) {
      
      parent_list.push_back(new Hough5DNode(sons+5*s, 
					    the_node->getLevel()+1,
					    points));
      
      Hough5DNode* the_son = parent_list.back();
      //the_son->setParamSpace(the_node->getParamMins(),
      //		     the_node->getParamMaxs());
      //the_son->print();

      //now loop over points for this son and do hit check
      for(int i=0; i<points; i++) {
	//we don't need to check if mother wasn't hit
	if(!hitList[i])
	  continue;
	Hyperplane5D plane =  hyperplanes[i];
	plane.testIntersect(*the_son);
      }
      if(the_son->getLevel() < 5) {
	if(the_son->getVote() < THRESHOLD) {
	  //std::cout<<"Deleting SON from list"<<std::endl;
	  delete parent_list.back();
	  parent_list.pop_back();
	}
      }
      else{
	if(the_son->getVote() < the_node->getVote()*SCALE) {
	  //std::cout<<"Deleting SON from list"<<std::endl;
	  delete parent_list.back();
	  parent_list.pop_back();
	}

      }
    } //end loop over sons
      //std::cout<<" . . . erasing mother . . . ";
    //(*parent_list.begin())->print();
    delete parent_list.front();
    parent_list.pop_front();
    //std::cout<<"parent_list now: "<<std::endl;
    //std::list<Hough5DNode*>::iterator it;
    //for(it = parent_list.begin(); it!=parent_list.end(); it++) 
    // (*it)->print();
    //std::cout<<" ------------------------------------------ \n"
    //     <<std::endl;
    
  }

  */

  timer.Stop();
  
  
  std::cout<<"There have been "<<parent_list->size()
	   <<" solutions: \n"<<std::endl;
      
  
  //for(int s=0; s<solution_list.size(); s++)  
  // (solution_list[s])->print();
  TFile* file = new TFile("plots.root");
  TH2D* phic = (TH2D*)file->Get("phic_80");
  


  std::vector<TBox*> boxlist;
    
  for(int s=0; s<parent_list->size(); s++) {
    //(solution_list[s])->print();
    float* center = (parent_list->at(s))->getCenter();
    float length = (parent_list->at(s))->getSideLength();
    float x1 = (center[3] - 0.5*length)*(m_Max-m_Min);
    float x2 = (center[3] + 0.5*length)*(m_Max-m_Min);
    float y1 = (center[4] - 0.5*length)*(t_Max-t_Min);
    float y2 = (center[4] + 0.5*length)*(t_Max-t_Min);
    boxlist.push_back(new TBox(x1,y1,x2,y2));
  }
  

  std::cout<<"Side Length of root: "<<root->getSideLength()
	   <<std::endl;

  gStyle->SetPalette(1);
  
  
  TCanvas* canv = new TCanvas();
  //canv->SetGrayscale();
  houghRZ->Draw();


  for(int b=0; b<boxlist.size(); ++b) {
    (boxlist[b])->SetLineColor(kPink+10);
    (boxlist[b])->SetFillStyle(0);
    (boxlist[b])->Draw("l");
  }

  TCanvas* canv2 = new TCanvas();
  phic->Draw("COLZ");

  std::vector<TBox*> boxlist2;
    
  for(int s=0; s<parent_list->size(); s++) {
    //(parent_list->at(s))->print();
    float* center = (parent_list->at(s))->getCenter();
    float length = (parent_list->at(s))->getSideLength();
    float x1 = (center[0] - 0.5*length)*(phi_Max-phi_Min) +90;
    float x2 = (center[0] + 0.5*length)*(phi_Max-phi_Min) +90;
    float y1 = (center[2] - 0.5*length)*(c_Max-c_Min);
    float y2 = (center[2] + 0.5*length)*(c_Max-c_Min);
    boxlist2.push_back(new TBox(x1,y1,x2,y2));
    //boxlist.back()->Print();
  }

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
  for(int s=0; s<parent_list->size(); s++) {
    float* center = (parent_list->at(s))->getCenter();
    float length = (parent_list->at(s))->getSideLength();
    float x1 = (center[0] - 0.5*length)*(phi_Max-phi_Min) +90;
    float x2 = (center[0] + 0.5*length)*(phi_Max-phi_Min) +90;
    float y1 = (center[3] - 0.5*length)*(m_Max-m_Min);
    float y2 = (center[3] + 0.5*length)*(m_Max-m_Min);
    boxlist3.push_back(new TBox(x1,y1,x2,y2));
    //boxlist.back()->Print();
  }

  for(int b=0; b<boxlist3.size(); ++b) {
    (boxlist3[b])->SetLineColor(kPink+10);
    (boxlist3[b])->SetFillStyle(0);
    (boxlist3[b])->Draw("l");
  }
      
  std::cout<<"\n\nFHT took "<<timer.RealTime()<<" seconds to process"
	   <<std::endl;

  gApplication->SetReturnFromRun(true);
  gSystem->Run();
  
  
}
