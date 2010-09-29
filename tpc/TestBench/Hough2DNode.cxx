#include "Hough2DNode.h"
#include <cmath>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

Hough2DNode::Hough2DNode() {

  _hitList = NULL;
  _center = NULL;
  _corners = NULL;
  _proj0 = NULL;
  _proj1 = NULL;
}
Hough2DNode::~Hough2DNode(){
  if(_hitList != NULL)
    free(_hitList); 
  if(_center != NULL)
    free(_center);
  if(_corners != NULL)
    free(_corners); 
  if(_proj0 != NULL)
    free(_proj0); 
  if(_proj1 != NULL)
    free(_proj1); 
}

Hough2DNode::Hough2DNode(const double* center, int level, int nHyperplanes) {
  _nPlanes = nHyperplanes;
  _hitList = (bool*) malloc(_nPlanes*sizeof(bool));
  _center = (double*) malloc(2*sizeof(double));
  _corners = (double*) malloc(8*sizeof(double));
  _proj0 = (double*) malloc(2*sizeof(double));
  _proj1 = (double*) malloc(2*sizeof(double));

  memset(_hitList, 0x00, _nPlanes);
  
  _votes = 0;
  _level = level;
  _center[0] = center[0];
  _center[1] = center[1];
  
  _length = 1.f/(pow(2,level));
  
  int count=0;
  for(int x=-1; x<2; x+=2){
    for(int y=-1; y<2; y+=2) {
      _corners[count*2] = _center[0] + 0.5*x*_length;
      _corners[count*2+1] = _center[1] + 0.5*y*_length;
      count++;
    }
  }
  _proj0[0]=_center[0] + 0.5*_length;
  _proj0[1]=_center[0] - 0.5*_length;
  _proj1[0]=_center[1] + 0.5*_length;
  _proj1[1]=_center[1] - 0.5*_length;
}

const double*  
Hough2DNode::getSonArray() const {   //return centers of sons
  double* arr = (double*) malloc(8*sizeof(double));
  int count=0;
  for(int x=-1; x<2; x+=2)
    for(int y=-1; y<2; y+=2) {
      arr[count*2] = _center[0] + 0.25*x*_length;
      arr[count*2+1] = _center[1] + 0.25*y*_length;
      count++;
    }
  return arr;
}

/*
void
Hough2DNode::print() {
  
  std::cout<<"**Hough2DNode**   level: "<<_level<<"   center: "
	   <<_center[0]<<", "<<_center[1]<<"   votes: "
	   <<_votes<<std::endl; 

}
*/

void Hough2DNode::removeHit(int j) {
  if(j>=_nPlanes){
    std::cout<<"removing hit "<<j<<" nplanes "<<_nPlanes<<std::endl;
  }
  
  _hitList[j] = false; 
  _votes--; 
}



