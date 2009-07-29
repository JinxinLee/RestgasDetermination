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

  std::set<float> phiCoords;
  std::set<float> thetaCoords;
  std::set<float> cCoords;
  
    
  for(int i=0; i<32; ++i) {
    phiCoords.insert(corners[i*5]);
    thetaCoords.insert(corners[i*5+1]);
    cCoords.insert(corners[i*5+2]);    
  }
  
  //assert(phiCoords.size() == 2 && thetaCoords.size() == 2
  // && cCoords.size() == 2 );
  
  
  std::set<float>::iterator it = cCoords.begin();
  float c1 = (*it) * (_maxs[2] - _mins[2]);
  it++;  
  float c2 = (*it) * (_maxs[2] - _mins[2]);
  
  
  float phi1 = (*phiCoords.begin()) * (_maxs[0] - _mins[0]) + 90;
  float phi2 = (*phiCoords.begin()++) * (_maxs[0] - _mins[0]) + 90;
  float phi_vals[2] = {phi1,phi2};

  float theta1 = (*thetaCoords.begin()) * (_maxs[1] - _mins[1]) +90;
  float theta2 = (*thetaCoords.begin()++) * (_maxs[1] - _mins[1]) + 90;
  float theta_vals[2] = {theta1,theta2};

  std::map<int,int> signs_3D;

  for(int p=0; p<2; p++)
    for(int t=0; t<2; t++) {
      TVector3 n(1.f,0.f,0.f);
      TVector3 x(_params[0],_params[1],_params[2]);
      n.SetPhi(phi_vals[p]);
      n.SetTheta(theta_vals[t]);
      float c = n*x;
      signs_3D[(c1-c > 0) - (c1-c < 0)]++;
      signs_3D[(c2-c > 0) - (c2-c < 0)]++;
      //std::cout<<"c: "<<c<<"   c1: "<<c1<<"   c2: "<<c2<<std::endl;
    }

  if (signs_3D.size() == 2 ) {
    node.setHit(_index);
    node.vote();
    return true;
  }
  else
    return false;
  
}


