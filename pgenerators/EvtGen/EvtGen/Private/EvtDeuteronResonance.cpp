//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtDeuteronSpectator.cc, v 1.0 2012/02/08 $
//
// Description:
//            Generator for pbar d -> pbarnSystem p(spectator)
//                          pbar d -> pbarpSystem n(spectator)
//            derived from EvtSingleParticle.cc
//
// Author List:
//            Albrecht Gillitzer
//            Alexander Blinov
//          
//------------------------------------------------------------------------

#include "EvtGenBase/EvtPatches.hh"
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtVector4R.hh"
//#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenModels/EvtDeuteronResonance.hh"
#include "EvtGenBase/EvtReport.hh"
#include <string>
#include "EvtGenBase/EvtConst.hh"
using std::endl;

EvtDeuteronResonance::~EvtDeuteronResonance() {}

std::string EvtDeuteronResonance::getName() {

  return "DeuteronResonance";

}

EvtDecayBase* EvtDeuteronResonance::clone(){

  return new EvtDeuteronResonance();

}

void EvtDeuteronResonance::init() {


  if (getNArg()==4) {
    
    _pmax=getArg(0);
    // expect default selection _pmax = 1.0 GeV/c for 0 or negative parameter value
    if(_pmax<=0.) _pmax = 1.0;

    // _pmax outside [0.2, 1.5] GeV/c is rejected
    if(_pmax<0.2 || _pmax>1.5) {
      report(EVT_ERROR,"EvtGen") << "Maximum spectator nucleon momentum should be within [0.2, 1.5] GeV/c!" << endl;
      report(EVT_ERROR,"EvtGen") << "Will terminate execution!"<<endl;
      ::abort();
    }

    // _mmin is the sum of the particle masses the pbar+N residual system is decaying to
    _mmin = getArg(1);

    // _rmass is a mass of resonance in the pbar+N system
    _rmass = getArg(2);

    // _rwidth is a width of resonance in the pbar+N system
    _rwidth = getArg(3);

  }else{
    
    report(EVT_ERROR,"EvtGen") << "EvtDeuteronResonance generator expects "
                           << " 4 arguments but found:"<<getNArg()<<endl;
    report(EVT_ERROR,"EvtGen") << "Will terminate execution!"<<endl;
    ::abort();

  }

  report(EVT_INFO,"EvtGen") << "The deuteron spectator generator has been configured:"
    <<endl;
  report(EVT_INFO,"EvtGen") << " spectator momentum < " << _pmax << " GeV/c" << endl;
  report(EVT_INFO,"EvtGen") << " minimum pbar+N mass < " << _mmin << " GeV/c2" << endl;

}

void EvtDeuteronResonance::decay( EvtParticle *p ) {

  EvtParticle *d1;
  EvtParticle *d2;
  EvtVector4R p4spec;
  EvtVector4R p4mlab;
  EvtVector4R p4mbst;
  EvtVector4R p4mother;
  EvtVector4R p4daug1;
  EvtVector4R p4daug2;

  double mass = EvtPDL::getMass(getDaug(0));
  double daug2m;

  p->makeDaughters(getNDaug(),getDaugs());
  d1 = p->getDaug(0);
  d2 = p->getDaug(1);

  // Parameters of Hulthen momentum distribution
  double cn = 4.025e-5;
  double a2 = 2.07936e-3;
  double b2 = 6.76e-2;

  double u, v, w;

  double r1, r2;

  do {
  do {

    //generate spectator proton momentum distribution in lab frame:
    //f(p) according to Hulthen distribution
    //isotropic angular distribution, i.e.flat in cos(theta) and phi

    do {
      u = EvtRandom::Flat(0,_pmax);
      v = cn*u*u/(u*u+a2)/(u*u+a2)/(u*u+b2)/(u*u+b2);
      w = EvtRandom::Flat(0,1);
    } while (w>v);

    double psp = u;

    double ctheta = EvtRandom::Flat(-1,1);
    double phi = EvtRandom::Flat(0,EvtConst::twoPi);

    double stheta = sqrt(1.0-ctheta*ctheta);

    p4spec.set(sqrt(mass*mass+psp*psp),psp*cos(phi)*stheta,psp*sin(phi)*stheta,psp*ctheta);

    //transform to 4-momentum in the pbar+d cm frame:
  
    //lab 4-momentum of pbardSystem
    p4mlab = p->getP4Lab();

    //reflect 3-momentum to get (backward) boost from lab to cm
    p4mbst.set(p4mlab.get(0),-p4mlab.get(1),-p4mlab.get(2),-p4mlab.get(3));

    p4daug1 = p4spec;
    p4daug1.applyBoostTo(p4mbst);

    //get 4-momentum of residual pbar+n system from 4-momentum balance
    p4mother = p->getP4Restframe();
    p4daug2 = p4mother - p4daug1;

    //check if pbar+n mass is large enough for specified decay
    daug2m = p4daug2.mass();
    //report(EVT_INFO,"EvtGen") << "daug2m: " << daug2m << endl;
    if(daug2m<_mmin) {
      report(EVT_INFO,"EvtGen") << "pbar+N mass " << daug2m << " GeV/c2 is below threshold for specified decay channel!" << endl;
    }
  } while (daug2m < _mmin);
  r1 = daug2m/_rwidth;
  r2 = _rmass/daug2m;
  } while (1.< (1.+r1*r1*(1.-r2*r2)*(1.-r2*r2))*EvtRandom::Flat(0,1.));
  d1->init(getDaug(0),p4daug1);
  d2->init(getDaug(1),p4daug2);

  return;
}
