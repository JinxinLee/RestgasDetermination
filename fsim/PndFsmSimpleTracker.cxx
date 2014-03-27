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

    return ( charge!=0.0 && theta>=_thtMin && theta<=_thtMax && p_t>_ptmin && p>_pmin && _rand->Rndm()<=_efficiency);
}


double
PndFsmSimpleTracker::dp(PndFsmTrack *t) const
{
  double p=t->p4().Vect().Mag();

  return (_pRes * p );
}

double
PndFsmSimpleTracker::dphi(PndFsmTrack *t) const
{
  return _phiRes;
}

double
PndFsmSimpleTracker::dtheta(PndFsmTrack *t) const
{
  return _thtRes;
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
  o  <<"  _thtRes     = " <<  _thtRes << endl;
  o  <<"  _phiRes     = " <<  _phiRes << endl;
  o  <<"  _efficiency = " << _efficiency<<endl;
}

void
PndFsmSimpleTracker::initParameters()
{
  _detName         = "PndFsmSimpleTracker";
  _thtMin          = 7.765;
  _thtMax          = 159.44;
  _ptmin           = 0.1;
  _pmin            = 0.0;
  _pRes            = 0.02;   // 2%
  _thtRes          = 0.005;
  _phiRes          = 0.005;
  _efficiency	   = 1.0;

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

