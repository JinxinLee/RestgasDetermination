//-----------------------------------------------------------
//
// Description:
//      Hit-representation  in a 2-dimensional Hough Space.
//      Performs intersection check with a subvolume thereof.
//      Interface to TF1 allows customized representation.
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


#include "Hough2DNode.h"
#include "TF1.h"

#ifndef HYPERPLANE2D_H
#define HYPERPLANE2D_H


class Hyperplane2D {

  class PndTpcCluster;
  
 public:

  // Constructors/Destructors ---------
  Hyperplane2D();
  Hyperplane2D(PndTpcCluster* cl, int ID); //deprecated, specialized version
  Hyperplane2D(PndTpcCluster* cl, TF1* rep, int ID);
  
  bool testIntersect(Hough2DNode& node);
  bool testIntersect(Hough2DNode* node);


  //set minima and maxima of parameter space
  void setParamSpace(float* mins, float* maxs);    
    
  
  //getters ---------------------------
  float* getParamMins() {return _mins;}
  float* getParamMaxs() {return _maxs;}
  
  float* getCoords() {return _coords;} //return coordinates in unit space
  
  int getID() {return _index;}
  
    

private:

  // Private Data Members ------------
  float* _coords;           //(x_R, y_R)

  int _index;   //hyperplane ID

  float* _mins;
  float* _maxs;  //minima and maxima of the 2 parameter dimensions

  TF1* _rep; //representation of the hit in the Hough Space

  // Private Methods -----------------
  

};

#endif

