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


Hypersurface2D::Hypersurface2D(float par1,float par2, const TF1& rep, int index) {

  _index = index;
  _rep = new TF1(rep);
  _pars = (float*) malloc(2*sizeof(float));
  _mins = (float*) malloc(2*sizeof(float));
  _maxs = (float*) malloc(2*sizeof(float));
  
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

  float* proj1 = node->getProjection0(); 
  float* proj2 = node->getProjection1();
 
  int signs = 0;
  
  if(!_paramsSet) {
    std::cerr<<"Hypersurface2D::testIntersect() Parameter space not specified!"
	     <<" Aborting."<<std::endl;
    return 0;
  }
  
 
  //inflate to full-scale parameter space
  float p1_par_max = (_maxs[0]-_mins[0])*(proj1[0]+0.5) + _mins[0];
  float p1_par_min = (_maxs[0]-_mins[0])*(proj1[1]+0.5) + _mins[0];
  float p2_par_max = (_maxs[1]-_mins[1])*(proj2[0]+0.5) + _mins[1];
  float p2_par_min = (_maxs[1]-_mins[1])*(proj2[1]+0.5) + _mins[1];
  
  //calculate rep value for both projection points
  float val1 = _rep->Eval(p1_par_min);
  float val2 = _rep->Eval(p1_par_max);
  
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
Hypersurface2D::setParamSpace(float* mins, float* maxs) {
  _mins = mins;
  _maxs = maxs;  
  _paramsSet=true;
}    



