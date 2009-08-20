#include <vector>
#include <list>
#include <iostream>

#include <TRandom.h>
#include <TH2D.h>
#include <TBox.h>
#include <TCevent.h>
#include <TCcluster.h>
#include <TVector.h>
#include <TStyle.h>
#include <TApplication.h>
#include <TSystem.h>

#include "TCabsHough.h"
#include "TCfast2Dhough.h"
#include "Hyperplane2D.h"
#include "Hough2DNode.h"

using std::cout;
using std::endl;
TCfast2DHough::TCfast2DHough(const TVector3 firstAxis, const TVector3 secondAxis)
  :TCabsHough(firstAxis,secondAxis){
}
void TCfast2DHough::make(std::vector<TCcluster>& _c){
  convert(_c);
  m_Max =0.5f;
  m_Min =-1.4f;
  t_Max = 5.f;
  t_Min =-1.f;
  float mins[2] = {m_Min, t_Min};
  float maxs[2] = {m_Max, t_Max};

  for(unsigned int i=0;i<ypHit.size();i++){
    hyperplanes.push_back(new Hyperplane2D(ypHit.at(i),zpHit.at(i),i));
    (hyperplanes.back())->setParamSpace(mins, maxs);
    nClusters++;
  }

  int BIN_m=500;
  int BIN_t=500;
  houghSpace = new TH2D("vfg", "hough space", 
			   BIN_m, m_Min, m_Max, BIN_t, t_Min, t_Max);

  double tBinWidth = (t_Max - t_Min)/BIN_t;
  
  for(unsigned int m=0; m<ypHit.size(); ++m) {
    
    double y = ypHit.at(m);
    double z = zpHit.at(m);
            
    for(int t=0; t<BIN_t; ++t) {
      
      double T = (t+0.5)*tBinWidth + t_Min;
      double M = (T-z) / (y*(-1.));
      houghSpace->Fill(M,T);
      if(t%100==0){
	cout<<"m "<<M<<" t "<<T<<endl;
      }
    }
  }
  doHough();
}
void TCfast2DHough::doHough(){
  
  int TREE_DEPTH = 5;  //number of space divisions
  int THRESHOLD = 4;

  
  //float mins[2] = {m_Min, t_Min};
  //float maxs[2] = {m_Max, t_Max};
 
  //init root node
  std::cout<<"Init root node: "<<std::endl;
  float center[2] = {0.f,0.f};
  Hough2DNode* root = new Hough2DNode(center, 0, nClusters); 

  //setting up parameter space
  //root->setParamSpace(mins, maxs);
  
  std::list<Hough2DNode*> parent_list;
  float* corners = root->getCorners();
  for(int i=0; i<8; i++) {
    if(i%2==0)
      std::cout<<std::endl;

    std::cout<<corners[i]<<"   ";
  }
  for(int i=0; i<nClusters; i++) {
    Hyperplane2D* plane = hyperplanes.at(i);
    plane->testIntersect(*root);
  }


  std::cout << "\nThere have been "<<root->getVote()
	    <<" of "<<nClusters<<" hyperplane crossings in root"
	    <<std::endl;
  if(root->getVote() < THRESHOLD){
    cout<<"top noe not enough votes"<<endl;
  }

  parent_list.push_back(root);
  std::cout<<"Starting Oct-Tree search ..."<<std::endl;

  
  //int counter = 0;
  while(parent_list.size()>0) {
    Hough2DNode* the_node = (*parent_list.begin());
    //std::cout<<"Starting with mother:"<<std::endl;
    //the_node->print();
    if(the_node->getLevel() >= TREE_DEPTH) {
      solution_list.push_back(the_node);
      parent_list.pop_front();
      continue;
    }
    float* sons = the_node->getSonArray();
    bool* hitList = the_node->getHitList();
    for(int s=0; s<8; s++) {
      parent_list.push_back(new Hough2DNode(sons+2*s, 
					    the_node->getLevel()+1,
					    nClusters));
      Hough2DNode* the_son = parent_list.back();
      //the_son->setParamSpace(the_node->getParamMins(),
      //		     the_node->getParamMaxs());
      //the_son->print();
      
      //now loop over points for this son and do hit check
      for(int i=0; i<nClusters; i++) {
	//we don't need to check if mother wasn't hit
	if(!hitList[i])
	  continue;
	Hyperplane2D* plane =  hyperplanes[i];
	plane->testIntersect(*the_son);
      }
      if(the_son->getLevel() < 6) {
	if(the_son->getVote() < THRESHOLD) {
	  //std::cout<<"Deleting SON from list"<<std::endl;
	  delete parent_list.back();
	  parent_list.pop_back();
	}
      }
      else{
	if(the_son->getVote() < the_node->getVote()*0.92) {
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
    //std::list<Hough2DNode*>::iterator it;
    //for(it = parent_list.begin(); it!=parent_list.end(); it++) 
    // (*it)->print();
    //std::cout<<" ------------------------------------------ \n"
    //     <<std::endl;
    
  }
  std::cout<<"There have been "<<solution_list.size()
	   <<" solutions: \n"<<std::endl;
  
}

void TCfast2DHough::draw(bool stop,int _x,int _y,int _w,int _h,TCevent* mcTruth){
  //TFile* file = new TFile("plots.root");
  //TH2D* phic = (TH2D*)file->Get("phic_80");
  


  std::vector<TBox*> boxlist;
    
  for(unsigned int s=0; s<solution_list.size(); s++) {
    //(solution_list[s])->print();
    float* center = (solution_list[s])->getCenter();
    std::cout<<"maxima "<<center[0]<<" "<<center[1]<<std::endl;
    float length = (solution_list[s])->getSideLength();
    float x1 = (center[0] - 0.5*length)*(m_Max-m_Min);
    float x2 = (center[0] + 0.5*length)*(m_Max-m_Min);
    float y1 = (center[1] - 0.5*length)*(t_Max-t_Min);
    float y2 = (center[1] + 0.5*length)*(t_Max-t_Min);
    boxlist.push_back(new TBox(x1,y1,x2,y2));
  }
  

  //  std::cout<<"Side Length of root: "<<root->getSideLength()
  //   <<std::endl;

  gStyle->SetPalette(1);

  static TRandom r(0);
  char buf[10];
  sprintf(buf,"c%5.5f",r.Uniform());
  TCanvas* canv = new TCanvas(buf,"Hough",_x,_y,_w,_h);
  //canv->SetGrayscale();
  houghSpace->Draw();


  for(unsigned int b=0; b<boxlist.size(); ++b) {
    (boxlist[b])->SetLineColor(kPink+10);
    (boxlist[b])->SetFillStyle(0);
    (boxlist[b])->Draw("l");
  }
  if(stop){
    gApplication->SetReturnFromRun(true);
    gSystem->Run();
  }
  
}
TCfast2DHough::~TCfast2DHough(){
  delete houghSpace;
  for(unsigned int i =0; i<solution_list.size();++i){
    delete solution_list.at(i);
  }
  solution_list.clear();

  for(unsigned int i =0; i<hyperplanes.size();++i){
    delete hyperplanes.at(i);
  }
  hyperplanes.clear();
  
}
