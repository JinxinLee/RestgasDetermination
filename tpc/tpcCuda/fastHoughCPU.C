#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TVector3.h"
#include "PndTpcCluster.h"
#include "PndTpcPoint.h"
#include "TClonesArray.h"
#include "TH3D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TString.h"
#include "TVectorD.h"

#include <cmath>
#include <vector>
#include <map>
#include <string>

#include "Hough2DNode.h"


int main() {



  //READ data and CREATE histograms and data containers ---------------------

  int TREE_DEPTH = 8;  //number of space divisions
  int THRESHOLD = 45;
 

  bool CUT_CHAMBER=true;   //only collect hits with x>0;
    
  unsigned int EVENT=6;
  double RIEMANNSCALING=40;

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
  
  double r, phi, z, x_R, y_R, z_R;

  //list of riemann points
  std::vector<TVector3> riemannList;
  std::vector<TVector3> riemannListRZ;
  

  double m_Max = 1.;
  double m_Min = -1.;
  double t_Max = 5.;
  double t_Min = -5.;
  double phi_Min = 0;
  double phi_Max = 180;
  

  //RIEMANN TRAFO ON CLUSTERS -----------------------------------------------
  //loop over clusters
  for(int cl=0; cl<nClusters; ++cl) {
    TVector3 pos = ((PndTpcCluster*)_clusters->At(cl))->pos();
    
    if(CUT_CHAMBER && pos.X() < 0.)
      continue;

    riemannListRZ.push_back(TVector3(pos.Perp(), 0., pos.Z()));

    // r = pos.Perp()/RIEMANNSCALING;
    //     phi = pos.Phi();
    
    //     x_R = r * cos(phi)/(1+r*r);
    //     y_R = r * sin(phi)/(1+r*r);
    //     z_R = r*r/(1+r*r);
    
    //     riemannList.push_back(TVector3(x_R,y_R,z_R));
  }
  
  
  // FAST HOUGH SEARCH -----------------------------------------------------

  unsigned int points = riemannListRZ.size();

  //init rood node
  std::cout<<"Init root node: "<<std::endl;
  float center[2] = {0.f,0.f};
  Hough2DNode* root = new Hough2DNode(center, 0, points); 

  std::vector<Hough2DNode*> parent_list;
  std::vector<Hough2DNode*> solution_list;
  
  for(int i=0; i<points; i++) {
    float R = (riemannListRZ[i]).X();
    float Z = (riemannListRZ[i]).Z();

    //std::cout<<"R: "<<R<<"   Z: "<<Z<<std::endl;
    
    float* corners = root->getCorners();
    std::map<int,int> signs; //store signs of "g(corner)-corner"
    
    //brute force (optimize: only calculate g once per m)
    for(int k=0; k<4; k++) {
      float m = corners[k*2];
      float t = corners[k*2+1];
      //scale m to the real parameter space
      float t_m = -R*(m*(m_Max-m_Min)) + Z;
      //std::cout<<"      t_m: "<<t_m<<std::endl;
      float diff = t*(t_Max-t_Min) - t_m;
      //std::cout<<"      diff: "<<diff<<std::endl;
      int sign = (diff > 0) - (diff < 0);
      signs[sign]++;      
    }

    if(signs.size() == 2) {
      root->setHit(i);
      root->vote();
    }
  }

  std::cout << "\nThere have been "<<root->getVote()
	    <<" of "<<points<<" hyperplane crossings in root"<<std::endl;
  if(root->getVote() < THRESHOLD)
    return 0;
  
  //float* sons = root->getSonArray();
  //for(int k=0; k<4; k++) {
  //    float m = sons[k*2];
  //    float t = sons[k*2+1];
  //    std::cout<<" m: "<<m<<"   t: "<<t<<std::endl;
  //}

  
  // made it through root, begin oct-tree search ------------------
  
  parent_list.push_back(root);
  std::cout<<"Starting Oct-Tree search ..."<<std::endl;

  //int counter = 0;
  while(parent_list.size()>0) {
    Hough2DNode* the_node = parent_list[0];
    std::cout<<"Starting with mother:"<<std::endl;
    the_node->print();
    if(the_node->getLevel() > TREE_DEPTH) {
      solution_list.push_back(the_node);
      parent_list.erase(parent_list.begin());
      continue;
    }
    float* sons = the_node->getSonArray();
    bool* hitList = the_node->getHitList();
    for(int s=0; s<4; s++) {
      float m_c = sons[s*2];
      float t_c = sons[s*2+1];
      float center[2] = {m_c, t_c};
      std::cout<<" . . . creating son at m_c: "<<m_c<<"    t_c: "
	       <<t_c<<std::endl;
      parent_list.push_back(new Hough2DNode(center, the_node->getLevel()+1,
					    points));
      
      Hough2DNode* the_son = parent_list[parent_list.size()-1];
      //the_son->print();

      //now loop over points for this son and do hit check
      for(int i=0; i<points; i++) {
	//we don't need to check if mother wasn't hit
	if(!hitList[i])
	  continue;
	
	float R = (riemannListRZ[i]).X();
	float Z = (riemannListRZ[i]).Z();
	
	float* corners = the_son->getCorners();
	std::map<int,int> signs; //store signs of "g(corner)-corner"
	
	//brute force (optimize: only calculate g once per m)
	for(int k=0; k<4; k++) {
	  float m = corners[k*2];
	  float t = corners[k*2+1];
	  //scale m to the real parameter space
	  float t_m = -R*(m*(m_Max-m_Min)) + Z;
	  float diff = t*(t_Max-t_Min) - t_m;
	  int sign = (diff > 0) - (diff < 0);
	  signs[sign]++;      
	}
	if(signs.size() == 2) {
	  the_son->setHit(i);
	  the_son->vote();
	}
      }
      //std::cout<<"           son got vote of "<<the_son->getVote()
      //       <<std::endl;	
      if(the_son->getVote() < THRESHOLD) {
	//std::cout<<"Deleting SON from list"<<std::endl;
	parent_list.erase(parent_list.end()-1);
      }
            
    } //end loop over sons
    //std::cout<<" . . . erasing mother . . . ";
    (*parent_list.begin())->print();
    parent_list.erase(parent_list.begin());
    //std::cout<<"parent_list now: "<<std::endl;
    //for(int p=0; p<parent_list.size(); p++) 
    //  (parent_list[p])->print();
    //std::cout<<" ------------------------------------------ \n"<<std::endl;
   
  }
    
  std::cout<<"There have been "<<solution_list.size()
	   <<" solutions: \n"<<std::endl;
  for(int s=0; s<solution_list.size(); s++) {
    (solution_list[s])->print();
  }
  

  std::cout<<"Side Length of root: "<<root->getSideLength()<<std::endl;

  
}
