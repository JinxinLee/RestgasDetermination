//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmMdcFS.cc,v 1.9 2007/05/24 08:07:40 klausg Exp $
//
// Description:
//      Class FsmMdcFS
//      
//  Implementation of the MDCs system for the Forward Spectrometer (DC3-DC8) for the FastSim
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
#include "PndFsmMdcFS.h"

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

PndFsmMdcFS::PndFsmMdcFS() 
{
  initParameters();

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  _angleXMax *= M_PI/180.0;
  _angleYMax *= M_PI/180.0;
  //print(std::cout);
}

PndFsmMdcFS::PndFsmMdcFS(ArgList &par) 
{
  initParameters();
  //set default parameter values and parses a parameter list
  //i.e. std::list<std::string> of the form
  //"a=1" "b=2" "c=3" 
  parseParameterList(par);

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  _angleXMax *= M_PI/180.0;
  _angleYMax *= M_PI/180.0;


  //print(std::cout);
}

//--------------
// Destructor --
//--------------

PndFsmMdcFS::~PndFsmMdcFS()
{
}

//--------------
// Operations --
//--------------

PndFsmResponse* 
PndFsmMdcFS::respond(PndFsmTrack *t)
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
PndFsmMdcFS::detected(PndFsmTrack *t) const
{
  if (t->hitMapValid()) {
    return (t->hitMapResponse(FsmDetEnum::Dch3) || t->hitMapResponse(FsmDetEnum::Dch4) || t->hitMapResponse(FsmDetEnum::Dch5) || t->hitMapResponse(FsmDetEnum::Dch6) || t->hitMapResponse(FsmDetEnum::Dch7) || t->hitMapResponse(FsmDetEnum::Dch8));
  } else {
    double theta = t->p4().Theta();

    if (t->p4().Z()<=0) return false;

    double angleX = fabs(atan(t->p4().X()/t->p4().Z()));
    double angleY = fabs(atan(t->p4().Y()/t->p4().Z()));
    double p=t->p4().Vect().Mag();
    double charge=t->charge();
    return ( charge!=0.0 && theta>=_thtMin && angleX<=_angleXMax && angleY<=_angleYMax && p>_pmin  && _rand->Rndm()<=_efficiency);
  }
}

double 
PndFsmMdcFS::dp(PndFsmTrack *t) const
{
  TLorentzVector p4=t->p4();
  double mom=p4.Vect().Mag();
  double rad=_a1Par*_a1Par*mom*mom + _a2Par*_a2Par;
  if (rad<0 || mom==0 ) return 0.0;
  double sigp = sqrt(rad) / _Bl * mom;   

  return sigp;
}

double
PndFsmMdcFS::dphi(PndFsmTrack *) const // t //[R.K.03/2017] unused variable(s)
{
  return 0.0002; //to be refined
}

double
PndFsmMdcFS::dtheta(PndFsmTrack *) const // t //[R.K.03/2017] unused variable(s)
{
  return 0.0002; //to be refined
}

void
PndFsmMdcFS::print(ostream &o)
{
  o <<"Parameters for detector <"<<detName()<<">"<<endl;
  o  <<"  _thtMin = "<<_thtMin<<endl; 
  o  <<"  _angleXMax = "<<_angleXMax<<endl; 
  o  <<"  _angleYMax = "<<_angleYMax<<endl; 
  o  <<"  _radiationLength = "<<_radiationLength<<endl; 
  o  <<"  _pmin = "<<_pmin<<endl; 
  o  <<"  _a1Par = "<<_a1Par<<endl; 
  o  <<"  _a2Par = "<<_a2Par<<endl; 
  o  <<"  _Bl = "<<_Bl<<endl; 
  o  <<"  _efficiency = "<<_efficiency<<endl; 
}

void 
PndFsmMdcFS::initParameters()
{
  _detName         = "MdcFS";
  _thtMin          = 0.5;
  _angleXMax       = 10.0;
  _angleYMax       = 5.0;
  _radiationLength = 0.0;
  _pmin            = 0.7;
  _a1Par           = 0.00113;
  _a2Par           = 0.00227;
  _Bl              = 2.0;
  _efficiency	   =1.0; 

}

bool
PndFsmMdcFS::setParameter(std::string &name, double value)
{
  // *****************
  // include here all parameters which should be settable via tcl
  // *****************
      
  bool knownName=true;
  
  if (name == "thtMin")
    _thtMin=value;
  else
  if (name == "angleXMax")
    _angleXMax=value;
  else
  if (name == "angleYMax")
    _angleYMax=value;
  else
  if (name == "radiationLength")
    _radiationLength=value;
  else
  if (name == "pmin")
    _pmin=value;
  else
  if (name == "a1Par")
    _a1Par=value;
  else
  if (name == "a2Par")
    _a2Par=value;
  else
  if (name == "Bl")
    _Bl=value;
  else
  if (name == "efficiency")
    _efficiency=value;
  else
    knownName=false;
  
  return knownName;
}


