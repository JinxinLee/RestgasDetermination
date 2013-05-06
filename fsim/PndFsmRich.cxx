//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmRich.cc,v 1.1 2007/05/24 08:07:40 klausg Exp $
//
// Description:
//      Class FsmRich
//      
//  Implementation of the Far Forward Rich for the FastSim
//
//  This software was developed for the PANDA collaboration.  If you
//  use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//      Klaus Goetzen                    Original Author
//
// Copyright Information:
//      Copyright (C) 2007              GSI
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndFsmRich.h"

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

PndFsmRich::PndFsmRich()    
{
  initParameters();

  _angleXMax *= M_PI/180.0;
  _angleYMax *= M_PI/180.0;
  print(std::cout);
}

PndFsmRich::PndFsmRich(ArgList &par) 
{
  initParameters();
  //set default parameter values and parses a parameter list
  //i.e. std::list<std::string> of the form
  //"a=1" "b=2" "c=3" 
  parseParameterList(par);

  _angleXMax *= M_PI/180.0;
  _angleYMax *= M_PI/180.0;
  print(std::cout);
}

//--------------
// Destructor --
//--------------

PndFsmRich::~PndFsmRich()
{
}

//--------------
// Operations --
//--------------

PndFsmResponse* 
PndFsmRich::respond(PndFsmTrack *t)
{
  PndFsmResponse *result=new PndFsmResponse();
  
  result->setDetector(this);
  bool wasDetected=detected(t);
  result->setDetected(wasDetected);
  
  if (wasDetected && fabs(t->charge())>1e-8)
  {
    TParticlePDG* part = _fdbPDG->GetParticle(t->pdt());
    double mass = (part) ? part->Mass() : t->p4().M();
    double theta = t->p4().Theta();
    double p=t->p4().Vect().Mag();
    double ctht=cos(theta);
    //double p_t=p*sin(theta);

    if ( p==0 || ctht==0 ) {result->setDetected(false);return result;} // floating point check ********************

    double lambda1 = 280e-9;  //range of wavelength, which is seen by the PMT/PD
    double lambda2 = 330e-9;

    double alpha=7.2974e-3;   //finestructure constant
    
    // curvature of track due to magnet field
    //double r = 3.3356 * p_t / _Bfield;
    
    // dip angle in phi direction (due to curvature of track in magnet field)
    //double psi = acos(_rBarrel/(2*r));

    // path length in radiator
    double l = _dRich/ctht;//  _dSlab*sqrt( 1/(sin(theta)*sin(theta)) + 1/(tan(psi)*tan(psi)) );
    
    // estimate the number of cherenkov photons hitting our photosensor
    double nPhot = _effNPhotons*2*M_PI*alpha*l*(1./lambda1 - 1./lambda2)*(1 - (mass*mass+p*p)/(p*p*_nRefrac*_nRefrac)); 

    if (nPhot==0) {result->setDetected(false);return result;} // floating point check ********************

    nPhot = _rand->Poisson(nPhot);

    // ************** reset detected and quit due to low numbers of photons 

    if (nPhot<=_nPhotMin) {
      result->setDetected(false);
      return result;
    }
    
    if (nPhot>100) nPhot=100;

    // overall resolution for the tht_c measurement
    double sig = _dthtc/sqrt(nPhot);
    double thtC = compThetaC(p,mass);
    
    double m_e  = _fdbPDG->GetParticle(11)->Mass();
    double m_mu = _fdbPDG->GetParticle(13)->Mass(); 
    double m_pi = _fdbPDG->GetParticle(211)->Mass(); 
    double m_K  = _fdbPDG->GetParticle(321)->Mass();
    double m_p  = _fdbPDG->GetParticle(2212)->Mass();

    // compute the expected cherenkov angles for all particle types
    // we need these to determine the pdf's (gaussian around nominal tht_c with res sig)
    // this Likelihood function has to be evaluated for the _measured_ momentum, which 
    // is smeared with dp!

    double measp=_rand->Gaus(p,_dp*p);
    if (measp<0) measp=0;

    double thtc_e  = compThetaC(measp,m_e);
    double thtc_mu = compThetaC(measp,m_mu);
    double thtc_pi = compThetaC(measp,m_pi);
    double thtc_K  = compThetaC(measp,m_K);
    double thtc_p  = compThetaC(measp,m_p);
    
    double measThetaC = _rand->Gaus(thtC,sig);
    if (measThetaC<0) measThetaC=0;
    
    result->setRichThtc(measThetaC,sig);
     
    if (thtc_e)  result->setLHElectron( gauss(measThetaC,thtc_e,sig) );
    if (thtc_mu) result->setLHMuon( gauss(measThetaC,thtc_mu,sig) );
    if (thtc_pi) result->setLHPion( gauss(measThetaC,thtc_pi,sig) );
    if (thtc_K)  result->setLHKaon( gauss(measThetaC,thtc_K,sig) );
    if (thtc_p)  result->setLHProton(gauss(measThetaC,thtc_p,sig) );
    
  }
  
  return result;
}

