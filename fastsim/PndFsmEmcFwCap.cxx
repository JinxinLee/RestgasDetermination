//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmEmcFwCap.cc,v 1.9 2007/05/24 08:07:40 klausg Exp $
//
// Description:
//      Class FsmEmcFwCap
//
//  Implementation of the EMC Barrel part for the FastSim
//
//  This software was developed for the PANDA collaboration.  If you
//  use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//      Klaus Goetzen                    Original Author
//
// Copyright Information:
//      Copyright (C) 2006              GSI
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndFsmEmcFwCap.h"

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

PndFsmEmcFwCap::PndFsmEmcFwCap()
{
  initParameters();

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  //print(std::cout);
}

PndFsmEmcFwCap::PndFsmEmcFwCap(ArgList &par)
{
  initParameters();
  //set default parameter values and parses a parameter list
  //i.e. std::list<std::string> of the form
  //"a=1" "b=2" "c=3"
  parseParameterList(par);

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  //print(std::cout);
}

//--------------
// Destructor --
//--------------

PndFsmEmcFwCap::~PndFsmEmcFwCap()
{
}

//--------------
// Operations --
//--------------

PndFsmResponse*
PndFsmEmcFwCap::respond(PndFsmTrack *t)
{
  PndFsmResponse *result=new PndFsmResponse();

  result->setDetector(this);
  bool wasDetected=detected(t);
  result->setDetected(wasDetected);

  if (wasDetected && fabs(t->charge())<1e-8)
  {
    result->setdE(dE(t));
    result->setdphi(dphi(t));
    result->setdtheta(dtheta(t));
    double zscale = t->p4().Pz()/_dist;
    double x = t->p4().Px()/zscale;
    double y = t->p4().Py()/zscale;
    TVector3 hitpos(x,y,_dist);
    t->setStopVtx(hitpos);
  }
  else
  {
    result->setdE(0.);
    result->setdphi(0.);
    result->setdtheta(0.);
  }

  return result;
}

bool
PndFsmEmcFwCap::detected(PndFsmTrack *t) const
{
  if (t->hitMapValid()) {
    return t->hitMapResponse(FsmDetEnum::EmcFwCap);
  } else {
    double theta = t->p4().Theta();
    double E=t->p4().E();
    double lund = t->pdt();
    return ( lund==22 && theta>=_thtMin && theta<=_thtMax && E>_Emin  && _rand->Rndm()<=_efficiency);
  }
}

double
PndFsmEmcFwCap::dE(PndFsmTrack *t) const
{
  double E = t->p4().E();

  return (_aPar+_bPar/E+_cPar/sqrt(E) ) * E;
}

double
PndFsmEmcFwCap::dphi(PndFsmTrack *t) const
{
  double theta = t->p4().Vect().Theta();

  return (_resFactor*M_PI/int(2*M_PI*_dist*tan(theta)/_xtalDim) );
}

double
PndFsmEmcFwCap::dtheta(PndFsmTrack *t) const
{
  double theta = t->p4().Vect().Theta();
  return ( _resFactor*atan(_xtalDim*cos(theta)/(2*_dist)) );
}

void
PndFsmEmcFwCap::print(ostream &o)
{
  o <<"Detector <"<<_detName<<">"<<endl;
  o  <<"  _aPar = "<<_aPar<<endl;
  o  <<"  _bPar = "<<_bPar<<endl;
  o  <<"  _cPar = "<<_cPar<<endl;
  o  <<"  _xtalDim = "<<_xtalDim<<endl;
  o  <<"  _Emin = "<<_Emin<<endl;
  o  <<"  _dist = "<<_dist<<endl;
  o  <<"  _resFactor = "<<_resFactor<<endl;
  o  <<"  _thtMin = "<<_thtMin<<endl;
  o  <<"  _thtMax = "<<_thtMax<<endl;
  o  <<"  _radiationLength = "<<_radiationLength<<endl;
  o  <<"  _efficiency = "<<_efficiency<<endl;
}

void
PndFsmEmcFwCap::initParameters()
{
  _detName = "EmcFwCap";
  _detName = FsmDetName::name(FsmDetEnum::EmcFwCap);
  _aPar=4.52495e-3;
  _bPar=2.9539e-3;
  _cPar=7.7596e-3;
  //_aPar  = 0.01;
  //_bPar = 0.01;
  _xtalDim = 0.02;
  _Emin = 0.01;
  _dist = 2.5;
  _resFactor = 0.25;
  _thtMin = 5.0;
  _thtMax = 22.0;
  _radiationLength = 0.0;
  _efficiency	   =1.0;
}

bool
PndFsmEmcFwCap::setParameter(std::string &name, double value)
{
  // *****************
  // include here all parameters which should be settable via tcl
  // *****************

  bool knownName=true;

  if (name == "aPar")
    _aPar=value;
  else
  if (name == "bPar")
    _bPar=value;
  else
  if (name == "cPar")
    _cPar=value;
  else
  if (name == "xtalDim")
    _xtalDim=value;
  else
  if (name == "Emin")
    _Emin=value;
  else
  if (name == "dist")
    _dist=value;
  else
  if (name == "resFactor")
    _resFactor=value;
  else
  if (name == "thtMin")
    _thtMin=value;
  else
  if (name == "thtMax")
    _thtMax=value;
  else
  if (name == "radiationLength")
    _radiationLength=value;
  else
  if (name == "efficiency")
    _efficiency=value;
  else
    knownName=false;

  return knownName;
}
