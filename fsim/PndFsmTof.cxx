//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmTof.cc,v 1.2 2007/05/24 08:07:40 klausg Exp $
//
// Description:
//      Class FsmTof
//      
//  Implementation of the barrel tof for the FastSim
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
#include "PndFsmTof.h"

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

PndFsmTof::PndFsmTof() 
{
  initParameters();
  
  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  print(std::cout);
}

PndFsmTof::PndFsmTof(ArgList &par) 
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

PndFsmTof::~PndFsmTof()
{
}

//--------------
// Operations --
//--------------

PndFsmResponse* 
PndFsmTof::respond(PndFsmTrack *t)
{
  PndFsmResponse *result=new PndFsmResponse();
  
  result->setDetector(this);
  bool wasDetected=detected(t);
//  result->setDetected(wasDetected);
  
  if (wasDetected && fabs(t->charge())>0.999)
  {
    TParticlePDG* part = _fdbPDG->GetParticle(t->pdt());
    double mass = (part) ? part->Mass() : t->p4().M();
    double theta = t->p4().Theta();
    double p=t->p4().Vect().Mag();
    double stht=sin(theta);
    double p_t=p*stht;
    double En=t->p4().E();
    double sigp=_dp*p;       
    double dt=_dt  * 1e-12 * 2.998*(1e+8) * sqrt(2.0); // time resolution of tof

    if ( p==0 || p_t==0 || En==0 ) {result->setDetected(false);return result;} // floating point check ********************
  
    // curvature of track due to magnet field
    double r = 3.3356 * p_t / _Bfield;

  // particle didn't pass through Tof 
  if( (2*r) < _rBarrel)  {

      result->setDetected(false);
      //std ::cout << "2r < rBarrel:: false from tof" << std::endl;
      return result;
    }

  else {
    
  double omega =  _Bfield / 3.3356 / En;          // angle velocity
  double time = 2.0*asin(_rBarrel/2.0/r)/omega;   // time
  double measp = _rand->Gaus(p,sigp);
//  std::cout<<"momen_smear_tof"<<p << "measp=" << measp;
  double measpt=measp*stht;

  double meas_t =  _rand->Gaus(0.0,dt);
  double meastime = time + meas_t ;   
  
//  std::cout<<"time_Tof"<<time << "meas_t=" << meas_t << std::endl;
 
  if (measpt==0) {result->setDetected(false);return result;} // floating point check ********************
  
  double arg = _rBarrel/2.0 *_Bfield/3.3356/measpt;
  
  if ( fabs(arg)>1.0 ) {result->setDetected(false);return result;} // floating point check ********************  

  double measE=0;
  measE = meastime*_Bfield /3.3356 *0.5/asin(arg);
  
  double measmas2 =  measE* measE -  measp* measp ;

  if (measmas2<0) {result->setDetected(false);return result;} // floating point check ********************

  double measmas = sqrt( measmas2);
  

  //std::cout<<"mass2 from tof" << measmas2 << std::endl;             
   
    double m_e  = _fdbPDG->GetParticle(11)->Mass();
    double m_mu = _fdbPDG->GetParticle(13)->Mass(); 
    double m_pi = _fdbPDG->GetParticle(211)->Mass(); 
    double m_K  = _fdbPDG->GetParticle(321)->Mass();
    double m_p  = _fdbPDG->GetParticle(2212)->Mass();
   
   double sig_e=0.05;   // to be refined
   double sig_mu=0.05;  // to be refined
   double sig_pi=0.05;  // to be refined
   double sig_K=0.05;   // to be refined
   double sig_p=0.05;   // to be refined
   
   double dm_p = measmas - mass; 
   
  //error calc of m^2
  double dE=measE*dt/meastime;
  double dm2=2*sqrt(measE*measE*dE*dE+measp*measp*sigp*sigp);
   
    result->setdm(dm_p);    
    double dm_test=result->dm();                              //aida
    //    std::cout <<"Tof_result_setdm ="<< dm_test <<std::endl;   //aida

/*          
     if(fabs(measmas2)<3.5) { result ->setm2(measmas2);}
     else {measmas2=3.5, result ->setm2(measmas2);}
*/
     result ->setm2(measmas2,dm2);
        
     result->setLHElectron( gauss_t(measmas,m_e,sig_e) );
     result->setLHMuon( gauss_t(measmas,m_mu,sig_mu) );
     result->setLHPion( gauss_t(measmas,m_pi,sig_pi) );;
     result->setLHKaon ( gauss_t(measmas,m_K,sig_K) ); 
     result->setLHProton( gauss_t(measmas,m_p,sig_p) );
  }
 }  
  return result;
}

