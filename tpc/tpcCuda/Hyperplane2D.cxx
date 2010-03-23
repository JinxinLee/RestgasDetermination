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

#include "Hyperplane2D.h"
#include "TMath.h"

#include <cmath>
#include <assert.h>
#include <cstdlib>


Hyperplane2D::Hyperplane2D(float coord1, float coord2, int index) {

  _index = index;
  _coords = (float*) malloc(2*sizeof(float));
  _mins = (float*) malloc(2*sizeof(float));
  _maxs = (float*) malloc(2*sizeof(float));
  
  _coords[0] = coord1; //mostly x/y
  _coords[1] = coord2; //mostly z
  //  x=m*z+t
  _mins[0] = -1.4f;       //m 
  _mins[1] = -1.f;       //t
     
  _maxs[0] = 0.4f;
  _maxs[1] = 5.f;
     
}


bool 
Hyperplane2D::testIntersect(Hough2DNode& node) {

  float* mCoords = node.getProjection0();
  float* tCoords = node.getProjection1();
    
  //  bool found2D;
  bool skip=false;
  
    
  if(!skip) {
    //TODO: optimize
    int signs2 = 0;
    for(int m_it=0; m_it<2; m_it++) {
      float m=(mCoords[m_it]+0.5)*(_maxs[0]-_mins[0])+_mins[0];
      float t_m = -_coords[1]*m + _coords[0];  //t= -m*z+(x or y)
      for(int t_it=0; t_it<2; t_it++) {
	float t=(tCoords[t_it]+0.5)*(_maxs[1]-_mins[1])+_mins[1];
	float diff =  t- t_m;
	signs2+=(diff > 0);
      }
    }
    
    if(signs2 == 4 || signs2 == 0) 
      return false; 
    
    else {
      node.setHit(_index);
      node.vote();
      return true;   
    }
  }
  
  return false;
}

bool 
Hyperplane2D::testIntersect(Hough2DNode* node) {

  float* mCoords = node->getProjection0();
  float* tCoords = node->getProjection1();
    
  //  bool found2D;
  bool skip=false;
  
    
  if(!skip) {
    //TODO: optimize
    int signs2 = 0;
    for(int m_it=0; m_it<2; m_it++) {
      float m=(mCoords[m_it]+0.5)*(_maxs[0]-_mins[0])+_mins[0];
      float t_m = -_coords[1]*m + _coords[0];  //t= -m*z+(x or y)
      for(int t_it=0; t_it<2; t_it++) {
	float t=(tCoords[t_it]+0.5)*(_maxs[1]-_mins[1])+_mins[1];
	float diff =  t- t_m;
	signs2+=(diff > 0);
      }
    }
    
    if(signs2 == 4 || signs2 == 0) 
      return false; 
    
    else {
      node->setHit(_index);
      node->vote();
      return true;   
    }
  }
  
  return false;
}


void 
Hyperplane2D::setParamSpace(float* mins, float* maxs) {
  _mins = mins;
  _maxs = maxs;  
}    