double
PndFsmRich::gauss(double x, double x0, double s)
{
  return (1.0/(sqrt(2.0*M_PI)*s))*
          exp(-(x-x0)*(x-x0)/(2.0*s*s));
}

double
PndFsmRich::compThetaC(double p, double m)
{
  double val=0.0;
  if (p==0) return 0.0;
  if ( (val = (p*p+m*m)/(p*p*_nRefrac*_nRefrac)) <= 1.0 ) return acos(sqrt(val));
  else return 0.0;
}


bool 
PndFsmRich::detected(PndFsmTrack *t) const
{
  if (t->hitMapValid()) {
    return t->hitMapResponse(FsmDetEnum::Drc);
  } else {
    int lundId=abs(t->pdt());
    TParticlePDG* part = _fdbPDG->GetParticle(lundId);
    double mass = (part) ? part->Mass() : t->p4().M();
    //    double mass = t->pdt()->mass();
    double p_cerenkov_min=mass/sqrt(_nRefrac*_nRefrac - 1.0);
    
    if (t->p4().Z()==0) return false;

    double angleX = fabs(atan(t->p4().X()/t->p4().Z()));
    double angleY = fabs(atan(t->p4().Y()/t->p4().Z()));
    double p=t->p4().Vect().Mag();
    
    bool correctPidType=false;
    
    if (lundId==11 || lundId==13 || lundId==211 || lundId==321 || lundId==2212) correctPidType=true;

    return ( p>p_cerenkov_min  && angleX<=_angleXMax && angleY<=_angleYMax && correctPidType  && _rand->Rndm()<=_efficiency);
  }
}

void
PndFsmRich::print(ostream &o)
{
  o <<"Parameters for detector <"<<detName()<<">"<<endl;
  o  <<"  _angleXMax = "<<_angleXMax<<endl; 
  o  <<"  _angleYMax = "<<_angleYMax<<endl; 
  o  <<"  _radiationLength = "<<_radiationLength<<endl; 
  o  <<"  _pmin = "<<_pmin<<endl; 
  o  <<"  _dthtc = "<<_dthtc<<endl; 
  o  <<"  _nPhotMin = "<<_nPhotMin<<endl; 
  o  <<"  _nRefrac = "<<_nRefrac<<endl; 
  o  <<"  _Bfield = "<<_Bfield<<endl; 
  o  <<"  _effNPhotons = "<<_effNPhotons<<endl; 
  o  <<"  _dRich = "<<_dRich<<endl; 
  o  <<"  _dp = "<<_dp<<endl; 
  o  <<"  _efficiency = "<<_efficiency<<endl; 
}

void 
PndFsmRich::initParameters()
{
  _detName = "Rich";
  _angleXMax = 5.0;
  _angleYMax = 10.0;
  _radiationLength = 0.0;
  _pmin = 0.0;
  _dthtc = 0.01;
  _nPhotMin = 5;
  _nRefrac = 1.05;
  _Bfield = 2.;               
  _effNPhotons = 0.075;         
  _dRich = 1.0;               
  _dp = 0.01;               
  _efficiency	   =1.0; 
}

bool
PndFsmRich::setParameter(std::string &name, double value)
{
  // *****************
  // include here all parameters which should be settable via tcl
  // *****************
      
  bool knownName=true;
  
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
  if (name == "dthtc")
    _dthtc=value;
  else
  if (name == "nPhotMin")
    _nPhotMin=value;
  else
  if (name == "nRefrac")
    _nRefrac=value;
  else
  if (name == "Bfield")
    _Bfield=value;
  else
  if (name == "effNPhotons")
    _effNPhotons=value;
  else
  if (name == "dRich")
    _dRich=value;
  else
  if (name == "dp")
    _dp=value;
  else
  if (name == "efficiency")
    _efficiency=value;
  else
    knownName=false;
  
  return knownName;
}

