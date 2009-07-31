#include "Hyperplane5D.h"
#include "TVector3.h"
#include "TMath.h"

#include <cmath>
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
  
  _mins[0] = 0.f;       //phi
  _mins[1] = 70.f;       //theta
  _mins[2] = -1.f;      //c
  _mins[3] = -1.f;      //m
  _mins[4] = -5.f;      //t
    
  _maxs[0] = 180.f;
  _maxs[1] = 80.f;
  _maxs[2] = 1.0f;
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
  
  float* mCoords = node.getProjection3();
  float* tCoords = node.getProjection4();
    
  //std::map<int,int> signs; //store signs of "g(corner)-corner"
   
  //TODO: optimize
  int signs2 = 0;
  for(int m_it=0; m_it<2; m_it++) {
    float t_m = -_params[3]*(mCoords[m_it]*(m_Max-m_Min)) + _params[4];
    for(int t_it=0; t_it<2; t_it++) {
      float diff = tCoords[t_it]*(t_Max-t_Min) - t_m;
      signs2+=(diff > 0);
    }
  }
 
  if(signs2 == 4 || signs2 == 0) 
    return false; //we don't need to proceed
  //else {
  //  node.setHit(_index);
  //  node.vote();
  //  return true;   
  //}

  
  //3D test ----------------------------------------------------

  float* phiCoords = node.getProjection0();
  float* thetaCoords = node.getProjection1();
  float* cCoords = node.getProjection2();
    
  float c1 = cCoords[0] * (_maxs[2] - _mins[2]);
  float c2 = cCoords[1] * (_maxs[2] - _mins[2]);

  float phi1 = phiCoords[0] * (_maxs[0] - _mins[0]) + 90;
  float phi2 = phiCoords[1] * (_maxs[0] - _mins[0]) + 90;
  float phi_vals[2] = {phi1,phi2};

  float theta1 = (thetaCoords[0] + 0.5)* (_maxs[1] - _mins[1]) +_mins[1];
  float theta2 = (thetaCoords[1] + 0.5)* (_maxs[1] - _mins[1]) +_mins[1];
  float theta_vals[2] = {theta1,theta2};

  int sign=0;
  int lastSign=0;
  int count=0;
  bool hit=false;
  for(int p=0; p<2; p++)
    for(int t=0; t<2; t++) {
      TVector3 n(1.f,0.f,0.f);
      TVector3 x(_params[0],_params[1],_params[2]);
      n.SetMagThetaPhi(1., theta_vals[t]*TMath::Pi()/180,
		       phi_vals[p]*TMath::Pi()/180);
      float c = n*x;
      sign=(int)(c1-c > 0);
      if(count>0 && (lastSign!=sign)) {
	hit=true;
	break;
      }
      lastSign=sign;
      sign=(int)(c2-c > 0);
      if(lastSign!=sign) {
	hit=true;
	break;
      }       
      lastSign=sign;
      count++;
    }
  
  if (hit) {
    node.setHit(_index);
    node.vote();
    return true;
  }
  else
    return false;
  
}


