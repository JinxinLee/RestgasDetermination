//-----------------------------------------------------------
//
// Description:
//      Representation of a Hyperplane in 5-dimensional
//      Hough-Space.
//      -- implementation
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

#include "Hypersurface2D.h"
#include "TMath.h"

#include <cmath>
#include <assert.h>
#include <cstdlib>


Hypersurface2D::Hypersurface2D(double par1,double par2, const TF1& rep, int index) {

  _index = index;
  _rep = new TF1(rep);
  _pars = (double*) malloc(2*sizeof(double));
  _mins = (double*) malloc(2*sizeof(double));
  _maxs = (double*) malloc(2*sizeof(double));
  
  _pars[0] = par1;
  _pars[1] = par2;

  _rep->SetParameter(0,par1);  //given  in actual parameter space, not unit space
  _rep->SetParameter(1,par2);  

  _mins[0] = 0.f;      
  _mins[1] = 0.f;      
     
  _maxs[0] = 0.f;
  _maxs[1] = 0.f;
  
  _paramsSet=false;
}


Hypersurface2D::~Hypersurface2D() {
  delete _rep;
}


bool 
Hypersurface2D::testIntersect(Hough2DNode* node) {

  const double* proj1 = node->getProjection0(); 
  const double* proj2 = node->getProjection1();
 
  int signs = 0;
  
  if(!_paramsSet) {
    std::cerr<<"Hypersurface2D::testIntersect() Parameter space not specified!"
	     <<" Aborting."<<std::endl;
    return 0;
  }
  
  //inflate to full-scale parameter space
  double p1_par_max = (_maxs[0]-_mins[0])*(proj1[0]+0.5) + _mins[0];
  double p1_par_min = (_maxs[0]-_mins[0])*(proj1[1]+0.5) + _mins[0];
  double p2_par_max = (_maxs[1]-_mins[1])*(proj2[0]+0.5) + _mins[1];
  double p2_par_min = (_maxs[1]-_mins[1])*(proj2[1]+0.5) + _mins[1];
  
  //calculate rep value for both projection points
  double val1 = _rep->Eval(p1_par_min);
  double val2 = _rep->Eval(p1_par_max);
  
  signs+=((val1-p2_par_min)>0);
  signs+=((val1-p2_par_max)>0);
  signs+=((val2-p2_par_min)>0);
  signs+=((val2-p2_par_max)>0);
  
  if(signs == 4 || signs == 0) 
    return false; 
  
  else {
    node->setHit(_index);
    node->vote();
    return true;   
  }    
  
}


void 
Hypersurface2D::setParamSpace(double* mins, double* maxs) {
  _mins = mins;
  _maxs = maxs;  
  _paramsSet=true;
}    



