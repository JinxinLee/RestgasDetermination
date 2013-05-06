//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmDrcBarrel.cc,v 1.9 2007/05/24 08:07:40 klausg Exp $
//
// Description:
//      Class FsmDrcBarrel
//      
//  Implementation of the barrel DIRC for the FastSim
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
#include "PndFsmDrcBarrel.h"

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

#include "TH2F.h"
#include "TFile.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

//----------------
// Constructors --
//----------------

PndFsmDrcBarrel::PndFsmDrcBarrel() 
{
  initParameters();
  
  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  
  readParameters();
  
  print(std::cout);
}

PndFsmDrcBarrel::PndFsmDrcBarrel(ArgList &par) 
{
  initParameters();
  //set default parameter values and parses a parameter list
  //i.e. std::list<std::string> of the form
  //"a=1" "b=2" "c=3" 
  parseParameterList(par);
  
  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  
  readParameters();
  
  print(std::cout);
}

//--------------
// Destructor --
//--------------

PndFsmDrcBarrel::~PndFsmDrcBarrel()
{
  for (int i=0;i<5;i++) 
    if (trapfrac[i]) delete trapfrac[i];
}

//--------------
// Operations --
//--------------

PndFsmResponse* 
PndFsmDrcBarrel::respond(PndFsmTrack *t)
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
    double stht=sin(theta);
    double p_t=p*stht;
    
    int    pid=abs(t->pdt());
    int    npid=-1;
    
    if (pid==11) npid=0;
    else if (pid==13)   npid=1;
    else if (pid==211)  npid=2;
    else if (pid==321)  npid=3;
    else if (pid==2212) npid=4;
    
    double thtdeg=theta/M_PI*180.;

    if ( p==0 || p_t==0 || stht==0 ) {result->setDetected(false);return result;} // floating point check ********************

    double lambda1 = 280e-9;  //range of wavelength, which is seen by the PMT/PD
    double lambda2 = 330e-9;

    double alpha=7.2974e-3;   //finestructure constant
    // curvature of track due to magnet field
    double r = 3.3356 * p_t / _Bfield;
    
    if (_rBarrel>(2*r)) {result->setDetected(false);return result;} // floating point check ********************

    // dip angle in phi direction (due to curvature of track in magnet field)
    double psi = acos(_rBarrel/(2*r));

    // path length in radiator
    double tpsi=tan(psi);

    if ( tpsi==0 ) {result->setDetected(false);return result;} // floating point check ********************

    double l = _dSlab*sqrt( 1/(stht*stht) + 1/(tpsi*tpsi) );
    
    // deteremine trapping fraction
    double trapped = _trap;
    
    if (npid>=0 && trapfrac[npid]) 
      trapped = npid<0 ? 0.0 : trapfrac[npid]->GetBinContent(trapfrac[npid]->FindBin(p<6.0?p:6.0,thtdeg)); 
    
    // estimate the number of initially produced cherenkov photons
    double nPhot = 2*M_PI*alpha*l*(1./lambda1 - 1./lambda2)*(1 - (mass*mass+p*p)/(p*p*_nRefrac*_nRefrac)); 
    
    // dice a poisson value
    nPhot = _rand->Poisson(nPhot);
    
    // determine the number of photons hitting the sensor
    nPhot *= trapped*_effNPhotons; 
   
    // ************** reset detected and quit due to low numbers of photons 

    if (nPhot<=_nPhotMin) {
      result->setDetected(false);
      return result;
    }
    
    if (nPhot>100) nPhot=100;

    // overall resolution for the tht_c measurement
    double sig = _dthtc/sqrt(nPhot);
    // the true tht_c value for the true momentum
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
    
    result->setDrcBarrelThtc(measThetaC,sig);
     
    if (thtc_e)  result->setLHElectron( gauss(measThetaC,thtc_e,sig) );
    if (thtc_mu) result->setLHMuon( gauss(measThetaC,thtc_mu,sig) );
    if (thtc_pi) result->setLHPion( gauss(measThetaC,thtc_pi,sig) );
    if (thtc_K)  result->setLHKaon( gauss(measThetaC,thtc_K,sig) );
    if (thtc_p)  result->setLHProton(gauss(measThetaC,thtc_p,sig) );
    
  }
  
  return result;
}

double
PndFsmDrcBarrel::gauss(double x, double x0, double s)
{
  return (1.0/(sqrt(2.0*M_PI)*s))*
          exp(-(x-x0)*(x-x0)/(2.0*s*s));
}

double
PndFsmDrcBarrel::compThetaC(double p, double m)
{
  double val=0.0;
  if (p==0) return 0.0;
  if ( (val = (p*p+m*m)/(p*p*_nRefrac*_nRefrac)) <= 1.0 ) return acos(sqrt(val));
  else return 0.0;
}


