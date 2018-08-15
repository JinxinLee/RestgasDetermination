//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmMdcTS.cc,v 1.10 2007/05/24 08:07:40 klausg Exp $
//
// Description:
//      Class FsmMdcTS
//      
//  Implementation of the MDCs system in the Traget Spectrometer (DC1-DC2) for the FastSim
//
//  This software was developed for the PANDA collaboration.  If you
//  use all or part of it, please give an appropriat2e acknowledgement.
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
#include "PndFsmMdcTS.h"

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

PndFsmMdcTS::PndFsmMdcTS() 
{
  initParameters();

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  //print(std::cout);
}

PndFsmMdcTS::PndFsmMdcTS(ArgList &par) 
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

PndFsmMdcTS::~PndFsmMdcTS()
{
}

//--------------
// Operations --
//--------------

PndFsmResponse* 
PndFsmMdcTS::respond(PndFsmTrack *t)
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
  else 
  {
    result->setdp(0.);
    result->setdphi(0.);
    result->setdtheta(0.);
  }
  
  return result;
}

bool 
PndFsmMdcTS::detected(PndFsmTrack *t) const
{
  if (t->hitMapValid()) {
    return (t->hitMapResponse(FsmDetEnum::Dch1) || t->hitMapResponse(FsmDetEnum::Dch2));
  } else {
    double theta = t->p4().Theta();
    double p=t->p4().Vect().Mag();
    double charge=t->charge();
    return ( charge!=0.0 && theta>=_thtMin && theta<=_thtMax && p>_pmin  && _rand->Rndm()<=_efficiency);
  }
}

double 
PndFsmMdcTS::dp(PndFsmTrack *t) const
{
  TLorentzVector p4=t->p4();
  double theta=p4.Theta();
  double mom=p4.Vect().Mag();

  double sigp = sqrt(_a3Par*_a3Par*mom*mom + _a4Par*_a4Par) / sin(theta) * mom;   

  return sigp;
}

double
PndFsmMdcTS::dphi(PndFsmTrack *) const // t //[R.K.03/2017] unused variable(s)
{
  return 0.0002; //to be refined
}

double
PndFsmMdcTS::dtheta(PndFsmTrack *) const // t //[R.K.03/2017] unused variable(s)
{
  return 0.0002; //to be refined
}

void
PndFsmMdcTS::print(ostream &o)
{
  o <<"Parameters for detector <"<<detName()<<">"<<endl;
  o  <<"  _thtMin = "<<_thtMin<<endl; 
  o  <<"  _thtMax = "<<_thtMax<<endl; 
  o  <<"  _radiationLength = "<<_radiationLength<<endl; 
  o  <<"  _pmin = "<<_pmin<<endl; 
  o  <<"  _a3Par = "<<_a3Par<<endl; 
  o  <<"  _a4Par = "<<_a4Par<<endl; 
  o  <<"  _efficiency = "<<_efficiency<<endl; 
}

void 
PndFsmMdcTS::initParameters()
{
  _detName         = "MdcTS";
  _thtMin          = 0.5;
  _thtMax          = 22.0;
  _radiationLength = 0.0;
  _pmin            = 0.0;
  _a3Par           = 0.00033;
  _a4Par           = 0.00077;
  _efficiency	   =1.0; 

}

bool
PndFsmMdcTS::setParameter(std::string &name, double value)
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
  if (name == "radiationLength")
    _radiationLength=value;
  else
  if (name == "pmin")
    _pmin=value;
  else
  if (name == "a3Par")
    _a3Par=value;
  else
  if (name == "a4Par")
    _a4Par=value;
  else
  if (name == "efficiency")
    _efficiency=value;
  else
    knownName=false;
  
  return knownName;
}
