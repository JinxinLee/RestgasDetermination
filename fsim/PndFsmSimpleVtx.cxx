//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: PndFsmSimpleVtx.cc,v 1.10 2007/05/24 08:07:40 klausg Exp $
//
// Description:
//      Class PndFsmSimpleVtx
//
//  Implementation of the MVD for the FastSim
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
#include "PndFsmSimpleVtx.h"

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

PndFsmSimpleVtx::PndFsmSimpleVtx()
{
  initParameters();

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  print(std::cout);
}

PndFsmSimpleVtx::PndFsmSimpleVtx(ArgList &par)
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

PndFsmSimpleVtx::~PndFsmSimpleVtx()
{
}

//--------------
// Operations --
//--------------

PndFsmResponse*
PndFsmSimpleVtx::respond(PndFsmTrack *t)
{
  PndFsmResponse *result=new PndFsmResponse();

  result->setDetector(this);
  bool wasDetected=detected(t);
  result->setDetected(wasDetected);

  if (wasDetected && fabs(t->charge())>1e-8)
  {
    result->setdV(_vtxRes, _vtxRes, _vtxRes);
  }

  return result;
}

bool
PndFsmSimpleVtx::detected(PndFsmTrack *t) const
{
    double theta = t->p4().Theta();
    double p_t=t->p4().Vect().Perp(TVector3(0.,0.,1.));
    double charge=t->charge();

    return ( charge!=0.0 && theta>=_thtMin && theta<=_thtMax && p_t>_ptmin  && _rand->Rndm()<=_efficiency);
}


void
PndFsmSimpleVtx::print(ostream &o)
{
  o <<"Parameters for detector <"<<detName()<<">"<<endl;
  o  <<"  _thtMin = "<<_thtMin<<endl;
  o  <<"  _thtMax = "<<_thtMax<<endl;
  o  <<"  _ptmin = "<<_ptmin<<endl;
  o  <<"  _vtxRes = "<<_vtxRes*1e5<<"um"<<endl;
  o  <<"  _efficiency = "<<_efficiency<<endl;
}

void
PndFsmSimpleVtx::initParameters()
{
  _detName         = FsmDetName::name(FsmDetEnum::Mvd);
  _thtMin          = 5.0;
  _thtMax          = 160.0;
  _ptmin            = 0.0;
  _vtxRes          = 100e-4;  //vtx res = 100 um
  _efficiency	   =1.0;
}

bool
PndFsmSimpleVtx::setParameter(std::string &name, double value)
{
  // *****************
  // include here all parameters which should be settable via tcl
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
  if (name == "vtxRes")
    _vtxRes=value;
  else
  if (name == "efficiency")
    _efficiency=value;
  else
    knownName=false;

  return knownName;
}

