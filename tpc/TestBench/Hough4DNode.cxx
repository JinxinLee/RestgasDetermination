#include "Hough4DNode.h"
#include <cmath>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

Hough4DNode::Hough4DNode() {

  _hitList = NULL;
  _center = NULL;
  _corners = NULL;
  _proj0 = NULL;
  _proj1 = NULL;
  _proj2 = NULL;
  _proj3 = NULL;
  _sons = NULL;
  
}
Hough4DNode::~Hough4DNode(){
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
  if(_proj2 != NULL)
    free(_proj2); 
  if(_proj3 != NULL)
    free(_proj3); 
  if(_sons!=NULL)
    free(_sons);  
}

Hough4DNode::Hough4DNode(const double* center, int level, 
			 int nHyperplanes) {
  _nPlanes = nHyperplanes;
  _hitList = (bool*) malloc(_nPlanes*sizeof(bool));
  _center = (double*) malloc(4*sizeof(double));
  _corners = (double*) malloc(16*4*sizeof(double));
  _proj0 = (double*) malloc(2*sizeof(double));
  _proj1 = (double*) malloc(2*sizeof(double));
  _proj2 = (double*) malloc(2*sizeof(double));
  _proj3 = (double*) malloc(2*sizeof(double));

  _sons = (double*) malloc(16*4*sizeof(double));
  
  memset(_hitList, 0x00, _nPlanes);
  memset(_sons, 0x00, 16*4);
  
  _votes = 0;
  _level = level;
  _center[0] = center[0];
  _center[1] = center[1];
  _center[2] = center[2];
  _center[3] = center[3];
  
  _length = 1.f/(pow(2,level));

  unsigned int count = 0;
  for(int x=-1; x<2; x+=2) {
    for(int y=-1; y<2; y+=2) {
      for(int z=-1; z<2; z+=2) {
	for(int v=-1; v<2; v+=2) {
	  _corners[count*4] = _center[0] + 0.5*x*_length;
	  _corners[count*4+1] = _center[1] + 0.5*y*_length;
	  _corners[count*4+2] = _center[2] + 0.5*z*_length;
	  _corners[count*4+3] = _center[3] + 0.5*v*_length;
	  count++;
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
 
}

const double*  
Hough4DNode::getSonArray() const {   //return centers of sons
  int count=0;
  for(int x=-1; x<2; x+=2)
    for(int y=-1; y<2; y+=2)
      for(int z=-1; z<2; z+=2)
	for(int v=-1; v<2; v+=2) {
	  _sons[count*4] = _center[0] + 0.25*x*_length;
	  _sons[count*4+1] = _center[1] + 0.25*y*_length;
	  _sons[count*4+2] = _center[2] + 0.25*z*_length;
	  _sons[count*4+3] = _center[3] + 0.25*v*_length;
	  count++;
	}
   
   return _sons;
}

/*
void
Hough4DNode::print() {
  
  std::cout<<"**Hough4DNode**   level: "<<_level<<"   center: "
	   <<_center[0]<<", "<<_center[1]<<"   votes: "
	   <<_votes<<std::endl; 

}
*/

void Hough4DNode::removeHit(int j) {
  if(j>=_nPlanes){
    std::cout<<"removing hit "<<j<<" nplanes "<<_nPlanes<<std::endl;
  }
  
  _hitList[j] = false; 
  _votes--; 
}

