//-----------------------------------------------------------
//
// Description:
//      Hit-representation  in a 2-dimensional Hough Space.
//      Performs intersection check with a subvolume thereof.
//      Interface to TF1 allows customized representation.
//      TF1 has to be of the form p1 = f(p0|x,y), where
//      p0, p1 are the parameter (Hough) space coordinates and 
//      x,y are the fixed real space coordinates of the hit
//      this hypersurface belongs to
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

#ifndef HYPERSURF2D_H
#define HYPERSURF2D_H


class Hypersurface2D {

  class PndTpcCluster;
  
 public:

  // Constructors/Destructors ---------
  Hypersurface2D();
  Hypersurface2D(float par1, float par2, const TF1& rep, int ID);
  ~Hypersurface2D();
  
  bool testIntersect(Hough2DNode* node);

  //set minima and maxima of parameter space
  void setParamSpace(float* mins, float* maxs);    
    
  
  //getters ---------------------------
  float* getParamMins() {return _mins;}
  float* getParamMaxs() {return _maxs;}
  
  float* getPars() {return _pars;} //return coordinates in parameter space
  
  int getID() {return _index;}
  
    

private:

  // Private Data Members ------------
  float* _pars;         

  int _index;     //hit ID

  float* _mins;
  float* _maxs;   //minima and maxima of the 2 parameter dimensions

  bool _paramsSet;

  TF1* _rep;      //representation of the hit in the Hough Space

  

};

#endif

