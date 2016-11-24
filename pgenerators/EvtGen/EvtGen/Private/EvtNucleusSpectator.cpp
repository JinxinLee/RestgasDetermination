//--------------------------------------------------------------------------
// File and Version Information:
// 	      $Id: EvtNucleusSpectator.cpp,v 1.0 2016/11/04 $
//
// Description:
//            Generator for pbar A -> (A-1)_spectator + pbarp system
//                          pbar A -> (A-1)_spectator + pbarn system
//            derived from EvtSingleParticle.hh
//
// Author List:
//            Albrecht Gillitzer
//
//------------------------------------------------------------------------

#include "EvtGenBase/EvtPatches.hh"
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtVector4R.hh"
//#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenModels/EvtNucleusSpectator.hh"
#include "EvtGenBase/EvtReport.hh"
#include <string>
#include "EvtGenBase/EvtConst.hh"
using std::endl;

EvtNucleusSpectator::EvtNucleusSpectator() : EvtDecayIncoherent(), _targA(0.), _pmax(0.), _mmin(0.)
{
}

EvtNucleusSpectator::~EvtNucleusSpectator() {}

std::string EvtNucleusSpectator::getName() {

  return "NucleusSpectator";

}

EvtDecayBase* EvtNucleusSpectator::clone(){

  return new EvtNucleusSpectator();

}

void EvtNucleusSpectator::init() {


  if (getNArg()==3) {
    
    _targA=getArg(0);
    // _targA outside [3,238] is rejected
    if(_targA<3. || _targA>238) {
      report(EVT_ERROR,"EvtGen") << "Target mass number must be within [3,238]!" << endl;
      report(EVT_ERROR,"EvtGen") << "Will terminate execution!"<<endl;
      ::abort();
    }

    _pmax=getArg(1);
    // expect default selection _pmax = 1.0 GeV/c for 0 or negative parameter value
    if(_pmax<=0.) _pmax = 1.0;

    // _pmax outside [0.3, 1.5] GeV/c is rejected
    if(_pmax<0.3 || _pmax>1.5) {
      report(EVT_ERROR,"EvtGen") << "Maximum spectator nucleon momentum should be within [0.3, 1.5] GeV/c!" << endl;
      report(EVT_ERROR,"EvtGen") << "Will terminate execution!"<<endl;
      ::abort();
    }

    // _mmin is the sum of the particle masses the reacting pbar+N system is decaying to
    _mmin = getArg(2);


  }else{
    
    report(EVT_ERROR,"EvtGen") << "EvtNucleusSpectator generator expects "
                           << " 3 arguments but found:"<<getNArg()<<endl;
    report(EVT_ERROR,"EvtGen") << "Will terminate execution!"<<endl;
    ::abort();

  }

  report(EVT_INFO,"EvtGen") << "The nucleus spectator generator has been configured:"
    <<endl;
  report(EVT_INFO,"EvtGen") << " target mass number = " << _targA << endl;
  report(EVT_INFO,"EvtGen") << " spectator momentum < " << _pmax << " GeV/c" << endl;
  report(EVT_INFO,"EvtGen") << " minimum pbar+N mass = " << _mmin << " GeV/c2" << endl;

}

void EvtNucleusSpectator::decay( EvtParticle *p ) {

  EvtParticle *d1;
  EvtParticle *d2;
  EvtVector4R p4spec;
  EvtVector4R p4mlab;
  EvtVector4R p4mbst;
  EvtVector4R p4mother;
  EvtVector4R p4daug1;
  EvtVector4R p4daug2;

  double um = 0.9315;
  double mass = um*(_targA-1);
  double daug2m;

  p->makeDaughters(getNDaug(),getDaugs());
  d1 = p->getDaug(0);
  d2 = p->getDaug(1);

  // Parameters of double Gaussian momentum distribution:
  // W(p) = a0*p**2*( exp(-0.5*(p/sig1)**2) + a2*exp(-0.5*(p/sig2)**2) )

  //double cn = 4.025e-5;
  //double a2 = 2.07936e-3;
  //double b2 = 6.76e-2;
  double a0 = 99.73;
  double a2 = 1.4569e-2;
  double sig1 = 0.115;
  double sig2 = 0.220;


  double u, v, w, u1, u2;
  do {

    //generate residual (A-1) spectator nucleus momentum distribution in lab frame:
    //f(p) according to double-gaussian distribution
    //isotropic angular distribution, i.e.flat in cos(theta) and phi

    do {
      u = EvtRandom::Flat(0,_pmax);
      u1 = u/sig1;
      u2 = u/sig2;
      v = a0*u*u*(exp(-0.5*u1*u1) + a2*exp(-0.5*u2*u2));
      w = EvtRandom::Flat(0,1);
    } while (w>v);

    double psp = u;

    double ctheta = EvtRandom::Flat(-1,1);
    double phi = EvtRandom::Flat(0,EvtConst::twoPi);

    double stheta = sqrt(1.0-ctheta*ctheta);

    p4spec.set(sqrt(mass*mass+psp*psp),psp*cos(phi)*stheta,psp*sin(phi)*stheta,psp*ctheta);
    //report(EVT_INFO,"EvtGen") << "p4spec: " << p4spec << endl;

    //transform to 4-momentum in the pbar+A cm frame:
  
    //lab 4-momentum of pbarASystem
    p4mlab = p->getP4Lab();

    //reflect 3-momentum to get (backward) boost from lab to cm
    p4mbst.set(p4mlab.get(0),-p4mlab.get(1),-p4mlab.get(2),-p4mlab.get(3));

    p4daug1 = p4spec;
    p4daug1.applyBoostTo(p4mbst);

    //get 4-momentum of reacting pbar+N system from 4-momentum balance
    p4mother = p->getP4Restframe();
    p4daug2 = p4mother - p4daug1;

    //check if pbar+N mass is large enough for specified decay
    daug2m = p4daug2.mass();
    //report(EVT_INFO,"EvtGen") << "daug2m: " << daug2m << endl;
    if(daug2m<_mmin) {
      report(EVT_INFO,"EvtGen") << "pbar+N mass " << daug2m << " GeV/c2 is below threshold for specified decay channel!" << endl;
    }
  } while (daug2m < _mmin);

  d1->init(getDaug(0),p4daug1);
  d2->init(getDaug(1),p4daug2);

  return;
}
