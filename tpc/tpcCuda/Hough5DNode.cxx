#include "Hough5DNode.h"
#include <cmath>
#include <iostream>
#include "stdlib.h"
#include <set>

//5-dim hypercube; dimensions: (x,y,z,v,w)

Hough5DNode::Hough5DNode(float* center, int level, int nHyperplanes) {
  _nPlanes = nHyperplanes;
  _hitList = (bool*) malloc(_nPlanes*sizeof(bool));
  _center = (float*) malloc(5*sizeof(float));
  _corners = (float*) malloc(32*5*sizeof(float));
  _proj0 = (float*) malloc(2*sizeof(float));
  _proj1 = (float*) malloc(2*sizeof(float));
  _proj2 = (float*) malloc(2*sizeof(float));
  _proj3 = (float*) malloc(2*sizeof(float));
  _proj4 = (float*) malloc(2*sizeof(float));
  _votes = 0;
  _level = level;
  _center[0] = center[0];
  _center[1] = center[1];
  _center[2] = center[2];
  _center[3] = center[3];
  _center[4] = center[4];
  
  _length = 1.f/(pow(2,level));

  int count=0;
  
  for(int x=-1; x<2; x+=2) {
    for(int y=-1; y<2; y+=2) {
      for(int z=-1; z<2; z+=2) {
	for(int v=-1; v<2; v+=2) {
	  for(int w=-1; w<2; w+=2) {
	    _corners[count*5] = _center[0] + 0.5*x*_length;
	    _corners[count*5+1] = _center[1] + 0.5*y*_length;
	    _corners[count*5+2] = _center[2] + 0.5*z*_length;
	    _corners[count*5+3] = _center[3] + 0.5*v*_length;
	    _corners[count*5+4] = _center[4] + 0.5*w*_length;
	    count++;
	  }
	}
      }
    }
  }

  _proj0[0]=_center[0] + 0.5*_length;
  _proj0[1]=_center[0] - 0.5*_length;
  _proj1[0]=_center[1] + 0.5*_length;
  _proj1[1]=_center[1] - 0.5*_length;
  _proj2[0]=_center[2] + 0.5*_length;
  _proj2[1]=_center[2] - 0.5*_length;
  _proj3[0]=_center[3] + 0.5*_length;
  _proj3[1]=_center[3] - 0.5*_length;
  _proj4[0]=_center[4] + 0.5*_length;
  _proj4[1]=_center[4] - 0.5*_length;

    
}

float*
Hough5DNode::getSonArray() {
    
  float* arr = (float*) malloc(32*5*sizeof(float));

  int count=0;
  
  for(int x=-1; x<2; x+=2)
    for(int y=-1; y<2; y+=2)
      for(int z=-1; z<2; z+=2)
	for(int v=-1; v<2; v+=2)
	  for(int w=-1; w<2; w+=2) {
	    arr[count*5] = _center[0] + 0.25*x*_length;
	    arr[count*5+1] = _center[1] + 0.25*y*_length;
	    arr[count*5+2] = _center[2] + 0.25*z*_length;
	    arr[count*5+3] = _center[3] + 0.25*v*_length;
	    arr[count*5+4] = _center[4] + 0.25*w*_length;
	    count++;
	  }
  
  return arr;
}

void
Hough5DNode::print() {
  
  std::cout<<"**Hough5DNode**   level: "<<_level<<"   center: "
	   <<_center[0]<<", "<<_center[1]<<"   votes: "
	   <<_votes<<std::endl; 

}
