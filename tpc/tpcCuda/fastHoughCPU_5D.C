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

#include <cmath>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <cstdlib>

#include "Hough5DNode.h"
#include "Hyperplane5D.h"


int main(int argc, char** argv) {

  extern char *optarg;
  int c;
  
  int TREE_DEPTH = 6;  //number of space divisions
  int THRESHOLD = 40;
 

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
    
  unsigned int EVENT=6;
  
  TString dir = "../../DATA/";
  TString project = "Test10";

  project=dir+project;
  TString mc_filename = project+".mc.root";
  TString reco_filename = project+".reco.root";

  
  TFile* reco_file =  new TFile(reco_filename);
  TTree* reco_tree = (TTree*)reco_file->Get("cbmsim");

  TFile* mc_file =  new TFile(mc_filename);
  TTree* mc_tree = (TTree*)mc_file->Get("cbmsim");

  TClonesArray* _clusters = new TClonesArray("PndTpcCluster");
  reco_tree->SetBranchAddress("PndTpcCluster", &_clusters);
  reco_tree->GetEntry(EVENT);
 
  TClonesArray* _points = new TClonesArray("PndTpcPoint");
  mc_tree->SetBranchAddress("PndTpcPoint", &_points);
  mc_tree->GetEntry(EVENT);
  
  int nClusters = _clusters->GetEntriesFast();
  int nPoints = _points->GetEntriesFast();
  
  double m_Max = 1.;
  double m_Min = -1.;
  double t_Max = 5.;
  double t_Min = -5.;
  double phi_Min = 0;
  double phi_Max = 180;

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

  std::list<Hough5DNode*> parent_list;
  std::vector<Hough5DNode*> solution_list;
  
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
  
    
  // made it through root, begin oct-tree search ------------------------
  
  parent_list.push_back(root);
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
      //the_son->print();

      //now loop over points for this son and do hit check
      for(int i=0; i<points; i++) {
	//we don't need to check if mother wasn't hit
	if(!hitList[i])
	  continue;
	Hyperplane5D plane =  hyperplanes[i];
	plane.testIntersect(*the_son);
      }
	
      if(the_son->getVote() < THRESHOLD) {
	//std::cout<<"Deleting SON from list"<<std::endl;
	parent_list.pop_back();
      }
      
    } //end loop over sons
      //std::cout<<" . . . erasing mother . . . ";
    //(*parent_list.begin())->print();
    parent_list.pop_front();
    //std::cout<<"parent_list now: "<<std::endl;
    //std::list<Hough5DNode*>::iterator it;
    //for(it = parent_list.begin(); it!=parent_list.end(); it++) 
    // (*it)->print();
    //std::cout<<" ------------------------------------------ \n"
    //     <<std::endl;
    
  }
    
  std::cout<<"There have been "<<solution_list.size()
	   <<" solutions: \n"<<std::endl;

  std::vector<TBox*> boxlist;
    
  for(int s=0; s<solution_list.size(); s++) {
    (solution_list[s])->print();
    float* center = (solution_list[s])->getCenter();
    float length = (solution_list[s])->getSideLength();
    float x1 = (center[0] - 0.5*length)*(m_Max-m_Min);
    float x2 = (center[0] + 0.5*length)*(m_Max-m_Min);
    float y1 = (center[1] - 0.5*length)*(t_Max-t_Min);
    float y2 = (center[1] + 0.5*length)*(t_Max-t_Min);
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

    
  gApplication->SetReturnFromRun(true);
  gSystem->Run();
  
  
}
