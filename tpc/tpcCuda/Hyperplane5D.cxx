//-----------------------------------------------------------
//
// Description:
//      Representation of a Hyperplane in 5-dimensional
//      Hough-Space.
//      -- implementation
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

#include "Hyperplane5D.h"
#include "TMath.h"

#include <cmath>
#include <assert.h>
#include <cstdlib>


Hyperplane5D::Hyperplane5D(PndTpcCluster* cl, int index) {

  _index = index;
  _RIEMANNSCALING = 40; //default value
  _coords = (float*) malloc(5*sizeof(float));
  _mins = (float*) malloc(5*sizeof(float));
  _maxs = (float*) malloc(5*sizeof(float));

  TVector3 pos = cl->pos();
  float r = (float) pos.Perp();
  float phi = (float) pos.Phi();
  
  _coords[3] = r;
  _coords[4] = (float) pos.Z();
  r = r/_RIEMANNSCALING;
  _coords[0] = r * cos(phi)/(1+r*r);
  _coords[1] = r * sin(phi)/(1+r*r);
  _coords[2] = r*r/(1+r*r); 
  
  _mins[0] = 0.f;       //phi
  _mins[1] = 60.f;       //theta
  _mins[2] = -0.1f;      //c
  _mins[3] = -1.f;      //m
  _mins[4] = -5.f;      //t
    
  _maxs[0] = 180.f;
  _maxs[1] = 100.f;
  _maxs[2] = 0.1f;
  _maxs[3] = 1.f;
  _maxs[4] = 5.f;  
    
}


bool
Hyperplane5D::testIntersect(Hough5DNode& node) {

  int level = node.getLevel();
  
  float PI_180 = 3.14159/180;

  
  //first test in R-Z Hough space ---------------------------
  
  float* mCoords = node.getProjection3();
  float* tCoords = node.getProjection4();
    
  bool found2D;
  bool skip=false;
  
    
  if(!skip) {
    //TODO: optimize
    int signs2 = 0;
    for(int m_it=0; m_it<2; m_it++) {
      float t_m = -_coords[3]*(mCoords[m_it]*(_maxs[3]-_mins[3])) + _coords[4];
      for(int t_it=0; t_it<2; t_it++) {
	float diff = tCoords[t_it]*(_maxs[4]-_mins[4]) - t_m;
	signs2+=(diff > 0);
      }
    }
    
    if(signs2 == 4 || signs2 == 0) {
      //(*the_map)[coords] = false;
      return false; } //we don't need to proceed
    // else {
      //(*the_map)[coords] = true;
    //node.setHit(_index);
    //node.vote();
    //return true;   
    //}
  }
  
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
  for(int p=0; p<2&&!hit; p++)
    for(int t=0; t<2; t++) {
      
      float x[3] = {_coords[0],_coords[1],_coords[2]};
      float n[3] = {sin(theta_vals[t]*PI_180)*cos(phi_vals[p]*PI_180),
		    sin(theta_vals[t]*PI_180)*sin(phi_vals[p]*PI_180),
		    cos(theta_vals[t]*PI_180)};
      
      //float c = n*x;
      float c = x[0]*n[0] + x[1]*n[1] + x[2]*n[2];
      
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

//       sign+=(int)(c1-c > 0);
//       sign+=(int)(c2-c > 0);

//     }

//   if(sign < 8 &&  sign > 0) {    
//     node.setHit(_index);
//     node.vote();
    
//     return true;
//   }
//   else
//     return false;
    
    
}


void 
Hyperplane5D::setParamSpace(float* mins, float* maxs) {
  _mins = mins;
  _maxs = maxs;  
}    
