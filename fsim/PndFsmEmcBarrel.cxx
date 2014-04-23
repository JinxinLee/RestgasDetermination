//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmEmcBarrel.cc,v 1.11 2007/05/24 08:07:40 klausg Exp $
//
// Description:
//      Class FsmEmcBarrel
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
#include "PndFsmEmcBarrel.h"

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

PndFsmEmcBarrel::PndFsmEmcBarrel()
{
  initParameters();

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  _phiMin=_phiMin*M_PI/180.0;
  _phiMax=_phiMax*M_PI/180.0;
  print(std::cout);
}

PndFsmEmcBarrel::PndFsmEmcBarrel(ArgList &par)
{
  initParameters();
  //set default parameter values and parses a parameter list
  //i.e. std::list<std::string> of the form
  //"a=1" "b=2" "c=3"
  parseParameterList(par);

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  _phiMin=_phiMin*M_PI/180.0;
  _phiMax=_phiMax*M_PI/180.0;
  print(std::cout);
}




//--------------
// Destructor --
//--------------

PndFsmEmcBarrel::~PndFsmEmcBarrel()
{
}

//--------------
// Operations --
//--------------

PndFsmResponse*
PndFsmEmcBarrel::respond(PndFsmTrack *t)
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
PndFsmEmcBarrel::detected(PndFsmTrack *t) const
{
  if (t->hitMapValid()) {
    return t->hitMapResponse(FsmDetEnum::EmcBarrel);
  } else {
    double theta = t->p4().Theta();
    double phi = t->p4().Phi();
    double E = t->p4().E();
    double lund = t->pdt();
    return ( lund==22 && theta>=_thtMin && theta<=_thtMax && E>_Emin && phi>=_phiMin && phi<=_phiMax && _rand->Rndm()<=_efficiency);
  }
}

double
PndFsmEmcBarrel::dE(PndFsmTrack *t) const
{
  double E = t->p4().E();
  return (_aPar+_bPar/E+_cPar/sqrt(E) ) * E;
  //return (sqrt(_aPar*_aPar+_bPar*_bPar/sqrt(E) ) * E);
}

double
PndFsmEmcBarrel::dphi(PndFsmTrack *t) const
{
  return (_resFactor*M_PI/int(2*M_PI*_barrelRadius/_xtalDim) );
}

double
PndFsmEmcBarrel::dtheta(PndFsmTrack *t) const
{
  double theta = t->p4().Vect().Theta();
  return ( _resFactor*atan(_xtalDim*sin(theta)/(2*_barrelRadius)) );
}

void
PndFsmEmcBarrel::print(ostream &o)
{
  o <<"Detector <"<<_detName<<">"<<endl;
  o  <<"  _aPar = "<<_aPar<<endl;
  o  <<"  _bPar = "<<_bPar<<endl;
  o  <<"  _cPar = "<<_cPar<<endl;
  o  <<"  _xtalDim = "<<_xtalDim<<endl;
  o  <<"  _Emin = "<<_Emin<<endl;
  o  <<"  _barrelRadius = "<<_barrelRadius<<endl;
  o  <<"  _resFactor = "<<_resFactor<<endl;
  o  <<"  _thtMin = "<<_thtMin<<endl;
  o  <<"  _thtMax = "<<_thtMax<<endl;
  o  <<"  _radiationLength = "<<_radiationLength<<endl;
  o  <<"  _phiMin = "<<_phiMin<<endl;
  o  <<"  _phiMax = "<<_phiMax<<endl;
  o  <<"  _efficiency = "<<_efficiency<<endl;
}

void
PndFsmEmcBarrel::initParameters()
{
  _detName = FsmDetName::name(FsmDetEnum::EmcBarrel);
  _aPar=4.52495e-3;
  _bPar=2.9539e-3;
  _cPar=7.7596e-3;
  //_aPar  = 0.01;
  //_bPar = 0.01;
  //_cPar = 0.01;
  _xtalDim = 0.02;
  _Emin = 0.01;
  _barrelRadius = 0.5;
  _resFactor = 0.25;
  _thtMin = 22.0;
  _thtMax = 140.0;
  _radiationLength = 0.0;
  _efficiency	   =1.0;
  _phiMin=-180.;
  _phiMax=180.;
}

bool
PndFsmEmcBarrel::setParameter(std::string &name, double value)
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
  if (name == "barrelRadius")
    _barrelRadius=value;
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
  if (name == "phiMin")
    _phiMin=value;
  else
  if (name == "phiMax")
    _phiMax=value;
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

