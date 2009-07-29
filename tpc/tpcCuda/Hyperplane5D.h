
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
  void setRiemannScaling(float sc) {_RIEMANNSCALING = sc;}
  void setParamSpace(float*, float*);
  
  
  float* getParams() {return _params;}
  int getID() {return _index;}

  
  

private:

  // Private Data Members ------------
  float* _params;           //(x_R, y_R, z_R, R, Z)
  float _RIEMANNSCALING;

  int _index;   //hyperplane ID

  float* _mins;
  float* _maxs;  //minima and maxima of the 5 parameter dimensions
  
  
  // Private Methods -----------------
  

};

#endif

