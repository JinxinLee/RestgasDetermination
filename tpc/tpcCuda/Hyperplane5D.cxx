#include "Hyperplane5D.h"
#include "TVector3.h"

#include <cmath>
#include <set>
#include <map>
#include <assert.h>
#include <cstdlib>


Hyperplane5D::Hyperplane5D(PndTpcCluster* cl, int index) {

  _index = index;
  _RIEMANNSCALING = 40; //default value
  _params = (float*) malloc(5*sizeof(float));
  _mins = (float*) malloc(5*sizeof(float));
  _maxs = (float*) malloc(5*sizeof(float));

  TVector3 pos = cl->pos();
  float r = (float) pos.Perp();
  float phi = (float) pos.Phi();
  
  _params[3] = r;
  _params[4] = (float) pos.Z();
  r = r/_RIEMANNSCALING;
  _params[0] = r * cos(phi)/(1+r*r);
  _params[1] = r * sin(phi)/(1+r*r);
  _params[2] = r*r/(1+r*r); 
  
  _mins[0] = 0.f;
  _mins[1] = 0.f;
  _mins[2] = -1.f;
  _mins[3] = -1.f;
  _mins[4] = -5.f;
    
  _maxs[0] = 180.f;
  _maxs[1] = 180.f;
  _maxs[2] = 1.f;
  _maxs[3] = 1.f;
  _maxs[4] = 5.f;  
    
}


bool
Hyperplane5D::testIntersect(Hough5DNode& node) {

  float m_Max = 1.f;
  float m_Min = -1.f;
  float t_Max = 5.f;
  float t_Min = -5.f;
  
  float* corners = node.getCorners();
  //first test in R-Z Hough space ---------------------------
  
  std::set<float> mCoords;
  std::set<float> tCoords;
  
  float t_ms[2];
  
  for(int i=0; i<32; ++i) {
    mCoords.insert(corners[i*5+3]);
    tCoords.insert(corners[i*5+4]);
  }
  
  std::set<float>::iterator m_it;

  // for(m_it=mCoords.begin(); m_it!=mCoords.end(); m_it++) 
  //     std::cout<<(*m_it)<<"  ";
  //   std::cout<<_index<<std::endl;
  
  assert(mCoords.size() == 2 && tCoords.size() == 2);
  
  std::map<int,int> signs; //store signs of "g(corner)-corner"
  
  
  int count=0;
  for(m_it=mCoords.begin(); m_it!=mCoords.end(); m_it++) {
    float m = (*m_it);
    t_ms[count] = -_params[3]*(m*(m_Max-m_Min)) + _params[4];
  }
  
  for(int y=0; y<2; y++) {
    std::set<float>::iterator t_it;   //ha ha
    for(t_it=tCoords.begin(); t_it!=tCoords.end(); t_it++) {
      float diff = (*t_it) - t_ms[y];
      signs[(diff > 0) - (diff < 0)]++;
    }
  }
 
  if(signs.size() != 2) 
    return false; //we don't need to proceed
  
  //3D test ----------------------------------------------------



  else {
    node.setHit(_index);
    node.vote();
    return true;
  }
  
}


