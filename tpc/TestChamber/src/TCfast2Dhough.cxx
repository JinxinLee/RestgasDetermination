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
  canv=NULL;
  houghSpace==NULL;
  m_Max =0.2;
  m_Min =-0.2f;
  t_Max = 1.5f;
  t_Min =-1.5f;
  dynamicParSpace=false;
 
  TREE_DEPTH = 5;  //number of space divisions
  THRESHOLD = 4;
}
TCfast2DHough::TCfast2DHough(const TVector3 firstAxis, const TVector3 secondAxis,bool dynamicParSpace_)
  :TCabsHough(firstAxis,secondAxis){
  canv=NULL;
  houghSpace==NULL;
  m_Max =0.2;
  m_Min =-0.2f;
  t_Max = 1.5f;
  t_Min =-1.5f;
  dynamicParSpace=dynamicParSpace_;
 
  TREE_DEPTH = 5;  //number of space divisions
  THRESHOLD = 4;
}
void TCfast2DHough::make(std::vector<TCcluster>& _c){
  for(unsigned int i=0;i<solution_list.size();++i){
    delete solution_list.at(i);
  }
  if(houghSpace!=NULL){
    delete houghSpace;
  }
  for(unsigned int i=0;i<hyperplanes.size();++i){
    delete hyperplanes.at(i);
  }
  delete canv;
  solution_list.clear();
  hyperplanes.clear();
  nClusters=0;
  convert(_c);
  float mins[2] = {m_Min, t_Min};
  float maxs[2] = {m_Max, t_Max};
  for(unsigned int i=0;i<ypHit.size();i++){
    float y=ypHit.at(i);
    float z=zpHit.at(i);
    hyperplanes.push_back(new Hyperplane2D(y,z,i));
    (hyperplanes.back())->setParamSpace(mins, maxs);
    
    nClusters++;
  } 
 int BIN_m=500;
  int BIN_t=500;
  houghSpace = new TH2D("vfg", "hough space", 
			   BIN_m, m_Min, m_Max, BIN_t, t_Min, t_Max);

  float tBinWidth = (t_Max - t_Min)/BIN_t;
  
  for(unsigned int m=0; m<ypHit.size(); ++m) {
    
    float y = ypHit.at(m);
    float z = zpHit.at(m);
            
    for(int t=0; t<BIN_t; ++t) {
      
      float T = (t+0.5)*tBinWidth + t_Min;
      float M = (T-y) / (z*(-1.));
      houghSpace->Fill(M,T);
      if(t%250==0){
	cout<<"clIn "<<m<<" m "<<M<<" t "<<T<<endl;
      }
    }
  }
  doHough();
}
void TCfast2DHough::doHough(){
  std::cout<<"Init root node: "<<std::endl;
  float center[2] = {0.f,0.f};
  Hough2DNode* root = new Hough2DNode(center, 0, nClusters); 
  std::list<Hough2DNode*> parent_list;
  float* corners = root->getCorners();
  for(int i=0; i<8; i++) {
    if(i%2==0)
      std::cout<<std::endl;
    if(i%2==0){
      std::cout<<((corners[i]+0.5)*(m_Max-m_Min)+m_Min)<<"   ";
    }else{
      std::cout<<((corners[i]+0.5)*(t_Max-t_Min)+t_Min)<<"   ";
    }
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
  while(parent_list.size()>0) {
    Hough2DNode* the_node = (*parent_list.begin());
    if(the_node->getLevel() >= TREE_DEPTH) {
      solution_list.push_back(the_node);
      parent_list.pop_front();
      continue;
    }
    float* sons = the_node->getSonArray();
    bool* hitList = the_node->getHitList();
    for(int s=0; s<4; s++) {
      parent_list.push_back(new Hough2DNode(sons+2*s, 
					    the_node->getLevel()+1,
					    nClusters));
      Hough2DNode* the_son = parent_list.back();
      //now loop over points for this son and do hit check
      for(int i=0; i<nClusters; i++) {
	if(!hitList[i]){
	  continue;//we don't need to check if mother wasn't hit
	}
	Hyperplane2D* plane =  hyperplanes[i];
	plane->testIntersect(*the_son);
      }
      if(the_son->getLevel() < 6) {
	if(the_son->getVote() < THRESHOLD) {
	  delete parent_list.back();
	  parent_list.pop_back();
	}
      }
      else{
	if(the_son->getVote() < the_node->getVote()*0.92) {
	  delete parent_list.back();
	  parent_list.pop_back();
	}
      }
    } //end loop over sons
    delete parent_list.front();
    parent_list.pop_front();    
  }
  std::cout<<"There have been "<<solution_list.size()
	   <<" solutions: \n"<<std::endl;
  
}

void TCfast2DHough::draw(bool stop,int _x,int _y,int _w,int _h,TCevent* mcTruth){
  std::vector<TBox*> boxlist;
  for(unsigned int s=0; s<solution_list.size(); s++) {
    float* center = (solution_list[s])->getCenter();
    std::cout<<"maxima "<<(center[0]+0.5)*(m_Max-m_Min)<<" "<<(center[1]+0.5)*(t_Max-t_Min)<<std::endl;
    float length = (solution_list[s])->getSideLength();
    float x1 = (center[0] - 0.5*length+0.5)*(m_Max-m_Min)+m_Min;
    float x2 = (center[0] + 0.5*length+0.5)*(m_Max-m_Min)+m_Min;
    float y1 = (center[1] - 0.5*length+0.5)*(t_Max-t_Min)+t_Min;
    float y2 = (center[1] + 0.5*length+0.5)*(t_Max-t_Min)+t_Min;
    boxlist.push_back(new TBox(x1,y1,x2,y2));
  }
  gStyle->SetPalette(1);
  
  static TRandom r(0);
  char buf[10];
  sprintf(buf,"c%5.5f",r.Uniform());
  canv = new TCanvas(buf,"Hough",_x,_y,_w,_h);
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
  if(canv!=NULL){
    delete canv;
  }
}
void TCfast2DHough::convert(std::vector<TCcluster>& _c){
  TVector3 xp=yp.Cross(zp);
  TMatrixT<double> S(3,3);
  S[0][0]=xp.X();
  S[1][0]=xp.Y();
  S[2][0]=xp.Z();
  S[0][1]=yp.X();
  S[1][1]=yp.Y();
  S[2][1]=yp.Z();
  S[0][2]=zp.X();
  S[1][2]=zp.Y();
  S[2][2]=zp.Z();

  //TMatrixT<double> Stransp = S;
  //Stransp.T();

  clear();
  float yMax=-999999;
  float yMin=999999;
  float zMax=-999999;
  float zMin=999999;
  for(unsigned int i=0;i<_c.size();++i){
    TVector3 hitPrime = S * _c.at(i).posXYZ();
    //xprime isnt needed
    float y=hitPrime.Y();
    float z=hitPrime.Z();
    std::cout<<"hough y "<<y<<" z "<<z<<std::endl;

    ypHit.push_back(y);

    zpHit.push_back(z);
    if(y>yMax){
      yMax=y;
    }
    if(z>zMax){
      zMax=z;
    }
    if(y<yMin){
      yMin=y;
    }
    if(z<zMin){
      zMin=z;
    }
  }
  m_Min=-(yMax-yMin)/(zMax - zMin);
  m_Max=-m_Min;
  t_Min=yMin-m_Max*zMin;
  t_Max=yMax-m_Min*zMax;

}
