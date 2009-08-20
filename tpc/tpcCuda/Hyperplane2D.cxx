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
  
  _coords[0] = coord1;
  _coords[1] = coord2;
  _mins[0] = -1.4f;       //m x=m*z+t
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
      float t_m = -_coords[0]*(mCoords[m_it]*(_maxs[0]-_mins[0])) + _coords[1];
      for(int t_it=0; t_it<2; t_it++) {
	float diff = tCoords[t_it]*(_maxs[1]-_mins[1]) - t_m;
	signs2+=(diff > 0);
      }
    }
    
    if(signs2 == 4 || signs2 == 0) {
      //(*_hitmap)[coords] = false;
      return false; 
    }else {
      //(*_hitmap)[coords] = true;
     node.setHit(_index);
     node.vote();
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