double
PndFsmTof::gauss_t(double x, double x0, double s)
{
  return (1.0/(sqrt(2.0*M_PI)*s))*
          exp(-(x-x0)*(x-x0)/(2.0*s*s));
}

bool 
PndFsmTof::detected(PndFsmTrack *t) const
{
  if (t->hitMapValid()) {
    return t->hitMapResponse(FsmDetEnum::Tof);
  } 
  else 
  {

    int    lundId = abs(t->pdt());
    TParticlePDG* part = _fdbPDG->GetParticle(lundId);
    double mass = (part) ? part->Mass() : t->p4().M();
    double theta  = t->p4().Theta();
    double p      = t->p4().Vect().Mag();
    double p_t    = t->p4().Vect().Pt();
    double charge=t->charge();
     
    //only charged particles give signal 
    if (fabs(charge)<0.001) return false; 
     
    // due to track curvature particle doesn't reach barrel
    double rho = 3.3356 * p_t / _Bfield;
    if (_rBarrel>(2*rho)) return false; 
    
    //particle doesn't produce cherenkov light
    if (!(lundId==11 || lundId==13 || lundId==211 || lundId==321 || lundId==2212)) return false;
    
    //due to helix trajectory particle doesn't hit detector (even with dip angle in tht range)
    double z=2*rho*asin(_rBarrel/(2*rho))/tan(theta);    
    double polar=atan2(_rBarrel,z);
    if (polar<_thtMin || polar>_thtMax) return false; 
    
    //finally check for efficiency;
    return ( _rand->Rndm()<=_efficiency); 
  }
}

void
PndFsmTof::print(ostream &o)
{
  o <<"Parameters for detector <"<<detName()<<">"<<endl;
  o  <<"  _thtMin = "<<_thtMin<<endl; 
  o  <<"  _thtMax = "<<_thtMax<<endl; 
  o  <<"  _radiationLength = "<<_radiationLength<<endl; 
  o  <<"  _pmin = "<<_pmin<<endl; 
  o  <<"  _Bfield = "<<_Bfield<<endl; 
  o  <<"  _rBarrel = "<<_rBarrel<<endl; 
  o  <<"  _dSlab = "<<_dSlab<<endl; 
  o  <<"  _dp = "<<_dp<<endl; 
  o  <<"  _dt = "<<_dt<<endl; 
  o  <<"  _efficiency = "<<_efficiency<<endl; 
}

void 
PndFsmTof::initParameters()
{
  _detName =  FsmDetName::name(FsmDetEnum::Tof);
  _thtMin = 22.0;
  _thtMax = 140.0;
  _pmin = 0.0;
  _radiationLength = 0.0;
  _Bfield = 2.;               
  _rBarrel = 0.38;             
  _dSlab = 0.01;               
  _dp=0.01;                  //1 % is resolution of Tpc
  _dt=100;                   //time res in ps
  _efficiency	   =1.0; 
}

bool
PndFsmTof::setParameter(std::string &name, double value)
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
  if (name == "Bfield")
    _Bfield=value;
  else
  if (name == "rBarrel")
    _rBarrel=value;
  else
  if (name == "dSlab")
    _dSlab=value;
  else
  if (name == "dp")
    _dp=value;
  else
   if (name == "dt")
    _dt=value;
  else
 if (name == "efficiency")
    _efficiency=value;
  else
    knownName=false;
  
  return knownName;
}
