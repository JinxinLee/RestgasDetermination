//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmEffTracker.cc,v 1.9 2007/05/24 08:07:40 klausg Exp $
//
// Description:
//      Class FsmEffTracker
//      
//  Implementation of an effective Tracker for the FastSim
//  which provides resolution for mom, tht, phi from full sim
//  NOTE: should be used INSTEAD Tpc/Stt, Mvd and the Mdc's
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
#include "PndFsmEffTracker.h"

//-------------
// C Headers --
//-------------

//---------------
// C++ Headers --
//---------------
#include <math.h>
#include <iostream>
#include <fstream>

using std::cout;
using std::endl;
using std::ostream;
using std::string;
using std::ifstream;

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

PndFsmEffTracker::PndFsmEffTracker() 
{
  initParameters();

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  _parFileName = "$VMCWORKDIR/fsim/resParsTracker.dat";
  readParameters();

  print(std::cout);
}

PndFsmEffTracker::PndFsmEffTracker(ArgList &par) 
{
  initParameters();
  //set default parameter values and parses a parameter list
  //i.e. std::list<std::string> of the form
  //"a=1" "b=2" "c=3" 
  parseParameterList(par);

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  _parFileName = "$VMCWORKDIR/fsim/resParsTracker.dat";
  readParameters();

  print(std::cout);
}

//--------------
// Destructor --
//--------------

PndFsmEffTracker::~PndFsmEffTracker()
{
}

//--------------
// Operations --
//--------------

PndFsmResponse* 
PndFsmEffTracker::respond(PndFsmTrack *t)
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
    
    // now the dEdx information
    TParticlePDG* part = _fdbPDG->GetParticle(t->pdt());
    double mass = (part) ? part->Mass() : t->p4().M();
    double p=t->p4().Vect().Mag();
    
    // overall resolution for the tht_c measurement
    double dEdx = compdEdx(p,mass);
    double sig = _dEdxRes*dEdx;
        
    double m_e  = _fdbPDG->GetParticle(11)->Mass();
    double m_mu = _fdbPDG->GetParticle(13)->Mass(); 
    double m_pi = _fdbPDG->GetParticle(211)->Mass(); 
    double m_K  = _fdbPDG->GetParticle(321)->Mass();
    double m_p  = _fdbPDG->GetParticle(2212)->Mass();


    // compute the expected dEdx values for all particle types
    // we need these to determine the pdf's (gaussian around nominal dEdx with res sig)

    double dEdx_e  = compdEdx(p,m_e);
    double dEdx_mu = compdEdx(p,m_mu);
    double dEdx_pi = compdEdx(p,m_pi);
    double dEdx_K  = compdEdx(p,m_K);
    double dEdx_p  = compdEdx(p,m_p);
    
    double measdEdx =_rand->Gaus(dEdx,sig);
    result ->setTpcdEdx(measdEdx);

    if (dEdx_e)  result->setLHElectron( gauss(measdEdx,dEdx_e,sig) );
    if (dEdx_mu) result->setLHMuon( gauss(measdEdx,dEdx_mu,sig) );
    if (dEdx_pi) result->setLHPion( gauss(measdEdx,dEdx_pi,sig) );
    if (dEdx_K)  result->setLHKaon( gauss(measdEdx,dEdx_K,sig) );
    if (dEdx_p)  result->setLHProton(gauss(measdEdx,dEdx_p,sig) );
  }
  
  return result;
}

double
PndFsmEffTracker::gauss(double x, double x0, double s)
{
  return (1.0/(sqrt(2.0*M_PI)*s))*
          exp(-(x-x0)*(x-x0)/(2.0*s*s));
}


double 
PndFsmEffTracker::compdEdx(double p, double M)
{
  double dEdX;
  
  p*=1000;
  M*=1000;
  
  const double Z=10;
  const double A=20;
  const double z=1;//charge of incident particle in unit of e

  double beta;
  beta=p/sqrt(M*M+p*p);  //CalculateBeta(KE,M);

  double gamma;
  gamma=1./sqrt(1-beta*beta);

  const double I=10e-6*Z;//0.000188;//MeV
  const double me=0.511;//Mev/c2

  double Wmax;
  Wmax=(2*me*beta*beta*gamma*gamma) / (1 + 2*gamma*me/M + (me/M)*(me/M));

  //const double C1=0.1535;//MeV cm2/g
  
  double X,X0,X1;
  double kappa=0.307075;
  X0=0.201;
  X1=3;
  X=log10(beta*gamma);
  double delta;
  double C,a;
  C=-5.217;
  a=0.196;
  
  if(X<=X0)
    delta=0;
  else if(X<=X1)
    delta=2*log(10.)*X+C+a*(X1-X)*(X1-X)*(X1-X);
  else
    delta=2*log(10.)*X+C;
  
  dEdX= ( kappa * (Z/A)   * z*z /(beta*beta)) * (log(2*me*beta*beta*gamma*gamma*Wmax / (I*I)) - 2*beta*beta - delta); //-Dshell??
  
  return dEdX;
}

