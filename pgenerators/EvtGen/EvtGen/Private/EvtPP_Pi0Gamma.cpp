/* 
 * File:   EvtPP_Pi0Gamma.cpp
 * Author: luchinsky
 * 
 * Created on October 8, 2014, 3:24 PM
 */

#include <EvtGenBase/EvtVector4R.hh>

#include "EvtGenModels/EvtPP_Pi0Gamma.hh"

#define diag1 1
#define diag2 1
#define diag3 1
#define diag4 1

EvtPP_Pi0Gamma::EvtPP_Pi0Gamma(const EvtPP_Pi0Gamma& orig) :
  EvtDecayProb(orig),
  p1(orig.p1), p2(orig.p2), mp(orig.mp), mPi(orig.mPi), s(orig.s),first_run(orig.first_run)
{
}

EvtPP_Pi0Gamma::~EvtPP_Pi0Gamma() {}

void EvtPP_Pi0Gamma::decay(EvtParticle* root) {
    root->initializePhaseSpace(getNDaug(), getDaugs());
    if(first_run) {
        first_run=false;
        s = root->getP4Lab().mass2();
        p2 = EvtVector4R(mp, 0, 0, 0);
        p1 = root->getP4Lab() - p2;     

    };
    
    EvtVector4R kPi=root->getDaug(0)->getP4Lab(), k=root->getDaug(1)->getP4Lab();
    double t=(p1-kPi).mass2(), u=(p1-k).mass2();
    double matr2 = -4*(2*pow(mp,8)*(-5*s + t - u + 17*pow(mPi,2)) + u*pow(t,2)*
      (-2*s*u + 2*(s + u)*pow(mPi,2) - pow(mPi,4) - pow(s,2) + pow(t,2) - pow(u,2)) + 
     pow(mp,6)*(2*t*u + 16*s*(t + u) - 2*(3*s + 26*t + 10*u)*pow(mPi,2) - 3*pow(mPi,4) + pow(s,2) - 5*pow(t,2) + 3*pow(u,2)) - 
     pow(mp,4)*((6*t - 11*u)*pow(mPi,4) + (2*t + u)*pow(s,2) - 3*u*pow(t,2) - 4*pow(t,3) - 
        2*pow(mPi,2)*(8*s*t - s*u + 12*t*u + 13*pow(t,2) - 3*pow(u,2)) + 6*t*pow(u,2) + 6*s*(4*t*u + pow(t,2) + pow(u,2)) + pow(u,3)) + 
     pow(mp,2)*(pow(mPi,4)*(-2*t*u + 5*pow(t,2) - 4*pow(u,2)) - 
        2*pow(mPi,2)*(4*s*t*u + 3*s*pow(t,2) + 4*u*pow(t,2) + 2*pow(t,3) - 2*s*pow(u,2) - 2*pow(u,3)) + 
        t*(8*s*u*(t + u) + (t + 2*u)*pow(s,2) - 4*u*pow(t,2) - pow(t,3) + 3*t*pow(u,2) + 2*pow(u,3))))*pow(-t + pow(mp,2),-2)*
   pow(-u + pow(mp,2),-2);


    
    setProb(matr2);
}

void EvtPP_Pi0Gamma::init() {
    checkNArg(0);
    mp = EvtPDL::getMass(EvtPDL::getId("p+"));
    mPi=EvtPDL::getMass(EvtPDL::getId("pi0"));
}
