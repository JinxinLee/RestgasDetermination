//--------------------------------------------------------------------------
// Description:
//      Class PndFsmSimpleTracker
//
//  Example Tracker for the PANDA Fast Sim Detectors
//
//  This software was developed for the PANDA collaboration.  If you
//  use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//      Klaus Goetzen                    Original Author
//
// Copyright Information:
//      Copyright (C) 2008              GSI
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndFsmSimpleTracker.h"

//-------------
// C Headers --
//-------------

//---------------
// C++ Headers --
//---------------
#include <math.h>
#include <iostream>

using std::cout;
using std::endl;
using std::ostream;
using std::string;

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "ArgList.h"
#include "PndFsmResponse.h"
#include "PndFsmTrack.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

//----------------
// Constructors --
//----------------

PndFsmSimpleTracker::PndFsmSimpleTracker()
{
  initParameters();

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;

  print(std::cout);
}

PndFsmSimpleTracker::PndFsmSimpleTracker(ArgList &par)
{
  initParameters();
  //set default parameter values and parses a parameter list
  //i.e. std::list<std::string> of the form
  //"a=1" "b=2" "c=3"
  parseParameterList(par);

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;

  print(std::cout);
}

//--------------
// Destructor --
//--------------

PndFsmSimpleTracker::~PndFsmSimpleTracker()
{
}

//--------------
// Operations --
//--------------

PndFsmResponse*
PndFsmSimpleTracker::respond(PndFsmTrack *t)
{
  PndFsmResponse *result=new PndFsmResponse();

  result->setDetector(this);
  bool wasDetected=detected(t);
  result->setDetected(wasDetected);

  if (wasDetected && fabs(t->charge())>1e-8)
  {
    result->setdp(dp(t));
    result->setdphi(dphi(t));
    result->setdtheta(dtheta(t));
  }

  return result;
}

bool
PndFsmSimpleTracker::detected(PndFsmTrack *t) const
{
    double theta = t->p4().Theta();
    double p_t=t->p4().Vect().Perp(TVector3(0.,0.,1.));
    double p=t->p4().Vect().Mag();
    double charge=t->charge();

	double eff = _efficiency;
	// use parametrization for eff(p) for p<0.6 as multiplier for _efficiency from 
    // https://panda-wiki.gsi.de/foswiki/pub/Computing/Minutes07April2014/Costanza_EVO_April07.pdf
	if (eff<0)
	{
		eff = -eff;
		if (p<0.6) eff = eff * (0.32602 + 1.91679*p -1.58341*p*p + 0.305179*p*p*p);
	}
	
    return ( charge!=0.0 && theta>=_thtMin && theta<=_thtMax && p_t>_ptmin && p>_pmin && _rand->Rndm()<=eff);
}


double
PndFsmSimpleTracker::dp(PndFsmTrack *t) const
{
  double p=t->p4().Vect().Mag();
  
  double res;
  
  // use parametrization dp/p(p) from 
  // https://panda-wiki.gsi.de/foswiki/pub/Computing/Minutes07April2014/Costanza_EVO_April07.pdf
  if (_pRes<0) 
	  res = 0.012 + p*0.0034;
  else // use fixed resolution
	  res = _pRes * p;
  
  return res;
}

double
PndFsmSimpleTracker::dphi(PndFsmTrack *t) const
{
  double res = _phiRes;
  
  // use parametrization dphi(p) from 
  // https://panda-wiki.gsi.de/foswiki/pub/Computing/Minutes07April2014/Costanza_EVO_April07.pdf
  if (_phiRes<0)
  {
	  double p=t->p4().Vect().Mag();
	  res = 2.497e-3/pow(p,1.307) + 8.77e-4;
  }
  
  return res;
}

double
PndFsmSimpleTracker::dtheta(PndFsmTrack *t) const
{
  double res = _thtRes;
  
  // use parametrization dtheta(p) from 
  // https://panda-wiki.gsi.de/foswiki/pub/Computing/Minutes07April2014/Costanza_EVO_April07.pdf
  if (_thtRes<0)
  {
	  double p=t->p4().Vect().Mag();
	  res = 1.81e-3/pow(p,1.131) + 3.85e-4;
  }
  return res;
}

void
PndFsmSimpleTracker::print(ostream &o)
{
  o <<"Parameters for detector <"<<detName()<<">"<<endl;
  o  <<"  _thtMin     = " <<  _thtMin << endl;
  o  <<"  _thtMax     = " <<  _thtMax << endl;
  o  <<"  _ptmin      = " <<  _ptmin  << endl;
  o  <<"  _pmin       = " <<  _pmin   << endl;
  o  <<"  _pRes       = " <<  _pRes   << " (rel)"<< endl;
  if (_pRes<0) o<<"     ... using parametrisation function for dp/p."<<endl;
  o  <<"  _thtRes     = " <<  _thtRes << endl;
  if (_thtRes<0) o<<"     ... using parametrisation function for dtht."<<endl;
  o  <<"  _phiRes     = " <<  _phiRes << endl;
  if (_phiRes<0) o<<"     ... using parametrisation function for dphi."<<endl;
  o  <<"  _efficiency = " << _efficiency<<endl;
  if (_efficiency<0) o<<"     ... using parametrisation function for p<0.6."<<endl;
}

void
PndFsmSimpleTracker::initParameters()
{
  _detName         = "PndFsmSimpleTracker";
  _thtMin          = 7.765;
  _thtMax          = 159.44;
  _ptmin           = 0.1;
  _pmin            = 0.0;
  // if the resolutions are <0, than the parametrisations according to
  // https://panda-wiki.gsi.de/foswiki/pub/Computing/Minutes07April2014/Costanza_EVO_April07.pdf
  // p.6, p.9, p.10 are used
  _pRes            = -1.;//0.02;   // 2%
  _thtRes          = -1.;//0.005;
  _phiRes          = -1.;//0.005;
  // if the efficiency<0 and p<0.6GeV/c, parametrisation as multiplier to _efficiency according to p.8; else flat _efficiency 
  _efficiency	   = -1.0;//

}

bool
PndFsmSimpleTracker::setParameter(std::string &name, double value)
{
  // *****************
  // include here all parameters which should be settable via script
  // *****************

  bool knownName=true;

  if (name == "thtMin")
    _thtMin=value;
  else
  if (name == "thtMax")
    _thtMax=value;
  else
  if (name == "ptmin")
    _ptmin=value;
  else
  if (name == "pmin")
    _pmin=value;
  else
  if (name == "pRes")
    _pRes=value;
  else
  if (name == "thtRes")
    _thtRes=value;
  else
  if (name == "phiRes")
    _phiRes=value;
  else
  if (name == "efficiency")
    _efficiency=value;
  else
    knownName=false;

  return knownName;
}

