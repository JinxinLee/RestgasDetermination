//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: PndFsmIdealPid.cxx $
//
// Description:
//      Class PndFsmIdealPid
//
//  Implementation of the Montecarlo true PID for the FastSim
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
#include "PndFsmIdealPid.h"

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

PndFsmIdealPid::PndFsmIdealPid()
{
  initParameters();
  readParameters();
  print(std::cout);
}

PndFsmIdealPid::PndFsmIdealPid(ArgList &par)
{
  initParameters();
  parseParameterList(par);
  readParameters();
  print(std::cout);
}

//--------------
// Destructor --
//--------------

PndFsmIdealPid::~PndFsmIdealPid()
{
}

//--------------
// Operations --
//--------------

PndFsmResponse*
PndFsmIdealPid::respond(PndFsmTrack *t)
{
  PndFsmResponse *result=new PndFsmResponse();

  result->setDetector(this);
  bool wasDetected=detected(t);
  result->setDetected(wasDetected);

  if (wasDetected)
  {
	int    type   = fabs(t->pdt());

	result->setLHElectron(0.);
	result->setLHMuon(0.);
	result->setLHPion(0.);
	result->setLHKaon(0.);
	result->setLHProton(0.);

	if (fabs(type)==11) result->setLHElectron(1.);
	else if (fabs(type)==13) result->setLHMuon(1.);
	else if (fabs(type)==211) result->setLHPion(1.);
	else if (fabs(type)==321) result->setLHKaon(1.);
	else if (type==2212) result->setLHProton(1.);
	else {
	  result->setLHElectron(0.2);
	  result->setLHMuon(0.2);
	  result->setLHPion(0.2);
	  result->setLHKaon(0.2);
	  result->setLHProton(0.2);
	}
  }
  return result;
}

bool
PndFsmIdealPid::detected(PndFsmTrack *t) const
{
	if(t) return true;
	else return false;
}

void
PndFsmIdealPid::print(ostream &o)
{
  o <<"Parameters for detector <"<<detName()<<">"<<endl;
}

void
PndFsmIdealPid::initParameters()
{
  _detName="IdealPid";
  return;
}

bool
PndFsmIdealPid::setParameter(std::string &name, double value)
{
  return true;
}

bool PndFsmIdealPid::readParameters()
{
  return true;
}