bool 
PndFsmDrcBarrel::detected(PndFsmTrack *t) const
{
  if (t->hitMapValid()) {
    return t->hitMapResponse(FsmDetEnum::Drc);
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
    
    // particles momentum below cherenkov threshold
    double p_cerenkov_min=mass/sqrt(_nRefrac*_nRefrac - 1.0);
    if (p<p_cerenkov_min) return false; 
    
    //due to helix trajectory particle doesn't hit detector (even with dip angle in tht range)
    double z=2*rho*asin(_rBarrel/(2*rho))/tan(theta);    
    double polar=atan2(_rBarrel,z);
    if (polar<_thtMin || polar>_thtMax) return false; 
    
    //finally check for efficiency;
    return ( _rand->Rndm()<=_efficiency); 
  }
}


void
PndFsmDrcBarrel::print(ostream &o)
{
  o <<"Parameters for detector <"<<detName()<<">"<<endl;
  o  <<"  _thtMin          = "<<_thtMin<<endl; 
  o  <<"  _thtMax          = "<<_thtMax<<endl; 
  o  <<"  _radiationLength = "<<_radiationLength<<endl; 
  o  <<"  _pmin            = "<<_pmin<<endl; 
  o  <<"  _dthtc           = "<<_dthtc<<endl; 
  o  <<"  _nPhotMin        = "<<_nPhotMin<<endl; 
  o  <<"  _nRefrac         = "<<_nRefrac<<endl; 
  o  <<"  _Bfield          = "<<_Bfield<<endl; 
  o  <<"  _effNPhotons     = "<<_effNPhotons<<endl; 
  o  <<"  _rBarrel         = "<<_rBarrel<<endl; 
  o  <<"  _dSlab           = "<<_dSlab<<endl; 
  o  <<"  _dp              = "<<_dp<<endl; 
  o  <<"  _trap            = "<<_trap<<endl; 
  o  <<"  _efficiency      = "<<_efficiency<<endl; 
  o  <<"  _parFileName     = "<<_parFileName<<endl; 
}

void 
PndFsmDrcBarrel::initParameters()
{
  _detName         = "DrcBarrel";
  _thtMin          = 22.0;
  _thtMax          = 140.0;
  _radiationLength = 0.0;
  _pmin            = 0.0;
  _dthtc           = 0.01;
  _nPhotMin        = 5;
  _nRefrac         = 1.472;
  _Bfield          = 2.;               
  _effNPhotons     = 0.10;         
  _rBarrel         = 0.48;             
  _dSlab           = 0.017;               
  _dp              = 0.01;
  _trap            = 0.7;
  _efficiency	   = 1.0;
  _parFileName     = "$VMCWORKDIR/fsim/trapfrac_barrel.root"; 
}

bool
PndFsmDrcBarrel::setParameter(std::string &name,std::string &value)
{
  // *****************
  // include here all string parameters which should be settable
  // *****************
  
  bool knownName=true;
  
  if (name == "parFileName")
    _parFileName=value;
  else
    knownName=false;
  
  return knownName;
}

bool
PndFsmDrcBarrel::setParameter(std::string &name, double value)
{
  // *****************
  // include here all float parameters which should be settable
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
  if (name == "rBarrel")
    _rBarrel=value;
  else
  if (name == "dSlab")
    _dSlab=value;
  else
  if (name == "dp")
    _dp=value;
  else
  if (name == "trap")
    _trap=value;
  else
  if (name == "efficiency")
    _efficiency=value;
  else
    knownName=false;
  
  return knownName;
}

bool PndFsmDrcBarrel::readParameters()
{
  
  TFile *f=new TFile(_parFileName.c_str());
  
  for (int i=0;i<5;i++) 
  {
    trapfrac[i]=0;
  }
  
  if (f->IsZombie()) 
  {
    cout <<" -W-  (PndFsmDrcBarrel::readParameters) - file "<<_parFileName.c_str()
         <<" doesn't exist. Using constant trapping fraction _trap="<<_trap<<endl;
  }
  else 
  {
    trapfrac[0]=(TH2F*)f->Get("hacc0");
    trapfrac[1]=(TH2F*)f->Get("hacc1");
    trapfrac[2]=(TH2F*)f->Get("hacc2");
    trapfrac[3]=(TH2F*)f->Get("hacc3");
    trapfrac[4]=(TH2F*)f->Get("hacc4");
    
    for (int i=0;i<5;i++) trapfrac[i]->SetDirectory(0);
      
    f->Close();
  }
  delete f;
  
  return true;
}
