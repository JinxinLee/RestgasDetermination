//-----------------------------------------------------------
//
// Description:
//      Representation of a 5-dimensional cube for the
//      FHT algorithm
//      -- implementation
//      
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

#include "Hough5DNode.h"
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

//5-dim hypercube; dimensions: (x,y,z,v,w)

Hough5DNode::Hough5DNode() {

  _hitList = NULL;
  _center = NULL;
  _corners = NULL;
  _proj0 = NULL;
  _proj1 = NULL;
  _proj2 = NULL;
  _proj3 = NULL;
  _proj4 = NULL;
  _sonsAllocd=false;

}



Hough5DNode::Hough5DNode(float* center, uint level, uint nHyperplanes) {
  _nPlanes = nHyperplanes;

  _hitList = (bool*) malloc(_nPlanes*sizeof(bool));
  memset(_hitList, 0x00, _nPlanes);

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

  _sonsAllocd=false;
  
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



Hough5DNode::~Hough5DNode() {
  
  if(_hitList != NULL)
    free(_hitList); 
  if(_center != NULL)
    free(_center);
  if(_sonsAllocd)
    free(_sons);
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
  if(_proj4 != NULL)
    free(_proj4);
}


float*
Hough5DNode::getSonArray() {
    
  _sons = (float*) malloc(32*5*sizeof(float));

  int count=0;
  
  for(int x=-1; x<2; x+=2)
    for(int y=-1; y<2; y+=2)
      for(int z=-1; z<2; z+=2)
	for(int v=-1; v<2; v+=2)
	  for(int w=-1; w<2; w+=2) {
	    _sons[count*5] = _center[0] + 0.25*x*_length;
	    _sons[count*5+1] = _center[1] + 0.25*y*_length;
	    _sons[count*5+2] = _center[2] + 0.25*z*_length;
	    _sons[count*5+3] = _center[3] + 0.25*v*_length;
	    _sons[count*5+4] = _center[4] + 0.25*w*_length;
	    count++;
	  }
  
  return _sons;
}

void
Hough5DNode::print() {
  
  std::cout<<"**Hough5DNode**   level: "<<_level<<"   center: "
	   <<_center[0]<<", "<<_center[1]<<", "<<_center[2]
	   <<", "<<_center[3]<<", "<<_center[4]<<"   votes: "
	   <<_votes<<std::endl; 

}
