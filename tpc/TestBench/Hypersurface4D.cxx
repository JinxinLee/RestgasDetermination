//-----------------------------------------------------------
//
// Description:
//      Representation of a Hyperplane in 4-dimensional
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

#include "Hypersurface4D.h"
#include "TMath.h"

#include <cmath>


Hypersurface4D::Hypersurface4D(double par1,double par2, const TF1& rep1,
			       double par3,double par4, const TF1& rep2,
			       int index) 
  : _paramsSet(false)
{
  _index = index;
  _rep1 = new TF1(rep1);
  _rep2 = new TF1(rep2);
  _rep1->SetLineWidth(1);
  _rep2->SetLineWidth(1);
  _pars = (double*) malloc(4*sizeof(double));
  _mins = (double*) malloc(4*sizeof(double));
  _maxs = (double*) malloc(4*sizeof(double));
  
  _pars[0] = par1;
  _pars[1] = par2;
  _pars[2] = par3;
  _pars[3] = par4;
  
  //given  in actual parameter space, not unit space
  _rep1->SetParameter(0,par1); 
  _rep1->SetParameter(1,par2);  
  _rep2->SetParameter(0,par3); 
  _rep2->SetParameter(1,par4);  
  
  _mins[0] = 0.;      
  _mins[1] = 0.;      
  _mins[2] = 0.;      
  _mins[3] = 0.;      
     
  _maxs[0] = 0.;
  _maxs[1] = 0.;
  _maxs[2] = 0.;
  _maxs[3] = 0.;
}


Hypersurface4D::~Hypersurface4D() {
  delete _rep1;
  delete _rep2;
  free(_pars);
  free(_mins);
  free(_maxs);
}


bool 
Hypersurface4D::testIntersect(Hough4DNode* node) const {
  //do 2 2D checks in a row

  const double* proj1 = node->getProjection0(); 
  const double* proj2 = node->getProjection1();
  const double* proj3 = node->getProjection2(); 
  const double* proj4 = node->getProjection3();
 
  int signs = 0;
  
  if(!_paramsSet) {
    std::cerr<<"Hypersurface4D::testIntersect():"
	     <<" Parameter space not specified!"
	     <<" Aborting."<<std::endl;
    return false;
  }
  
  //inflate to full-scale parameter space
  double p1_par_max = (_maxs[0]-_mins[0])*(proj1[0]+0.5) + _mins[0];
  double p1_par_min = (_maxs[0]-_mins[0])*(proj1[1]+0.5) + _mins[0];
  double p2_par_max = (_maxs[1]-_mins[1])*(proj2[0]+0.5) + _mins[1];
  double p2_par_min = (_maxs[1]-_mins[1])*(proj2[1]+0.5) + _mins[1];
  
  //calculate rep value for both projection points
  double val1 = _rep1->Eval(p1_par_min);
  double val2 = _rep1->Eval(p1_par_max);
  
  signs+=((val1-p2_par_min)>0);
  signs+=((val1-p2_par_max)>0);
  signs+=((val2-p2_par_min)>0);
  signs+=((val2-p2_par_max)>0);
  
  if(signs == 4 || signs == 0) 
    return false; 

  signs = 0;
  
  //inflate to full-scale parameter space
  double p3_par_max = (_maxs[2]-_mins[2])*(proj3[0]+0.5) + _mins[2];
  double p3_par_min = (_maxs[2]-_mins[2])*(proj3[1]+0.5) + _mins[2];
  double p4_par_max = (_maxs[3]-_mins[3])*(proj4[0]+0.5) + _mins[3];
  double p4_par_min = (_maxs[3]-_mins[3])*(proj4[1]+0.5) + _mins[3];
  
  //calculate rep value for both projection points
  val1 = _rep2->Eval(p3_par_min);
  val2 = _rep2->Eval(p3_par_max);
  
  signs+=((val1-p4_par_min)>0);
  signs+=((val1-p4_par_max)>0);
  signs+=((val2-p4_par_min)>0);
  signs+=((val2-p4_par_max)>0);
    
  if(signs == 4 || signs == 0) 
    return false;
  else {
    node->setHit(_index);
    node->vote();
    return true;   
  }    
  
}


void 
Hypersurface4D::setParamSpace(double* mins, double* maxs) {
  for(unsigned int c=0; c<4; c++) {
    _mins[c]=mins[c];
    _maxs[c]=maxs[c];
  }
  _paramsSet=true;
}    
