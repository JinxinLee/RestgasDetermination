//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmEmcFS.cc,v 1.9 2007/05/24 08:07:40 klausg Exp $
//
// Description:
//      Class FsmEmcFS
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
#include "PndFsmEmcFS.h"

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

PndFsmEmcFS::PndFsmEmcFS()
{
  initParameters();

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  print(std::cout);
}

PndFsmEmcFS::PndFsmEmcFS(ArgList &par)
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

PndFsmEmcFS::~PndFsmEmcFS()
{
}

//--------------
// Operations --
//--------------

PndFsmResponse*
PndFsmEmcFS::respond(PndFsmTrack *t)
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
PndFsmEmcFS::detected(PndFsmTrack *t) const
{
  if (t->hitMapValid()) {
    return t->hitMapResponse(FsmDetEnum::EmcFS);
  } else {
    double theta = t->p4().Theta();
    double E=t->p4().E();
    double lund = t->pdt();
    return ( lund==22 && theta>=_thtMin && theta<=_thtMax && E>_Emin  && _rand->Rndm()<=_efficiency);
  }
}

double
PndFsmEmcFS::dE(PndFsmTrack *t) const
{
  double E = t->p4().E();

  return (sqrt(_aPar*_aPar+_bPar*_bPar/E ) * E);  //Jerzy's version
}

double
PndFsmEmcFS::dphi(PndFsmTrack *t) const
{
  double theta = t->p4().Vect().Theta();
  int denom=int(2*M_PI*_dist*tan(theta)/_xtalDim);
  if (denom==0) return 0;
  else
    return (_resFactor*M_PI/(double)denom );
}

double
PndFsmEmcFS::dtheta(PndFsmTrack *t) const
{
  //double theta = t->p4().vect().theta();// Alex's version
  //return ( _resFactor*atan(_xtalDim*cos(theta)/(2*_dist)) );

  double E = t->p4().E();                 //Jerzy's version
  return (0.002/sqrt(E));
}

void
PndFsmEmcFS::print(ostream &o)
{
  o <<"Detector <"<<_detName<<">"<<endl;
  o  <<"  _aPar = "<<_aPar<<endl;
  o  <<"  _bPar = "<<_bPar<<endl;
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
PndFsmEmcFS::initParameters()
{
  _detName = FsmDetName::name(FsmDetEnum::EmcFS);
  _aPar  = 0.013;
  _bPar = 0.0283;
  _xtalDim = 0.0555;
  _Emin = 0.01;
  _dist = 8.2;
  _resFactor = 0.25;
  _thtMin = 0.05;
  _thtMax = 5.0;
  _radiationLength = 0.0;
  _efficiency	   =1.0;
}

bool
PndFsmEmcFS::setParameter(std::string &name, double value)
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

