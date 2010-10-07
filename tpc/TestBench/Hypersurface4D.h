//-----------------------------------------------------------
//
// Description:
//      Quasi-4D hitrep for a true 4D Hough Space search.
//      Performs intersection check in 2 indendent 2D 
//      projection planes, both of which are treated with 
//      a TF1 (see Hypersurface2D.h)
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



#include "TF1.h"
#include "Hough4DNode.h"

#ifndef HYPERSURF4D_H
#define HYPERSURF4D_H

class Hypersurface4D {


 public:
  
  Hypersurface4D();
  Hypersurface4D(double par1, double par2, const TF1& rep1,
		 double par3, double par4, const TF1& rep2, int ID);
  ~Hypersurface4D();

  bool testIntersect(Hough4DNode*) const;

  //set minima and maxima of parameter space
  void setParamSpace(double* mins, double* maxs);    
    
  
  //getters ---------------------------
  const double* getParamMins() const {return _mins;}
  const double* getParamMaxs() const {return _maxs;}
  TF1* getTF1_1() const {return _rep1;}
  TF1* getTF1_2() const {return _rep2;}

  
  //return coordinates in parameter space:
  const double* getPars() const {return _pars;} 
    
  int getID() {return _index;}

 private:
  
  // Private Data Members ------------
  double* _pars;         

  int _index;     //hit ID

  double* _mins;
  double* _maxs;   //minima and maxima of the 4 parameter dimensions

  bool _paramsSet;

  TF1* _rep1;
  TF1* _rep2;
 
};

#endif
