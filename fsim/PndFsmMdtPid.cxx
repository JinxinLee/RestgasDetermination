//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: PndFsmMdtPid.cxx $
//
// Description:
//      Class PndFsmMdtPid
//
//  Implementation of the MDT for the FastSim
//
//  This software was developed for the PANDA collaboration.  If you
//  use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//      Ralf Kliemt                     Original Author
//
// Copyright Information:
//      Copyright (C) 2014              GSI
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndFsmMdtPid.h"

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

PndFsmMdtPid::PndFsmMdtPid()
{
  initParameters();

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  print(std::cout);
}

PndFsmMdtPid::PndFsmMdtPid(ArgList &par)
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

PndFsmMdtPid::~PndFsmMdtPid()
{
}

//--------------
// Operations --
//--------------

PndFsmResponse*
PndFsmMdtPid::respond(PndFsmTrack *t)
{
  PndFsmResponse *result=new PndFsmResponse();

  result->setDetector(this);
  bool wasDetected=detected(t);
  result->setDetected(wasDetected);

  if (wasDetected)
  {
    result->setLHElectron(0.);
    result->setLHMuon( 1-_misId );
    result->setLHPion( _misId );
    result->setLHKaon( 0. );
    result->setLHProton( 0. );
  }


  return result;
}

bool
PndFsmMdtPid::detected(PndFsmTrack *t) const
{
    double theta = t->p4().Theta();
    double p=t->p4().Vect().Mag();
    double charge=t->charge();
    int type=fabs(t->pdt());

    return ( charge!=0.0 && theta>=_thtMin && theta<=_thtMax && p>_pmin
            && (type==13 && _rand->Rndm()<=_efficiency) || (type==211 && _rand->Rndm()<=_misId));
}

void
PndFsmMdtPid::print(ostream &o)
{
  o <<"Parameters for detector <"<<detName()<<">"<<endl;
  o  <<"  _thtMin = "<<_thtMin<<endl;
  o  <<"  _thtMax = "<<_thtMax<<endl;
  o  <<"  _pmin = "<<_pmin<<endl;
  o  <<"  _misId = "<<_misId<<endl;
  o  <<"  _efficiency = "<<_efficiency<<endl;
}

void
PndFsmMdtPid::initParameters()
{
  _detName         = "MdtPid";
  _thtMin          = 5.;
  _thtMax          = 160.;
  _pmin            = 1.0;
  _misId	   = 0.01;
  _efficiency	   = 1.0;
}

bool
PndFsmMdtPid::setParameter(std::string &name, double value)
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
  if (name == "pmin")
	_pmin=value;
  else
  if (name == "misId")
        _misId=value;
  else
  if (name == "efficiency")
    _efficiency=value;
  else
    knownName=false;

  return knownName;
}

