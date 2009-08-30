//-----------------------------------------------------------
//
// Description:
//      Representation of a Hyperplane in 5-dimensional
//      Hough-Space.
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


#include <map>
#include <vector>
#include "PndTpcCluster.h"
#include "Hough5DNode.h"

#ifndef HYPERPLANE5D_H
#define HYPERPLANE5D_H


class Hyperplane5D {

  
public:

  // Constructors/Destructors ---------
  Hyperplane5D();
  Hyperplane5D(PndTpcCluster*, int); 
  
  bool testIntersect(Hough5DNode& node);
  bool testIntersect(Hough5DNode* node);
  void setRiemannScaling(float sc) {_RIEMANNSCALING = sc;}

  //set minima and maxima of parameter space
  void setParamSpace(float* mins, float* maxs);    
    
  
  //getters ---------------------------
  float* getParamMins() {return _mins;}
  float* getParamMaxs() {return _maxs;}
  
  float* getCoords() {return _coords;} //returns (phi,theta,c,m,t);
  
  int getID() {return _index;}
  
    

private:

  // Private Data Members ------------
  float* _coords;           //(x_R, y_R, z_R, R, Z)
  float _RIEMANNSCALING;

  int _index;   //hyperplane ID

  float* _mins;
  float* _maxs;  //minima and maxima of the 5 parameter dimensions

  std::vector<std::map<std::vector<float>,bool>*> _hitmap;
  
  
  // Private Methods -----------------
  

};

#endif