bool 
PndFsmEffTracker::detected(PndFsmTrack *t) const
{
  if (t->hitMapValid()) {
    return t->hitMapResponse(FsmDetEnum::Tpc);
  } else {
    double theta = t->p4().Theta();
    double p=t->p4().Vect().Mag();
    double charge=t->charge();
    return ( charge!=0.0 && theta>=_thtMin && theta<=_thtMax && p>_pmin  && _rand->Rndm()<=_efficiency);
  }
}

double 
PndFsmEffTracker::dp(PndFsmTrack *t) const
{
  double p=t->p4().Vect().Mag();
  double th=t->p4().Vect().Theta();
  //assert(th>=0 && p>=0);

  int idxR = int(p/6.0*_cols); 
  if (idxR>=_cols) idxR=_cols-1;

  int idxT = int(th/160.*_rows); 
  if (idxT>=_rows) idxT=_rows-1;

  double Dp=_sigMom[idxT][idxR]*p;  

  return ( Dp ); 
}

double
PndFsmEffTracker::dphi(PndFsmTrack *t) const
{ 
  double p=t->p4().Vect().Mag();
  double th=t->p4().Vect().Theta();
  //assert(th>=0 && p>=0);

  int idxR = int(p/6.0*_cols); 
  if (idxR>=_cols) idxR=_cols-1;

  int idxT = int(th/160.*_rows); 
  if (idxT>=_rows) idxT=_rows-1;

  double Dphi=_sigPhi[idxT][idxR]*M_PI/180.0;
   
  return Dphi; 
}

double
PndFsmEffTracker::dtheta(PndFsmTrack *t) const
{ 
  double p=t->p4().Vect().Mag();
  double th=t->p4().Vect().Theta();
  //assert(th>=0 && p>=0);

  int idxR = int(p/6.0*_cols); 
  if (idxR>=_cols) idxR=_cols-1;

  int idxT = int(th/160.*_rows); 
  if (idxT>=_rows) idxT=_rows-1;

  double dt=_sigTht[idxT][idxR]*M_PI/180.0;   

  return dt; //to be refined
}

void
PndFsmEffTracker::print(ostream &o)
{
  o <<"Parameters for detector <"<<detName()<<">"<<endl;
  o  <<"  _thtMin = "<<_thtMin<<endl; 
  o  <<"  _thtMax = "<<_thtMax<<endl; 
  o  <<"  _radiationLength = "<<_radiationLength<<endl; 
  o  <<"  _pmin = "<<_pmin<<endl; 
  o  <<"  _pRes = "<<_pRes <<endl; 
  o  <<"  _phiRes = "<<_phiRes << " degree" << endl;
  o  <<"  _thetaRes = "<<_thetaRes <<" degree" << endl;    
  o  <<"  _dEdxRes = "<<_dEdxRes << " (rel)"<< endl;    
  o  <<"  _efficiency = "<<_efficiency<<endl; 
  o  <<" Parametrizations:"<<endl<<"Phi:"<<endl;

  for (int i=0;i<_rows;i++){
    for (int j=0;j<_cols;j++) o << _sigPhi[i][j]<<" ";
    o<<endl;
  }
  o <<endl<<"Theta:"<<endl;
  for (int i=0;i<_rows;i++){
    for (int j=0;j<_cols;j++) o << _sigTht[i][j]<<" ";
    o<<endl;
  }
  o <<endl<<"Mom:"<<endl;

  for (int i=0;i<_rows;i++){
    for (int j=0;j<_cols;j++) o << _sigMom[i][j]<<" ";
    o<<endl;
  }
  o <<endl;
}

void 
PndFsmEffTracker::initParameters()
{
  _detName         = "PndFsmEffTracker";
  _thtMin          = 1.0;
  _thtMax          = 150.00;
  _radiationLength = 0.0;
  _pmin            = 0.0;
  _pRes            =0.005;//0.5% smearing
  _phiRes          =0.1; //0.1 degree smearing
  _thetaRes        =0.1; //0.1 degree smearing
  _dEdxRes         =0.07; // 7% dEdx resolution
  _efficiency	   =1.0; 

  _rows = 6;             //for the parameterization of res
  _cols = 12;            //in mom, tht, phi
}

bool
PndFsmEffTracker::setParameter(std::string &name, double value)
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
  if (name == "pRes")
    _pRes=value;
  else
  if (name == "phiRes")
    _phiRes=value;
  else
  if (name == "thetaRes")
    _thetaRes=value;
  else
  if (name == "dEdxRes")
    _dEdxRes=value;
  else
  if (name == "efficiency")
    _efficiency=value;
  else
    knownName=false;
  
  return knownName;
}

bool PndFsmEffTracker::readParameters()
{
  ifstream parFile(_parFileName.c_str());
  
  for (int i=0;i<_rows;i++)
    for (int j=0;j<_cols;j++) parFile >> _sigPhi[i][j];

  for (int i=0;i<_rows;i++)
    for (int j=0;j<_cols;j++) parFile >> _sigTht[i][j];

  for (int i=0;i<_rows;i++)
    for (int j=0;j<_cols;j++) parFile >> _sigMom[i][j];

  parFile.close();
  
  return true;
}
