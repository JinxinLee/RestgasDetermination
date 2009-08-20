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
#include "Hough2DNode.h"

#ifndef HYPERPLANE2D_H
#define HYPERPLANE2D_H


class Hyperplane2D {

  
public:

  // Constructors/Destructors ---------
  Hyperplane2D();
  Hyperplane2D(float x,float z, int); 
  
  bool testIntersect(Hough2DNode& node);

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

  int _index;   //hyperplane ID

  float* _mins;
  float* _maxs;  //minima and maxima of the 2 parameter dimensions

  std::vector<std::map<std::vector<float>,bool>*> _hitmap;
  
  
  // Private Methods -----------------
  

};

#endif

