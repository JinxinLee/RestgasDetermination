#include "Hough2DNode.h"
#include <cmath>
#include <iostream>
#include "stdlib.h"



Hough2DNode::Hough2DNode(float* center, int level, int nHyperplanes) {
  _nPlanes = nHyperplanes;
  _hitList = (bool*) malloc(_nPlanes*sizeof(bool));
  _center = (float*) malloc(2*sizeof(float));
  _corners = (float*) malloc(8*sizeof(float));
  _votes = 0;
  _level = level;
  _center[0] = center[0];
  _center[1] = center[1];
  
  _length = 1.f/(pow(2,level));
  float x1 = _center[0] - 0.5*_length;
  float x2 = _center[0] + 0.5*_length;
  float y1 = _center[1] - 0.5*_length;
  float y2 = _center[1] + 0.5*_length;
  _corners[0] = x1;
  _corners[1] = y1;
  _corners[2] = x1;
  _corners[3] = y2;
  _corners[4] = x2;
  _corners[5] = y1;
  _corners[6] = x2;
  _corners[7] = y2;
   
}

float*
Hough2DNode::getSonArray() {
  float x1 = _center[0] - 0.25f*_length;
  float x2 = _center[0] + 0.25f*_length;
  float y1 = _center[1] - 0.25f*_length;
  float y2 = _center[1] + 0.25f*_length;
  
  float* arr = (float*) malloc(8*sizeof(float));
  arr[0] = x1;
  arr[1] = y1;
  arr[2] = x1;
  arr[3] = y2;
  arr[4] = x2;
  arr[5] = y1;
  arr[6] = x2;
  arr[7] = y2;
  
  return arr;
}

void
Hough2DNode::print() {
  
  std::cout<<"**Hough2DNode**   level: "<<_level<<"   center: "
	   <<_center[0]<<", "<<_center[1]<<"   votes: "
	   <<_votes<<std::endl; 

}


